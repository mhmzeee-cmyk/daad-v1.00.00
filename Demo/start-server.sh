#!/bin/bash
# ============================================================
#  تشغيل سيرفر «ض ستوديو» بضغطة واحدة
#  بعده افتح المتصفح على:  http://localhost:3000/login.html
#  للإيقاف: أغلق هذه النافذة (Ctrl+C)
# ============================================================
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRV="$ROOT/Studio/desktop-app/server"
PORT=3000

command -v node >/dev/null 2>&1 || { echo "❌ Node.js غير مثبت — أخبر التقني."; read -r -p "اضغط Enter للإغلاق... " _; exit 1; }
[ -f "$SRV/prisma/dev.db" ] || { echo "❌ قاعدة البيانات غير موجودة — أخبر التقني."; read -r -p "اضغط Enter للإغلاق... " _; exit 1; }
if curl -s --max-time 3 "http://localhost:$PORT/health" > /dev/null 2>&1; then
  echo "✅ الخادم يعمل أصلًا — افتح: http://localhost:$PORT/login.html"
  read -r -p "اضغط Enter للإغلاق... " _
  exit 0
fi

echo "⏳ جارٍ تشغيل الخادم..."
echo ""
echo "  🌐 الموقع: http://localhost:$PORT/login.html"
echo "  📝 المحرر:  http://localhost:$PORT/pages/dhad-editor.html"
echo "  👤 البريد:  demo@example.com   |   🔑 كلمة المرور:  Test1234!"
echo ""
echo "  ⚠️  أبقِ هذه النافذة مفتوحة — إغلاقها يُطفئ الموقع."
echo ""
DATABASE_URL="file:$SRV/prisma/dev.db" PORT=$PORT NODE_ENV=development node "$SRV/src/index.js"
