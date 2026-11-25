cmake_minimum_required(VERSION 3.23)

project(discrete-math-bits)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(discrete-math-bits)

target_sources(discrete-math-bits
  PRIVATE
    src/main.cpp
    src/problems/109.cpp
    src/problems/110.cpp
    src/problems/111.cpp
)

target_include_directories(discrete-math-bits
  PRIVATE
    include
)
