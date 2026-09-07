# PHASE3_RUNTIME_MATRIX.md — evidence-backed verdicts

| Component | Build | Runtime | Integration | Result | Evidence |
|---|---|---|---|---|---|
| Compiler | PASS | PASS | PASS | WORKS | cmake 0/build 0; CLI help/version; matrix 10/10; e2e prints 5 |
| Generated C++ | PASS | PASS | PASS | WORKS | single `-I` resolves Daad+stdlib; g++ 0; Arabic file/ids clean |
| Sandbox | PASS | PASS | PASS | WORKS | 9/9 (7 attacks incl. whitespace bypass) |
| Server | PASS | PASS | PASS | WORKS | boot 127.0.0.1; health×3 200; 12 endpoints as expected |
| Auth | PASS | PARTIAL | PASS | PARTIALLY WORKS | register/login/logout/RBAC ok; **refresh always 401 (missing await, :579)** |
| Database | PASS | PASS | PASS | WORKS | validate/generate/push 0; XP persists restart |
| Redis | PASS | PASS | PASS | WORKS | connected+ready; leaderboard via Redis; fallback without it |
| Frontend | PASS | PARTIAL | PASS | PARTIALLY WORKS | 13/13 pages 200; 0 missing assets; API flow on :3000; no browser automation |
| Bridge | PASS | PARTIAL | PASS | PARTIALLY WORKS | build 0; offscreen alive; 4/4 API paths live |
| Electron | PASS | PARTIAL | BLOCKED | PARTIALLY WORKS | main alive 40s; paths exist; backend needs `npm install` (proven) |
| VSCode | PASS | PARTIAL | PASS | PARTIALLY WORKS | tsc 0; real activation on .ض logged; pixels/F9 not checked |
| CPU | PASS | PASS | PASS | WORKS | suite 96/96; GUI offscreen alive; Arabic clean |

Grammar probes: `^`→8 ok; `^` chains left-assoc (64); `انتقل` emits invalid C++ (FOUND); hex/scientific clean-reject (safe).
