@echo off
chcp 65001 >nul 2>&1
title تشغيل المنصة الكاملة — Dhad Studio Full
color 0B

echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║              ض ستوديو — المنصة الكاملة                         ║
echo  ║         سيرفر + واجهة ويب + قاعدة بيانات                      ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.

cd /d "%~dp0Dhad-Studio-Full\server"

echo  فحص المتطلبات...

:: فحص Node.js
where node >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo  ╔══════════════════════════════════════════════════════════════╗
    echo  ║  Node.js غير مثبّت!                                         ║
    echo  ║  يرجى تثبيته من: https://nodejs.org                        ║
    echo  ║  اختر الإصدار LTS                                           ║
    echo  ╚══════════════════════════════════════════════════════════════╝
    echo.
    pause
    exit /b 1
)

echo  ✓ Node.js موجود
node --version

:: تثبيت المكتبات
if not exist "node_modules" (
    echo.
    echo  جاري تثبيت المكتبات (يأخذ دقيقة واحدة)...
    call npm install --production
    if %errorlevel% neq 0 (
        echo  ✗ فشل تثبيت المكتبات
        pause
        exit /b 1
    )
    echo  ✓ تم تثبيت المكتبات
) else (
    echo  ✓ المكتبات مثبّتة مسبقاً
)

:: إعداد قاعدة البيانات
echo.
echo  جاري إعداد قاعدة البيانات...
call npx prisma generate --schema=prisma/schema.prisma 2>nul
call npx prisma db push --schema=prisma/schema.prisma 2>nul
echo  ✓ قاعدة البيانات جاهزة

:: تشغيل السيرفر
echo.
echo  ═══════════════════════════════════════════════════════════════
echo   جاري تشغيل السيرفر على المنفذ 3000...
echo   افتح المتصفح على: http://localhost:3000
echo   للإيقاف: اضغط Ctrl+C
echo  ═══════════════════════════════════════════════════════════════
echo.

start http://localhost:3000
set NODE_ENV=production
node src/index.js
pause
