# PHASE_K_WINDOWS10_11_REPORT.md — Windows 10 + Windows 11 Native Release

## 1. Baseline
`7d7e1f5`, tree clean; gates: 47/47 + 102/102 + 11/11 + 125/125 GREEN before any work.
No compiler/CPU/toolchain changes in this phase (packaging + validation only).

## 2. Windows build
Same proven pipeline (no new compiler): `Demo/build-windows.sh` for
`01_hello.ض`, `02_arithmetic.ض`, `06_factorial.ض` → 3× PE32+ static exes.

## 3. PE verification
- All 3: PE32+ x86-64 console (CUI 3, subsystem 5.2), non-zero sizes (~2.4 MB static).
- Successful link (MinGW static: `-static -static-libgcc -static-libstdc++`).
- Import surface: KERNEL32 (core + Vista-era condition variables only) + classic msvcrt CRT.
  No API-set DLLs, no Win8+ APIs, no .NET/VC-redist.
- No hardcoded dev-machine paths (`/home/`, msys, repo names: zero hits in strings).
- No fake/undocumented DLLs. Real documented deps: KERNEL32.dll + msvcrt.dll (both inbox).

## 4. Dependencies
None beyond stock OS DLLs (see §3). Nothing removed to beautify the report.

## 5. Windows 10 result
**WINDOWS 10 BOOT PENDING** — no Wine/VM/Windows runtime exists in this Ubuntu-only
environment (verified: zero `wine*`/`qemu*` binaries). Recorded independently, not merged.

## 6. Windows 11 result
**WINDOWS 11 BOOT PENDING** — same verified absence, recorded independently.

## 7. MSI result
- **REAL MSI**: `release/windows-10-11/DhadStudio-Windows-x64.msi` (1.9 MB) built with `wixl`
  (msitools 0.106): fresh Product/Upgrade GUIDs, v1.1.0, perMachine, ASCII-only properties
  (wixl 0.106 segfaults on non-ASCII Property values — proven in K-W7, respected here).
- Verified: `msiinfo` tables + SummaryInfo; `msiextract` yields all 6 files **byte-identical**
  (sha256 MATCH ×6); extracted exes re-verified PE32+.

## 8. Installer/uninstaller result
**PENDING** real Windows (install/launch/uninstall). The MSI is package-verified;
`validate-windows10.bat` / `validate-windows11.bat` cover post-install acceptance
(each: file-exists + real execution + marker + exit-code checks, explicit PASS/FAIL,
nonzero exit on failure — no fake PASS possible).

## 9. Exact outputs
- exes run on real machines must print: `مرحبا…` / `15` / `120`, exit 0 (script-enforced).
- MSI sha256: `38dd1680…` (full list in `release/windows-10-11/SHA256SUMS.txt`, 8 entries).

## 10. Regression results
47/47 · 11/11 · 102/102 · 125/125 — unchanged (nothing in toolchains was modified).

## 11. Files changed
`release/windows-10-11/` (bin×3, 2 validate scripts, README, SHA256SUMS, .wxs, .msi),
`PHASE_K_WINDOWS10_11_REPORT.md`. No source changes; no new compiler targets
(Win10/11 are OS-acceptance targets of the single `windows-x64` backend, per registry rule).

## 12. Blockers
1. Win10 boot/run (needs machine) → then `validate-windows10.bat` PASS.
2. Win11 boot/run (needs machine) → then `validate-windows11.bat` PASS.
3. MSI install/uninstall on both (needs machines).

## 13. Final target status
`windows-x64`: AVAILABLE / proof=`generated` (unchanged — boot would promote nothing structurally;
per J.5 the registry already separates generated vs runtime; a `runtime` note may be appended
after §12 without any code change).
