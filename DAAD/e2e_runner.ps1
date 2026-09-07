$ErrorActionPreference = "SilentlyContinue"
$results = @()
$e2eDir = "C:\Projects\DAAD\e2e"
$daad = "C:\Projects\DAAD\daad.exe"

function Write-Daad($path, $content) {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($content + "`n")
    [System.IO.File]::WriteAllBytes($path, $bytes)
}

function Run-Test($name, $daadContent, $expected) {
    $dpath = Join-Path $e2eDir "$name.daad"
    $spath = Join-Path $e2eDir "$name.s"
    $epath = Join-Path $e2eDir "$name.exe"
    
    Write-Daad $dpath $daadContent
    
    & $daad $dpath "-o" $spath 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  FAIL $name (compile)" -ForegroundColor Red
        return "COMPILE_FAIL"
    }
    
    & gcc $spath "-o" $epath 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  FAIL $name (link)" -ForegroundColor Red
        return "LINK_FAIL"
    }
    
    & $epath | Out-Null
    $actual = $LASTEXITCODE
    if ($actual -eq $expected) {
        Write-Host "  PASS $name (exit=$actual)" -ForegroundColor Green
        return "PASS"
    } else {
        Write-Host "  FAIL $name (expected=$expected got=$actual)" -ForegroundColor Red
        return "FAIL"
    }
}

Write-Host "=== Part 2: Extended Runtime Tests ===" -ForegroundColor Cyan
Write-Host ""

# Fix: Function call with multiply
Write-Host "--- Fix: Function Call (multiply) ---" -ForegroundColor Yellow
$r = Run-Test "t01_func_mul" "function mul(a: number, b: number) -> number :`n    return(a * b)`nend`n`nfunction main() -> number :`n    return(mul(5, 3))`nend" 15

# Let me just create the .daad files directly with Arabic content using UTF-8

# t02: If-else == true
Write-Host ""
Write-Host "--- Comparisons + If/Else ---" -ForegroundColor Yellow

$code = [char]0x062F + [char]0x0627 + [char]0x0644 + [char]0x0629 + ": " + [char]0x0627 + [char]0x0644 + [char]0x0631 + [char]0x0626 + [char]0x064A + [char]0x0633 + [char]0x064A + [char]0x0629 + "() -> " + [char]0x0631 + [char]0x0642 + [char]0x0645 + " :-`n    " + [char]0x0645 + [char]0x062A + [char]0x063A + [char]0x064A + [char]0x0631 + ": " + [char]0x0623 + " = 5`n    " + [char]0x0625 + [char]0x0630 + [char]0x0627 + " (" + [char]0x0623 + " == 5) :-`n        " + [char]0x0627 + [char]0x0631 + [char]0x062C + [char]0x0639 + "(1)`n    " + [char]0x0648 + [char]0x0625 + [char]0x0644 + [char]0x0627 + " :-`n        " + [char]0x0627 + [char]0x0631 + [char]0x062C + [char]0x0639 + "(0)`n    " + [char]0x0646 + [char]0x0647 + [char]0x0627 + [char]0x064A + [char]0x0629 + "`n" + [char]0x0646 + [char]0x0647 + [char]0x0627 + [char]0x064A + [char]0x0629

Write-Host "Arabic code length: $($code.Length)"
Write-Host "First chars: $([int][char]$code[0]) $([int][char]$code[1]) $([int][char]$code[2])"
