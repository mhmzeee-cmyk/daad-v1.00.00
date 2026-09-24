@echo off
REM ============================================
REM  Dhad Studio - Production Startup Script
REM  Developer: محمد محمود الحموز | Dhad Studio
REM ============================================

echo.
echo ========================================
echo   Dhad Studio - Production Server
echo ========================================
echo.

REM Check Node.js
node -v >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Node.js not found! Please install Node.js 18+
    pause
    exit /b 1
)

REM Check PM2
pm2 -v >nul 2>&1
if errorlevel 1 (
    echo [INFO] Installing PM2 globally...
    npm install -g pm2
    if errorlevel 1 (
        echo [ERROR] Failed to install PM2
        pause
        exit /b 1
    )
)

REM Install dependencies
echo [1/4] Installing dependencies...
call npm install --production
if errorlevel 1 (
    echo [ERROR] Failed to install dependencies
    pause
    exit /b 1
)

REM Generate Prisma client
echo [2/4] Generating Prisma client...
call npx prisma generate
if errorlevel 1 (
    echo [ERROR] Failed to generate Prisma client
    pause
    exit /b 1
)

REM Run migrations
echo [3/4] Running database migrations...
call npx prisma migrate deploy
if errorlevel 1 (
    echo [WARNING] Migrations failed (may already be up to date)
)

REM Start with PM2
echo [4/4] Starting server with PM2...
pm2 delete dhad-studio 2>nul
pm2 start ecosystem.config.js --env production
pm2 save

echo.
echo ========================================
echo   Server started successfully!
echo   Status: pm2 status
echo   Logs: pm2 logs dhad-studio
echo   Stop: pm2 stop dhad-studio
echo ========================================
echo.

pm2 status
