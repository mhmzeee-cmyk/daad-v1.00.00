@echo off
chcp 65001 >nul 2>&1
title معالج ض — المحاكي والمجمّع
echo.
echo ╔══════════════════════════════════════════╗
echo ║  معالج ض — المحاكي والمجمّع (8-bit)     ║
echo ╠══════════════════════════════════════════╣
echo ║  arrastrar archivo .ضasm sobre هذا الملف ║
echo ║  أو: شغّل من التيرمنال                   ║
echo ╚══════════════════════════════════════════╝
echo.
if "%~1"=="" (
    echo اسحب ملف .ضasm وأفلته على هذا الملف
    echo.
    echo أو شغّل من التيرمنال:
    echo   dhad_cpu.exe examples\add.ضasm --state
    echo.
    pause
    goto :eof
)
dhad_cpu.exe "%~1" --state
echo.
pause
