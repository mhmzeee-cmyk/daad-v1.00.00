@echo off
chcp 65001 >nul 2>&1
title معالج ض — Dhad CPU Assembler & Simulator
color 0B

:loop
cls
echo.
echo  ╔══════════════════════════════════════════════════════════════════╗
echo  ║                  معالج ض — Dhad CPU v2.0                       ║
echo  ║         مجمّع + محاكي + مصحح (8-بت)                            ║
echo  ╚══════════════════════════════════════════════════════════════════╝
echo.
echo  اختر:
echo.
echo  1)  تشغيل ملف
echo  2)  سحب ملف وأفلته هنا (Drag & Drop)
echo  3)  عرض جميع أمثلة
echo  4)  اختبار شامل
echo  5)  العودة
echo.
set /p choice="  اختر رقم: "

if "%choice%"=="1" goto run_file
if "%choice%"=="2" goto drag_drop
if "%choice%"=="3" goto examples
if "%choice%"=="4" goto test_all
if "%choice%"=="5" goto end
goto loop

:run_file
echo.
echo  أدخل مسار ملف .ضasm:
set /p asmfile=""
if "%asmfile%"=="" goto loop
set asmfile=%asmfile:"=%
echo.
echo  اختر طريقة العرض:
echo  1)  --state  (حالة المعالج الكاملة)
echo  2)  --hex    (الكود السداسي عشري)
echo  3)  --debug  (تتبع تعليمة بتعليقمة)
echo  4)  --bin    (حفظ كملف binary)
echo.
set /p display="  اختر: "
if "%display%"=="1" set flag=--state
if "%display%"=="2" set flag=--hex
if "%display%"=="3" set flag=--debug
if "%display%"=="4" set flag=--bin
if "%display%"=="" set flag=--state
echo.
"%~dp0desktop\dhad_cpu.exe" "%asmfile%" %flag%
echo.
pause
goto loop

:drag_drop
cls
echo.
echo  ═══════════════════════════════════════════════════════════════
echo   سحب ملف .ضasm وأفلته هنا ثم اضغط Enter
echo  ═══════════════════════════════════════════════════════════════
set /p dragfile=""
if "%dragfile%"=="" goto loop
set dragfile=%dragfile:"=%
echo.
"%~dp0desktop\dhad_cpu.exe" "%dragfile%" --state
echo.
pause
goto loop

:examples
cls
echo.
echo  ═══════════════════════════════════════════════════════════════
echo   الأمثلة المتاحة:
echo  ═══════════════════════════════════════════════════════════════
echo.
echo   1)  add.ضasm           — جمع رقمين (3+5=8)
echo   2)  countdown.ضasm     — عد تنازلي (5 إلى 1)
echo   3)  factorial.ضasm     — مضروب
echo   4)  function.ضasm      — نداء دالة (ضرب)
echo   5)  hello.ضasm         — طباعة أحرف
echo   6)  logic.ضasm         — عمليات منطقية
echo   7)  test_full.ضasm     — اختبار شامل
echo.
set /p exchoice="  اختر رقم: "

if "%exchoice%"=="1" "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\add.ضasm" --state
if "%exchoice%"=="2" "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\countdown.ضasm" --state
if "%exchoice%"=="3" "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\factorial.ضasm" --state
if "%exchoice%"=="4" "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\function.ضasm" --state
if "%exchoice%"=="5" "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\hello.ضasm" --state
if "%exchoice%"=="6" "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\logic.ضasm" --state
if "%exchoice%"=="7" "%~dp0desktop\dhad_cpu.exe" "%~dp0desktop\examples\test_full.ضasm" --state
echo.
pause
goto loop

:test_all
cls
echo  جاري تشغيل جميع الاختبارات...
echo.
set /a pass=0
set /a fail=0
set /a total=0

for %%f in ("%~dp0cpu-original\tests\*.ضasm") do (
    set /a total+=1
    "%~dp0desktop\dhad_cpu.exe" "%%f" --state >nul 2>&1
    if !errorlevel!==0 (
        set /a pass+=1
        echo   ✓ %%~nxf
    ) else (
        set /a fail+=1
        echo   ✗ %%~nxf
    )
)

echo.
echo  ═══════════════════════════════════════════════════════════════
echo   النتيجة: %pass% نجح / %fail% فشل / %total% إجمالي
echo  ═══════════════════════════════════════════════════════════════
echo.
pause
goto loop

:end
exit
