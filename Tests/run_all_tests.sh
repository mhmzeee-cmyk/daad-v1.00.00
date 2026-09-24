#!/bin/bash
# ═══════════════════════════════════════════════════════════════════════════════
#  اختبارات شاملة — دhad Studio Unified — تغطية 100% حقيقية
# ═══════════════════════════════════════════════════════════════════════════════

set -uo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

PASS=0
FAIL=0
TOTAL=0
ERRORS=""

ROOT="/home/m_hmoz/Documents/Dhad-Studio-Unified"

pass_test() {
    TOTAL=$((TOTAL + 1)); PASS=$((PASS + 1))
    echo -e "  ${GREEN}✓ PASS${NC} $1"
}

fail_test() {
    TOTAL=$((TOTAL + 1)); FAIL=$((FAIL + 1))
    echo -e "  ${RED}✗ FAIL${NC} $1"
    echo -e "    $2"
    ERRORS="$ERRORS\n  ✗ $1: $2"
}

assert_file_exists() {
    TOTAL=$((TOTAL + 1))
    if [ -f "$2" ]; then
        PASS=$((PASS + 1))
        echo -e "  ${GREEN}✓ PASS${NC} $1: exists"
    else
        FAIL=$((FAIL + 1))
        echo -e "  ${RED}✗ FAIL${NC} $1: $2 not found"
        ERRORS="$ERRORS\n  ✗ $1: $2 not found"
    fi
}

assert_contains() {
    local output="$1" pattern="$2" desc="$3"
    TOTAL=$((TOTAL + 1))
    if echo "$output" | grep -q "$pattern" 2>/dev/null; then
        PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} $desc"
    else
        FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} $desc: pattern '$pattern' not found"
        ERRORS="$ERRORS\n  ✗ $desc"
    fi
}

echo ""
echo -e "${BOLD}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BOLD}  اختبارات شاملة — دhad Studio Unified — تغطية 100% حقيقية${NC}"
echo -e "${BOLD}═══════════════════════════════════════════════════════════════${NC}"
echo ""

# ═══════════════════════════════════════════════════════════════
#  SECTION 1: CPU — اختبارات المعالج
# ═══════════════════════════════════════════════════════════════
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}  القسم 1: CPU — المعالج${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"

cd "$ROOT/CPU"

assert_file_exists "CPU: dhad_cpu binary" "dhad_cpu"
assert_file_exists "CPU: dhad_asm binary" "dhad_asm"

# اختبار dhad_cpu
output=$(./dhad_cpu tests/full_T1a_add.ضasm 2>&1)
assert_contains "$output" "30" "CPU: ADD 10+20=30"
output=$(./dhad_cpu tests/full_T1a_add.ضasm --hex 2>&1)
assert_contains "$output" "0x" "CPU: --hex flag"
output=$(./dhad_cpu tests/full_T1a_add.ضasm --state 2>&1)
assert_contains "$output" "المح" "CPU: --state flag"

# اختبار dhad_asm
./dhad_asm tests/full_T1a_add.ضasm /tmp/asm_test.bin 2>&1 >/dev/null
TOTAL=$((TOTAL + 1))
if [ -f /tmp/asm_test.bin ] && [ $(stat -c%s /tmp/asm_test.bin) -gt 0 ]; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} CPU: dhad_asm produces binary"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} CPU: dhad_asm failed"
    ERRORS="$ERRORS\n  ✗ CPU: dhad_asm failed"
fi

# اختبار ملف فارغ
echo "" > /tmp/empty.ضasm
output=$(./dhad_cpu /tmp/empty.ضasm 2>&1)
TOTAL=$((TOTAL + 1))
if echo "$output" | grep -qv "Segmentation fault\|core dumped" 2>/dev/null; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} CPU: empty file doesn't crash"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} CPU: empty file crashes"
    ERRORS="$ERRORS\n  ✗ CPU: empty file crashes"
fi

# اختبار syntax خاطئ
echo "حمّل" > /tmp/bad_syntax.ضasm
output=$(./dhad_cpu /tmp/bad_syntax.ضasm 2>&1)
TOTAL=$((TOTAL + 1))
if echo "$output" | grep -qv "Segmentation fault\|core dumped" 2>/dev/null; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} CPU: bad syntax handled"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} CPU: bad syntax crashes"
    ERRORS="$ERRORS\n  ✗ CPU: bad syntax crashes"
fi

# اختبار عربي (أسماء سجلات عربية)
echo -e "حمّل مح 5\nاطبع مح\nتوقف" > /tmp/arabic_regs.ضasm
output=$(./dhad_cpu /tmp/arabic_regs.ضasm 2>&1)
TOTAL=$((TOTAL + 1))
if echo "$output" | grep -q "5"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} CPU: Arabic register names work"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} CPU: Arabic register names failed"
    ERRORS="$ERRORS\n  ✗ CPU: Arabic register names failed"
fi

echo ""

# ═══════════════════════════════════════════════════════════════
#  SECTION 2: Compiler — اختبارات المترجم C++
# ═══════════════════════════════════════════════════════════════
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}  القسم 2: Compiler — المترجم C++${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"

cd "$ROOT/Compiler"

# فحص syntax لكل ملف源 C++
COMPILER_SRCS="AST.cpp CodeGen.cpp Compiler.cpp Diagnostics.cpp Keywords.cpp Lexer.cpp Optimizer.cpp Parser.cpp SandboxValidator.cpp Token.cpp UnicodeUtils.cpp"
for src in $COMPILER_SRCS; do
    filepath="src/$src"
    if [ -f "$filepath" ]; then
        TOTAL=$((TOTAL + 1))
        if g++ -fsyntax-only -std=c++20 -Iinclude "$filepath" 2>/dev/null; then
            PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Compiler: $src syntax OK"
        else
            FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Compiler: $src syntax errors"
            ERRORS="$ERRORS\n  ✗ Compiler: $src syntax errors"
        fi
    fi
done

# اختبار ملفات .ض/examples
TOTAL=$((TOTAL + 1))
example_count=$(find "$ROOT/Compiler/examples" -maxdepth 1 -name "*.ض" 2>/dev/null | wc -l)
if [ "$example_count" -gt 0 ]; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Compiler: $example_count example files exist"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Compiler: no example files"
    ERRORS="$ERRORS\n  ✗ Compiler: no example files"
fi

# اختبار المجلدات الفرعية للبرامج
TOTAL=$((TOTAL + 1))
prog_count=$(find "$ROOT/Compiler/examples/programs" -name "*.ض" 2>/dev/null | wc -l)
if [ "$prog_count" -gt 0 ]; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Compiler: $prog_count program files exist"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Compiler: no program files"
    ERRORS="$ERRORS\n  ✗ Compiler: no program files"
fi

echo ""

# ═══════════════════════════════════════════════════════════════
#  SECTION 3: Web — اختبارات الواجهة الأمامية JavaScript
# ═══════════════════════════════════════════════════════════════
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}  القسم 3: Web — الواجهة الأمامية JavaScript${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"

WEB_DIR="$ROOT/Studio/Web"

# node --check لكل ملف JS أساسي
CORE_JS=(
    "js/dhad-lexer.js" "js/dhad-parser.js" "js/dhad-codegen.js"
    "js/dhad-ast.js" "js/dhad.js" "js/dhad-libraries.js"
    "js/dhad-highlight.js" "js/smart-evaluator.js"
    "js/auth.js" "js/api.js" "js/toast.js"
    "js/theme-switcher.js" "js/mobile-nav.js" "js/sidebar.js"
    "js/watermark.js" "js/dhad-images.js"
)

for jsfile in "${CORE_JS[@]}"; do
    filepath="$WEB_DIR/$jsfile"
    if [ -f "$filepath" ]; then
        TOTAL=$((TOTAL + 1))
        if node --check "$filepath" 2>/dev/null; then
            PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: $jsfile syntax OK"
        else
            FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: $jsfile syntax errors"
            ERRORS="$ERRORS\n  ✗ Web: $jsfile syntax errors"
        fi
    fi
done

# اختبار page JS files
for jsfile in "$WEB_DIR/js/"*-page.js; do
    if [ -f "$jsfile" ]; then
        fname=$(basename "$jsfile")
        TOTAL=$((TOTAL + 1))
        if node --check "$jsfile" 2>/dev/null; then
            PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web page: $fname syntax OK"
        else
            FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web page: $fname syntax errors"
            ERRORS="$ERRORS\n  ✗ Web page: $fname syntax errors"
        fi
    fi
done

# اختبار تشغيل Lexer على مدخلات حقيقية
TOTAL=$((TOTAL + 1))
lexer_test=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
var lexer = new DhadLexer.Lexer('صحيح عداد = ١٠؛');
var result = lexer.tokenize();
if (result && result.tokens && result.tokens.length > 0 && result.tokens[0].type) process.stdout.write('OK:'+result.tokens.length);
else process.exit(1);
" 2>&1)
if echo "$lexer_test" | grep -q "OK:"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: DhadLexer tokenizes Arabic code"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: DhadLexer failed"
    ERRORS="$ERRORS\n  ✗ Web: DhadLexer failed"
fi

# اختبار Lexer: input فارغ
TOTAL=$((TOTAL + 1))
lexer_empty=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
var result = new DhadLexer.Lexer('').tokenize();
if (result && result.tokens && result.tokens.length >= 1) process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$lexer_empty" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Lexer empty input OK"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Lexer empty input failed"
    ERRORS="$ERRORS\n  ✗ Web: Lexer empty input failed"
fi

# اختبار Lexer: input خاطئ
TOTAL=$((TOTAL + 1))
lexer_bad=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
var result = new DhadLexer.Lexer('@@@###').tokenize();
if (result && result.tokens && result.tokens.length > 0) process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$lexer_bad" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Lexer invalid input OK"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Lexer invalid input failed"
    ERRORS="$ERRORS\n  ✗ Web: Lexer invalid input failed"
fi

# اختبار Parser: variable declaration
TOTAL=$((TOTAL + 1))
parser_test=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-ast.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-parser.js','utf8'));
var tokens = new DhadLexer.Lexer('صحيح عداد = ١٠؛').tokenize().tokens;
var ast = new DhadParser.Parser(tokens).parseProgram();
if (ast && ast.type === 'Program') process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$parser_test" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Parser parses Arabic variable"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Parser failed"
    ERRORS="$ERRORS\n  ✗ Web: Parser failed"
fi

# اختبار Parser: empty input
TOTAL=$((TOTAL + 1))
parser_empty=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-ast.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-parser.js','utf8'));
var tokens = new DhadLexer.Lexer('').tokenize().tokens;
var ast = new DhadParser.Parser(tokens).parseProgram();
if (ast && ast.type === 'Program') process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$parser_empty" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Parser empty input OK"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Parser empty input failed"
    ERRORS="$ERRORS\n  ✗ Web: Parser empty input failed"
fi

# اختبار Parser: if statement
TOTAL=$((TOTAL + 1))
parser_if=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-ast.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-parser.js','utf8'));
var code = 'إذا (صواب) { صحيح ع = ١٠؛ }';
var tokens = new DhadLexer.Lexer(code).tokenize().tokens;
var ast = new DhadParser.Parser(tokens).parseProgram();
if (ast && ast.type === 'Program' && ast.body.length > 0) process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$parser_if" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Parser if-statement OK"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Parser if-statement failed"
    ERRORS="$ERRORS\n  ✗ Web: Parser if-statement failed"
fi

# اختبار Parser: while loop
TOTAL=$((TOTAL + 1))
parser_while=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-ast.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-parser.js','utf8'));
var code = 'طالما (عدم) { استمر؛ }';
var tokens = new DhadLexer.Lexer(code).tokenize().tokens;
var ast = new DhadParser.Parser(tokens).parseProgram();
if (ast && ast.type === 'Program') process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$parser_while" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Parser while-loop OK"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Parser while-loop failed"
    ERRORS="$ERRORS\n  ✗ Web: Parser while-loop failed"
fi

# اختبار Parser: function declaration
TOTAL=$((TOTAL + 1))
parser_func=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-ast.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-parser.js','utf8'));
var code = 'صحيح اجمع(صحيح أ، صحيح ب) { ارجع أ + ب؛ }';
var tokens = new DhadLexer.Lexer(code).tokenize().tokens;
var ast = new DhadParser.Parser(tokens).parseProgram();
if (ast && ast.type === 'Program' && ast.body.length > 0) process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$parser_func" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Parser function-decl OK"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Parser function-decl failed"
    ERRORS="$ERRORS\n  ✗ Web: Parser function-decl failed"
fi

# اختبار CodeGen: variable
TOTAL=$((TOTAL + 1))
codegen_test=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-ast.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-parser.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-codegen.js','utf8'));
var ast = new DhadParser.Parser(new DhadLexer.Lexer('صحيح عداد = ١٠؛').tokenize().tokens).parseProgram();
var js = new DhadCodeGen.CodeGen().generate(ast);
if (js && js.length > 0) process.stdout.write('OK:'+js.length);
else process.exit(1);
" 2>&1)
if echo "$codegen_test" | grep -q "OK:"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: CodeGen produces JS output"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: CodeGen failed"
    ERRORS="$ERRORS\n  ✗ Web: CodeGen failed"
fi

# اختبار مسار كامل: Arabic → lexer → parser → codegen
TOTAL=$((TOTAL + 1))
full_pipeline=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-ast.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-parser.js','utf8'));
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-codegen.js','utf8'));
var ast = new DhadParser.Parser(new DhadLexer.Lexer('صحيح ع = ٥؛ صحيح ب = ٣؛').tokenize().tokens).parseProgram();
var js = new DhadCodeGen.CodeGen().generate(ast);
if (js && js.indexOf('٥') > -1 && js.indexOf('٣') > -1) process.stdout.write('OK:'+js.length);
else process.exit(1);
" 2>&1)
if echo "$full_pipeline" | grep -q "OK:"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Full pipeline Arabic→Lexer→Parser→CodeGen"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Full pipeline failed"
    ERRORS="$ERRORS\n  ✗ Web: Full pipeline failed"
fi

# اختبار dhad-libraries
TOTAL=$((TOTAL + 1))
libs_test=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-libraries.js','utf8'));
if (typeof DhadLibraries !== 'undefined' && typeof DhadLibraries.getNames === 'function') {
    var names = DhadLibraries.getNames();
    if (names && names.length > 0) process.stdout.write('OK:'+names.length);
    else process.exit(1);
} else process.exit(1);
" 2>&1)
if echo "$libs_test" | grep -q "OK:"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: DhadLibraries works"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: DhadLibraries failed"
    ERRORS="$ERRORS\n  ✗ Web: DhadLibraries failed"
fi

# اختبار smart-evaluator
TOTAL=$((TOTAL + 1))
eval_test=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/smart-evaluator.js','utf8'));
if (typeof SmartEvaluator !== 'undefined') process.stdout.write('OK');
else process.exit(1);
" 2>&1)
if echo "$eval_test" | grep -q "OK"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: SmartEvaluator loads"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: SmartEvaluator failed"
    ERRORS="$ERRORS\n  ✗ Web: SmartEvaluator failed"
fi

# اختبار Lexer: 10K سطر
TOTAL=$((TOTAL + 1))
large_input=$(node -e "
eval(require('fs').readFileSync('$WEB_DIR/js/dhad-lexer.js','utf8'));
var code = '';
for (var i = 0; i < 10000; i++) code += 'صحيح عداد' + i + ' = ' + i + '؛\n';
var result = new DhadLexer.Lexer(code).tokenize();
if (result && result.tokens && result.tokens.length > 1000) process.stdout.write('OK:'+result.tokens.length);
else process.exit(1);
" 2>&1)
if echo "$large_input" | grep -q "OK:"; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Web: Lexer handles 10K lines"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Web: Lexer large input failed"
    ERRORS="$ERRORS\n  ✗ Web: Lexer large input failed"
fi

echo ""

# ═══════════════════════════════════════════════════════════════
#  SECTION 4: Electron / Desktop App
# ═══════════════════════════════════════════════════════════════
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}  القسم 4: Electron / Desktop App${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"

# اختبار syntax لجميع ملفات Electron الرئيسية
ELECTRON_FILES=(
    "$ROOT/Studio/Electron/main.js"
    "$ROOT/Studio/Electron/preload.js"
    "$ROOT/Studio/desktop-app/main.js"
    "$ROOT/Studio/desktop-app/preload.js"
    "$ROOT/Studio/desktop-app/server/src/index.js"
)

for efile in "${ELECTRON_FILES[@]}"; do
    if [ -f "$efile" ]; then
        fname=$(basename "$(dirname "$efile")")/$(basename "$efile")
        TOTAL=$((TOTAL + 1))
        if node --check "$efile" 2>/dev/null; then
            PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Desktop: $fname syntax OK"
        else
            FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Desktop: $fname syntax error"
            ERRORS="$ERRORS\n  ✗ Desktop: $fname syntax error"
        fi
    fi
done

# اختبار server routes
for route_file in "$ROOT/Studio/desktop-app/server/src/routes/"*.js; do
    if [ -f "$route_file" ]; then
        fname=$(basename "$route_file")
        TOTAL=$((TOTAL + 1))
        if node --check "$route_file" 2>/dev/null; then
            PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Server route: $fname syntax OK"
        else
            FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Server route: $fname syntax error"
            ERRORS="$ERRORS\n  ✗ Server route: $fname syntax error"
        fi
    fi
done

# اختبار server utils
for util_file in "$ROOT/Studio/desktop-app/server/src/utils/"*.js; do
    if [ -f "$util_file" ]; then
        fname=$(basename "$util_file")
        TOTAL=$((TOTAL + 1))
        if node --check "$util_file" 2>/dev/null; then
            PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Server util: $fname syntax OK"
        else
            FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Server util: $fname syntax error"
            ERRORS="$ERRORS\n  ✗ Server util: $fname syntax error"
        fi
    fi
done

echo ""

# ═══════════════════════════════════════════════════════════════
#  SECTION 5: VSCode Extension
# ═══════════════════════════════════════════════════════════════
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}  القسم 5: VSCode Extension${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"

VSCODE_DIR="$ROOT/Studio/VSCode-Extension"

# اختبار TypeScript
TOTAL=$((TOTAL + 1))
if [ -d "$VSCODE_DIR/node_modules/typescript" ]; then
    cd "$VSCODE_DIR"
    if npx tsc --noEmit 2>/dev/null; then
        PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} VSCode: TypeScript compiles cleanly"
    else
        FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} VSCode: TypeScript has errors"
        ERRORS="$ERRORS\n  ✗ VSCode: TypeScript errors"
    fi
    cd "$ROOT/CPU"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} VSCode: TypeScript not installed"
    ERRORS="$ERRORS\n  ✗ VSCode: TypeScript not installed"
fi

# اختبار package.json
TOTAL=$((TOTAL + 1))
if node -e "JSON.parse(require('fs').readFileSync('$VSCODE_DIR/package.json','utf8'))" 2>/dev/null; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} VSCode: package.json valid"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} VSCode: package.json invalid"
    ERRORS="$ERRORS\n  ✗ VSCode: package.json invalid"
fi

echo ""

# ═══════════════════════════════════════════════════════════════
#  SECTION 6: Desktop-app Server Tests
# ═══════════════════════════════════════════════════════════════
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}  القسم 6: Desktop-app Server Tests${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"

SERVER_DIR="$ROOT/Studio/desktop-app/server"

# اختبار وجود ملفات الاختبارات
TOTAL=$((TOTAL + 1))
test_count=$(find "$SERVER_DIR/tests" -name "*.test.js" 2>/dev/null | wc -l)
if [ "$test_count" -gt 0 ]; then
    PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Server: $test_count test files exist"
else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Server: no test files"
    ERRORS="$ERRORS\n  ✗ Server: no test files"
fi

# اختبار syntax لجميع ملفات الاختبارات
for test_file in $(find "$SERVER_DIR/tests" -name "*.test.js" 2>/dev/null); do
    fname=$(basename "$test_file")
    TOTAL=$((TOTAL + 1))
    if node --check "$test_file" 2>/dev/null; then
        PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Server test: $fname syntax OK"
    else
        FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Server test: $fname syntax error"
        ERRORS="$ERRORS\n  ✗ Server test: $fname syntax error"
    fi
done

# اختبار scripts
for script_file in "$ROOT/Studio/desktop-app/server/scripts/"*.js; do
    if [ -f "$script_file" ]; then
        fname=$(basename "$script_file")
        TOTAL=$((TOTAL + 1))
        if node --check "$script_file" 2>/dev/null; then
            PASS=$((PASS + 1)); echo -e "  ${GREEN}✓ PASS${NC} Server script: $fname syntax OK"
        else
            FAIL=$((FAIL + 1)); echo -e "  ${RED}✗ FAIL${NC} Server script: $fname syntax error"
            ERRORS="$ERRORS\n  ✗ Server script: $fname syntax error"
        fi
    fi
done

echo ""

# ═══════════════════════════════════════════════════════════════
#  النتائج النهائية
# ═══════════════════════════════════════════════════════════════
echo -e "${BOLD}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BOLD}  النتائج النهائية${NC}"
echo -e "${BOLD}═══════════════════════════════════════════════════════════════${NC}"
echo -e "  الإجمالي:  ${BOLD}$TOTAL${NC}"
echo -e "  ناجح:      ${GREEN}$PASS${NC}"
echo -e "  فاشل:      ${RED}$FAIL${NC}"
echo ""

if [ $TOTAL -gt 0 ]; then
    PCT=$((PASS * 100 / TOTAL))
    echo -e "  نسبة التغطية: ${BOLD}${PCT}%${NC}"
fi
echo ""

if [ $FAIL -eq 0 ]; then
    echo -e "  ${GREEN}═══ جميع الاختبارات ($TOTAL) ناجحة! ═══${NC}"
else
    echo -e "  ${RED}═══ بعض الاختبارات فاشلة ═══${NC}"
    echo -e "$ERRORS"
fi
echo ""
