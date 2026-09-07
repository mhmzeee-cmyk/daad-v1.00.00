#!/bin/bash
# Phase 12.5-F.2: Real E2E Execution Test Harness
# Pipeline: DAAD (.daad) → Compiler → DHAD Assembly (.s) → Assembler+Emulator → Output

DAAD_COMPILER="/home/m_hmoz/Documents/مشروع ض/DAAD/build/daad"
DHAD_CPU="/home/m_hmoz/Documents/cpu/dhad_cpu"
TEST_DIR="/home/m_hmoz/Documents/مشروع ض/DAAD/tests/phase12_5"
TMP_DIR="/tmp/phase_f_e2e"
RESULT_FILE="$TMP_DIR/results.txt"

mkdir -p "$TMP_DIR"

PASS=0
FAIL=0
TOTAL=0

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

run_e2e() {
    local test_name="$1"
    local daad_file="$2"
    local expected_output="$3"
    local expected_acc="$4"
    
    TOTAL=$((TOTAL + 1))
    local s_file="$TMP_DIR/${test_name}.s"
    
    # Step 1: Compile DAAD → DHAD Assembly
    local compile_out
    compile_out=$("$DAAD_COMPILER" "$daad_file" --target=dhad -o "$s_file" 2>&1)
    local compile_rc=$?
    
    if [ $compile_rc -ne 0 ]; then
        echo -e "${RED}FAIL${NC} $test_name — COMPILATION FAILED"
        echo "  $compile_out" | head -3
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Step 2: Assemble + Execute via DHAD CPU
    local exec_out
    exec_out=$(echo "" | "$DHAD_CPU" "$s_file" 2>&1)
    local exec_rc=$?
    
    # Extract ACC value from output
    local actual_acc
    actual_acc=$(echo "$exec_out" | grep -oP 'النتيجة \(مح\): \K[0-9-]+' | head -1)
    
    # Extract cycles
    local cycles
    cycles=$(echo "$exec_out" | grep -oP 'الدورات:\s+\K[0-9]+' | head -1)
    
    # Extract printed output (lines between assembly info and result line)
    local actual_output
    actual_output=$(echo "$exec_out" | sed -n '/^  تجميع/,/النتيجة/p' | grep -v '^\s' | grep -v '^$' | grep -v 'تجميع' | grep -v 'النتيجة' | grep -v 'الدورات' | grep -v 'الحالة' | tr '\n' ' ' | sed 's/^[[:space:]]*//' | sed 's/[[:space:]]*$//')
    
    # Check for crash/hang
    if echo "$exec_out" | grep -q "تم تجاوز الحد الأقصى"; then
        echo -e "${RED}FAIL${NC} $test_name — EXECUTION TIMEOUT"
        FAIL=$((FAIL + 1))
        return
    fi
    
    if echo "$exec_out" | grep -q " SEGFAULT\|Segmentation\|崩溃\| خطأ"; then
        echo -e "${RED}FAIL${NC} $test_name — CRASH"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Compare results
    local status="PASS"
    local detail=""
    
    if [ -n "$expected_acc" ] && [ "$actual_acc" != "$expected_acc" ]; then
        status="FAIL"
        detail="ACC: expected=$expected_acc actual=$actual_acc"
    fi
    
    if [ -n "$expected_output" ] && [ "$actual_output" != "$expected_output" ]; then
        status="FAIL"
        detail="${detail:+$detail; }OUTPUT: expected='$expected_output' actual='$actual_output'"
    fi
    
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}PASS${NC} $test_name — ACC=$actual_acc cycles=$cycles"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}FAIL${NC} $test_name — $detail"
        FAIL=$((FAIL + 1))
    fi
    
    # Log full details
    echo "=== $test_name ===" >> "$RESULT_FILE"
    echo "  Compile: OK" >> "$RESULT_FILE"
    echo "  Expected ACC: $expected_acc" >> "$RESULT_FILE"
    echo "  Actual ACC: $actual_acc" >> "$RESULT_FILE"
    echo "  Expected output: $expected_output" >> "$RESULT_FILE"
    echo "  Actual output: $actual_output" >> "$RESULT_FILE"
    echo "  Cycles: $cycles" >> "$RESULT_FILE"
    echo "  Status: $status" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
}

echo "═══════════════════════════════════════════════════════"
echo "  Phase 12.5-F.2: Real E2E Execution Tests"
echo "═══════════════════════════════════════════════════════"
echo ""

# Clear previous results
> "$RESULT_FILE"

# ─── F11: Basic Execution ───
echo "── F11: Basic Execution Tests ──"

run_e2e "11_main_arabic" \
    "$TEST_DIR/11_main_arabic.daad" \
    "" "42"

# ─── F12: Recursion Tests ───
echo ""
echo "── F12: Recursion Tests ──"

run_e2e "06_recursive_factorial" \
    "$TEST_DIR/06_recursive_factorial.daad" \
    "" ""

run_e2e "07_recursive_deep" \
    "$TEST_DIR/07_recursive_deep.daad" \
    "" ""

# ─── F15: Nested Frame Tests ───
echo ""
echo "── F15: Nested Frame Tests ──"

run_e2e "05_nested_calls" \
    "$TEST_DIR/05_nested_calls.daad" \
    "" ""

# ─── F9: Scratch Conflict Tests ───
echo ""
echo "── F9: Scratch Conflict Tests ──"

run_e2e "09_scratch_conflict" \
    "$TEST_DIR/09_scratch_conflict.daad" \
    "" ""

# ─── Register Pressure Tests ───
echo ""
echo "── Register Pressure Tests ──"

run_e2e "01_many_live_values" \
    "$TEST_DIR/01_many_live_values.daad" \
    "" ""

# ─── Pointer Tests ───
echo ""
echo "── Pointer Tests ──"

run_e2e "08_spilled_pointer" \
    "$TEST_DIR/08_spilled_pointer.daad" \
    "" ""

# ─── Frame Layout Tests ───
echo ""
echo "── Frame Layout Tests ──"

run_e2e "02_frame_layout" \
    "$TEST_DIR/02_frame_layout.daad" \
    "" ""

run_e2e "03_scratch_preservation" \
    "$TEST_DIR/03_scratch_preservation.daad" \
    "" ""

run_e2e "04_multiple_functions" \
    "$TEST_DIR/04_multiple_functions.daad" \
    "" ""

run_e2e "10_nested_calls" \
    "$TEST_DIR/10_nested_calls.daad" \
    "" ""

run_e2e "12_mixed_pressure" \
    "$TEST_DIR/12_mixed_pressure.daad" \
    "" ""

# ─── Summary ───
echo ""
echo "═══════════════════════════════════════════════════════"
echo "  Results: $PASS/$TOTAL passed, $FAIL failed"
echo "═══════════════════════════════════════════════════════"

if [ $FAIL -eq 0 ]; then
    echo -e "  ${GREEN}ALL TESTS PASSED${NC}"
else
    echo -e "  ${RED}$FAIL TEST(S) FAILED${NC}"
fi

exit $FAIL
