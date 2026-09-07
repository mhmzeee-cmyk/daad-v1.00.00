# Core Library: DaadCore
add_library(DaadCore STATIC
    Lexer.cpp
    Token.cpp
    Keywords.cpp
    UnicodeUtils.cpp
    AST.cpp
    Parser.cpp
    CodeGen.cpp
    Compiler.cpp
    Optimizer.cpp
    Diagnostics.cpp
    SandboxValidator.cpp
)

target_include_directories(DaadCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/stdlib
)

set_target_properties(DaadCore PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# CLI Tool
add_executable(daad-compiler main.cpp)
target_link_libraries(daad-compiler PRIVATE DaadCore)
set_target_properties(daad-compiler PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install rules with component assignments
install(TARGETS daad-compiler
    RUNTIME DESTINATION bin
    COMPONENT runtime
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/Daad
    DESTINATION include
    COMPONENT headers
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/stdlib/
    DESTINATION include/stdlib
    COMPONENT stdlib
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/examples/
    DESTINATION share/daad/examples
    COMPONENT examples
)
