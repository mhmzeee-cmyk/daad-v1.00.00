# PHASE6_VSCODE_RELEASE_AUDIT.md — marketplace readiness

Installs: package.json (daad id, .ض, grammar, snippets, lang-config, onLanguage:daad, F9 command). Compiled `out/` ABSENT in tree → publisher must run `vscode:prepublish` (tsc, proven). User must install: VSCode ≥1.80 + `daad-compiler` on PATH (Linux binary exists; Windows via release bundle; PATH step must be documented — R2). Activation on .ض PROVEN live. Block comments supported. No secrets. No runtime deps (0 dependencies).
