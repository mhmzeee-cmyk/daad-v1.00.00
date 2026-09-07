# PHASE6_BRIDGE_RELEASE_AUDIT.md — Qt tool requirements

Qt 5.15 modules: Core, Quick, Qml, Network, Widgets. Sources: main.cpp, SchoolOnboarder.*, bridge_main.qml, bridge.qrc. QML imports QtQuick 2.15/Controls/Layouts. Default server http://localhost:3000 (in-app editable). Rebuild proven 3× (cmake 0/build 0) → binary reproducible on Qt5 Linux. Target needs Qt5 runtime libs + network to server. Tree `DhadBridge` binary STALE (pre-main.cpp) → refresh at packaging, never ship stale. No secrets in sources.
