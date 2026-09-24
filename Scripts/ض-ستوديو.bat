@echo off
chcp 65001 >nul 2>&1
title ض ستوديو — Dhad Studio v1.0.0
color 0A

:menu
cls
echo.
echo  ╔══════════════════════════════════════════════════════════════════════╗
echo  ║                                                                      ║
echo  ║                    ض ستوديو — Dhad Studio v1.0.0                   ║
echo  ║              منصة تعليم البرمجة باللغة العربية                      ║
echo  ║                                                                      ║
echo  ╚══════════════════════════════════════════════════════════════════════╝
echo.
echo  اختر أحد الخيارات التالية:
echo.
echo  ┌─────────────────────────────────────────────────────────────────────┐
echo  │                                                                     │
echo  │   1)  تشغيل المنصة الكاملة (سيرفر + واجهة ويب)                    │
echo  │   2)  تشغيل تطبيق سطح المكتب (Electron)                          │
echo  │   3)  تشغيل مترجم ض (C++ Compiler)                                │
echo  │   4)  تشغيل معالج ض (CPU Assembler + Simulator)                   │
echo  │   5)  تشغيل واجهة التيرمنال                                       │
echo  │   6)  تشغيل واجهة Qt الرسومية                                     │
echo  │                                                                     │
echo  │   ─────────────────────────────────────────────────────────         │
echo  │                                                                     │
echo  │   7)  تثبيت إضافة VS Code                                         │
echo  │   8)  فتح محرر الكود على الويب                                    │
echo  │   9)  عرض документة المشروع                                        │
echo  │                                                                     │
echo  │   ─────────────────────────────────────────────────────────         │
echo  │                                                                     │
echo  │   0)  خروج                                                         │
echo  │                                                                     │
echo  └─────────────────────────────────────────────────────────────────────┘
echo.
set /p choice="  اختر رقم: "

if "%choice%"=="1" goto full_server
if "%choice%"=="2" goto electron
if "%choice%"=="3" goto compiler
if "%choice%"=="4" goto cpu
if "%choice%"=="5" goto terminal
if "%choice%"=="6" goto qt_gui
if "%choice%"=="7" goto vscode
if "%choice%"=="8" goto web_editor
if "%choice%"=="9" goto docs
if "%choice%"=="0" goto exit
goto menu

:full_server
cls
echo  جاري تشغيل المنصة الكاملة...
echo.
call "%~dp0Dhad-Studio-Full\Dhad-Studio.bat"
goto menu

:electron
cls
echo  جاري تشغيل تطبيق سطح المكتب...
echo.
cd /d "%~dp0Dhad-Studio-Electron"
if not exist "node_modules" (
    echo  جاري تثبيت المكتبات...
    call npm install
)
call npm start
goto menu

:compiler
cls
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║                  مترجم ض — Daad Compiler                       ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.
echo  الاستخدام: daad-compiler.exe ملف.ض -o output.cpp
echo.
set /p srcfile="  أدخل مسار ملف .ض: 
if "%srcfile%"=="" goto compiler
set /p outfile="  أدخل مسار الإخراج (output.cpp): 
if "%outfile%"=="" set outfile=output.cpp
echo.
echo  جاري الترجمة...
"%~dp0Daad-Compiler\daad-compiler.exe" "%srcfile%" -o "%outfile%"
echo.
if %errorlevel%==0 (
    echo  ✓ تم الترجمة بنجاح إلى: %outfile%
) else (
    echo  ✗ فشلت الترجمة
)
echo.
pause
goto compiler

:cpu
cls
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║                  معالج ض — Dhad CPU                            ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.
echo  اختر طريقة التشغيل:
echo.
echo  1)  تشغيل ملف (with state)
echo  2)  تشغيل ملف (with hex)
echo  3)  تشغيل ملف (debug mode)
echo  4)  إضافة ملف пример جديد
echo  5)  العودة
echo.
set /p cpuchoice="  اختر رقم: "

if "%cpuchoice%"=="1" goto cpu_run_state
if "%cpuchoice%"=="2" goto cpu_run_hex
if "%cpuchoice%"=="3" goto cpu_run_debug
if "%cpuchoice%"=="4" goto cpu_add_example
if "%cpuchoice%"=="5" goto menu
goto cpu

:cpu_run_state
set /p asmfile="  أدخل مسار ملف .ضasm: 
if "%asmfile%"=="" goto cpu
"%~dp0desktop\dhad_cpu.exe" "%asmfile%" --state
echo.
pause
goto cpu

:cpu_run_hex
set /p asmfile="  أدخل مسار ملف .ضasm: 
if "%asmfile%"=="" goto cpu
"%~dp0desktop\dhad_cpu.exe" "%asmfile%" --hex
echo.
pause
goto cpu

:cpu_run_debug
set /p asmfile="  أدخل مسار ملف .ضasm: 
if "%asmfile%"=="" goto cpu
"%~dp0desktop\dhad_cpu.exe" "%asmfile%" --debug
echo.
pause
goto cpu

:cpu_add_example
echo.
echo  سحب ملف .ضasm وأفلته هنا ثم اضغط Enter:
set /p dragfile=""
if "%dragfile%"=="" goto cpu
"%~dp0desktop\dhad_cpu.exe" "%dragfile%" --state
echo.
pause
goto cpu

:terminal
cls
echo  جاري تشغيل واجهة التيرمنال...
echo.
"%~dp0cpu-original\dhad_live.exe"
pause
goto menu

:qt_gui
cls
echo  جاري تشغيل الواجهة الرسومية...
echo.
"%~dp0cpu-original\gui\dhad_gui.exe"
pause
goto menu

:vscode
cls
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║                  تثبيت إضافة VS Code                           ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.
where code >nul 2>&1
if %errorlevel% neq 0 (
    echo  ✗ VS Code غير مثبّت أو غير موجود في PATH
    echo  حمّله من: https://code.visualstudio.com
    echo.
    pause
    goto menu
)
echo  جاري تثبيت الإضافة...
code --install-extension "%~dp0VSCode-Extension"
echo.
echo  ✓ تم التثبيت بنجاح
echo  اضغط F9 داخل ملف .ض لترجمته إلى C++
echo.
pause
goto menu

:web_editor
cls
echo  جاري فتح محرر الكود على الويب...
start "" "http://localhost:3000/pages/dhad-editor.html"
if %errorlevel% neq 0 (
    echo  لم يتم فتح المتصفح. تأكد من تشغيل السيرفر أولاً (الخيار 1)
    pause
)
goto menu

:docs
cls
start "" "%~dp0README.md"
goto menu

:exit
echo.
echo  شكراً لاستخدام ض ستوديو!
echo.
timeout /t 2 >nul
exit
