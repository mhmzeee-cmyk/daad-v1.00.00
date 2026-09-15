cmake_minimum_required(VERSION 3.21)
project(DaadStudio
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "ض استديو: مترجم لغة البرمجة العربية 'ض' إلى C++20"
)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# UTF-8 Support and Cross-Platform Definitions
if(MSVC)
    add_compile_definitions(_WIN32_WINNT=0x0601)
    add_compile_options(/utf-8 /EHsc /W4 /permissive-)
    add_compile_options(/wd4251 /wd4275 /wd4244 /wd4267 /wd4018)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# FetchContent for GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Core Engine (Always built)
add_subdirectory(src)

# Unit Tests (Built by default, can be disabled)
option(DAAD_BUILD_TESTS "Build unit tests" ON)
if(DAAD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# IDE Frontend (Optional: built only if Qt5 is found)
option(DAAD_BUILD_IDE "Build Daad Studio IDE (requires Qt5 5.15+)" OFF)
if(DAAD_BUILD_IDE)
    find_package(Qt5 5.15 QUIET COMPONENTS Core Quick Qml)
    if(Qt5_FOUND)
        message(STATUS "Qt5 found. Building Daad Studio IDE.")
        add_subdirectory(frontend)
    else()
        message(WARNING "Qt5 5.15+ not found. Skipping Daad Studio IDE build.")
    endif()
endif()

# Dhad Bridge (Optional: built only if Qt5 is found)
option(DAAD_BUILD_BRIDGE "Build Dhad Bridge onboarding utility" OFF)
if(DAAD_BUILD_BRIDGE)
    find_package(Qt5 5.15 QUIET COMPONENTS Core Quick Qml Network Widgets)
    if(Qt5_FOUND)
        message(STATUS "Qt5 found. Building Dhad Bridge.")
        add_subdirectory(bridge)
    else()
        message(WARNING "Qt5 5.15+ not found. Skipping Dhad Bridge build.")
    endif()
endif()

# =============================================================================
# CPack Configuration - NSIS Installer
# =============================================================================
#
# ┌─────────────────────────────────────────────────────────────────────┐
# │  Build & Package Instructions (PowerShell / Terminal)             │
# │                                                                   │
# │  Step 1: Configure (one-time)                                     │
# │  ─────────────────────────────                                    │
# │  cd build                                                         │
# │  cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release                  │
# │           -DCMAKE_INSTALL_PREFIX="C:/DaadStudio"                  │
# │           -DDAAD_BUILD_TESTS=OFF                                  │
# │                                                                   │
# │  Step 2: Build                                                    │
# │  ─────────────                                                    │
# │  ninja                                                            │
# │                                                                   │
# │  Step 3: Install to local staging directory                       │
# │  ────────────────────────────────────────────                     │
# │  cmake --install . --prefix ./_install                            │
# │                                                                   │
# │  Step 4: (Optional) Deploy Qt DLLs if IDE was built               │
# │  ─────────────────────────────────────────────────                │
# │  windeployqt.exe --qmldir ../frontend/qml                         │
# │                   ./bin/DaadStudioIDE.exe                         │
# │                                                                   │
# │  5: Generate NSIS installer                                       │
# │     cpack -G NSIS                                                 │
# │                                                                   │
# │  The .exe installer will appear in the build directory.           │
# │                                                                   │
# │  Prerequisites:                                                   │
# │  - NSIS >= 3.0 installed                                          │
# │    https://nsis.sourceforge.io/Download                            │
# │  - Default install path: C:\Program Files (x86)\NSIS              │
# └─────────────────────────────────────────────────────────────────────┘

# Installer metadata
set(CPACK_PACKAGE_NAME "DaadStudio")
set(CPACK_PACKAGE_VENDOR "Daad Studio Team")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Daad Studio - Arabic Programming Language Compiler and IDE")
set(CPACK_PACKAGE_DESCRIPTION
    "A complete development environment for the Daad Arabic programming language, "
    "featuring a compiler (daad-compiler), IDE with Andalusian Manuscript theme, "
    "and VS Code extension."
)
set(CPACK_PACKAGE_CONTACT "https://github.com/daad-studio")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# Icon for Windows installer (optional)
set(DAAD_ICON "${CMAKE_SOURCE_DIR}/assets/daad.ico")
if(EXISTS "${DAAD_ICON}")
    set(CPACK_PACKAGE_ICON "${DAAD_ICON}")
    set(CPACK_WIX_PRODUCT_ICON "${DAAD_ICON}")
endif()

# WIX-specific settings (enable when WIX Toolset is installed)
# set(CPACK_GENERATOR "WIX")
# set(CPACK_WIX_PACKAGE_NAME "DaadStudio")
# set(CPACK_WIX_PRODUCT_NAME "Daad Studio")
# set(CPACK_WIX_MANUFACTURER "Daad Studio Team")
# set(CPACK_WIX_UPGRADE_GUID "E1A2B3C4-D5E6-F789-ABCD-EF0123456789")
# if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#     set(CPACK_WIX_ARCHITECTURE "x64")
# else()
#     set(CPACK_WIX_ARCHITECTURE "x86")
# endif()
# set(CPACK_WIX_PROGRAM_MENU_SHORTCUT "Daad Studio")

# NSIS-specific settings
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_PACKAGE_NAME "DaadStudio")
set(CPACK_NSIS_DISPLAY_NAME "Daad Studio")
set(CPACK_NSIS_MANUFACTURER "Daad Studio Team")
set(CPACK_NSIS_HELP_LINK "https://github.com/daad-studio")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/daad-studio")
set(CPACK_NSIS_CONTACT "https://github.com/daad-studio")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$DESKTOP\\\\Daad Studio.lnk' '$INSTDIR\\\\bin\\\\daad-compiler.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$DESKTOP\\\\Daad Studio.lnk'"
)
set(CPACK_NSIS_MODIFY_PATH ON)

# License display in installer
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/daad-studio")
endif()

# Component-based installation
set(CPACK_COMPONENTS_ALL
    runtime
    headers
    stdlib
    examples
)

set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Daad Compiler")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "The daad-compiler CLI executable")

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "Development Headers")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "C++ header files for the Daad engine (include/Daad/)")
set(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Full Developer)

set(CPACK_COMPONENT_STDLIB_DISPLAY_NAME "Standard Library")
set(CPACK_COMPONENT_STDLIB_DESCRIPTION "Daad standard library headers (Math.hpp, String.hpp)")

set(CPACK_COMPONENT_EXAMPLES_DISPLAY_NAME "Example Programs")
set(CPACK_COMPONENT_EXAMPLES_DESCRIPTION "Sample .daad source files to get started")
set(CPACK_COMPONENT_EXAMPLES_INSTALL_TYPES Full)

# Include CPack at the very end
include(CPack)
