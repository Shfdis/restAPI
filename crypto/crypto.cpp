//
// Created by shfdis on 5/16/25.
//
#include "crypto.h"
std::string GenerateUuid() {
  boost::uuids::random_generator gen;
  boost::uuids::uuid uuid = gen();
  return boost::uuids::to_string(uuid);
}

std::string sha256(const std::string& input) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, input.c_str(), input.size());
  SHA256_Final(hash, &sha256);

  std::stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(hash[i]);
  }
  return ss.str();
}

std::string GenerateHash(const std::string& password) {
  return sha256(password);
}

bool VerifyHash(const std::string& password, const std::string& stored_hash) {
  std::string computed_hash = sha256(password);
  return computed_hash == stored_hash;
}
