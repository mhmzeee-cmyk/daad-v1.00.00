# PHASE3_CPU_TESTS.md — full suite + GUI + Unicode

- Method: tree copied to /tmp (vfat USB has no exec bit — ENVIRONMENT LIMITATION, not code), chmod +x, ran unmodified `run_tests.sh`.
- Result: **96/96 PASS, 0 FAIL** (T1 arithmetic … T15 stress, T14 error detection, 8 BUG regression tests incl. nested-.if and SHL-carry).
- GUI (`dhad_gui`, Qt5): QT_QPA_PLATFORM=offscreen → survives 12s (exit 124, no crash) → BUILD+RUNTIME survival; widget interaction NOT VERIFIED.
- Unicode: Arabic filename `اختبار_عربي.ض` compiles+runs (7, no mojibake); Arabic identifiers/strings/`؛` covered in compiler matrix.
- Variants (4-bit core vs 8-bit main/live/visual) re-confirmed INTENTIONALLY_DIFFERENT; all execute correctly.
