cmake_minimum_required(VERSION 3.31)
project(restAPI)
set(CMAKE_CXX_STANDARD 20)
# Find dependencies
find_package(Boost 1.66 REQUIRED COMPONENTS system thread json)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(restAPI main.cpp)

# Add subdirectories (make sure they export their targets properly)
add_subdirectory(network_handler)
add_subdirectory(tests)
add_subdirectory(db)
add_subdirectory(crypto)
add_subdirectory(src)

# Link libraries using modern CMake targets
target_link_libraries(restAPI PRIVATE
        Boost::system
        Boost::thread
        Boost::json
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
        network_handler
        db
        messenger
)