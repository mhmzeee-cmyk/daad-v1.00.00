# PHASE3_ELECTRON_TESTS.md — paths + syntax + live boot attempt

- `node --check` main.js + preload.js: OK.
- Resolved paths exist: `../Dhad-Studio-Full/server/src/index.js` ✓, `.../frontend-web/login.html` ✓ (Phase-2 fix verified via node path resolution).
- Security flags present: contextIsolation:true ×1, nodeIntegration:false ×2 (main + webPreferences).
- Live boot (electron 33 from /tmp + xvfb, repo untouched): main process starts, splash code reached, app survives 40s+ (exit 124), BUT spawned backend crashes: `Cannot find module 'dotenv'` — `server/node_modules` absent in shipped tree.
- Control experiment: same server dir + NODE_PATH→/tmp deps → boots, health 200. So the ONLY blocker is missing installed dependencies (by design: `npm install` step in guide/bat), not code.
- Preload/IPC/window-open: NOT VERIFIED interactively (headless).
- Verdict: PARTIALLY WORKS (shell+paths+windows OK) / BLOCKED (backend deps not shipped).
