cmake_minimum_required(VERSION 3.23)

project(discrete-math-bits)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(discrete-math-bits)

target_sources(discrete-math-bits
  PRIVATE
    src/problems/111.cpp

  PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
    BASE_DIRS include
)
