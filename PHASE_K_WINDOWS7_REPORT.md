# PHASE_K_WINDOWS7_REPORT.md — Windows 7 Native Acceptance + MSI

## 1. Baseline
`6f8cf89`, tree clean; J.0 gates re-run: 47/47 + 102/102 + 11/11 + 125/125 GREEN. No compiler/CPU
changes in this phase (build-only + packaging + one validation script).

## 2. Files changed
- `release/windows-7/`: `bin/{hello,arithmetic,factorial,loops,functions}.exe` (fresh cross-builds
  from current Track-A sources), `validate-windows7.bat` (new), `DhadStudio.wxs` (new),
  `DhadStudio-Windows7.msi` (new, built), `README-WINDOWS7.txt` (new), `SHA256SUMS.txt` (new).
- `Docs/Reference/targets.json`: windows-x64 `proof_detail` extended (MSI verified).
- No source-code changes; `./dhad`, `Demo/build-windows.sh` untouched.

## 3. Build commands
```bash
bash Demo/build-windows.sh Examples/01_hello.ض      release/windows-7/bin/hello.exe
bash Demo/build-windows.sh Examples/02_arithmetic.ض release/windows-7/bin/arithmetic.exe
bash Demo/build-windows.sh Examples/06_factorial.ض  release/windows-7/bin/factorial.exe
bash Demo/build-windows.sh Examples/04_loops.ض      release/windows-7/bin/loops.exe
bash Demo/build-windows.sh Examples/05_functions.ض  release/windows-7/bin/functions.exe
wixl -D PayloadDir=release/windows-7 -o release/windows-7/DhadStudio-Windows7.msi release/windows-7/DhadStudio.wxs
```

## 4. PE verification
- All 5: **PE32+ x86-64 console (CUI, subsystem 5.2)**, static link.
- Imports: KERNEL32 only (CloseHandle, CriticalSections, FormatMessageA, GetLastError,
  GetModuleHandleA/GetProcAddress, GetSystemTimeAsFileTime, GetThreadId, condition variables,
  LoadLibraryW, LocalFree, MultiByteToWideChar, RaiseException, Rtl*Unwind/Capture,
  Sleep*, TLS, VirtualProtect/Query, WideCharToMultiByte) + classic msvcrt CRT.
- **Every imported API exists on Windows 7** (newest used: Vista-era condition variables;
  no API-sets, no Win8+ APIs). Min subsystem/OS headers target the 5.x era.
- Honesty note: API presence ≠ boot proof. Status stays **PE VERIFIED**, not runtime.

## 5. Windows 7 real-machine result
**WINDOWS 7 BOOT PENDING REAL MACHINE.** No Wine/QEMU/VirtualBox/VMware exists in this
Ubuntu-only environment (verified by tool search). `validate-windows7.bat` is written for the
real machine: runs all 5 exes, checks ASCII markers (`مرحبا` via findstr + `15/120/5050/26`)
and exit code 0 each, prints explicit PASS/FAIL + totals, exits nonzero on any failure.
No fake PASS possible (every check executes its binary first).

## 6. MSI result
- **REAL MSI BUILT** with `wixl` 0.106 (msitools): `DhadStudio-Windows7.msi` 3.2 MB.
- Verified: `msiinfo` tables + SummaryInfo (v1.0.1, perMachine); `msiextract` yields all
  7 files **byte-identical** (sha256 MATCH ×7); extracted exes re-verified PE32+.
- Found + fixed during packaging: **non-ASCII (Arabic `.ض`) inside the ARPCOMMENTS Property
  segfaults wixl 0.106** (libmsi commit). Property kept ASCII-only; Arabic lives in
  README/filenames (proven safe: multi-file components, subdir sources, nested dirs all build).
- Install/launch/uninstall on real Windows: **PENDING** (no Windows here) — the MSI is
  *package-verified*, and `validate-windows7.bat` covers post-install acceptance.

## 7. Exact outputs
- exes: PE32+ ×5 (2.4–2.5 MB static each); expected on-machine outputs: `مرحبا…/15/120/5050/26`, exit 0.
- MSI: 3229696 bytes, sha256 in `SHA256SUMS.txt` (9 entries: msi+5 exes+bat+readme+wxs).

## 8. Dependency analysis
Zero third-party runtime deps: KERNEL32.dll + msvcrt.dll (both inbox since Windows 2000/XP).
No libgcc/libstdc++ DLLs (static), no .NET, no VC redist. See §4 for per-API verdict.

## 9. Regression results
47/47 · 11/11 · 102/102 · 125/125 — unchanged (no toolchain/source modifications).

## 10. Remaining blockers
1. Real Win7 boot + `validate-windows7.bat` run + MSI install/launch/uninstall (needs machine).
2. Then flip registry `proof` to `runtime` for windows-x64.
STOP here per instructions — no macOS/Android/iOS started.
