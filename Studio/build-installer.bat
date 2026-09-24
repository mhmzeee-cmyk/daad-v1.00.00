@echo off
title ض استديو - بناء المثبّت
echo ==========================================
echo    ض استديو - Daad Studio Installer Builder
echo ==========================================
echo.

REM التحقق من NSIS
where makensis >nul 2>&1
if %errorlevel% neq 0 (
    echo [خطأ] NSIS غير مثبت!
    echo يرجى تثبيت NSIS من: https://nsis.sourceforge.io/
    pause
    exit /b 1
)

echo [1/3] بناء المشروع...
call build.bat
if %errorlevel% neq 0 (
    echo [خطأ] فشل بناء المشروع!
    pause
    exit /b 1
)

echo [2/3] إنشاء صور المثبّت...
REM يمكن إنشاء الصور هنا

echo [3/3] بناء المثبّت...
cd installer
makensis installer.nsi
if %errorlevel% neq 0 (
    echo [خطأ] فشل بناء المثبّت!
    pause
    exit /b 1
)

echo.
echo ==========================================
echo    تم بناء المثبّت بنجاح!
echo ==========================================
echo.
echo الملف الناتج: installer\DaadStudio-Setup.exe
cd ..
pause