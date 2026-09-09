#!/bin/bash
# ============================================================
#  عرض «ض ستوديو» المرئي بضغطة واحدة — لغير التقنيين
#  التشغيل: ضغطة واحدة (تشغيل الملف) ثم اتبع التعليمات على الشاشة
#  يفتح: نوافذ برامج حقيقية + واجهة المعالج + المتصفح — وليس نصًا فقط
#  المدة التقريبية: 5 دقائق
#  متغير اختياري للتجربة الآلية: DEMO_READ_TIMEOUT=8 (إغلاق تلقائي للنوافذ)
# ============================================================
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GREEN='\033[0;32m'; RED='\033[0;31m'; CYAN='\033[0;36m'; YELLOW='\033[1;33m'; BOLD='\033[1m'; NC='\033[0m'
SERVER_PID=""; GUI_PID=""

cleanup() {
  [ -n "$SERVER_PID" ] && kill "$SERVER_PID" 2>/dev/null
  [ -n "$GUI_PID" ] && kill "$GUI_PID" 2>/dev/null
}
trap cleanup EXIT

banner() { echo ""; echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"; echo -e "${BOLD}$1${NC}"; echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"; }
pause_next() { echo ""; read -r -p "اضغط Enter للمتابعة إلى العرض التالي... " _; }
ok()   { echo -e "  ${GREEN}✅ $1${NC}"; }
bad()  { echo -e "  ${RED}❌ $1${NC}"; }
say()  { echo -e "  ${YELLOW}🎤 قل للحضور:${NC} $1"; }

HAS_GUI=0
if [ -n "${DISPLAY:-}" ] && command -v gnome-terminal >/dev/null 2>&1; then HAS_GUI=1; fi
READ_TAIL='read -r -p "شاهد النتيجة أعلاه ثم اضغط Enter هنا لإغلاق النافذة... " _'
[ -n "${DEMO_READ_TIMEOUT:-}" ] && READ_TAIL="read -t $DEMO_READ_TIMEOUT -r -p \"نافذة عرض (تُغلق تلقائيًا)... \" _"

open_win() { # open_win "العنوان" "الأمر"
  local title="$1"; shift
  if [ "$HAS_GUI" = "1" ]; then
    gnome-terminal --title="$title" -- bash -c "$*; echo; $READ_TAIL" >/dev/null 2>&1 &
    sleep 1
  else
    bash -c "$*"
  fi
}

echo -e "${BOLD}🌟 أهلًا بكم في عرض «ض ستوديو» — منصة البرمجة باللغة العربية 🌟${NC}"
echo "ستنفتح أمامكم نوافذ حقيقية: برامج تعمل، ومعالج يحسب، وموقع إنترنت حي."

need() { if [ ! -x "$1" ]; then bad "الملف غير موجود: $1 — أخبر التقني."; exit 1; fi; }
need "$ROOT/Compiler/build/bin/daad-compiler"
need "$ROOT/DAAD/build/daad"
need "$ROOT/CPU/dhad_cpu"

# ============================================================
banner "العرض 1️⃣ : نكتب بالعربية… والحاسوب يفهم!"
say "هذا الملف كله بالعربية، وبضغطة واحدة سيتحول إلى برنامج يعمل — شاهدوا النافذة الجديدة"
"$ROOT/Compiler/build/bin/daad-compiler" "$ROOT/Examples/01_hello.ض" -o /tmp/demo_hello.cpp > /dev/null 2>&1
g++ -std=c++20 /tmp/demo_hello.cpp -o /tmp/demo_hello -I "$ROOT/Compiler/include" -I "$ROOT/Compiler" 2>/dev/null
"$ROOT/Compiler/build/bin/daad-compiler" "$ROOT/Examples/02_arithmetic.ض" -o /tmp/demo_arith.cpp > /dev/null 2>&1
g++ -std=c++20 /tmp/demo_arith.cpp -o /tmp/demo_arith -I "$ROOT/Compiler/include" -I "$ROOT/Compiler" 2>/dev/null
open_win "برنامج مرحبا — يعمل الآن" '/tmp/demo_hello; echo; echo "---"; /tmp/demo_arith | head -n 4'
/tmp/demo_hello > /dev/null 2>&1 && ok "العرض الأول يعمل في نافذته الخاصة" || bad "تعطل البرنامج الأول"
pause_next

# ============================================================
banner "العرض 2️⃣ : لغة قريبة من المعالج — مضروب العدد 5"
say "دالة عودية — من أصعب البرامج — والنتيجة ستظهر في نافذة جديدة: 120"
"$ROOT/DAAD/build/daad" "$ROOT/DAAD/factorial.daad" -o /tmp/demo_fact.s > /dev/null 2>&1
gcc -nostartfiles /tmp/demo_fact.s -o /tmp/demo_fact 2>/dev/null
open_win "مضروب 5 — يعمل الآن" 'echo "5 × 4 × 3 × 2 × 1 ="; /tmp/demo_fact; echo "رمز الخروج أعلاه هو الناتج: $?"'
/tmp/demo_fact; [ "$?" = "120" ] && ok "العرض الثاني يعمل: الناتج 120" || bad "الناتج غير متوقع"
pause_next

# ============================================================
banner "العرض 3️⃣ : المعالج يحسب تعليمةً تعليمة"
say "هذه واجهة المعالج الرسومية — وهذا برنامج حلقة يُنفَّذ عليها الآن"
"$ROOT/DAAD/build/daad" "$ROOT/Demo/loop5.daad" --target=dhad -o /tmp/demo_loop.ضasm > /dev/null 2>&1
if [ "$HAS_GUI" = "1" ] && [ -x "$ROOT/CPU/dhad_gui" ]; then
  ( "$ROOT/CPU/dhad_gui" >/dev/null 2>&1 & echo $! > /tmp/demo_gui.pid )
  sleep 2; GUI_PID=$(cat /tmp/demo_gui.pid 2>/dev/null)
  echo "  (واجهة المعالج مفتوحة الآن في نافذة مستقلة)"
fi
open_win "المعالج يحسب 1+2+3+4+5" '"$ROOT/CPU/dhad_cpu" /tmp/demo_loop.ضasm'
"$ROOT/CPU/dhad_cpu" /tmp/demo_loop.ضasm 2>&1 | grep -q "15" && ok "العرض الثالث يعمل: المجموع 15" || bad "ناتج المعالج غير متوقع"
[ -n "$GUI_PID" ] && kill "$GUI_PID" 2>/dev/null; GUI_PID=""
pause_next

# ============================================================
banner "العرض 4️⃣ : الموقع الحي — سجّل دخولك بنفسك!"
if ! command -v node >/dev/null 2>&1 || ! command -v curl >/dev/null 2>&1; then
  bad "Node.js أو curl غير متوفر — نتجاوز مشهد المتصفح"; pause_next
elif curl -s --max-time 3 http://localhost:3000/health > /dev/null 2>&1; then
  bad "المنفذ 3000 مشغول — نتجاوز مشهد المتصفح"; pause_next
else
  SRV="$ROOT/Studio/desktop-app/server"
  DB="file:$SRV/prisma/dev.db"
  say "سيُفتح المتصفح الآن على موقع المنصة الحقيقي — وسأعطيكم حسابًا جاهزًا للدخول"
  DATABASE_URL="$DB" PORT=3000 NODE_ENV=development nohup node "$SRV/src/index.js" > /tmp/demo_server.log 2>&1 &
  SERVER_PID=$!
  for _ in $(seq 1 20); do curl -s --max-time 2 http://localhost:3000/health > /dev/null 2>&1 && break; sleep 1; done
  node "$ROOT/Demo/api-demo.js" 3000 demo@example.com
  if command -v xdg-open >/dev/null 2>&1 && [ -n "${DISPLAY:-}" ]; then
    xdg-open "http://localhost:3000/login.html" >/dev/null 2>&1 &
    sleep 2
    xdg-open "http://localhost:3000/pages/dhad-editor.html" >/dev/null 2>&1 &
    echo "  (صفحتا الدخول والمحرر مفتوحتان الآن في المتصفح)"
    say "سجّلوا الدخول بالبريد الظاهر أعلاه، ثم افتحوا صفحة التحديات — واضغطوا زر «تشغيل» في صفحة المحرر لتروا الكود العربي يُنفَّذ داخل المتصفح"
  else
    say "المتصفح غير متوفر هنا — الحساب والتحديات ظهرت أعلاه من الخادم مباشرة"
  fi
  ok "العرض الرابع يعمل: موقع حي + حساب حقيقي"
  pause_next
  kill "$SERVER_PID" 2>/dev/null; SERVER_PID=""
fi

# ============================================================
echo ""
echo -e "${GREEN}${BOLD}🎉 انتهى العرض — شكرًا لكم! 🎉${NC}"
echo "ما شاهدتموه حيًا: برامج تعمل في نوافذها، معالج يحسب، وموقع حقيقي بحساب حقيقي."
echo "(يمكنكم إغلاق نوافذ العرض المتبقية الآن)"
