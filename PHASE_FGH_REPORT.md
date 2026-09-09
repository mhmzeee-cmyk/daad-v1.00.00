# PHASE_FGH_REPORT.md — Stdlib Abstraction, Platform Roadmap, Studio Wiring (design-gated)

> Nothing here is implemented as code: this environment has no Apple/Android toolchains and no
> Win32/Cocoa/UIKit bindings in the repo. Per code-quality rules, unimplemented targets are
> documented as **Not Implemented**, never scaffolded as fake. Each item below lists its
> acceptance gate (what must pass before its status may change).

## PHASE F — Standard library platform abstraction
1. What was inspected: `Compiler/stdlib/*.hpp` + `stdlib/*.ض` (+ Arabic mirror): math/io/net/db/gfx/web/time/text/data_structures/concurrency — all assume a hosted C++ stdlib + POSIX-ish I/O; no OS-branching exists.
2. Problems found: none functional (they compile under Linux/macOS/Windows C++ as-is). The gap is
   *documented intent*: future OS-specific surface (Window/Thread/Audio) has no home.
3. Design (no code): when first OS-bound API arrives, place portable declarations under
   `Compiler/stdlib/<area>/` with per-OS backends selected by the consuming toolchain
   (`__linux__` / `_WIN32` / `__APPLE__`), mirroring how Track A already delegates platform
   choice to the C++ toolchain. Do NOT add `WindowsAPI()`-style calls into the ض language.
4. Gate: any new OS API must build+run on Linux AND cross-build to Windows PE before merging.

## PHASE G — Five-platform roadmap
| Order | Target | Vehicle | Missing piece | Acceptance gate |
|---|---|---|---|---|
| 1 | Windows x64 | Track A + MinGW/MSVC | nothing (done) | PE boots on real Windows (pending interview machine) |
| 2 | Linux x64 | Track B x86 / Track A + gcc | done | 25/25 suite (green) |
| 3 | macOS x64/arm64 | Track A + Apple clang | Apple toolchain + Mach-O packaging test | hello.ض → boot on Mac |
| 4 | Android arm64 | Track A + NDK (JNI entry) | NDK wrapper + APK packaging | APK installs+launches |
| 5 | iOS arm64 | Track A + Xcode | signing + toolchain + device/VM test | signed .app launches |
| — | DHAD CPU | Track B DHAD | done (educational) | loop15/fact120, 96/96 |
| — | bare arm64/riscv64 | BACKEND_ARM/RISCV | full `emit_function` implementations | 5+ programs each in regression |

## PHASE H — Dhad Studio wiring (when backends exist)
1. Editor already compiles+runs in-browser (verified). Required additions ONLY:
   - **Target selector** bound to `Docs/Reference/TARGETS.md` statuses: show Available targets
     normally; Experimental/Not-Implemented greyed with reason. Never offer unbuilt targets.
   - **Build** streams the real pipeline commands (`Demo/build-windows.sh` pattern) into the
     Output pane; **binary export** writes to `dist/<Target>/` only on exit-0 (never placeholder files).
   - Reuse existing Output/Errors panes; no redesign.
2. Gate: a target appears selectable only after its row passes the PHASE D checklist.

## Files changed
`PHASE_FGH_REPORT.md` (this file). No source changes in F/G/H.

## Execution / tests
None (design phases). All gates refer to existing suites (`Tests/real_regression.sh`, ctest, CPU suite).

## Final checkpoint
Committed after this report (see git log). Remaining work is exactly the table above.
