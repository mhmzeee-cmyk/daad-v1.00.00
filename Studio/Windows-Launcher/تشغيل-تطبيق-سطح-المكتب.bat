@echo off
chcp 65001 >nul 2>&1
title تشغيل تطبيق سطح المكتب — Dhad Studio Electron
color 0B

echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║              ض ستوديو — تطبيق سطح المكتب                       ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.

cd /d "%~dp0Dhad-Studio-Electron"

:: فحص Node.js
where node >nul 2>&1
if %errorlevel% neq 0 (
    echo  ✗ Node.js غير مثبّت!
    echo  حمّله من: https://nodejs.org
    pause
    exit /b 1
)

:: تثبيت المكتبات
if not exist "node_modules" (
    echo  جاري تثبيت المكتبات...
    call npm install
    if %errorlevel% neq 0 (
        echo  ✗ فشل تثبيت المكتبات
        pause
        exit /b 1
    )
    echo  ✓ تم التثبيت
)

:: تشغيل التطبيق
echo  جاري تشغيل التطبيق...
call npm start
pause
