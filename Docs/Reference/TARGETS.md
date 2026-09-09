# TARGETS.md — Dhad Studio Target Registry (ground truth)

> A target is **Available** only if its row is proven by real execution in this repo.
> Anything else is **Experimental** (partial proof) or **Not Implemented**.
> Studio MUST NOT offer a target marked otherwise. See `PHASE_D_REPORT.md`.

## Frontend → neutral form
- **Track A** (`Compiler/`, `.ض` sources): emits standard **C++20**. Platform is chosen by the
  C++ toolchain invoked afterwards — the compiler itself is platform-neutral by construction.
- **Track B** (`DAAD/`, `.daad` sources): lowers to **DAAD IR**, then a `BackendTarget` emits
  machine-specific assembly. x86 output is Linux-SysV-specific (custom `_start` + `syscall`).

## Registry

| Target | Frontend | Backend / toolchain | Output | Status |
|---|---|---|---|---|
| `linux-x64` | Track B | `BACKEND_X86` + `gcc -nostartfiles` | ELF executable | **Available** |
| `windows-x64` | Track A | C++20 + MinGW-w64 (`x86_64-w64-mingw32-g++`) or MSVC | PE32+ `.exe` | **Available** |
| `dhad-cpu` | Track B | `BACKEND_DHAD` + `dhad_asm` + `dhad_cpu` | `.ضasm` / `.bin` | **Available** (educational) |
| `macos-x64`, `macos-arm64` | Track A | Apple clang (Mach-O) | `.app`/Mach-O | **Not Implemented** |
| `android-arm64` | Track A / `BACKEND_ARM` | NDK / backend scaffold (`emit_function = NULL`) | APK/AAB | **Not Implemented** |
| `ios-arm64` | Track A | Xcode + signing | `.app` | **Not Implemented** |
| `arm64`, `riscv64` (bare) | `BACKEND_ARM`, `BACKEND_RISCV` | scaffolds (`emit_function = NULL`) | — | **Not Implemented** |
| GUI application | — | no Win32/Cocoa/UIKit bindings exist | — | **Not Implemented** |

## Notes
- Track B x86 programs link with `gcc -nostartfiles` (custom `_start`); plain `gcc` fails by design.
- DHAD frame contract: S7 = frame pointer, high byte `0xE0`; `LDRI`/`STRI` are 4-byte 3-register, full 16-bit.
- Legacy CPU EI/DI/RETI use `0xE1–0xE3`; CMP uses `0xE0,0xE4–0xE7` (backend avoids s1–s3 for CMP scratch).
