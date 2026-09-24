#############################################################################
# Makefile for Dhad Processor — builds dhad_cpu + dhad_asm + dhad_gui
#############################################################################

CC       = gcc
CXX      = g++
CFLAGS   = -pipe -O2 -Wall -Wextra -D_REENTRANT -fPIC -std=c11
CXXFLAGS = -pipe -Wall -Wextra -O2 -std=gnu++1z -D_REENTRANT -fPIC
INCPATH  = -Iinclude

# Qt flags (only for dhad_gui)
QT_INC   = -I/usr/include/x86_64-linux-gnu/qt5 \
           -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
           -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
           -I/usr/include/x86_64-linux-gnu/qt5/QtCore
QT_LIBS  = /usr/lib/x86_64-linux-gnu/libQt5Widgets.so \
           /usr/lib/x86_64-linux-gnu/libQt5Gui.so \
           /usr/lib/x86_64-linux-gnu/libQt5Core.so -lGL -lpthread

MOC      = /usr/lib/qt5/bin/moc
MOC_DEFS = -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB

DEL_FILE = rm -f

####### Default target

all: dhad_cpu dhad_asm

gui: dhad_gui

# ──────────────────────────────────────
#  dhad_cpu: CPU Simulator + Assembler + Debugger (CLI)
#  Sources: main.c + dhad_debug.c
#  Note: main.c has its own inline CPU+ASM; does NOT link dhad_cpu.c
# ──────────────────────────────────────

dhad_cpu: src/main.c src/dhad_debug.c include/dhad_debug.h
	$(CC) $(CFLAGS) $(INCPATH) -o $@ src/main.c src/dhad_debug.c -lm

# ──────────────────────────────────────
#  dhad_asm: Standalone Assembler
#  Sources: asm_main.c + dhad_asm.c
# ──────────────────────────────────────

dhad_asm: src/asm_main.c src/dhad_asm.c include/dhad_asm.h
	$(CC) $(CFLAGS) $(INCPATH) -o $@ src/asm_main.c src/dhad_asm.c

# ──────────────────────────────────────
#  dhad_gui: Qt GUI Application
#  Sources: gui/main.cpp + dhad_asm.c + dhad_cpu.c
# ──────────────────────────────────────

moc_predefs.h: /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp
	g++ -pipe -Wall -Wextra -O2 -std=gnu++1z -dM -E -o $@ $<

main.moc: gui/main.cpp moc_predefs.h include/dhad_asm.h include/dhad_cpu.h
	$(MOC) $(MOC_DEFS) --include $(CURDIR)/moc_predefs.h \
		$(QT_INC) $(INCPATH) -o $@ gui/main.cpp

dhad_gui: gui/main.cpp src/dhad_asm.c src/dhad_cpu.c include/dhad_asm.h include/dhad_cpu.h main.moc
	$(CXX) $(CXXFLAGS) $(INCPATH) -I. $(QT_INC) -o $@ gui/main.cpp src/dhad_asm.c src/dhad_cpu.c $(QT_LIBS)

# ──────────────────────────────────────
#  Clean
# ──────────────────────────────────────

clean:
	-$(DEL_FILE) dhad_cpu dhad_asm dhad_gui
	-$(DEL_FILE) main.moc moc_predefs.h
	-$(DEL_FILE) *~ core *.core

FORCE:
