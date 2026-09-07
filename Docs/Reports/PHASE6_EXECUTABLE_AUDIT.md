# PHASE6_EXECUTABLE_AUDIT.md — every binary in tree (none deleted)

| Binary | Type | Origin | Reproducible? | Required? |
|---|---|---|---|---|
| windows-x64/bin/daad-compiler.exe | PE32+ x64 console, MinGW fam. | prebuilt, strings match current CodeGen | Windows rebuild NOT VERIFIED (no Win toolchain) → RISK | YES (release bundle) |
| Daad-Compiler/daad-compiler.exe | PE32+ x64, MinGW, DIFFERENT hash | prebuilt, unreferenced | same RISK | UNKNOWN (keep) |
| cpu/desktop/dhad_cpu.exe | PE32+ x64 console, MinGW | prebuilt | same RISK | distro convenience |
| cpu/dhad_cpu,live,vis | ELF x64, libc-only | sources present; behavior proven 96/96 | YES (gcc) | dev/dist |
| cpu/dhad_gui | ELF x64, Qt5Widgets/Gui/Core | gui/main.cpp+qmake | YES (Qt5 dev) | GUI distro |
| bridge/DhadBridge | ELF x64 | STALE (predates main.cpp fix) | YES (rebuilt clean 3× in /tmp) → tree copy must be refreshed at packaging | NO (stale) |
| root DLL 5.9M | PE32 i386 DLL, std imports | unknown | NO | UNKNOWN (never run) |

No .so/.dylib. No `dll` needed on Linux paths. vfat: exec bit unavailable → release media/process must restore +x for ELF/sh (documented Phase 3).
