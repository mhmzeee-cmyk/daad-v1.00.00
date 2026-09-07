# Dhad Studio Windows 7 Interview Release — FINAL REPORT (build side)

## Windows 7 Environment
- Build host: Linux x86_64 (MinGW-GCC 13 cross-toolchain). Target: Windows 7 SP1 x64.
- Test machine: user's physical Windows 7 (this report covers everything UP TO the user test; install/run verdicts below are PENDING-USER-TEST).

## Architecture (what ships)
- `DhadStudio-Setup-Win7.msi` (5.5MB, MSI schema v200, per-machine): 9 static console exes + demo launcher + README + 7 generated .cpp + Start-Menu/Desktop shortcuts. Arabic example files are MATERIALIZED at first launch (wixl cannot store non-ASCII filenames — proven by 4 controlled experiments; content is byte-identical, verified 13/13 round-trips).
- Release folder `Dhad-Studio-Interview-Release/`: MSI + Source/ (Daad-Compiler + cpu src/include/gui) + Examples/ (.ض+.cpp+.ضasm) + Documentation/ + SHA256SUMS.txt (332 entries).

## Components Tested (Linux side)
- Cross-compile daad-compiler.exe: PE32+ x64, imports ONLY KERNEL32/msvcrt (no MinGW DLLs needed — old tree exes needed libstdc++-6.dll which was never shipped).
- 7 demo exes + dhad_cpu.exe: same portable profile; logic proven by running identical generated code on Linux (hello/arithmetic/conditions/loops/functions/factorial-120/fibonacci + CPU 96/96 suite).
- MSI structure: 35 tables, 18/18 files linked, 2 shortcuts (menu+desktop → launcher, workdir set), validated with msitools.

## MSI Build / Installation / Launch / Compiler / hello.ض / Runtime Deps / Uninstall
- Build: PASS (wixl 0.106, deterministic GUIDs). Install/launch/demo/uninstall: PENDING-USER-TEST (no Windows here; Wine deliberately not used as proof).

## Source Code / SHA256 / Known Limitations
- Source/ beside MSI ✓. SHA256SUMS ✓. Limits: exes untested on Windows yet; launcher .bat untested (logic reviewed + blob pipeline verified); no g++ on target needed (prebuilt exes); live-compile needs only the bundled daad-compiler.exe.

## Classification: A=compiler+demos+cpu-console (static, Win7-safe APIs) · B=old DLL-dependent exes (superseded, not shipped) · C=Electron/Node/Qt-bridge/VSCode-host/Server (need Win10+ or Qt/Node runtimes — excluded from Win7 MSI) · D=web-only/seeds (not needed).

## Exact Interview Demo Steps
1. Install MSI → Start Menu → Dhad Studio Demo. 2. Choice 1: live-compile hello.ض→hello_live.cpp (show the generated C++). 3. Choice 2: run hello.exe (Arabic output). 4. Choice 3: all 7 programs. 5. Choice 4: CPU demo. 6. Open Source\Daad-Compiler\src to explain Lexer→Parser→AST→Optimizer→CodeGen→SandboxValidator.

MSI STATUS: BUILT+STRUCTURE-VALIDATED (install UNTESTED here)
WINDOWS 7 INSTALL: PENDING-USER-TEST
DHAD STUDIO: N/A on Win7 (Electron/Node are Win10+; demo = compiler+CPU console set)
COMPILER: BUILT (run PENDING-USER-TEST)
HELLO.ض: READY (source+prebuilt+live-compile path)
OVERALL: CONDITIONALLY READY (one user test session away from READY)
