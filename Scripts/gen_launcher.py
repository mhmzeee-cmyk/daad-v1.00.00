#!/usr/bin/env python3
"""Generate Demo-Dhad-Studio.bat (windows-1256) that materializes Arabic example
files via certutil base64 decode on first run. Pure-ASCII structure + base64."""
import os, base64

PKG = '/tmp/dhad-win7/pkg'
OUT = '/tmp/dhad-win7/Demo-Dhad-Studio.bat'

# (target subdir, filename) in install order
FILES = []
for fn in sorted(os.listdir(os.path.join(PKG, 'examples'))):
    if fn.endswith('.ض'):
        FILES.append(('examples', fn))
for fn in sorted(os.listdir(os.path.join(PKG, 'cpu-examples'))):
    if fn.endswith('.ضasm'):
        FILES.append(('cpu-examples', fn))

L = []
L.append('@echo off')
L.append('chcp 65001 >nul 2>&1')
L.append('title Dhad Studio - Logical Demo')
L.append('set ROOT=%~dp0')
L.append('if not exist "%ROOT%examples" mkdir "%ROOT%examples"')
L.append('if not exist "%ROOT%cpu-examples" mkdir "%ROOT%cpu-examples"')
L.append('call :seed >nul 2>&1')
L.append('goto menu')
L.append(':seed')
for sub, fn in FILES:
    data = open(os.path.join(PKG, sub, fn), 'rb').read()
    b64 = base64.b64encode(data).decode('ascii')
    tmp = 'dhad_seed.tmp'
    L.append('if not exist "%%ROOT%%%s\\%s" (' % (sub, fn))
    first = True
    for i in range(0, len(b64), 64):
        op = '>' if first else '>>'
        first = False
        L.append('  echo %s %s "%%TEMP%%\\%s"' % (b64[i:i+64], op, tmp))
    L.append('  certutil -decode -f "%%TEMP%%\\%s" "%%ROOT%%%s\\%s" >nul' % (tmp, sub, fn))
    L.append('  del "%%TEMP%%\\%s"' % tmp)
    L.append(')')
L.append('goto :eof')
L.append(':menu')
L.append('cls')
L.append('echo.')
L.append('echo  ==========================================')
L.append('echo   Dhad Studio v1.0.0 - Interactive Demo')
L.append('echo  ==========================================')
L.append('echo.')
L.append('echo   1 - Compile hello live (.dad to .cpp)')
L.append('echo   2 - Run hello.exe')
L.append('echo   3 - Run all demo programs')
L.append('echo   4 - CPU demo (assembler + simulator)')
L.append('echo   5 - Open examples folder')
L.append('echo   0 - Exit')
L.append('echo.')
L.append('set /p CH=Choose [0-5]: ')
L.append('if "%CH%"=="1" goto compile')
L.append('if "%CH%"=="2" goto runhello')
L.append('if "%CH%"=="3" goto runall')
L.append('if "%CH%"=="4" goto cpu')
L.append('if "%CH%"=="5" start "" "%ROOT%examples" & goto menu')
L.append('if "%CH%"=="0" goto :eof')
L.append('goto menu')
L.append(':compile')
L.append('echo.')
L.append('"%ROOT%bin\\daad-compiler.exe" "%ROOT%examples\\01_hello.ض" -o "%ROOT%examples\\hello_live.cpp"')
L.append('echo.')
L.append('echo Generated: examples\\hello_live.cpp')
L.append('pause')
L.append('goto menu')
L.append(':runhello')
L.append('echo.')
L.append('"%ROOT%bin\\01_hello.exe"')
L.append('echo.')
L.append('pause')
L.append('goto menu')
L.append(':runall')
L.append('echo.')
L.append('for %%F in (01_hello 02_arithmetic 03_conditions 04_loops 05_functions 06_factorial 07_fibonacci) do (')
L.append('  echo ----- %%F -----')
L.append('  "%ROOT%bin\\%%F.exe"')
L.append('  echo.')
L.append(')')
L.append('pause')
L.append('goto menu')
L.append(':cpu')
L.append('echo.')
L.append('"%ROOT%bin\\dhad_cpu.exe" "%ROOT%cpu-examples\\add.ضasm"')
L.append('echo.')
L.append('pause')
L.append('goto menu')

text = '\r\n'.join(L) + '\r\n'
# Must be encodable in cp1256 (Arabic + ASCII). Menu is ASCII; filenames Arabic.
raw = text.encode('cp1256')
open(OUT, 'wb').write(raw)
print('files embedded:', len(FILES), '| bat bytes:', len(raw))
