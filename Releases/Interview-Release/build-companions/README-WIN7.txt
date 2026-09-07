Dhad Studio v1.0.0 - Windows 7 Interview Demo
================================================

QUICK START (no internet, no installs beyond the MSI):
1. Run DhadStudio-Setup-Win7.msi and complete the wizard.
2. Start Menu -> Dhad Studio -> Dhad Studio Demo
   (or Desktop icon "Dhad Studio Demo").
3. Choose 1 to COMPILE hello live (.ض -> .cpp).
4. Choose 2 to RUN hello.exe and see Arabic output.
5. Choose 3 to run all 7 demo programs.
6. Choose 4 for the CPU assembler/simulator demo.

WHAT IS INCLUDED:
- bin\daad-compiler.exe : Dhad language compiler (Arabic -> C++20)
- bin\*.exe             : 7 prebuilt demo programs (Arabic output)
- bin\dhad_cpu.exe      : Arabic CPU assembler + simulator (8-bit)
- examples\*.ض          : Dhad source files shown to the committee
- examples\*.cpp        : generated C++ (also produced live by choice 1)
- cpu-examples\*.ضasm   : CPU assembly examples

REQUIREMENTS: Windows 7 SP1 x64 or newer. No admin rights needed
beyond a normal per-machine install. No internet. No Node, no Qt.

SOURCE CODE: see Source\ folder next to the MSI (full project source).
UNINSTALL: Control Panel -> Programs -> Dhad Studio -> Uninstall
(or re-run the MSI and choose Remove).
