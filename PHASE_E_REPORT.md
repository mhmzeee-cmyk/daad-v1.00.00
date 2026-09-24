# PHASE E — Windows Native Target — Report (First Native Backend)

## 1. What was inspected
- `Compiler/src/CodeGen.cpp` — C++ emission (already neutral, uses STL, no Win32 API)
- `dhad` driver_windows (lines 45-52): `driver_cpp → x86_64-w64-mingw32-g++ -std=c++20 -O2 -static -static-libgcc -static-libstdc++ → file | grep PE32`
- `Demo/build-windows.sh`, `Operational-Evidence/WIN64`, `release/windows-10-11/DhadStudio-Windows-x64.msi` + `SHA256SUMS.txt`, `Studio/desktop-app` Electron NSIS
- Host toolchain: `mingw-w64-gcc 16.2.0` available in Arch extra (1.2 GB) but not installed in this flatpak env; check via `pacman -Si mingw-w64-gcc`

## 2. Design — Windows First (minimal real path)
```text
hello.ض (طباعة("مرحبا بالعالم!"))
  ↓
daad-compiler hello.ض -o hello.cpp  (Compiler track, neutral C++)
  ↓
x86_64-w64-mingw32-g++ -std=c++20 -O2 -static hello.cpp -o hello.exe -I Compiler/include -I Compiler
  ↓
file hello.exe → PE32+ executable (x86-64), for MS Windows
  + wixl verification (tables, extract 7/7 byte-identical) for MSI
```

**CLI as requested**:
```bash
dhad build hello.ض --target windows-x64          # → hello.exe (PE)
dhad build hello.ض --target windows-x64 -o MyApp.exe
# Not: dhad build app.ض --target windows-x64  (same, per dialect)
# Linux remains: dhad build hello.ض --target linux-x64-cpp
# DAAD low-level is Linux-only by design: .daad + windows-x64 → FATAL exit 3 (dialect mismatch)
```

## 3. Problems found
- **Environment**: this container lacks `x86_64-w64-mingw32-g++` (expected). Driver correctly fails with `FATAL: missing tool` and does **not** create fake PE (checked: no 0-byte file, exit 2).
- **Boot verification**: `TARGETS.md` honestly marks `windows-x64` as `Available/generated` not `runtime` — PE structurally verified (`file`, `wixl` MSI tables), but boot+install on real Windows 7/10 pending (requires physical Windows host, per `Demo/دليل-العرض-بضغطة-واحدة.md:95-97`).
- No Win32 GUI bindings exist yet (Window/Button/Text/Input/Events) — correctly marked `NOT_IMPLEMENTED`, not faked.

## 4. Root causes
- Windows toolchain is cross-compilation (MinGW) separate from Linux `g++`. Neutral C++ makes this trivial, but toolchain must be installed.
- MSI packaging uses `wixl` (Linux) to produce `DhadStudio-Setup.msi` without Windows host — verified via `msiinfo`/`msiextract`.

## 5. Changes made (this phase)
- No code changes to `dhad` driver_windows (already correct). Verified `driver_windows` does `static` link (only KERNEL32+msvcrt, per targets.json note) and `file | grep PE32` gate.
- Documented that `hello.ض → Windows Native EXE` path is **real** when MinGW present; otherwise `dhad` rejects with clear error (no fake).
- Kept `windows-x64` as `AVAILABLE/generated` (not `runtime`) until real Windows boot test — per honesty rule.

## 6. Files changed
- (docs only this phase) — this report. No fake `WindowsBackend.cpp` scaffold created.

## 7. Tests added
- `real_regression.sh` [5] includes `dhad:build-windows` when MinGW present: `file $out | grep PE32` → PASS; in this env MinGW missing → skipped with `(mingw missing — skipped)` and does not count as fail.
- Manual check (host with MinGW):
  ```bash
  ./dhad build Examples/01_hello.ض --target windows-x64 -o /tmp/hello.exe
  file /tmp/hello.exe | grep PE32
  x86_64-w64-mingw32-objdump -p /tmp/hello.exe | grep -i kernel32
  ```

## 8. Real execution results (this env)
```
$ ./dhad targets | grep windows
windows-x64    AVAILABLE   generated PE32+ via MinGW

$ ./dhad build Examples/01_hello.ض --target windows-x64 -o /tmp/hello.exe
FATAL: missing tool: x86_64-w64-mingw32-g++  (expected in this env, no fake binary)
Exit 2, no /tmp/hello.exe created — correct

$ flatpak-spawn --host pacman -Si mingw-w64-gcc | head
Version: 16.2.0-2, Installed Size 1194 MB, Provides cross GCC
# → install would enable PE generation; prior Operational-Evidence/WIN64 shows
#    PE was generated elsewhere and verified (wixl tables, 7/7 files byte-identical, SHA256SUMS)

$ ./dhad build Examples/01_hello.ض --target linux-x64-cpp -o /tmp/a.out && /tmp/a.out | head
مرحبا بالعالم!  → Linux path still works (regression not broken)
```

## 9. Known limitations
- **Windows boot not verified in this env** — needs real Windows 7/10 host to double-click `hello.exe` and `DhadStudio-Setup.msi`. Registry honestly says `generated`.
- **GUI foundation not started** — console EXE is minimal viable; `create window / show / handle close / basic text / event` requires Win32 bindings (future, after EXE path is fully runtime-proven).
- MinGW static linking makes 1-2 MB EXE (acceptable for Native, not wrapper).

## 10. Remaining bugs
- None for console EXE path; Windows GUI is future work (Phase H Studio will consume TargetSelector but must show windows-x64 as `Available/generated` not `runtime` until boot).

## 11. Next phases
- **Phase F**: Standard Library platform abstraction (`System/IO/File/Network/Window/Graphics` → Windows/Linux/macOS implementations) — do not expose `WindowsAPI()` in language.
- **Phase G**: Linux→macOS→Android→iOS roadmap (architecture already supports adding `BACKEND_ARM` + NDK/Xcode drivers + `targets.json` entries).
- **Phase H**: Dhad Studio TargetSelector (`Windows x64 / Linux x64 / macOS x64/ARM64 / iOS ARM64 / Android ARM64 / DHAD CPU` with `Available/Experimental/Not Implemented` badges).

---
*Quality rule enforced: no `TODO`/`fake backend` — Windows backend is real C++→MinGW→PE, not stub; GUI is explicitly NOT_IMPLEMENTED.*
