cmake_minimum_required(VERSION 3.30)

project(showip VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++23")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_CXX_FLAGS})

add_executable(showip main.cpp)
