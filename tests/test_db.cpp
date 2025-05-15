#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <vector>
#include "../db/request.h"
#include "../db/db_connection.h"
using std::vector;
TEST_CASE("test select queries", "[all]") {
    Request request = SelectRequest::Select("*").From("alan").Where("id==1").Finalize();
    DatabaseConnection connection("/home/shfdis/CLionProjects/restAPI/tests/db");
    vector<vector<std::string>> vec = connection.Exec(request, 2);
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0].size() == 2);
    REQUIRE(vec[0][0] == "1");
    REQUIRE(vec[0][1] == "Shfdis");
    request = SelectRequest::Select("*").From("alan").Where("id==17").Finalize();
    vec = connection.Exec(request, 2);
    REQUIRE(vec.size() == 0);
    request = SelectRequest::Select("*").From("alan1").Where("id==1").Finalize();
    REQUIRE_THROWS(connection.Exec(request, 2));
    request = SelectRequest::Select("Name").From("alan").Where("id==1").Finalize();
    vec = connection.Exec(request, 1);
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0][0] == "Shfdis");
    REQUIRE(vec[0].size() == 1);
}