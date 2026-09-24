@echo off
chcp 65001 >nul 2>&1
title تثبيت إضافة VS Code — Dhad Extension
color 0B

echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║              تثبيت إضافة لغة ض لـ VS Code                      ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.

:: فحص VS Code
where code >nul 2>&1
if %errorlevel% neq 0 (
    echo  ╔══════════════════════════════════════════════════════════════╗
    echo  ║  VS Code غير مثبّت!                                         ║
    echo  ║  يرجى تثبيته من: https://code.visualstudio.com             ║
    echo  ╚══════════════════════════════════════════════════════════════╝
    echo.
    echo  هل تريد فتح صفحة التحميل؟ (Y/N)
    set /p open=""
    if /i "%open%"=="Y" start https://code.visualstudio.com/download
    pause
    exit /b 1
)

echo  VS Code موجود
code --version
echo.

echo  جاري تثبيت الإضافة...
code --install-extension "%~dp0VSCode-Extension"

if %errorlevel%==0 (
    echo.
    echo  ═══════════════════════════════════════════════════════════════
    echo   ✓ تم التثبيت بنجاح!
    echo.
    echo   المميزات:
    echo   • تلوين صيغة الكود العربي
    echo   • قصاصات كود جاهزة
    echo   • اضغط F9 لترجمة ملف .ض إلى C++
    echo  ═══════════════════════════════════════════════════════════════
) else (
    echo.
    echo  ✗ فشل التثبيت
    echo  تأكد من أن VS Code مفتوح أو أعد تشغيل الكمبيوتر
)
echo.
pause
