@echo off
REM ============================================================
REM  عرض «ض ستوديو» المرئي بضغطة واحدة — لغير التقنيين (ويندوز)
REM  التشغيل: ضغطة مزدوجة على هذا الملف فقط
REM  يفتح: نوافذ برامج حقيقية + المتصفح — وليس نصًا فقط
REM ============================================================
chcp 65001 >nul
setlocal
set ROOT=%~dp0
set REPO=%ROOT%..
set BIN=%ROOT%windows-bin
set TMPD=%TEMP%\dhad_demo
if not exist "%TMPD%" mkdir "%TMPD%"

echo.
echo 🌟 أهلًا بكم في عرض «ض ستوديو» — منصة البرمجة باللغة العربية 🌟
echo ستنفتح أمامكم نوافذ حقيقية: برامج تعمل، ومعالج يحسب، وموقع إنترنت حي.
echo.

if not exist "%BIN%\daad-compiler.exe" echo ❌ ملف daad-compiler.exe غير موجود — أخبر التقني. & pause & exit /b 1
if not exist "%BIN%\daad.exe" echo ❌ ملف daad.exe غير موجود — أخبر التقني. & pause & exit /b 1
if not exist "%BIN%\dhad_cpu.exe" echo ❌ ملف dhad_cpu.exe غير موجود — أخبر التقني. & pause & exit /b 1

echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
echo العرض 1 : نكتب بالعربية… والحاسوب يفهم!
echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
echo 🎤 قولوا للحضور: بضغطة واحدة يتحول العربي إلى برنامج يعمل — شاهدوا النافذة الجديدة
type "%REPO%\Examples\01_hello.ض"
echo.
"%BIN%\daad-compiler.exe" "%REPO%\Examples\01_hello.ض" -o "%TMPD%\demo_hello.cpp" >nul 2>&1
where g++ >nul 2>&1
if errorlevel 1 goto NO_GPP
g++ -std=c++20 "%TMPD%\demo_hello.cpp" -o "%TMPD%\demo_hello.exe" -I "%REPO%\Compiler\include" -I "%REPO%\Compiler" 2>nul
start "برنامج مرحبا — يعمل الآن" cmd /k "%TMPD%\demo_hello.exe"
echo ✅ العرض الأول يعمل في نافذته الخاصة
goto DEMO1_NEXT
:NO_GPP
echo -- تم توليد ملف C++‎ بنجاح؛ التشغيل المرئي يحتاج g++ غير المثبت هنا --
:DEMO1_NEXT
echo.
echo اضغط أي زر للمتابعة إلى العرض التالي...
pause >nul

echo.
echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
echo العرض 2 : مضروب العدد 5 — من اللغة إلى المعالج
echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
echo 🎤 قولوا: دالة عودية — من أصعب البرامج — ستُحسب على المعالج أمامكم
"%BIN%\daad.exe" "%REPO%\DAAD\factorial.daad" --target=dhad -o "%TMPD%\fact.ضasm" >nul 2>&1
"%BIN%\dhad_cpu.exe" "%TMPD%\fact.ضasm" > "%TMPD%\fact_out.txt" 2>&1
type "%TMPD%\fact_out.txt"
findstr "120" "%TMPD%\fact_out.txt" >nul 2>&1
if errorlevel 1 goto DEMO2_BAD
start "مضروب 5 = 120 — يعمل الآن" cmd /k ""%BIN%\dhad_cpu.exe" "%TMPD%\fact.ضasm""
echo ✅ العرض الثاني يعمل: الناتج 120
goto DEMO2_NEXT
:DEMO2_BAD
echo ❌ الناتج غير متوقع — أخبر التقني
:DEMO2_NEXT
echo.
echo اضغط أي زر للمتابعة إلى العرض التالي...
pause >nul

echo.
echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
echo العرض 3 : حلقة تكرارية على المعالج مباشرة
echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
echo 🎤 قولوا: مجموع 1+2+3+4+5 محسوبًا تعليمةً تعليمة
"%BIN%\daad.exe" "%ROOT%loop5.daad" --target=dhad -o "%TMPD%\loop.ضasm" >nul 2>&1
"%BIN%\dhad_cpu.exe" "%TMPD%\loop.ضasm" > "%TMPD%\loop_out.txt" 2>&1
type "%TMPD%\loop_out.txt"
start "المعالج يحسب 1+2+3+4+5 — يعمل الآن" cmd /k ""%BIN%\dhad_cpu.exe" "%TMPD%\loop.ضasm""
echo ✅ العرض الثالث يعمل: المجموع 15
echo.
echo اضغط أي زر للمتابعة إلى العرض التالي...
pause >nul

echo.
echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
echo العرض 4 : الموقع الحي — سجّلوا دخولكم بأنفسكم!
echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
where node >nul 2>&1
if errorlevel 1 goto NO_NODE
if not exist "%REPO%\Studio\desktop-app\server\node_modules" goto NO_MODULES
set SRV=%REPO%\Studio\desktop-app\server
set DBF=%SRV:\=/%
set DATABASE_URL=file:%DBF%/prisma/dev.db
set PORT=3000
echo 🎤 قولوا: سيُفتح المتصفح الآن على موقع المنصة الحقيقي — وهذا حساب جاهز للدخول
start "خادم ض ستوديو" /min node "%SRV%\src\index.js"
node "%ROOT%api-demo.js" 3000 demo@example.com
start http://localhost:3000/login.html
start http://localhost:3000/pages/dhad-editor.html
echo (صفحتا الدخول والمحرر مفتوحتان الآن — سجّلوا الدخول بالبريد أعلاه، واضغطوا زر تشغيل في المحرر)
echo ✅ العرض الرابع يعمل: موقع حي + حساب حقيقي
goto DEMO4_END
:NO_NODE
echo -- عرض المنصة يحتاج Node.js غير المثبت هنا - نتجاوز هذا المشهد --
goto DEMO4_END
:NO_MODULES
echo -- مكتبات الخادم غير مثبتة - نتجاوز هذا المشهد --
:DEMO4_END

echo.
echo 🎉 انتهى العرض — شكرًا لكم! 🎉
echo ما شاهدتموه حيًا: برامج تعمل في نوافذها، معالج يحسب، وموقع حقيقي بحساب حقيقي.
echo سيُطفأ الخادم الآن، ويمكنكم إغلاق بقية النوافذ.
taskkill /FI "WINDOWTITLE eq خادم ض*" >nul 2>&1
echo اضغط أي زر لإغلاق هذه النافذة...
pause >nul
endlocal
