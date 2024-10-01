cmake_minimum_required(VERSION 3.30)

project(client-server VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++23")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_CXX_FLAGS})

# add_executable(client client.cpp)
add_executable(server server.cpp)
