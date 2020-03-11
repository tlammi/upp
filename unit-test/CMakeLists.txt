
file(GLOB_RECURSE UNIT_TEST_SRCS *.cpp)

add_executable(unit-test EXCLUDE_FROM_ALL ${UNIT_TEST_SRCS})
target_link_libraries(unit-test upp gtest gmock ${COMMON_LIBS})

add_custom_target(run-unit-test COMMAND $<TARGET_FILE:unit-test> DEPENDS unit-test)
add_custom_target(
    coverage-html
    COMMAND mkdir -p cov &&
        python -m gcovr ${CMAKE_CURRENT_BINARY_DIR}
            --html --html-details --exclude "unit-test" -o cov/upp.html DEPENDS run-unit-test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})