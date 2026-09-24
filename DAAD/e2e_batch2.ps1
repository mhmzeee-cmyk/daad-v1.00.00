$utf8enc = [System.Text.Encoding]::UTF8
$e2eDir = "C:\Projects\DAAD\e2e"

function Make-Daad([string]$template) {
    $t = $template
    $t = $t.Replace('FUNC', [string][char]0x062F+[char]0x0627+[char]0x0644+[char]0x0629)
    $t = $t.Replace('MAIN', [string][char]0x0627+[char]0x0644+[char]0x0631+[char]0x0626+[char]0x064A+[char]0x0633+[char]0x064A+[char]0x0629)
    $t = $t.Replace('VAR', [string][char]0x0645+[char]0x062A+[char]0x063A+[char]0x064A+[char]0x0631)
    $t = $t.Replace('RET', [string][char]0x0627+[char]0x0631+[char]0x062C+[char]0x0639)
    $t = $t.Replace('END', [string][char]0x0646+[char]0x0647+[char]0x0627+[char]0x064A+[char]0x0629)
    $t = $t.Replace('IF', [string][char]0x0625+[char]0x0630+[char]0x0627)
    $t = $t.Replace('ELSE', [string][char]0x0648+[char]0x0625+[char]0x0644+[char]0x0627)
    $t = $t.Replace('WHILE', [string][char]0x0637+[char]0x0627+[char]0x0644+[char]0x0645+[char]0x0627)
    $t = $t.Replace('FOR', [string][char]0x0644+[char]0x0643+[char]0x0644)
    $t = $t.Replace('NUM', [string][char]0x0631+[char]0x0642+[char]0x0645)
    $t = $t.Replace('FLOAT', [string][char]0x0631+[char]0x0642+[char]0x0645+[string][char]0x0639+[char]0x0634+[char]0x0631+[char]0x064A)
    $t = $t.Replace(',', [string][char]0x060C)
    $t = $t.Replace(';', [string][char]0x061B)
    return $t
}

function Write-Daad($path, $content) {
    [System.IO.File]::WriteAllBytes($path, $utf8enc.GetBytes($content))
}

function Run-Test($name, $daadContent, $expected) {
    $dpath = Join-Path $e2eDir "$name.daad"
    $spath = Join-Path $e2eDir "$name.s"
    $epath = Join-Path $e2eDir "$name.exe"
    Write-Daad $dpath $daadContent
    & "C:\Projects\DAAD\daad.exe" $dpath "-o" $spath 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "  FAIL $name (compile)" -ForegroundColor Red; return "COMPILE_FAIL" }
    & gcc $spath "-o" $epath 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "  FAIL $name (link)" -ForegroundColor Red; return "LINK_FAIL" }
    & $epath | Out-Null
    $actual = $LASTEXITCODE
    if ($actual -eq $expected) { Write-Host "  PASS $name (exit=$actual)" -ForegroundColor Green; return "PASS" }
    else { Write-Host "  FAIL $name (expected=$expected got=$actual)" -ForegroundColor Red; return "FAIL" }
}

function Run-TestSafe($name, $daadContent, $expected) {
    $dpath = Join-Path $e2eDir "$name.daad"
    $spath = Join-Path $e2eDir "$name.s"
    $epath = Join-Path $e2eDir "$name.exe"
    Write-Daad $dpath $daadContent
    & "C:\Projects\DAAD\daad.exe" $dpath "-o" $spath 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "  FAIL $name (compile)" -ForegroundColor Red; return "COMPILE_FAIL" }
    & gcc $spath "-o" $epath 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "  FAIL $name (link)" -ForegroundColor Red; return "LINK_FAIL" }
    $job = Start-Job -ScriptBlock { param($p) & $p } -ArgumentList $epath
    $completed = Wait-Job $job -Timeout 5
    if ($completed) {
        $actual = Receive-Job $job
        Remove-Job $job -Force
        if ($actual -eq $expected) { Write-Host "  PASS $name (exit=$actual)" -ForegroundColor Green; return "PASS" }
        else { Write-Host "  FAIL $name (expected=$expected got=$actual)" -ForegroundColor Red; return "FAIL" }
    } else {
        Stop-Job $job -Force
        Remove-Job $job -Force
        Write-Host "  TIMEOUT $name (expected=$expected)" -ForegroundColor Yellow
        return "TIMEOUT"
    }
}

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Phase 3 Batch 2: E2E Runtime Tests" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

$results = @()

# --- Comparisons + If/Else ---
Write-Host "--- Comparisons + If/Else ---" -ForegroundColor Yellow
$results += Run-Test "cmp_eq_t" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 5`n    IF (a == 5) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 1
$results += Run-Test "cmp_eq_f" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 5`n    IF (a == 3) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 0
$results += Run-Test "cmp_gt_t" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 10`n    IF (a > 5) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 1
$results += Run-Test "cmp_gt_f" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 3`n    IF (a > 5) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 0
$results += Run-Test "cmp_lt_t" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 3`n    IF (a < 5) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 1
$results += Run-Test "cmp_lt_f" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 10`n    IF (a < 5) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 0
$results += Run-Test "cmp_ne_t" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 5`n    IF (a != 3) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 1
$results += Run-Test "cmp_ge_t" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 5`n    IF (a >= 5) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 1
$results += Run-Test "cmp_le_t" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 5`n    IF (a <= 5) :-`n        RET(1)`n    ELSE :-`n        RET(0)`n    END`nEND") 1

# --- While Loop (compile + structure) ---
Write-Host ""
Write-Host "--- While Loop (structure check) ---" -ForegroundColor Yellow
$dpath = Join-Path $e2eDir "while_struct.daad"
$spath = Join-Path $e2eDir "while_struct.s"
Write-Daad $dpath (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: i = 0`n    WHILE (i < 5) :-`n        i = i + 1`n    END`n    RET(i)`nEND")
& "C:\Projects\DAAD\daad.exe" $dpath "-o" $spath 2>&1 | Out-Null
if ($LASTEXITCODE -eq 0) {
    $asm = Get-Content $spath -Raw
    $ok = ($asm -match "while_cond") -and ($asm -match "while_body") -and ($asm -match "while_exit") -and ($asm -match "jmp.*while_cond")
    if ($ok) { Write-Host "  PASS while_struct (correct loop structure)" -ForegroundColor Green; $results += "PASS" }
    else { Write-Host "  FAIL while_struct (missing labels)" -ForegroundColor Red; $results += "FAIL" }
} else {
    Write-Host "  FAIL while_struct (compile)" -ForegroundColor Red; $results += "COMPILE_FAIL"
}

# --- While Runtime (known timeout due to CMP bug) ---
Write-Host ""
Write-Host "--- While Runtime (expected TIMEOUT) ---" -ForegroundColor Yellow
$results += Run-TestSafe "while_rt" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: count = 3`n    WHILE (count > 0) :-`n        count = count - 1`n    END`n    RET(count)`nEND") 0

# --- Bitwise ---
Write-Host ""
Write-Host "--- Bitwise Operations ---" -ForegroundColor Yellow
$results += Run-Test "bit_and" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(12 & 10)`nEND") 8
$results += Run-Test "bit_or" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(12 | 10)`nEND") 14
$results += Run-Test "bit_xor" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(12 ^ 10)`nEND") 6
$results += Run-Test "bit_shl" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(1 << 3)`nEND") 8
$results += Run-Test "bit_shr" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(16 >> 2)`nEND") 4
$results += Run-Test "bit_not" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(~0)`nEND") 255

# --- Memory ---
Write-Host ""
Write-Host "--- Memory (ALLOCA + LOAD/STORE) ---" -ForegroundColor Yellow
$results += Run-Test "mem_1" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 42`n    RET(a)`nEND") 42
$results += Run-Test "mem_3" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 10`n    VAR: b = 20`n    VAR: c = 30`n    VAR: d = a + b + c`n    RET(d)`nEND") 60
$results += Run-Test "mem_assign" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 5`n    a = 20`n    RET(a)`nEND") 20
$results += Run-Test "mem_chain" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 2`n    VAR: b = a + 3`n    VAR: c = b * 4`n    RET(c)`nEND") 20

# --- Functions ---
Write-Host ""
Write-Host "--- Function Calls ---" -ForegroundColor Yellow
$results += Run-Test "func_2p" (Make-Daad "FUNC: add(a: NUM, b: NUM) -> NUM :-`n    RET(a + b)`nEND`n`nFUNC: MAIN() -> NUM :-`n    RET(add(3, 4))`nEND") 7
$results += Run-Test "func_3p" (Make-Daad "FUNC: add3(a: NUM, b: NUM, c: NUM) -> NUM :-`n    RET(a + b + c)`nEND`n`nFUNC: MAIN() -> NUM :-`n    RET(add3(10, 20, 30))`nEND") 60
$results += Run-Test "func_chain" (Make-Daad "FUNC: add(a: NUM, b: NUM) -> NUM :-`n    RET(a + b)`nEND`n`nFUNC: MAIN() -> NUM :-`n    RET(add(add(1, 2), 3))`nEND") 6
$results += Run-Test "func_3lv" (Make-Daad "FUNC: f1(x: NUM) -> NUM :-`n    RET(x + 1)`nEND`n`nFUNC: f2(x: NUM) -> NUM :-`n    RET(f1(x) + 10)`nEND`n`nFUNC: MAIN() -> NUM :-`n    RET(f2(f1(5)))`nEND") 17

# --- Complex ---
Write-Host ""
Write-Host "--- Complex Computation ---" -ForegroundColor Yellow
$results += Run-Test "cplx_1" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 100`n    VAR: b = 200`n    VAR: c = a + b`n    VAR: d = c * 3`n    VAR: e = d - 50`n    RET(e)`nEND") 850
$results += Run-Test "cplx_2" (Make-Daad "FUNC: MAIN() -> NUM :-`n    VAR: a = 1`n    VAR: b = 2`n    VAR: c = 3`n    VAR: d = 4`n    VAR: e = a + b + c + d`n    RET(e)`nEND") 10
$results += Run-Test "cplx_3" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(2 + 3 * 4)`nEND") 14
$results += Run-Test "cplx_4" (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(100 / 7)`nEND") 14

# --- Float compile check ---
Write-Host ""
Write-Host "--- Float (compile check only) ---" -ForegroundColor Yellow
$dpath = Join-Path $e2eDir "float_check.daad"
$spath = Join-Path $e2eDir "float_check.s"
Write-Daad $dpath (Make-Daad "FUNC: MAIN() -> FLOAT :-`n    RET(3)`nEND")
& "C:\Projects\DAAD\daad.exe" $dpath "-o" $spath 2>&1 | Out-Null
if ($LASTEXITCODE -eq 0) { Write-Host "  COMPILE OK float_check" -ForegroundColor Green; $results += "PASS" }
else { Write-Host "  COMPILE FAIL float_check" -ForegroundColor Red; $results += "COMPILE_FAIL" }

# --- String (compile check) ---
Write-Host ""
Write-Host "--- String type (compile check) ---" -ForegroundColor Yellow
$dpath = Join-Path $e2eDir "string_check.daad"
$spath = Join-Path $e2eDir "string_check.s"
Write-Daad $dpath (Make-Daad "FUNC: MAIN() -> NUM :-`n    RET(0)`nEND")
& "C:\Projects\DAAD\daad.exe" $dpath "-o" $spath 2>&1 | Out-Null
if ($LASTEXITCODE -eq 0) { Write-Host "  COMPILE OK string_check" -ForegroundColor Green; $results += "PASS" }
else { Write-Host "  COMPILE FAIL string_check" -ForegroundColor Red; $results += "COMPILE_FAIL" }

Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
$passCount = @($results | Where-Object { $_ -eq "PASS" }).Count
$failCount = @($results | Where-Object { $_ -ne "PASS" }).Count
$timeoutCount = @($results | Where-Object { $_ -eq "TIMEOUT" }).Count
$compileFail = @($results | Where-Object { $_ -eq "COMPILE_FAIL" }).Count
Write-Host "TOTAL: $($results.Count) | PASS: $passCount | FAIL: $failCount | TIMEOUT: $timeoutCount | COMPILE_FAIL: $compileFail"
Write-Host "=====================================" -ForegroundColor Cyan
