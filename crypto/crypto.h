//
// Created by shfdis on 5/16/25.
//

#ifndef CRYPTO_H
#define CRYPTO_H
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <boost/uuid.hpp>
std::string GenerateUuid();

std::string sha256(const std::string& input);
std::string GenerateHash(const std::string& password);
// Verify password against stored hash & salt
bool VerifyHash(const std::string& password, const std::string& stored_hash);

#endif  // CRYPTO_H
