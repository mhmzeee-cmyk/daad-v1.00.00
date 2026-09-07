@echo off
title Dhad Studio
cd /d "%~dp0server"
if not exist "node_modules" (
    echo Installing dependencies...
    call npm install --production
)
call npx prisma generate --schema=prisma/schema.prisma 2>nul
call npx prisma db push --schema=prisma/schema.prisma 2>nul
start http://localhost:3000
set NODE_ENV=production
node src/index.js
pause
