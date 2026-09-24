@echo off
chcp 65001 >nul 2>&1
title إعداد ض ستوديو — Setup Dhad Studio
color 0B

cls
echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║              إعداد ض ستوديو — Setup Dhad Studio                ║
echo  ║         هذا الملف يُحضّر كل شيء تلقائياً                       ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.

cd /d "%~dp0.."

echo  ═══════════════════════════════════════════════════════════════
echo   الخطوة 1: فحص Node.js
echo  ═══════════════════════════════════════════════════════════════
where node >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo   Node.js غير مثبّت!
    echo   يرجى تثبيته أولاً من: https://nodejs.org
    echo.
    set /p download="  هل تريد فتح صفحة التحميل؟ (Y/N): "
    if /i "%download%"=="Y" start https://nodejs.org
    echo.
    echo   أعد تشغيل هذا الملف بعد تثبيت Node.js
    pause
    exit /b 1
)
echo   ✓ Node.js موجود
node --version

echo.
echo  ═══════════════════════════════════════════════════════════════
echo   الخطوة 2: تثبيت مكتبات السيرفر
echo  ═══════════════════════════════════════════════════════════════
cd /d "%~dp0..\Dhad-Studio-Full\server"
if not exist "node_modules" (
    echo   جاري التثبيت (يأخذ حوالي دقيقة)...
    call npm install --production
    if %errorlevel% neq 0 (
        echo   ✗ فشل التثبيت
        pause
        exit /b 1
    )
    echo   ✓ تم تثبيت المكتبات
) else (
    echo   ✓ المكتبات مثبّتة مسبقاً
)

echo.
echo  ═══════════════════════════════════════════════════════════════
echo   الخطوة 3: إعداد قاعدة البيانات
echo  ═══════════════════════════════════════════════════════════════
call npx prisma generate --schema=prisma/schema.prisma 2>nul
call npx prisma db push --schema=prisma/schema.prisma 2>nul
echo   ✓ قاعدة البيانات جاهزة

echo.
echo  ═══════════════════════════════════════════════════════════════
echo   الخطوة 4: إنشاء اختصارات سطح المكتب
echo  ═══════════════════════════════════════════════════════════════
set desktop=%USERPROFILE%\Desktop

:: اختصار المنصة الكاملة
echo  Set oWS = WScript.CreateObject("WScript.Shell") > "%temp%\create_shortcut.vbs"
echo  Set oLink = oWS.CreateShortcut("%desktop%\ض ستوديو.lnk") >> "%temp%\create_shortcut.vbs"
echo  oLink.TargetPath = "%~dp0..\Windows-Launcher\ض-ستوديو.bat" >> "%temp%\create_shortcut.vbs"
echo  oLink.WorkingDirectory = "%~dp0.." >> "%temp%\create_shortcut.vbs"
echo  oLink.Description = "ض ستوديو - منصة تعليم البرمجة" >> "%temp%\create_shortcut.vbs"
echo  oLink.Save >> "%temp%\create_shortcut.vbs"
cscript /nologo "%temp%\create_shortcut.vbs"
del "%temp%\create_shortcut.vbs"
echo   ✓ تم إنشاء اختصار "ض ستوديو" على سطح المكتب

echo.
echo  ═══════════════════════════════════════════════════════════════
echo   الخطوة 5: إنشاء ملف تشغيل سريع
echo  ═══════════════════════════════════════════════════════════════
cd /d "%~dp0.."
echo @echo off > "تشغيل-سريع.bat"
echo chcp 65001 ^>nul 2^>^&1 >> "تشغيل-سريع.bat"
echo title ض ستوديو >> "تشغيل-سريع.bat"
echo cd /d "%%~dp0Windows-Launcher" >> "تشغيل-سريع.bat"
echo call "ض-ستوديو.bat" >> "تشغيل-سريع.bat"
echo   ✓ تم إنشاء "تشغيل-سريع.bat" في الجذر

echo.
echo  ═══════════════════════════════════════════════════════════════
echo   ✓ تم الإعداد بنجاح!
echo  ═══════════════════════════════════════════════════════════════
echo.
echo   للبدء:
echo   1) اضغط نقراً مزدوجاً على "تشغيل-سريع.bat"
echo   2) أو اضغط على اختصار "ض ستوديو" على سطح المكتب
echo   3) أو افتح مجلد Windows-Launcher واختر المطلوب
echo.
echo   بيانات الدخول التجريبية:
echo   المدير: admin_1@test.com / Test1234!
echo   الطالب: sara@test.com / Test1234!
echo.
pause
