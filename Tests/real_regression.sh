#!/bin/bash
# ============================================================
#  Real Regression Suite — Dhad Studio Unified
#  BUILD → EXECUTE → CORRECT OUTPUT (exit codes + exact bits).
#  NOT a syntax checker: every case runs a real binary and
#  verifies its real output. Fails loudly on first mismatch
#  only if STRICT=1; otherwise runs all and reports.
#  Usage: bash Tests/real_regression.sh [--strict]
#  Requires: cmake, gcc, g++, python3 (node optional, for web part: skipped if absent)
# ============================================================
set -u
STRICT=0; [ "${1:-}" = "--strict" ] && STRICT=1
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d /tmp/dhad_regress.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT
PASS=0; FAIL=0; FAILED_CASES=""

pass() { PASS=$((PASS+1)); echo "  ✅ PASS $1"; }
fail() { FAIL=$((FAIL+1)); FAILED_CASES="$FAILED_CASES\n  ❌ $1 ($2)"; echo "  ❌ FAIL $1 -- $2"; [ "$STRICT" = "1" ] && { echo "STRICT mode: aborting"; exit 1; }; }

echo "== Dhad real regression =="; echo "ROOT=$ROOT  TMP=$TMP"

# ---------- ensure binaries (build if missing) ----------
if [ ! -x "$ROOT/Compiler/build/bin/daad-compiler" ]; then
  echo "-- building Compiler..."; cmake -B "$ROOT/Compiler/build" -S "$ROOT/Compiler" -DCMAKE_BUILD_TYPE=Release -DDAAD_BUILD_TESTS=OFF > /dev/null 2>&1
  cmake --build "$ROOT/Compiler/build" -j"$(nproc)" > /dev/null 2>&1 || { echo "FATAL: Compiler build failed"; exit 2; }
fi
if [ ! -x "$ROOT/DAAD/build/daad" ]; then
  echo "-- building DAAD..."; cmake -B "$ROOT/DAAD/build" -S "$ROOT/DAAD" -DCMAKE_BUILD_TYPE=Release > /dev/null 2>&1
  cmake --build "$ROOT/DAAD/build" -j"$(nproc)" > /dev/null 2>&1 || { echo "FATAL: DAAD build failed"; exit 2; }
fi
if [ ! -x "$ROOT/CPU/dhad_cpu" ] || [ ! -x "$ROOT/CPU/dhad_asm" ]; then
  echo "-- building CPU..."; make -C "$ROOT/CPU" > /dev/null 2>&1 || { echo "FATAL: CPU build failed"; exit 2; }
fi
DCOMP="$ROOT/Compiler/build/bin/daad-compiler"; DAAD="$ROOT/DAAD/build/daad"; CPU="$ROOT/CPU/dhad_cpu"

# ---------- 1. Compiler track (.ض → C++ → EXE → run) ----------
echo "-- [1] Compiler track"
declare -A CEXP=( [01_hello]="مرحبا بالعالم!" [02_arithmetic]="15" [03_conditions]="أنت بالغ" [05_functions]="26" [06_factorial]="120" )
for f in 01_hello 02_arithmetic 03_conditions 05_functions 06_factorial; do
  "$DCOMP" "$ROOT/Examples/$f.ض" -o "$TMP/$f.cpp" > /dev/null 2>&1 || { fail "compiler:$f" "compile"; continue; }
  g++ -std=c++20 "$TMP/$f.cpp" -o "$TMP/$f" -I "$ROOT/Compiler/include" -I "$ROOT/Compiler" 2>/dev/null || { fail "compiler:$f" "g++"; continue; }
  out=$("$TMP/$f" 2>&1)
  echo "$out" | grep -q "${CEXP[$f]}" && pass "compiler:$f" || fail "compiler:$f" "missing '${CEXP[$f]}'"
done
# loops sum 1..100 = 5050
"$DCOMP" "$ROOT/Examples/04_loops.ض" -o "$TMP/loops.cpp" > /dev/null 2>&1 && g++ -std=c++20 "$TMP/loops.cpp" -o "$TMP/loops" -I "$ROOT/Compiler/include" -I "$ROOT/Compiler" 2>/dev/null && "$TMP/loops" 2>&1 | grep -q "5050" && pass "compiler:loops" || fail "compiler:loops" "missing 5050"

# ---------- 2. DAAD integer (exit codes) ----------
echo "-- [2] DAAD integer"
dcase() { # name, source(printf %b), expect_exit
  printf "%b" "$2" > "$TMP/c.daad"
  "$DAAD" "$TMP/c.daad" -o "$TMP/c.s" 2>/dev/null || { fail "daad:$1" "compile"; return; }
  gcc -nostartfiles "$TMP/c.s" -o "$TMP/c.exe" 2>/dev/null || { fail "daad:$1" "link"; return; }
  "$TMP/c.exe" >/dev/null 2>&1; code=$?
  [ "$code" = "$3" ] && pass "daad:$1" || fail "daad:$1" "exit=$code expect=$3"
}
dcase "hello42"    'دالة: الرئيسية() -> رقم :-\n    ارجع(42)\nنهاية\n' 42
dcase "calc60"     'دالة: الرئيسية() -> رقم :-\n    متغير: أ: رقم = 10\n    متغير: ب: رقم = 20\n    متغير: ج: رقم = أ + ب\n    متغير: د: رقم = ج * 2\n    ارجع(د)\nنهاية\n' 60
dcase "branch"     'دالة: الرئيسية() -> رقم :-\n    متغير: س: رقم = 20\n    إذا(س >= 18) :-\n        ارجع(1)\n    نهاية\n    ارجع(0)\nنهاية\n' 1
dcase "loop55"     'دالة: الرئيسية() -> رقم :-\n    متغير: م: رقم = 0\n    متغير: ع: رقم = 1\n    طالما(ع <= 10) :-\n        م = م + ع\n        ع = ع + 1\n    نهاية\n    ارجع(م)\nنهاية\n' 55
dcase "multifunc"  'دالة: جمع(أ: رقم, ب: رقم) -> رقم :-\n    ارجع(أ + ب)\nنهاية\nدالة: الرئيسية() -> رقم :-\n    ارجع(جمع(20, 22))\nنهاية\n' 42
dcase "fact120"    'دالة: مضروب(ن: رقم) -> رقم :-\n    إذا(ن <= 1) :-\n        ارجع(1)\n    نهاية\n    ارجع(ن * مضروب(ن - 1))\nنهاية\nدالة: الرئيسية() -> رقم :-\n    ارجع(مضروب(5))\nنهاية\n' 120
dcase "fib13"      'دالة: ليفي(ن: رقم) -> رقم :-\n    إذا(ن <= 1) :-\n        ارجع(ن)\n    نهاية\n    ارجع(ليفي(ن - 1) + ليفي(ن - 2))\nنهاية\nدالة: الرئيسية() -> رقم :-\n    ارجع(ليفي(7))\nنهاية\n' 13
# invalid program must FAIL with S001
printf 'دالة: الرئيسية() -> رقم :-\n    ارجع(غير_معرف + 1)\nنهاية\n' > "$TMP/neg.daad"
if "$DAAD" "$TMP/neg.daad" -o "$TMP/neg.s" 2>&1 | grep -q "S001"; then pass "daad:invalid-S001"; else fail "daad:invalid-S001" "no S001 diagnostic"; fi

# ---------- 3. DAAD float (bit-exact, needs python3) ----------
echo "-- [3] DAAD float"
if command -v python3 >/dev/null 2>&1; then
  fcase() { # name, body-lines, python-expr-for-expected-bits
    printf "دالة: الرئيسية() -> رقم :-\n    %b\n    ارجع(0)\nنهاية\n" "$1" > "$TMP/f.daad"
    "$DAAD" "$TMP/f.daad" -o "$TMP/f.s" 2>/dev/null || { fail "float:$2" "compile"; return; }
    gcc -nostartfiles "$TMP/f.s" -o "$TMP/f.exe" 2>/dev/null || { fail "float:$2" "link"; return; }
    got=$("$TMP/f.exe" 2>&1 | head -n 1 | tr -d '[:space:]')
    expect=$(python3 -c "import struct; print(struct.unpack('<q', struct.pack('<d', $3))[0])")
    [ "$got" = "$expect" ] && pass "float:$2" || fail "float:$2" "got=$got expect=$expect"
  }
  fcase 'متغير: س: رقم_عشري = 1.5\n    اطبع(س)'                       "lit1.5"    "1.5"
  fcase 'متغير: س: رقم_عشري = 3.14159\n    اطبع(س)'                   "litpi"     "3.14159"
  fcase 'متغير: س: رقم_عشري = 1.5 + 2.75\n    اطبع(س)'                "add"       "1.5+2.75"
  fcase 'متغير: س: رقم_عشري = 2.5 * 3.0\n    اطبع(س)'                 "mul"       "2.5*3.0"
  fcase 'متغير: س: رقم_عشري = 7.5 / 2.5\n    اطبع(س)'                 "div"       "7.5/2.5"
  fcase 'متغير: س: رقم_عشري = 5.5\n    متغير: ص: رقم_عشري = 2.5\n    متغير: م: رقم_عشري = س + ص\n    اطبع(م)' "vars" "5.5+2.5"
  printf 'دالة: ضعف(س: رقم_عشري) -> رقم_عشري :-\n    ارجع(س * 2.5)\nنهاية\nدالة: الرئيسية() -> رقم :-\n    متغير: ن: رقم_عشري = ضعف(5.5)\n    اطبع(ن)\n    ارجع(0)\nنهاية\n' > "$TMP/fn.daad"
  "$DAAD" "$TMP/fn.daad" -o "$TMP/fn.s" 2>/dev/null && gcc -nostartfiles "$TMP/fn.s" -o "$TMP/fn.exe" 2>/dev/null && got=$("$TMP/fn.exe" | head -n 1 | tr -d '[:space:]') && expect=$(python3 -c "import struct; print(struct.unpack('<q', struct.pack('<d', 5.5*2.5))[0])") && { [ "$got" = "$expect" ] && pass "float:funarg" || fail "float:funarg" "got=$got expect=$expect"; } || fail "float:funarg" "build"
  printf 'دالة: مجموع(ن: رقم_عشري) -> رقم_عشري :-\n    إذا(ن <= 1.0) :-\n        ارجع(1.0)\n    نهاية\n    ارجع(ن + مجموع(ن - 1.0))\nنهاية\nدالة: الرئيسية() -> رقم :-\n    متغير: م: رقم_عشري = مجموع(5.0)\n    اطبع(م)\n    ارجع(0)\nنهاية\n' > "$TMP/fr.daad"
  "$DAAD" "$TMP/fr.daad" -o "$TMP/fr.s" 2>/dev/null && gcc -nostartfiles "$TMP/fr.s" -o "$TMP/fr.exe" 2>/dev/null && got=$("$TMP/fr.exe" | head -n 1 | tr -d '[:space:]') && expect=$(python3 -c "import struct; print(struct.unpack('<q', struct.pack('<d', 15.0))[0])") && { [ "$got" = "$expect" ] && pass "float:recfloat" || fail "float:recfloat" "got=$got expect=$expect"; } || fail "float:recfloat" "build"
else
  echo "  (python3 missing — float section skipped)"
fi

# ---------- 4. DAAD→DHAD→CPU ----------
echo "-- [4] DAAD-DHAD-CPU"
"$DAAD" "$ROOT/Demo/loop5.daad" --target=dhad -o "$TMP/loop.ضasm" 2>/dev/null || fail "dhad:gen" "compile"
out=$("$CPU" "$TMP/loop.ضasm" 2>&1)
echo "$out" | grep -q "15" && ! echo "$out" | grep -q "خطأ" && pass "dhad:loop15" || fail "dhad:loop15" "no 15 or assembler errors"
"$DAAD" "$ROOT/DAAD/factorial.daad" --target=dhad -o "$TMP/fact.ضasm" 2>/dev/null
"$CPU" "$TMP/fact.ضasm" 2>&1 | grep -q "120" && pass "dhad:fact120" || fail "dhad:fact120" "no 120"

# ---------- 5. CPU suite ----------
echo "-- [5] CPU suite"
cpulog=$(cd "$ROOT/CPU" && bash ./run_tests.sh 2>/dev/null | sed 's/\x1b\[[0-9;]*m//g')
cpass=$(echo "$cpulog" | grep -E "ناجح:" | grep -o "[0-9]*" | head -n 1)
cfail=$(echo "$cpulog" | grep -E "فاشل:" | grep -o "[0-9]*" | head -n 1)
[ "${cfail:-1}" = "0" ] && [ -n "$cpass" ] && pass "cpu:suite-$cpass" || fail "cpu:suite" "fail=$cfail"

# ---------- summary ----------
echo ""
echo "== RESULT: $PASS passed, $FAIL failed =="
if [ -n "$FAILED_CASES" ]; then echo -e "$FAILED_CASES"; fi
[ "$FAIL" = "0" ]
