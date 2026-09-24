@echo off
chcp 65001 >nul 2>&1
title اختبار جميع المكونات — Dhad Studio
color 0B

cls
echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║              اختبار جميع مكونات ض ستوديو                       ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.

set /a pass=0
set /a fail=0

echo  ─── 1. فحص Node.js ───
where node >nul 2>&1
if %errorlevel%==0 (
    echo   ✓ Node.js موجود
    node --version
    set /a pass+=1
) else (
    echo   ✗ Node.js غير مثبّت (مطلوب للسيرفر)
    set /a fail+=1
)

echo.
echo  ─── 2. فحص المترجم ───
if exist "%~dp0Daad-Compiler\daad-compiler.exe" (
    echo   ✓ المترجم موجود
    "%~dp0Daad-Compiler\daad-compiler.exe" --version 2>nul
    set /a pass+=1
) else (
    echo   ✗ المترجم غير موجود
    set /a fail+=1
)

echo.
echo  ─── 3. فحص معالج ض ───
if exist "%~dp0desktop\dhad_cpu.exe" (
    echo   ✓ معالج ض موجود
    set /a pass+=1
) else (
    echo   ✗ معالج ض غير موجود
    set /a fail+=1
)

echo.
echo  ─── 4. اختبار المترجم بملف تجريبي ───
if exist "%~dp0Daad-Compiler\examples\basic.ض" (
    "%~dp0Daad-Compiler\daad-compiler.exe" "%~dp0Daad-Compiler\examples\basic.ض" -o "%temp%\test_daad.cpp" 2>nul
    if %errorlevel%==0 (
        echo   ✓ الترجمة تعمل بنجاح
        set /a pass+=1
    ) else (
        echo   ✗ الترجمة فشلت
        set /a fail+=1
    )
) else (
    echo   - ملف تجريبي غير موجود (متخطي)
)

echo.
echo  ─── 5. اختبار المعالج بملف تجريبي ───
if exist "%~dp0desktop\examples\add.ضasm" (
    "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\add.ضasm" --state >nul 2>&1
    if %errorlevel%==0 (
        echo   ✓ المعالج يعمل بنجاح
        set /a pass+=1
    ) else (
        echo   ✗ المعالج فشل
        set /a fail+=1
    )
) else (
    echo   - ملف تجريبي غير موجود (متخطي)
)

echo.
echo  ─── 6. فحص ملفات السيرفر ───
if exist "%~dp0Dhad-Studio-Full\server\src\index.js" (
    echo   ✓ ملفات السيرفر موجودة
    set /a pass+=1
) else (
    echo   ✗ ملفات السيرفر غير موجودة
    set /a fail+=1
)

echo.
echo  ─── 7. فحص ملفات الواجهة ───
if exist "%~dp0Dhad-Studio-Web\index.html" (
    echo   ✓ ملفات الواجهة موجودة
    set /a pass+=1
) else (
    echo   ✗ ملفات الواجهة غير موجودة
    set /a fail+=1
)

echo.
echo  ─── 8. فحص إضافة VS Code ───
if exist "%~dp0VSCode-Extension\package.json" (
    echo   ✓ إضافة VS Code موجودة
    set /a pass+=1
) else (
    echo   ✗ إضافة VS Code غير موجودة
    set /a fail+=1
)

echo.
echo  ═══════════════════════════════════════════════════════════════
echo   النتيجة النهائية: %pass% نجح / %fail% فشل
echo  ═══════════════════════════════════════════════════════════════
echo.

if %fail%==0 (
    echo   ✓ جميع المكونات جاهزة للعمل!
) else (
    echo   ⚠ بعض المكونات تحتاج تثبيت
)
echo.
pause
