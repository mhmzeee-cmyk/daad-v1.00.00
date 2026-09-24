cmake_minimum_required(VERSION 3.16)
project(DhadBridge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Core Quick Qml Network Widgets)

set(BRIDGE_SOURCES
    main.cpp
    SchoolOnboarder.cpp
)

set(BRIDGE_RESOURCES
    bridge.qrc
)

qt5_add_resources(BRIDGE_RC ${BRIDGE_RESOURCES})

add_executable(DhadBridge
    ${BRIDGE_SOURCES}
    ${BRIDGE_RC}
)

target_link_libraries(DhadBridge PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Qml
    Qt5::Network
    Qt5::Widgets
)

target_include_directories(DhadBridge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS DhadBridge RUNTIME DESTINATION bin)
