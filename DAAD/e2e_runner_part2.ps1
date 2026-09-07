$results = @()

function Write-Test {
    param($name, $daad, $expected)
    
    $daadPath = "C:\Projects\DAAD\e2e_test.daad"
    $sPath = "C:\Projects\DAAD\e2e_test.s"
    $exePath = "C:\Projects\DAAD\e2e_test.exe"
    
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($daad + "`n")
    [System.IO.File]::WriteAllBytes($daadPath, $bytes)
    
    $compOut = & "C:\Projects\DAAD\daad.exe" $daadPath "-o" $sPath 2>&1
    if ($LASTEXITCODE -ne 0) {
        $firstErr = ($compOut | Select-Object -First 1)
        Write-Host "  FAIL $name (compile error)" -ForegroundColor Red
        return @{ name=$name; status="COMPILE_FAIL"; expected=$expected; actual="COMPILE_ERROR" }
    }
    
    & gcc $sPath "-o" $exePath 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  FAIL $name (link error)" -ForegroundColor Red
        return @{ name=$name; status="LINK_FAIL"; expected=$expected; actual="LINK_ERROR" }
    }
    
    & $exePath | Out-Null
    $exitCode = $LASTEXITCODE
    if ($exitCode -eq $expected) {
        Write-Host "  PASS $name (exit=$exitCode)" -ForegroundColor Green
        return @{ name=$name; status="PASS"; expected=$expected; actual=$exitCode }
    } else {
        Write-Host "  FAIL $name (expected=$expected, got=$exitCode)" -ForegroundColor Red
        return @{ name=$name; status="FAIL"; expected=$expected; actual=$exitCode }
    }
}

Write-Host "=== Part 2: Fixed Tests ===" -ForegroundColor Cyan

# Fix: use correct Arabic function name
$r = Write-Test "e2e_func_mul" "دالة: اضرب(أ: رقم، ب: رقم) -> رقم :-`n    ارجع(أ * ب)`nنهاية`n`nدالة: الرئيسية() -> رقم :-`n    ارجع(اضرب(5، 3))`nنهاية" 15; $results += $r

$r = Write-Test "e2e_cmp_eq_true" "دالة:女主角() -> رقم :-`n    متغير: أ = 5`n    اذا (أ == 5) :-`n        ارجع(1)`n    والا :-`n        ارجع(0)`n    نهاية`nنهاية" 1; $results += $r
