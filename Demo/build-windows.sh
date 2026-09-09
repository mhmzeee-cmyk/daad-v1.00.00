#!/bin/bash
# ============================================================
#  build-windows.sh — .ض → Windows x64 native EXE (cross-compile)
#  Pipeline: daad-compiler → C++20 → MinGW-w64 → PE32+ console EXE
#  Usage: bash Demo/build-windows.sh <program.ض> [output.exe]
#  Requires: cmake-built daad-compiler, x86_64-w64-mingw32-g++
#  Output links ONLY KERNEL32 + msvcrt (runs on stock Windows 7+).
# ============================================================
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="${1:?Usage: bash Demo/build-windows.sh <program.ض> [output.exe]}"
OUT="${2:-${SRC%.ض}.exe}"
TMPD="$(mktemp -d /tmp/dhad_winbuild.XXXXXX)"
trap 'rm -rf "$TMPD"' EXIT

DCOMP="$ROOT/Compiler/build/bin/daad-compiler"
[ -x "$DCOMP" ] || { echo "BUILD: daad-compiler missing, building..."; cmake -B "$ROOT/Compiler/build" -S "$ROOT/Compiler" -DCMAKE_BUILD_TYPE=Release -DDAAD_BUILD_TESTS=OFF >/dev/null 2>&1 && cmake --build "$ROOT/Compiler/build" -j"$(nproc)" >/dev/null 2>&1 || { echo "FATAL: compiler build failed"; exit 2; }; }
command -v x86_64-w64-mingw32-g++ >/dev/null 2>&1 || { echo "FATAL: mingw-w64 not installed (apt install g++-mingw-w64-x86-64)"; exit 2; }
[ -f "$SRC" ] || SRC="$ROOT/$SRC"
[ -f "$SRC" ] || { echo "FATAL: source not found: $1"; exit 2; }

echo "[1/3] $SRC → C++..."
"$DCOMP" "$SRC" -o "$TMPD/out.cpp" 2>&1 | tail -n 1
echo "[2/3] C++ → Windows PE (static)..."
x86_64-w64-mingw32-g++ -std=c++20 -O2 -static -static-libgcc -static-libstdc++ "$TMPD/out.cpp" -o "$OUT" -I "$ROOT/Compiler/include" -I "$ROOT/Compiler" || { echo "FATAL: cross-compile failed"; exit 2; }
echo "[3/3] verify..."
file "$OUT"
x86_64-w64-mingw32-objdump -p "$OUT" 2>/dev/null | grep "DLL Name"
echo "OK: $OUT"
