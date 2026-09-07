# PHASE3_VSCODE_TESTS.md — compile + real activation in VSCode 1.136.1

- Setup: extension copied to /tmp, `npm install typescript@5 @types/vscode @types/node` 0, `tsc -p ./` 0 → `out/extension.js` (+maps).
- Runtime (xvfb, real VSCode, dev extension, opened /tmp/t.ض):
  - Window opened file (`filesToOpenOrCreate /tmp/t.ض exists:true`).
  - `ExtensionService#_doActivateExtension DaadStudio.daad-language, activationEvent: 'onLanguage:daad'` + `_callActivateOptional` — ACTIVATED, no activation error for it.
- Verdict: BUILD WORKS, ACTIVATION WORKS. Pixel-level highlighting + F9 command (needs external daad-compiler on PATH): NOT VERIFIED interactively.
