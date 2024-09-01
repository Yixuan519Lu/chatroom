# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(ChatServerClient)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the source directory
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Add the executable for the server
add_executable(server ${SRC_DIR}/server.cpp)

# Add the executable for the client
add_executable(client ${SRC_DIR}/client.cpp)

# Include the config.h header
target_include_directories(server PRIVATE ${SRC_DIR})
target_include_directories(client PRIVATE ${SRC_DIR})

# Link pthread library for multithreading
target_link_libraries(server pthread)
target_link_libraries(client pthread)
