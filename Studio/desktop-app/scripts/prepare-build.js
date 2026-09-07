/**
 * prepare-build.js
 * Copies server/ and frontend-web/ into desktop-app/ for electron-builder packaging.
 * Also ensures .prisma generated client is in a visible location for ASAR inclusion.
 */
const fs = require('fs');
const path = require('path');

const DESKTOP_DIR = path.join(__dirname, '..'); // desktop-app/
const PROJECT_ROOT = path.join(DESKTOP_DIR, '..'); // dhad-studio/

const DIRS_TO_COPY = [
  { src: 'server', dst: 'server', exclude: ['node_modules', '.env', 'prisma/dev.db'] },
  { src: 'frontend-web', dst: 'frontend-web', exclude: [] },
];

function copyRecursive(src, dest, exclude) {
  if (!fs.existsSync(src)) return;
  const stat = fs.statSync(src);
  if (stat.isDirectory()) {
    fs.mkdirSync(dest, { recursive: true });
    for (const entry of fs.readdirSync(src)) {
      if (exclude.includes(entry)) continue;
      if (entry === 'node_modules' && src.endsWith('/server')) continue;
      copyRecursive(path.join(src, entry), path.join(dest, entry), exclude);
    }
  } else {
    fs.copyFileSync(src, dest);
  }
}

for (const { src, dst, exclude } of DIRS_TO_COPY) {
  const srcPath = path.join(PROJECT_ROOT, src);
  const dstPath = path.join(DESKTOP_DIR, dst);
  if (!fs.existsSync(srcPath)) {
    console.warn(`[prepare-build] Source not found: ${srcPath}`);
    continue;
  }
  if (fs.existsSync(dstPath)) {
    fs.rmSync(dstPath, { recursive: true, force: true });
  }
  console.log(`[prepare-build] Copying ${src} -> desktop-app/${dst}`);
  copyRecursive(srcPath, dstPath, exclude);
}

// Copy node_modules dependencies needed at runtime
const serverDst = path.join(DESKTOP_DIR, 'server');
const RUNTIME_DEPS = [
  '@prisma/client', '@msgpack/msgpack',
  'bcryptjs', 'compression', 'cookie-parser', 'dotenv',
  'express', 'express-rate-limit', 'helmet', 'ioredis',
  'jsonwebtoken', 'multer', 'swagger-jsdoc', 'swagger-ui-express',
  'winston', 'xlsx',
];

for (const dep of RUNTIME_DEPS) {
  const srcModules = path.join(PROJECT_ROOT, 'server', 'node_modules', dep);
  const dstModules = path.join(serverDst, 'node_modules', dep);
  if (fs.existsSync(srcModules) && !fs.existsSync(dstModules)) {
    console.log(`[prepare-build] Copying node_modules/${dep}`);
    copyRecursive(srcModules, dstModules, []);
  }
}

// Copy .prisma generated client to server/node_modules
const srcPrisma = path.join(PROJECT_ROOT, 'server', 'node_modules', '.prisma');
const dstPrisma = path.join(serverDst, 'node_modules', '.prisma');
if (fs.existsSync(srcPrisma) && !fs.existsSync(dstPrisma)) {
  console.log('[prepare-build] Copying server node_modules/.prisma');
  copyRecursive(srcPrisma, dstPrisma, []);
}

// Copy Prisma schema and migrations for runtime
const srcPrismaDir = path.join(PROJECT_ROOT, 'server', 'prisma');
const dstPrismaDir = path.join(serverDst, 'prisma');
if (fs.existsSync(srcPrismaDir) && !fs.existsSync(dstPrismaDir)) {
  console.log('[prepare-build] Copying prisma/ schema + migrations');
  fs.mkdirSync(dstPrismaDir, { recursive: true });
  const schemaFile = path.join(srcPrismaDir, 'schema.prisma');
  if (fs.existsSync(schemaFile)) fs.copyFileSync(schemaFile, path.join(dstPrismaDir, 'schema.prisma'));
  const migrationsDir = path.join(srcPrismaDir, 'migrations');
  if (fs.existsSync(migrationsDir)) copyRecursive(migrationsDir, path.join(dstPrismaDir, 'migrations'), []);
}

// Ensure .prisma is in desktop-app's own node_modules (electron-builder picks from here)
const desktopPrisma = path.join(DESKTOP_DIR, 'node_modules', '.prisma');
if (fs.existsSync(srcPrisma) && !fs.existsSync(desktopPrisma)) {
  console.log('[prepare-build] Copying .prisma to desktop-app/node_modules');
  copyRecursive(srcPrisma, desktopPrisma, []);
}

// electron-builder excludes hidden directories (starting with .) from asar,
// and excludes all node_modules via !**/node_modules pattern.
// Solution: copy .prisma to a top-level visible directory (not inside node_modules).
const visiblePrisma = path.join(DESKTOP_DIR, 'prisma_client');
if (fs.existsSync(srcPrisma) && !fs.existsSync(visiblePrisma)) {
  console.log('[prepare-build] Creating top-level prisma_client for ASAR');
  copyRecursive(srcPrisma, visiblePrisma, []);
}

// Patch @prisma/client/index.js to use the top-level visible path
const prismaClientIndex = path.join(DESKTOP_DIR, 'node_modules', '@prisma', 'client', 'index.js');
if (fs.existsSync(prismaClientIndex)) {
  let content = fs.readFileSync(prismaClientIndex, 'utf8');
  if (content.includes('prisma') && !content.includes("../../prisma_client/client/default")) {
    content = content.replace(
      /require\(['"][^'"]*prisma[^'"]*client\/default['"]\)/g,
      "require('../../prisma_client/client/default')"
    );
    fs.writeFileSync(prismaClientIndex, content, 'utf8');
    console.log('[prepare-build] Patched desktop-app @prisma/client');
  }
}

// Also patch server's copy of @prisma/client
const serverPrismaClientIndex = path.join(serverDst, 'node_modules', '@prisma', 'client', 'index.js');
if (fs.existsSync(serverPrismaClientIndex)) {
  let content = fs.readFileSync(serverPrismaClientIndex, 'utf8');
  if (content.includes('.prisma/client/default') && !content.includes("require('../../../prisma_client/client/default')")) {
    content = content.replace(
      /require\(['"]\.prisma\/client\/default['"]\)/g,
      "require('../../../prisma_client/client/default')"
    );
    fs.writeFileSync(serverPrismaClientIndex, content, 'utf8');
    console.log('[prepare-build] Patched server @prisma/client');
  }
}

console.log('[prepare-build] Done.');
