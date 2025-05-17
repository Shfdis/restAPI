#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <iostream>

#include "../crypto/crypto.h"
TEST_CASE("crypto") {
  std::string password = "psdgushdfsvvdfhgj";
  std::string hash;
  REQUIRE_NOTHROW(hash = GenerateHash(password));
  REQUIRE(VerifyHash(password, hash));
  password = "17817990";
  REQUIRE(!VerifyHash(password, hash));

  REQUIRE(GenerateUuid().size() == 36);
  std::cerr << GenerateUuid() << std::endl;
}