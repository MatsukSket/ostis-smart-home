cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
project(ostis-example-app VERSION "0.10.0")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(sc-machine REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/problem-solver)
