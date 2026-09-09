const { app, BrowserWindow, Menu, shell, dialog, ipcMain } = require("electron");
const path = require("path");
const fs = require("fs");
const { spawn } = require("child_process");

let mainWindow;
let splashWindow;
let serverProcess;
const SERVER_PORT = 3000;
const SERVER_URL = `http://localhost:${SERVER_PORT}`;

app.disableHardwareAcceleration();

app.whenReady().then(async () => {
  createSplash();
  await startServer();
  createMainWindow();
  createMenu();
});

// ── Splash Screen ────────────────────────────────────────────────────────────
function createSplash() {
  splashWindow = new BrowserWindow({
    width: 500,
    height: 350,
    transparent: true,
    frame: false,
    alwaysOnTop: true,
    resizable: false,
    webPreferences: { nodeIntegration: false },
  });
  splashWindow.loadFile(path.join(__dirname, "splash.html"));
  splashWindow.center();
}

// ── Start Backend Server ─────────────────────────────────────────────────────
function startServer() {
  return new Promise((resolve) => {
    const serverPath = path.join(__dirname, "server", "src", "index.js");
    serverProcess = spawn(process.execPath, [serverPath], {
      cwd: path.join(__dirname, "server"),
      env: { ...process.env, PORT: SERVER_PORT },
      stdio: "pipe",
    });
    serverProcess.stdout.on("data", (data) => {
      const output = data.toString();
      if (output.includes("Dhad Studio") || output.includes("Server started") || output.includes("running on")) {
        waitForHealth(resolve);
      }
    });
    serverProcess.stderr.on("data", (d) => console.error(`Server: ${d}`));
    serverProcess.on("error", () => {
      dialog.showErrorBox("خطأ في السيرفر", "فشل تشغيل السيرفر. تأكد من تثبيت Node.js.");
      app.quit();
    });
    // Fallback: if server log not detected, try health endpoint then resolve
    setTimeout(() => waitForHealth(resolve), 3000);
  });
}

function waitForHealth(resolve, attempts) {
  attempts = attempts || 0;
  const http = require("http");
  const req = http.get(`${SERVER_URL}/health/live`, (res) => {
    if (res.statusCode === 200) {
      resolve();
    } else {
      retryHealth(resolve, attempts);
    }
  });
  req.on("error", () => retryHealth(resolve, attempts));
  req.setTimeout(1000, () => { req.destroy(); retryHealth(resolve, attempts); });
}

function retryHealth(resolve, attempts) {
  if (attempts < 10) {
    setTimeout(() => waitForHealth(resolve, attempts + 1), 500);
  } else {
    resolve(); // give up after 10 retries, proceed anyway
  }
}

// ── Main Window ──────────────────────────────────────────────────────────────
function createMainWindow() {
  mainWindow = new BrowserWindow({
    width: 1400,
    height: 900,
    minWidth: 1000,
    minHeight: 600,
    title: "ض ستوديو",
    icon: fs.existsSync(path.join(__dirname, "icon.ico")) ? path.join(__dirname, "icon.ico") : undefined,
    show: false,
    frame: false,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, "preload.js"),
    },
  });

  mainWindow.webContents.session.webRequest.onHeadersReceived((details, callback) => {
    callback({
      responseHeaders: {
        ...details.responseHeaders,
        'Content-Security-Policy': ["default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'; img-src 'self' data:; font-src 'self' data:; connect-src 'self' http://localhost:3000 https:; object-src 'none'; base-uri 'self'; form-action 'self'; frame-ancestors 'none'"],
        'X-Content-Type-Options': ['nosniff'],
        'X-Frame-Options': ['DENY'],
        'X-XSS-Protection': ['1; mode=block'],
        'Referrer-Policy': ['strict-origin-when-cross-origin'],
      },
    });
  });

  mainWindow.loadFile(path.join(__dirname, "frontend-web", "login.html"));

  // Inject titlebar CSS into every page
  mainWindow.webContents.on("did-finish-load", () => {
    const css = `
      const style = document.createElement('style');
      style.textContent = \`
        .titlebar{position:fixed;top:0;left:0;right:0;height:36px;background:#0a0e1a;border-bottom:1px solid #1e293b;display:flex;align-items:center;justify-content:space-between;z-index:9999;-webkit-app-region:drag;padding:0 8px;user-select:none}
        .titlebar-title{font-size:12px;font-weight:600;color:#94a3b8;padding-right:12px}
        .titlebar-controls{display:flex;gap:2px;-webkit-app-region:no-drag}
        .titlebar-btn{width:36px;height:36px;background:transparent;border:none;color:#94a3b8;font-size:14px;cursor:pointer;display:flex;align-items:center;justify-content:center;transition:all .15s;border-radius:4px}
        .titlebar-btn:hover{background:#1e293b;color:#e2e8f0}
        .titlebar-btn.close:hover{background:#ef4444;color:white}
        body.has-titlebar{padding-top:36px!important}
      \`;
      document.head.appendChild(style);
      
      // Add titlebar element
      if (!document.getElementById('electron-titlebar')) {
        const tb = document.createElement('div');
        tb.id = 'electron-titlebar';
        tb.className = 'titlebar';
        tb.innerHTML = '        <div class="titlebar-title">🚀 ض ستوديو</div><div class="titlebar-controls"><button class="titlebar-btn" onclick="window.electronAPI?.minimize()">─</button><button class="titlebar-btn" onclick="window.electronAPI?.maximize()">□</button><button class="titlebar-btn close" onclick="window.electronAPI?.close()">×</button></div>';
        document.body.prepend(tb);
        document.body.classList.add('has-titlebar');
      }
    `;
    mainWindow.webContents.executeJavaScript(css).catch(() => {});
  });

  mainWindow.once("ready-to-show", () => {
    if (splashWindow) { splashWindow.close(); splashWindow = null; }
    mainWindow.show();
  });

  mainWindow.on("closed", () => { mainWindow = null; });

  mainWindow.webContents.setWindowOpenHandler(({ url }) => {
    shell.openExternal(url);
    return { action: "deny" };
  });
}

// ── Menu ─────────────────────────────────────────────────────────────────────
function createMenu() {
  const template = [
    {
      label: "ملف",
      submenu: [
        { label: "لوحة التحكم", accelerator: "CmdOrCtrl+D", click: () => nav("pages/student-dashboard.html") },
        { label: "التحديات", accelerator: "CmdOrCtrl+Shift+C", click: () => nav("pages/challenges.html") },
        { label: "خارطة الطريق", accelerator: "CmdOrCtrl+Shift+R", click: () => nav("pages/roadmap.html") },
        { type: "separator" },
        { label: "الملف الشخصي", accelerator: "CmdOrCtrl+P", click: () => nav("pages/profile.html") },
        { type: "separator" },
        { label: "خروج", accelerator: "CmdOrCtrl+Q", click: () => app.quit() },
      ],
    },
    {
      label: "عرض",
      submenu: [
        { label: "إعادة تحميل", accelerator: "CmdOrCtrl+R", click: () => mainWindow?.reload() },
        { label: "تكبير", accelerator: "CmdOrCtrl+=", click: () => { if (mainWindow) mainWindow.webContents.setZoomLevel(mainWindow.webContents.getZoomLevel() + 0.5); }},
        { label: "تصغير", accelerator: "CmdOrCtrl+-", click: () => { if (mainWindow) mainWindow.webContents.setZoomLevel(mainWindow.webContents.getZoomLevel() - 0.5); }},
        { label: "حجم طبيعي", accelerator: "CmdOrCtrl+0", click: () => { if (mainWindow) mainWindow.webContents.setZoomLevel(0); }},
        { type: "separator" },
        { label: "أدوات المطور", accelerator: "F12", click: () => mainWindow?.webContents.toggleDevTools() },
        { label: "شاشة كاملة", accelerator: "F11", click: () => mainWindow?.setFullScreen(!mainWindow?.isFullScreen()) },
      ],
    },
    {
      label: "مساعدة",
      submenu: [
        { label: "حول ض ستوديو", click: () => {
          dialog.showMessageBox(mainWindow, {
            type: "info", title: "حول ض ستوديو", message: "ض ستوديو v1.0.0",
            detail: "منصة تعليم البرمجة للطلاب والمعلمين\n\nتعلم البرمجة بلغتك العربية مع تحديات تفاعلية وتقييم ذكي.\n\n© 2026 ض ستوديو",
            buttons: ["موافق"],
          });
        }},
        { label: "التوثيق", click: () => shell.openExternal("https://daad-studio.com/docs") },
      ],
    },
  ];
  Menu.setApplicationMenu(Menu.buildFromTemplate(template));
}

function nav(page) {
  mainWindow?.loadFile(path.join(__dirname, "frontend-web", page));
}

// ── IPC ──────────────────────────────────────────────────────────────────────
ipcMain.on("window-minimize", () => mainWindow?.minimize());
ipcMain.on("window-maximize", () => {
  mainWindow?.isMaximized() ? mainWindow.unmaximize() : mainWindow?.maximize();
});
ipcMain.on("window-close", () => mainWindow?.close());
ipcMain.handle("window-is-maximized", () => mainWindow?.isMaximized() || false);

// ── Quit ─────────────────────────────────────────────────────────────────────
app.on("window-all-closed", () => { serverProcess?.kill(); app.quit(); });
app.on("before-quit", () => { serverProcess?.kill(); });
