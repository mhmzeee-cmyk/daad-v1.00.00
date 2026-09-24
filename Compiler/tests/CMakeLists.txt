# Tests CMake — Unit tests for Compiler Track A
cmake_minimum_required(VERSION 3.21)

# Gather all test sources
file(GLOB TEST_SOURCES "test_*.cpp")

add_executable(DaadTests ${TEST_SOURCES})

target_link_libraries(DaadTests PRIVATE DaadCore GTest::gtest_main)

target_include_directories(DaadTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(DaadTests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Enable warnings for tests as well
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(DaadTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GoogleTest)
gtest_discover_tests(DaadTests)
