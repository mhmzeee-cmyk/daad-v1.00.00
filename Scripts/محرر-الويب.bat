@echo off
chcp 65001 >nul 2>&1
title محرر ض على الويب — Dhad Web Editor
color 0B

echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║              محرر ض على الويب — Dhad Web Editor                ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.

cd /d "%~dp0Dhad-Studio-Web"

:: فحص npx
where npx >nul 2>&1
if %errorlevel% neq 0 (
    echo  ✗ Node.js غير مثبّت!
    echo  حمّله من: https://nodejs.org
    pause
    exit /b 1
)

echo  جاري تشغيل الخادم المحلي...
echo  افتح المتصفح على: http://localhost:3000
echo.
start http://localhost:3000
npx serve . -l 3000
pause
