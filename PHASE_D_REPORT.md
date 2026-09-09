# PHASE_D_REPORT.md — Target / Backend Architecture

## 1. What was inspected
- `DAAD/compiler/backend/backend_interface.h` (+`.c`): `BackendTarget` enum, `Backend` struct, per-target constructors.
- `DAAD/compiler/main.c` CLI: only `--target=x86|dhad` exposed.
- `Compiler/` Track A: emits standard C++20 (no platform constructs; platform selected by the C++ toolchain, not the compiler).
- Available toolchains in this environment: `gcc` (Linux ELF x86-64), `x86_64-w64-mingw32-*` (Windows PE x86-64). No Apple/Android toolchains → no mobile backends can be honestly built here.

## 2. Problems found
1. ARM64/RISC-V constructors exist with `emit_function = NULL` — correctly NOT exposed via CLI (no fake backend; keep it that way).
2. No machine-readable target registry: supported targets live only in code + docs. Risk of Studio showing unimplemented targets as available.
3. Track A has no "target" concept at all (correct by construction — C++ is neutral — but undocumented, causing confusion like "does daad-compiler target Windows?").

## 3. Root causes
Organic growth: each track grew its own output path; nobody wrote the mapping layer. No code defect — missing architecture document + registry.

## 4. Changes made
Design only (no backend code added — per rule: no fake backends):
- This report + `Docs/Reference/TARGETS.md` (registry with Available/Experimental/Not-Implemented).

## 5. Design (uses the project's own vocabulary: `Backend`, `BackendTarget`)

```text
                    ┌─ Track A (.ض) ── C++20 (neutral) ──┬─► gcc    ──► Linux ELF
                    │                                    ├─► mingw  ──► Windows EXE   (PHASE E)
                    │                                    ├─► clang  ──► macOS Mach-O  (needs Apple toolchain)
                    │                                    └─► NDK/Xcode ─► Android/iOS (needs SDKs)
  ض source ─────────┤
                    └─ Track B (.daad) ── DAAD IR ──┬─► BACKEND_X86   ──► Linux ELF     (Available)
                                                    ├─► BACKEND_DHAD  ──► .ضasm/.bin   (Available, educational)
                                                    ├─► BACKEND_ARM   ──► scaffold NULL (Not Implemented)
                                                    └─► BACKEND_RISCV ──► scaffold NULL (Not Implemented)
```

### Target descriptor (conceptual; field names follow existing code where present)
```text
Target { id, OS, Arch, ABI, ObjectFormat, BinaryFormat,
         Frontend(TrackA|TrackB), Backend(constructor or toolchain),
         Runtime, Linker, Packager, Status }
```

### Status table (ground truth, verified)
| Target ID | Path | Status | Evidence |
|---|---|---|---|
| `linux-x64` | Track B → BACKEND_X86 → gcc → ELF | **Available** | 25/25 regression incl. recursion+float |
| `windows-x64` | Track A → C++ → mingw/MSVC → PE/EXE | **Available** | PHASE E (real PE built+verified here) |
| `dhad-cpu` | Track B → BACKEND_DHAD → asm → .bin | **Available** | DHAD loop15/fact120, CPU 96/96 |
| `macos-x64/arm64` | Track A → C++ → Apple clang | **Not Implemented** | no Apple toolchain in repo/env |
| `android-arm64` | Track A → C++ → NDK / Track B → BACKEND_ARM | **Not Implemented** | scaffold NULL; no SDK |
| `ios-arm64` | Track A → C++ → Xcode | **Not Implemented** | signing/toolchain unavailable |
| `arm64`/`riscv64` (bare) | BACKEND_ARM/RISCV | **Not Implemented** | constructors return NULL emit |
| GUI apps (any OS) | — | **Not Implemented** | Win32/Cocoa/UIKit bindings absent |

### Adding-a-target checklist (so future work is mechanical, not creative)
1. Constructor with non-NULL `emit_function` (+ `reg_name`, `type_suffix`).
2. Runtime helpers for the target (print/input/exit equivalents).
3. Linker/packager invocation documented + scripted (`Demo/build-<target>.sh` pattern).
4. ≥5 end-to-end programs with expected outputs in `Tests/real_regression.sh`.
5. Registry row flipped to Available ONLY after 4 passes here.

## 6. Files changed
`PHASE_D_REPORT.md`, `Docs/Reference/TARGETS.md`.

## 7-8. Tests / execution
No new code → no new execution. Existing gates green (re-verified in PHASE B/E runs).

## 9. Known limitations
- Track B x86 output is Linux-SysV-specific (`syscall`, custom `_start`) — NOT portable; Windows-native path is Track A by design (documented, not a defect).
- Registry is documentation until Studio reads it (PHASE H).

## 10-11. Remaining / next
PHASE E: prove `windows-x64` row with a real PE from `hello.ض` (+ script). Then F/G/H as honest roadmaps.
