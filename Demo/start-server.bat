@echo off
REM ============================================================
REM  تشغيل سيرفر «ض ستوديو» بضغطة واحدة (ويندوز)
REM  بعده افتح المتصفح على:  http://localhost:3000/login.html
REM  للإيقاف: أغلق هذه النافذة
REM ============================================================
chcp 65001 >nul
setlocal
set ROOT=%~dp0
set SRV=%ROOT%..\Studio\desktop-app\server
set PORT=3000

where node >nul 2>&1
if errorlevel 1 echo ❌ برنامج Node.js غير مثبت — أخبر التقني. & pause & exit /b 1
if not exist "%SRV%\prisma\dev.db" echo ❌ قاعدة البيانات غير موجودة — أخبر التقني. & pause & exit /b 1

echo ⏳ جارٍ تشغيل الخادم...
echo.
echo   🌐 الموقع: http://localhost:%PORT%/login.html
echo   📝 المحرر:  http://localhost:%PORT%/pages/dhad-editor.html
echo   👤 البريد:  demo@example.com   ^|   🔑 كلمة المرور:  Test1234!
echo.
echo   ⚠️  أبقِ هذه النافذة مفتوحة — إغلاقها يُطفئ الموقع.
echo.
set DBF=%SRV:\=/%
set DATABASE_URL=file:%DBF%/prisma/dev.db
set PORT=3000
node "%SRV%\src\index.js"
endlocal
