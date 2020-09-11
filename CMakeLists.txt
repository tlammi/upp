cmake_minimum_required(VERSION 3.10)
project(upp)


set(COMMON_LIBS pthread gcov)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS "--std=c++17 -Wall -Werror -Wextra -Wno-unused -Wcast-align -Wcast-qual \
#    -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self \
#    -Wmissing-declarations -Wmissing-include-dirs \
#    -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion \
#    -Wsign-promo -Wstrict-overflow=5 -Wundef -Wabi -Winline \
#    -Wzero-as-null-pointer-constant")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -g")

file(GLOB_RECURSE LIB_SRCS src/**.cpp)
add_library(upp ${LIB_SRCS})
target_include_directories(upp PUBLIC include)

add_subdirectory(unit-test EXCLUDE_FROM_ALL)
add_subdirectory(samples EXCLUDE_FROM_ALL)
