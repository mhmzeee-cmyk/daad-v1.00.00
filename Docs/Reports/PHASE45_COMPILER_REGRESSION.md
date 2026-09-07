# PHASE45_COMPILER_REGRESSION.md — build + CLI + A-J + e2e + grammar

- Clean rebuild (patched Parser.cpp): cmake 0, build 0, same 10 pre-existing warnings.
- CLI: --help/--version/usage exit 0 (unchanged).
- A-J matrix: 10/10 PASS (identical vectors as Phase 3).
- E2E `صحيح س = 5؛ طباعة(س)؛` → prints **5** ✓.
- Grammar: `2^3`→8 SUPPORTED; `2^3^2`→64 (unchanged semantics, documented); `انتقل`×6 → BUG_FIXED (clean reject, was invalid-C++); hex/scientific → exit 1 EXPECTED_UNSUPPORTED.
- No other compiler behavior changed (single Parser branch added; Lexer/AST/CodeGen/Optimizer/Sandbox untouched).
