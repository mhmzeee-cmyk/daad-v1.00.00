cmake_minimum_required(VERSION 3.16)
project(DaadStudioIDE VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Core Quick Qml Concurrent Network)

if(TARGET DaadCore)
else()
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src" "${CMAKE_BINARY_DIR}/DaadCore-build")
endif()

set(PROJECT_SOURCES
    cpp/main.cpp
    cpp/CompilerWrapper.cpp
    cpp/SyntaxHighlighter.cpp
    cpp/Translator.cpp
    cpp/CppToDaadTranspiler.cpp
    cpp/GeminiBackend.cpp
    cpp/FileSystemModel.cpp
    cpp/WidgetNode.cpp
    cpp/GuiDesignerModel.cpp
    cpp/CloudNetworkService.cpp
    cpp/ChallengeManager.cpp
    cpp/MockDataLoader.cpp
)

set(RESOURCES
    qml_resources.qrc
)

qt5_add_resources(RESOURCE_FILES ${RESOURCES})

add_executable(DaadStudioIDE
    ${PROJECT_SOURCES}
    ${RESOURCE_FILES}
)

target_link_libraries(DaadStudioIDE PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Qml
    Qt5::Concurrent
    Qt5::Network
    DaadCore
)

target_include_directories(DaadStudioIDE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)

if(WIN32)
    set(QML_DEPLOY_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/deploy_qml.cmake")
    add_custom_command(TARGET DaadStudioIDE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Deploying QML modules and MinGW runtime..."
        COMMAND ${CMAKE_COMMAND} -P "${QML_DEPLOY_SCRIPT}" "$<TARGET_FILE_DIR:DaadStudioIDE>"
        COMMENT "Deploying QML modules..."
    )
endif()

install(TARGETS DaadStudioIDE
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
