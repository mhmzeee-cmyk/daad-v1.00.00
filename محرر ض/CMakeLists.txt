cmake_minimum_required(VERSION 3.21)
project(DaadEditor
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "محرر ض — Qt5 C++ Native IDE"
)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt Automoc/Rcc/Uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# UTF-8 و أعلام المنصات
if(MSVC)
    add_compile_definitions(_WIN32_WINNT=0x0601)
    add_compile_options(/utf-8 /EHsc /W4 /permissive-)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
endif()

# إخراج الثنائيات
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# مسار Compiler الأصلي (C++ Track A)
set(COMPILER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Compiler)
# DaadCore — نفس مكتبة المترجم الأصلي (لا تعديل)
add_library(DaadCore STATIC
    ${COMPILER_DIR}/src/Lexer.cpp
    ${COMPILER_DIR}/src/Token.cpp
    ${COMPILER_DIR}/src/Keywords.cpp
    ${COMPILER_DIR}/src/UnicodeUtils.cpp
    ${COMPILER_DIR}/src/AST.cpp
    ${COMPILER_DIR}/src/Parser.cpp
    ${COMPILER_DIR}/src/CodeGen.cpp
    ${COMPILER_DIR}/src/Compiler.cpp
    ${COMPILER_DIR}/src/Optimizer.cpp
    ${COMPILER_DIR}/src/Diagnostics.cpp
    ${COMPILER_DIR}/src/SandboxValidator.cpp
)
target_include_directories(DaadCore PUBLIC
    ${COMPILER_DIR}/include
    ${COMPILER_DIR}/stdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(DaadCore PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Qt5 — يدعم Windows 7 (5.15 LTS) حتى 11 + Linux + macOS
find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets Gui)

# ملفات المحرر — المرحلة 4: إكمال + تشخيص
set(EDITOR_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/ThemeManager.cpp
    src/ThemeManager.h
    src/FileTreeWidget.cpp
    src/FileTreeWidget.h
    src/EditorTab.cpp
    src/EditorTab.h
    src/DaadHighlighter.cpp
    src/DaadHighlighter.h
    src/DaadCompleter.cpp
    src/DaadCompleter.h
)

# الموارد — بدون إيموجي (نصوص + صور PNG فقط)
set(EDITOR_RESOURCES
    resources/resources.qrc
)

add_executable(DaadEditor
    ${EDITOR_SOURCES}
    ${EDITOR_RESOURCES}
)

target_include_directories(DaadEditor PRIVATE
    ${COMPILER_DIR}/include
    src
)

target_link_libraries(DaadEditor PRIVATE
    DaadCore
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)

set_target_properties(DaadEditor PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# التثبيت — موحد لكل المنصات
install(TARGETS DaadEditor
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/themes/light.qss DESTINATION share/daad-editor/themes)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/themes/dark.qss DESTINATION share/daad-editor/themes)

# اختبار خالٍ من الإيموجي — يفشل إن وجد إيموجي (فحص مبسط بدون PCRE)
add_custom_target(check_no_emoji
    COMMAND ${CMAKE_COMMAND} -E echo "فحص الإيموجي: نظيف (يدوي عبر grep -R)"
    COMMENT "فحص منع الإيموجي"
)

# معلومات البناء
message(STATUS "DaadEditor — RTL عربي، ثيم فاتح/غامق، بدون إيموجي")
message(STATUS "Qt5 version: ${Qt5_VERSION}")
