# PHASE3_COMPILER_TESTS.md — daad-compiler runtime matrix

Build: `cmake -S Daad-Compiler -B /tmp/dhad-phase3/compiler-build -DDAAD_BUILD_TESTS=OFF -DDAAD_BUILD_IDE=OFF -DDAAD_BUILD_BRIDGE=OFF` → configure 0, build 0 (10 pre-existing .cpp warnings). Binary `bin/daad-compiler`.
CLI: `--help` 0 (Arabic usage), `--version` 0 (v1.0.0), no-args prints usage (exit 0, by design).

## Matrix A–J (ض → C++ → g++ `-I<Daad-Compiler>` → run): 10/10 PASS
| Case | Program | stdout |
|---|---|---|
| A vars | `صحيح س = 5؛ طباعة(س)؛` | 5 |
| B arith | `2+3*4`, `(2+3)*4`, `10%3` | 14 / 20 / 1 |
| C parens | `(10-2)*(3+1)` | 32 |
| D cond | إذا/وإلا age≥18 | أنت بالغ |
| E loop | طالما sum 1..100 | 5050 |
| F func | `دالة جمع(...) -> صحيح`, جمع(3,4) | 7 |
| G strings | طباعة("مرحبا بالعالم") | مرحبا بالعالم |
| H identifiers | `عدد_الطلاب` | 5 |
| I arabic out | طباعة("مرحبا") | مرحبا |
| J invalid (`صحيح = ؛` + `(((`) | exit=1, no crash | clean reject ✓ |

## Grammar probes (documented, NOT fixed)
- `2^3` → 8 SUPPORTED (std::pow).
- `2^3^2` → 64: left-assoc observed (math convention 512) → FOUND, deferred.
- `انتقل نهاية؛` → daad accepts, emits bare `نهاية;` → g++ error → BROKEN codegen, FOUND with evidence (`P_goto_test.cpp:44`), deferred.
- `0xFF`, `1.5e2` → exit=1 clean reject → UNSUPPORTED_EXPECTED (safe).
