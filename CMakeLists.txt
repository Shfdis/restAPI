cmake_minimum_required(VERSION 3.31)
project(restAPI)
set(CMAKE_CXX_STANDARD 20)
add_executable(restAPI
        main.cpp
)
add_subdirectory(network_handler)
add_subdirectory(tests)
add_subdirectory(db)
find_package(Boost 1.66 REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(restAPI PRIVATE
        Boost::system
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
)
target_link_libraries(restAPI PUBLIC network_handler)
target_link_libraries(restAPI PUBLIC db)
