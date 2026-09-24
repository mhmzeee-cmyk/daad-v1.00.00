@echo off
REM ============================================================
REM  validate-windows11.bat — قبول Windows 11 الحقيقي (يُشغَّل على الجهاز فقط)
REM  يشغّل كل تنفيذي فعليًا ويفحص المخرجات ورموز الخروج — لا PASS وهمي.
REM ============================================================
chcp 65001 >nul
setlocal EnableDelayedExpansion
set DIR=%~dp0
set PASS=0
set FAIL=0

echo ========================================
echo  Dhad Studio — Windows 11 Acceptance
echo ========================================
echo.

call :CHECK hello.exe "مرحبا" 0 "hello"
call :CHECK arithmetic.exe "15" 0 "arithmetic"
call :CHECK factorial.exe "120" 0 "factorial"

echo.
echo ========================================
echo  PASS: %PASS%   FAIL: %FAIL%
echo ========================================
if %FAIL%==0 (
  echo  RESULT: PASS — Windows 11 VERIFIED
) else (
  echo  RESULT: FAIL — see lines above
)
echo اضغط أي زر للإغلاق...
pause >nul
endlocal
exit /b %FAIL%

:CHECK
set EXE=%~1
set MARK=%~2
set EXP=%~3
set NAME=%~4
if not exist "%DIR%%EXE%" (
  echo [FAIL] %NAME% — الملف غير موجود: %EXE%
  set /a FAIL+=1
  exit /b 1
)
"%DIR%%EXE%" > "%TEMP%\dhad_val11.txt" 2>&1
set CODE=!ERRORLEVEL!
findstr /C:"%MARK%" "%TEMP%\dhad_val11.txt" >nul 2>&1
if errorlevel 1 (
  echo [FAIL] %NAME% — العلامة غير موجودة في المخرجات ^(رمز الخروج !CODE!^)
  set /a FAIL+=1
  exit /b 1
)
if not "!CODE!"=="%EXP%" (
  echo [FAIL] %NAME% — رمز الخروج !CODE! والمتوقع %EXP%
  set /a FAIL+=1
  exit /b 1
)
echo [PASS] %NAME% — المخرجات ورمز الخروج صحيحان
set /a PASS+=1
exit /b 0
