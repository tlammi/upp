cmake_minimum_required(VERSION 3.10)
project(upp)

set(COMMON_LIBS pthread gcov)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

add_definitions(--std=c++17 -fprofile-arcs -ftest-coverage)
include_directories("${PROJECT_SOURCE_DIR}/include")


add_subdirectory(unit-test)
