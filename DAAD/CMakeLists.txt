cmake_minimum_required(VERSION 3.10)
project(DAAD VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/compiler)

# Library sources (everything except main.c and tests)
file(GLOB_RECURSE LIB_SOURCES
    "compiler/token/*.c"
    "compiler/lexer/*.c"
    "compiler/parser/*.c"
    "compiler/ast/*.c"
    "compiler/semantic/*.c"
    "compiler/ir/*.c"
    "compiler/optimizer/*.c"
    "compiler/cfg/*.c"
    "compiler/ssa/*.c"
    "compiler/codegen/*.c"
    "compiler/backend/*.c"
)

# Remove test files from lib sources
list(FILTER LIB_SOURCES EXCLUDE REGEX "tests/")

# Create static library
add_library(daad_lib STATIC ${LIB_SOURCES})
target_link_libraries(daad_lib m)

# Main compiler executable
add_executable(daad compiler/main.c)
target_link_libraries(daad daad_lib m)

# Test executables
file(GLOB TEST_SOURCES "compiler/tests/test_*.c")
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} daad_lib m)
endforeach()

# Install targets
install(TARGETS daad RUNTIME DESTINATION bin)

# Register tests with CTest
enable_testing()
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
