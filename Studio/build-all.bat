@echo off
title ض استديو - بناء شامل
echo ══════════════════════════════════════════════════════════════
echo    ض استديو - Daad Studio Complete Build
echo ══════════════════════════════════════════════════════════════
echo.

REM التحقق من الأدوات المطلوبة
echo [0/4] التحقق من الأدوات المطلوبة...

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [خطأ] CMake غير مثبت!
    echo يرجى تشغيل: winget install Kitware.CMake
    pause
    exit /b 1
)
echo   ✓ CMake

where g++ >nul 2>&1
if %errorlevel% neq 0 (
    where cl >nul 2>&1
    if %errorlevel% neq 0 (
        echo [خطأ] مترجم C++ غير مثبت!
        echo يرجى تثبيت MSYS2 أو Visual Studio Build Tools
        pause
        exit /b 1
    )
)
echo   ✓ مترجم C++

where makensis >nul 2>&1
if %errorlevel% neq 0 (
    echo [تحذير] NSIS غير مثبت - سيتم بناء المشروع فقط
    set SKIP_INSTALLER=1
) else (
    echo   ✓ NSIS
)

echo.
echo [1/4] بناء المشروع...
if not exist "build" mkdir build
cd build

REM تحديد المولد حسب المترجم المتوفر
where g++ >nul 2>&1
if %errorlevel% equ 0 (
    cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DDAAD_BUILD_TESTS=ON
) else (
    cmake .. -G "Visual Studio 17 2022" -A x64 -DDAAD_BUILD_TESTS=ON
)

if %errorlevel% neq 0 (
    echo [خطأ] فشل CMake!
    cd ..
    pause
    exit /b 1
)

cmake --build . --config Release --parallel
if %errorlevel% neq 0 (
    echo [خطأ] فشل البناء!
    cd ..
    pause
    exit /b 1
)

echo.
echo [2/4] تشغيل الاختبارات...
ctest --output-on-failure
echo.

cd ..

echo [3/4] بناء أمثلة تجريبية...
if not exist "examples" mkdir examples

echo [4/4] بناء المثبّت...
if "%SKIP_INSTALLER%"=="1" (
    echo تخطي بناء المثبّت NSIS غير مثبت
) else (
    cd installer
    makensis installer.nsi
    cd ..
)

echo.
echo ══════════════════════════════════════════════════════════════
echo    تم البناء بنجاح!
echo ══════════════════════════════════════════════════════════════
echo.
echo الملفات الناتجة:
echo   build\bin\daad-compiler.exe    - أداة سطر الأوامر
echo   build\bin\DaadTests.exe        - اختبارات الوحدة
echo   build\lib\libDaadCore.a        - المكتبة الأساسية
if not "%SKIP_INSTALLER%"=="1" (
    echo   installer\DaadStudio-Setup.exe  - المثبّت
)
echo.
pause