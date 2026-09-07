@echo off
chcp 65001 >nul 2>&1
title مترجم ض — Daad Compiler
color 0B

:loop
cls
echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║                  مترجم ض — Daad Compiler v1.0.0               ║
echo  ║         يترجم ملفات .ض إلى كود C++20                          ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.
echo  اختر:
echo.
echo  1)  ترجمة ملف
echo  2)  ترجمة جميع الأمثلة
echo  3)  فتح مجلد الأمثلة
echo  4)  العودة للقائمة الرئيسية
echo.
set /p choice="  اختر رقم: "

if "%choice%"=="1" goto translate
if "%choice%"=="2" goto translate_all
if "%choice%"=="3" goto open_examples
if "%choice%"=="4" goto end
goto loop

:translate
echo.
echo  ═══════════════════════════════════════════════════════════════
echo   سحب ملف .ض وأفلته هنا ثم اضغط Enter
echo  ═══════════════════════════════════════════════════════════════
set /p srcfile=""
if "%srcfile%"=="" goto loop

:: إزالة علامات الاقتباس إن وُجدت
set srcfile=%srcfile:"=%

:: تحديد ملف الإخراج
set outfile=%srcfile:.ض=.cpp%
set outfile=%outfile:.daad=.cpp%
if "%outfile%"=="%srcfile%" set outfile=%srcfile%.cpp

echo.
echo  الملف المصدر: %srcfile%
echo  ملف الإخراج: %outfile%
echo.
echo  جاري الترجمة...

"%~dp0Daad-Compiler\daad-compiler.exe" "%srcfile%" -o "%outfile%"

if %errorlevel%==0 (
    echo.
    echo  ═══════════════════════════════════════════════════════════════
    echo   ✓ تم الترجمة بنجاح!
    echo   الملف المترجم: %outfile%
    echo  ═══════════════════════════════════════════════════════════════
) else (
    echo.
    echo  ═══════════════════════════════════════════════════════════════
    echo   ✗ فشلت الترجمة - تحقق من الملف
    echo  ═══════════════════════════════════════════════════════════════
)
echo.
pause
goto loop

:translate_all
cls
echo  جاري ترجمة جميع الأمثلة...
echo.

set examples=%~dp0Daad-Compiler\examples
set output=%~dp0Daad-Compiler\examples\translated
if not exist "%output%" mkdir "%output%"

for %%f in ("%examples%\*.ض") do (
    echo  ترجمة: %%~nxf
    set "base=%%~nxf"
    set "cppname=!base:ض=.cpp!"
    "%~dp0Daad-Compiler\daad-compiler.exe" "%%f" -o "%output%\!cppname!" 2>nul
)

echo.
echo  ✓ تم ترجمة جميع الأمثلة
echo  المجلد: %output%
echo.
pause
goto loop

:open_examples
explorer "%~dp0Daad-Compiler\examples"
goto loop

:end
exit
