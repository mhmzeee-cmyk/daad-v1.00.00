/**
 * after-pack.js
 * electron-builder afterPack hook.
 * Copies server/node_modules and prisma_client into app.asar.unpacked
 * so they're accessible at runtime (electron-builder excludes node_modules from asar).
 */
const fs = require('fs');
const path = require('path');

function copyRecursive(src, dest) {
  if (!fs.existsSync(src)) return;
  const stat = fs.statSync(src);
  if (stat.isDirectory()) {
    fs.mkdirSync(dest, { recursive: true });
    for (const entry of fs.readdirSync(src)) {
      copyRecursive(path.join(src, entry), path.join(dest, entry));
    }
  } else {
    fs.copyFileSync(src, dest);
  }
}

module.exports = function(context) {
  var appDir = context.appOutDir;
  var unpackedDir = path.join(appDir, 'resources', 'app.asar.unpacked');
  var cwd = process.cwd();

  // 1. Copy server/node_modules to unpacked
  var serverNM = path.join(cwd, 'server', 'node_modules');
  var unpackedServerNM = path.join(unpackedDir, 'server', 'node_modules');
  if (fs.existsSync(serverNM)) {
    console.log('[afterPack] Copying server/node_modules');
    copyRecursive(serverNM, unpackedServerNM);
  }

  // 2. Copy prisma_client to unpacked
  var prismaClient = path.join(cwd, 'prisma_client');
  var unpackedPrismaClient = path.join(unpackedDir, 'prisma_client');
  if (fs.existsSync(prismaClient) && !fs.existsSync(unpackedPrismaClient)) {
    console.log('[afterPack] Copying prisma_client');
    copyRecursive(prismaClient, unpackedPrismaClient);
  }

  console.log('[afterPack] Done');
};
