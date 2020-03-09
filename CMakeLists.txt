cmake_minimum_required(VERSION 3.10)
project(upp)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(COMMON_LIBS pthread gcov)

set(CMAKE_CXX_FLAGS "--std=c++17 -Wall -Werror -Wextra -Wno-unused -Wcast-align -Wcast-qual \
    -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self \
    -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast \
    -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion \
    -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wabi -Winline \
    -Wzero-as-null-pointer-constant")

set(CMAKE_CXX_FLAGS_DEBUG "-fprofile-arcs -ftest-coverage")

include_directories("${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE LIB_SRCS src/**.cpp)

add_executable(demo demo.cpp ${LIB_SRCS})
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
add_subdirectory(unit-test)
endif()
