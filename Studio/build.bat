@echo off
title ض استديو - بناء المشروع
echo ==========================================
echo    ض استديو - Daad Studio Build
echo ==========================================
echo.

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [خطأ] CMake غير مثبت!
    pause
    exit /b 1
)

if not exist "build" mkdir build
cd build

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

cd ..

echo.
echo ==========================================
echo    تم البناء بنجاح!
echo ==========================================
echo.
echo   build\bin\daad-compiler.exe    - أداة سطر الأوامر
echo   build\bin\DaadTests.exe        - اختبارات الوحدة
echo.
pause
