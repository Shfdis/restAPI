#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <vector>

#include "../db/db_connection.h"
#include "../db/request.h"
using std::vector;
TEST_CASE("test select queries") {
  RequestValue request =
      SelectRequest::Select("*").From("alan").Where("id==1").Finalize();
  DatabaseConnection connection("/home/shfdis/CLionProjects/restAPI/tests/db");
  vector<vector<std::string> > vec = connection.Exec(request, 2);
  REQUIRE(vec.size() == 1);
  REQUIRE(vec[0].size() == 2);
  REQUIRE(vec[0][0] == "1");
  REQUIRE(vec[0][1] == "Shfdis");
  request = SelectRequest::Select("*").From("alan").Where("id==17").Finalize();
  vec = connection.Exec(request, 2);
  REQUIRE(vec.empty());
  request = SelectRequest::Select("*").From("alan1").Where("id==1").Finalize();
  REQUIRE_THROWS(connection.Exec(request, 2));
  request =
      SelectRequest::Select("Name").From("alan").Where("id==1").Finalize();
  vec = connection.Exec(request, 1);
  REQUIRE(vec.size() == 1);
  REQUIRE(vec[0][0] == "Shfdis");
  REQUIRE(vec[0].size() == 1);
  request = SelectRequest::Select("*")
                .From("alan")
                .Where("id==1")
                .Limit(1)
                .Finalize();
  vec = connection.Exec(request, 2);
  REQUIRE(vec.size() == 1);
  REQUIRE(vec[0].size() == 2);
  REQUIRE(vec[0][0] == "1");
  REQUIRE(vec[0][1] == "Shfdis");
}

TEST_CASE("test insert/delete queries") {
  {
    RequestValue req = InsertRequest::Insert("alan")
                           .Columns(std::vector<std::string>{"Name"})
                           .Values(std::vector<std::string>{"\"your_mama\""})
                           .Finalize();
    DatabaseConnection connection(
        "/home/shfdis/CLionProjects/restAPI/tests/db");
    vector<vector<std::string> > vec = connection.Exec(req, 2);
    REQUIRE(vec.size() == 0);
    req = SelectRequest::Select("*").From("alan").Finalize();
    vec = connection.Exec(req, 2);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0][0] == "1");
    REQUIRE(vec[1][0] == "2");
    req = DeleteRequest::Delete().From("alan").Where("id==2").Finalize();
    vec = connection.Exec(req, 2);
    REQUIRE(vec.size() == 0);
    req = SelectRequest::Select("*").From("alan").Finalize();
    vec = connection.Exec(req, 2);
    REQUIRE(vec.size() == 1);
  }
  for (size_t iterations = 0; iterations < 100; ++iterations) {
    int inserts = rand() % 10;
    vector<std::string> names;
    DatabaseConnection connection(
        "/home/shfdis/CLionProjects/restAPI/tests/db");
    while (inserts--) {
      std::string name = std::to_string(inserts);
      RequestValue req = InsertRequest::Insert("alan")
                             .Columns(std::vector<std::string>{"Name"})
                             .Values(std::vector<std::string>{name})
                             .Finalize();
      names.push_back(name);

      connection.Exec(req, 2);
    }
    for (auto name : names) {
      RequestValue req = DeleteRequest::Delete()
                             .From("alan")
                             .Where("Name==" + name)
                             .Finalize();
      connection.Exec(req, 2);
    }
    RequestValue req = SelectRequest::Select("*").From("alan").Finalize();
    REQUIRE(connection.Exec(req, 2).size() == 1);
  }
}
