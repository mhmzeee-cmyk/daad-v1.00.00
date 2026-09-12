# PHASE6_ELECTRON_RELEASE_AUDIT.md — ships vs expects (no packaging done)

Ships: main.js, preload.js, splash/titlebar, package.json (electron ^33; build:linux/mac scripts untested here).
Expects externally: Full tree alongside (`../Dhad-Studio-Full/...` — dev layout; packager must define flat layout), `server/node_modules` INSTALLED (proven blocker without it), login.html reachable, localhost:3000 servable.
Must bundle: electron runtime, server sources + node_modules (or documented `npm install` step), frontend bundle, preload. Must install: node NOT needed at runtime (electron ships it); Qt NOT needed.
Security gate HELD: contextIsolation=true, nodeIntegration=false, IPC window-controls only, no `nodeIntegration=true` anywhere. Missing icon.ico → packaging TODO (R4).
Verdict: design understood; NOT READY until layout+deps bundled by packager.
