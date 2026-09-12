# PHASE6_CPU_RELEASE_AUDIT.md — variants are intentional, keep all

Canonical per use: `dhad_cpu` (8-bit main sim + assembler, libc-only, 96/96), `dhad_gui` (Qt5 IDE), `dhad_live`/`dhad_vis` (terminal UIs), `cpu/desktop/` (ready-to-run folder incl. Windows .exe). Required: src+include+examples+tests+run_tests.sh+qmake file. Qt runtime needed only for GUI. vfat exec-bit caveat for direct-USB execution. Sources for ALL shipped binaries present except Windows .exe rebuild (MinGW, NOT VERIFIED here). Do NOT merge variants.
