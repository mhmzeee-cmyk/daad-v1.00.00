#!/bin/bash
# Build demo programs: .ض -> (linux daad-compiler) -> .cpp -> (mingw) -> .exe
# Original: BASE="/run/media/m_hmoz/UBUNTU 26_0/"$'\xc2\xa0'  (external drive)
# Original: LINUX_CC=/tmp/dhad-phase4-5/compiler-build/bin/daad-compiler
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BASE="${DAAD_BASE:-$REPO_ROOT}"
LINUX_CC="${DAAD_CC:-$REPO_ROOT/Compiler/build/bin/daad-compiler}"
OUT="${DAAD_OUT:-/tmp/dhad-interview/demos}"
mkdir -p "$OUT"
SRC="$BASE/Daad-Compiler/examples/conversions/simple/before"
declare -A DEMOS=(
  [01_hello]="01_hello.ض"
  [02_arithmetic]="02_arithmetic.ض"
  [03_conditions]="03_conditions.ض"
  [04_loops]="04_loops.ض"
  [05_functions]="05_functions.ض"
)
for name in 01_hello 02_arithmetic 03_conditions 04_loops 05_functions; do
  src="$SRC/${DEMOS[$name]}"
  [ -f "$src" ] || { echo "MISSING SRC $src"; continue; }
  cp "$src" "$OUT/$name.ض"
  "$LINUX_CC" "$OUT/$name.ض" -o "$OUT/$name.cpp" >/dev/null 2>&1 || { echo "DAAD-FAIL $name"; continue; }
  # fix extension already .cpp; compile with mingw
  x86_64-w64-mingw32-g++ -std=c++20 -O2 -I"$BASE/Daad-Compiler" "$OUT/$name.cpp" -static-libgcc -static-libstdc++ -o "$OUT/$name.exe" 2>"$OUT/err_$name.log" \
    && echo "OK $name.exe ($(stat -c%s "$OUT/$name.exe") bytes)" \
    || { echo "MINGW-FAIL $name"; head -8 "$OUT/err_$name.log"; }
done
# factorial + fibonacci from complex examples
for pair in "06_factorial:06_factorial.ض" "07_fibonacci:07_fibonacci.ض"; do
  name="${pair%%:*}"; file="${pair##*:}"
  src="$BASE/Daad-Compiler/examples/conversions/complex/before/$file"
  [ -f "$src" ] || { echo "MISSING SRC $src"; continue; }
  cp "$src" "$OUT/$name.ض"
  "$LINUX_CC" "$OUT/$name.ض" -o "$OUT/$name.cpp" >/dev/null 2>&1 || { echo "DAAD-FAIL $name"; continue; }
  x86_64-w64-mingw32-g++ -std=c++20 -O2 -I"$BASE/Daad-Compiler" "$OUT/$name.cpp" -static-libgcc -static-libstdc++ -o "$OUT/$name.exe" 2>"$OUT/err_$name.log" \
    && echo "OK $name.exe ($(stat -c%s "$OUT/$name.exe") bytes)" \
    || { echo "MINGW-FAIL $name"; head -8 "$OUT/err_$name.log"; }
done
file "$OUT"/*.exe | sed 's/,.*//'
