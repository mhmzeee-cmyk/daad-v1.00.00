# REPAIR_CHANGELOG.md — Dhad Studio v1.0.0

## 1. web-editor-page.js (Web + Full) — DhadAPI crash
- Problem: `const api = new DhadAPI()` → ReferenceError, page dead.
- Root cause: class renamed to `ApiClient`; stale reference.
- Fix: removed line, uses global `api` from api.js (loads first, verified).
- Tests: grep zero `DhadAPI`; script-order check.

## 2. settings-page.js (Web + Full) — Toast crash
- Problem: 7× `Toast.show` (class has no static show) → settings page dead.
- Fix: `toast.error/success` matching toast.js instance + other pages.
- Tests: grep zero `Toast.show`; 7 replacements verified.

## 3. SandboxValidator.cpp — patterns dead (P0 security)
- Problem: `string::find("system\\s*\\(")` never matches real calls.
- Fix: `#include <regex>` + `std::regex_search` with try/catch + indented-comment skip.
- Tests: 9/9 (safe, system, ws-variant, popen, execl, socket, const_cast, indented comment, dangerous header).

## 4. include/Daad/*.hpp (11 new) — unbuildable project
- Problem: `include/` absent; all src includes dangling.
- Fix: reconstructed Token/UnicodeUtils/Keywords/Diagnostics/AST(header-only, 50+ nodes)/Lexer/Parser/CodeGen/Optimizer/SandboxValidator/Compiler.
- Tests: `cmake+make` SUCCESS; ض→C++ e2e ok.

## 5. Daad/Runtime/DaadRuntime.hpp (new wrapper)
- Problem: generated `#include "Daad/Runtime/..."` matched no file.
- Fix: wrapper including `../../stdlib/Runtime/DaadRuntime.hpp`. No CodeGen change.
- Tests: g++ compiles generated code; binary prints `5`.

## 6. Keywords.cpp — KwBase/KwFunction
- Problem: `KwBase→"this"` (wrong, means super), `KwFunction→"void"` (not a return type).
- Fix: `""` + comment (verified metadata-only, zero external users).
- Tests: grep zero external users; rebuild clean.

## 7. seed_admin.js — default password
- Problem: hardcoded `admin123` + printed.
- Fix: `SEED_ADMIN_PASSWORD` (min 8) or random hex; static secret never printed.
- Tests: grep zero `admin123`; node --check.

## 8. onboardController.js — shared batch OTP
- Problem: one OTP/hash for whole batch; anyone activates anyone.
- Fix: unique OTP+hash per user; `activationCodes[]`; singular kept only for single-user onboard (Bridge compat).
- Tests: grep zero `batchOtp`; node --check.

## 9. serverEvaluator.js — Function() injection
- Problem: `Function('"use strict";return('+clean+')')()`.
- Fix: shunting-yard (+-*/%, parens, unary, isFinite→null), keeps filter+interface.
- Tests: 10/10 (precedence, parens, unary, div0→null, malformed→null).

## 10. index.js — HMAC_CONFIRM_SECRET + fail-closed errors
- Fix: auto-generate when missing/insecure/equals HMAC_SECRET (logs 4 chars only); `isProd = production || unset`.
- Tests: node --check; grep.

## 11. api.js (Web + Full) — localStorage PII
- Fix: setUser stores only name/role/displayName.
- Tests: role guard intact (role preserved).

## 12. VSCode tmLanguage.json — block comments
- Fix: added `comment.block.daad` (`/* */`).
- Tests: JSON valid.

## 13. bridge/main.cpp + bridge.qrc (new)
- Problem: CMake referenced both, both absent.
- Fix: minimal main (QApplication + qmlRegisterType DaadBridge 1.0 + qrc:/bridge_main.qml) + minimal qrc.
- Tests: `cmake+make` 100% clean (twice, incl. after SchoolOnboarder.cpp compat edit).

## 14. bridge/SchoolOnboarder.cpp — activationCodes compat
- Fix: reads `activationCodes[]` joined by newline; falls back to singular.
- Tests: rebuild clean.

## Deliberately NOT changed (verified safe/different)
- CSRF Bearer bypass (correct for non-browser Bridge); tokens-in-JSON (Bridge needs them); CPU variants (intentional); CoreFoundation (guarded); `trust proxy`/audit persistence/SW coverage/STUB implementations/grammar extensions (future phase).
