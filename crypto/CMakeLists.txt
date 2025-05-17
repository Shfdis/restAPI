add_library(crypto
        crypto.h
        crypto.cpp)
find_package(OpenSSL REQUIRED)
target_link_libraries(crypto OpenSSL::Crypto)