# Desktop App - ض ستوديو

Electron-based desktop application for the Dhad Studio platform.

## Architecture (البنية)

```
desktop-app/
├── main.js            # Electron main process — starts server + loads frontend
├── preload.js         # Context bridge (window.electronAPI)
├── splash.html        # Splash screen shown during server startup
├── server/            # Copy of server/ (copied by prepare-build.js)
├── frontend-web/      # Copy of frontend-web/ (copied by prepare-build.js)
├── prisma_client/     # Prisma generated client (visible to ASAR)
├── scripts/
│   ├── prepare-build.js   # Pre-build: copies server + frontend into this dir
│   └── after-pack.js      # Post-pack: copies node_modules to app.asar.unpacked
├── dist/              # Build output
└── package.json
```

**Flow on startup:**
1. `main.js` shows splash screen
2. Spawns `server/src/index.js` as a child process on port 3000
3. Waits for `/health/live` to return 200
4. Loads `frontend-web/login.html` in a frameless BrowserWindow
5. Injects custom titlebar CSS and IPC handlers

## prepare-build.js

Pre-build hook that runs before `electron-builder`. It:

1. **Copies `server/`** into `desktop-app/server/` (excluding `node_modules`, `.env`, `dev.db`)
2. **Copies `frontend-web/`** into `desktop-app/frontend-web/`
3. **Copies required `node_modules`** (express, prisma, bcryptjs, etc.) into `desktop-app/server/node_modules/`
4. **Copies Prisma client** (`.prisma/`) to multiple locations:
   - `desktop-app/node_modules/.prisma/`
   - `desktop-app/prisma_client/` (top-level, visible to ASAR)
   - `desktop-app/server/node_modules/.prisma/`
5. **Copies Prisma schema + migrations** to `desktop-app/server/prisma/`
6. **Patches `@prisma/client/index.js`** to point to the visible `prisma_client/` directory instead of hidden `.prisma/`

This ensures all runtime dependencies are bundled inside the Electron app, even though electron-builder's ASAR packing excludes hidden directories and most `node_modules`.

## after-pack.js

Post-pack hook that runs after `electron-builder` creates the ASAR archive. It:

1. **Copies `server/node_modules/`** into `app.asar.unpacked/server/node_modules/` — native modules (like Prisma engine binaries) cannot be loaded from inside an ASAR archive
2. **Copies `prisma_client/`** into `app.asar.unpacked/prisma_client/`

This ensures native binaries and Prisma engine files are accessible at runtime outside the ASAR.

## Development Run (التشغيل التجريفي)

```bash
cd desktop-app
npm start
```

This runs `electron .` which:
- Shows splash screen
- Starts the server on port 3000
- Opens the main window with the frontend

**Requirements:**
- `server/node_modules/` must exist (run `npm install` in `server/`)
- `frontend-web/` must exist at the project root
- Port 3000 must be free

## Linux Build (بناء لينكس)

```bash
cd desktop-app
npm run build:linux
```

Output: `desktop-app/dist/` containing:
- AppImage (portable, runs on most Linux distros)
- `.deb` package (Debian/Ubuntu)

**Note:** `prepare-build.js` runs automatically as a `prebuild` script.

## Windows Build (بناء ويندوز)

```bash
cd desktop-app
npm run build:win
```

Output: `desktop-app/dist/` containing:
- NSIS installer (`.exe`)

The installer:
- Allows custom install directory
- Creates desktop shortcut named "ض ستوديو"
- Not a one-click installer (user can configure options)

## macOS Build (بناء ماك)

```bash
cd desktop-app
npm run build:mac
```

Output: `desktop-app/dist/` containing:
- `.dmg` disk image

## ASAR Behavior

electron-builder packs the app into `app.asar` by default. However:

- **`node_modules/@prisma/client/**/*`** is listed in `asarUnpack` — these files stay outside ASAR in `app.asar.unpacked/`
- **`after-pack.js`** copies the full `server/node_modules/` and `prisma_client/` to `app.asar.unpacked/`
- **Hidden directories** (starting with `.`) are excluded from ASAR by electron-builder — this is why `prepare-build.js` creates a visible `prisma_client/` directory

The `main.js` accesses files using `__dirname`-relative paths, which resolve correctly whether running from source or from inside ASAR.

## Prisma Packaging

Prisma requires special handling in Electron:

1. **Schema + migrations** are copied to `desktop-app/server/prisma/`
2. **Generated client** (`.prisma/client/`) is copied to multiple locations
3. **`@prisma/client/index.js` is patched** to use relative paths to the visible `prisma_client/` directory
4. **`asarUnpack`** ensures the Prisma query engine binary is accessible outside ASAR

At runtime, Prisma finds its engine binary in `app.asar.unpacked/node_modules/.prisma/client/`.

## Troubleshooting (حل المشاكل)

### "Cannot find module" errors after build

`prepare-build.js` may not have copied all dependencies:
```bash
cd desktop-app
node scripts/prepare-build.js
npm run build:linux
```

### Blank screen / server not starting

Port 3000 may be occupied:
```bash
lsof -ti:3000 | xargs kill -9
npm start
```

### Prisma engine not found

Ensure `after-pack.js` ran and `app.asar.unpacked/` contains the Prisma binaries:
```bash
ls dist/linux-unpacked/resources/app.asar.unpacked/node_modules/.prisma/
```

If missing, rebuild:
```bash
npm run build:linux
```

### Build fails with "EACCES" permission error

Clean and rebuild:
```bash
rm -rf dist/ server/ frontend-web/ prisma_client/
npm run build:linux
```

### Server crashes in packaged app

Check that `server/.env` was not accidentally packaged (it's excluded in `build.files`). Create `.env` manually in the installed app directory or ensure defaults work without it.
