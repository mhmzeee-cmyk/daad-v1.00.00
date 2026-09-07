# ═══════════════════════════════════════════════════════════════
#  مشروع المحاكي البصري — معالج ض (Qt GUI)
# ═══════════════════════════════════════════════════════════════

QT       += core gui widgets
TARGET    = dhad_gui
TEMPLATE  = app
CONFIG   += c++17

SOURCES  += gui/main.cpp src/dhad_asm.c
HEADERS  += include/dhad_asm.h
INCLUDEPATH += include

# FLAGS
QMAKE_CXXFLAGS += -Wall -Wextra
