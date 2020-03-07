
file(GLOB_RECURSE UNIT_TEST_SRCS *.cpp)

add_executable(unit-test EXCLUDE_FROM_ALL ${UNIT_TEST_SRCS})
target_link_libraries(unit-test gtest ${COMMON_LIBS})
