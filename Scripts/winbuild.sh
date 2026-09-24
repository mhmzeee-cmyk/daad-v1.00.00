#!/bin/bash
# BASE: configurable root; override via environment or leave default for relative usage
# Original: BASE="/run/media/m_hmoz/UBUNTU 26_0/"$'\xc2\xa0'  (external drive)
BASE="${DAAD_BASE:-$(cd "$(dirname "$0")/.." && pwd)}"
OUT="${DAAD_OUT:-/tmp/dhad-interview/winbuild}"
mkdir -p "$OUT"
find "$BASE/Daad-Compiler/src" -maxdepth 1 -name "*.cpp" | sort | while IFS= read -r f; do
  name=$(basename "$f" .cpp)
  echo "compiling $name"
  x86_64-w64-mingw32-g++ -std=c++20 -O2 -I"$BASE/Daad-Compiler/include" -I"$BASE/Daad-Compiler/src" -c "$f" -o "$OUT/$name.o" 2>"$OUT/err_$name.log" || { echo "FAILED $name"; head -10 "$OUT/err_$name.log"; }
done
echo "objects: $(ls "$OUT"/*.o 2>/dev/null | wc -l)"
