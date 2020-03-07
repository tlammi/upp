cmake_minimum_required(VERSION 3.10)
project(upp)

set(COMMON_LIBS pthread)

add_definitions(--std=c++17)
include_directories("${PROJECT_SOURCE_DIR}/include")


add_subdirectory(unit-test)
