#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "../db/request.h"
TEST_CASE("test select construction", "[all]") {
  REQUIRE(SelectRequest::Select("mom").ToString() == "SELECT mom");
  REQUIRE_THROWS(SelectRequest::Select("mom").Where("alan > 8"));
  REQUIRE(SelectRequest::Select("*")
              .From("sosal")
              .Where("sex > 3")
              .Limit(10)
              .Finalize()
              .ToString() == "SELECT * FROM sosal WHERE sex > 3 LIMIT 10;");
}

TEST_CASE("test delete construction", "[all]") {
  REQUIRE(DeleteRequest::Delete().ToString() == "DELETE");
  REQUIRE_THROWS(DeleteRequest::Delete().Where("alan > 8"));
  REQUIRE(DeleteRequest::Delete()
              .From("sosal")
              .Where("sex > 3")
              .Limit(10)
              .Finalize()
              .ToString() == "DELETE FROM sosal WHERE sex > 3 LIMIT 10;");
}

TEST_CASE("test insert construction", "[all]") {
  REQUIRE(InsertRequest::Insert("mom").ToString() == "INSERT INTO mom");
  REQUIRE_THROWS(InsertRequest::Insert("mom")
                     .Values(std::vector<std::string>{"1", "ssss", "sfdkf"})
                     .Finalize()
                     .ToString() == "INSERT INTO mom VALUES(1,ssss,sfdkf);");
  REQUIRE(InsertRequest::Insert("mom")
              .Columns(std::vector<std::string>{"number", "goids", "z"})
              .Values(std::vector<std::string>{"1", "ssss", "sfdkf"})
              .Finalize()
              .ToString() ==
          "INSERT INTO mom (number,goids,z) VALUES(1,ssss,sfdkf);");
  REQUIRE_THROWS(InsertRequest::Insert("mom")
                     .Columns(std::vector<std::string>{"number", "goids", "z"})
                     .Values(std::vector<std::string>{"1", "ssss"})
                     .Finalize()
                     .ToString());
}
