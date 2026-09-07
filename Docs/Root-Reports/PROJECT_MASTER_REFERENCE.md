# ═══════════════════════════════════════════════════════════════════
# PROJECT MASTER REFERENCE — COMPLETE ARCHITECTURE & SOURCE OF TRUTH
# ═══════════════════════════════════════════════════════════════════
# لغة ض + ض بسيطة + معالج ض
# Generated: 2026-09-04
# ═══════════════════════════════════════════════════════════════════

---

# PART 1 — PROJECT PHILOSOPHY

## 1.1 Why This Project Exists

This project is an attempt to build a **complete computing stack** — from programming language to CPU hardware — using Arabic as the primary interface. The motivation is threefold:

1. **Educational**: Provide Arabic-speaking computer science students a native-language path to understand compilers, ISA design, CPU architecture, and systems programming.
2. **Cultural**: Prove that Arabic is viable as a technical language for the lowest levels of computing.
3. **Technical**: Build a real, working, end-to-end system (not a toy) that compiles, assembles, and executes on a custom CPU architecture.

## 1.2 Why Arabic?

The Arabic language has 400+ million native speakers but virtually no serious low-level programming tools. Assembly languages, ISA documentation, and compiler internals are universally English-only. This project challenges that assumption.

## 1.3 Why a Custom CPU?

A custom CPU (معالج ض) was chosen because:
- x86/ARM/RISC-V documentation and tooling is English-centric
- A custom ISA allows designing concepts (like Arabic register names, hardware stack, MMIO) from scratch
- It provides a clean target for the compiler without legacy baggage
- It enables a complete hardware path from RTL to physical silicon

## 1.4 Why Two Languages?

| Component | Name | Purpose |
|-----------|------|---------|
| High-level | لغة ض (`.ض`) | User-facing Arabic programming language |
| Low-level | ض بسيطة (`.ضب`) | Assembly language for معالج ض |

The high-level language compiles down to the low-level assembly, which assembles to machine code. This mirrors the C → x86 Assembly → Machine Code pipeline.

## 1.5 Vision vs. Reality

### VISION (Long-term)
- Complete Arabic computing ecosystem from language to hardware
- FPGA implementation of معالج ض
- Physical chip fabrication
- Arabic operating system

### CURRENT REALITY (2026-09-04)
- ✅ Working compiler (C, 23,597 LOC)
- ✅ Working assembler + CPU emulator (C, 16 source files)
- ✅ Working web transpiler (JavaScript, Node.js)
- ✅ Working desktop IDE (Electron + Qt)
- ✅ VS Code extension
- ✅ 114 DHAD CPU tests passing
- ✅ 12 E2E compiler→assembler→emulator tests passing
- ⚠️ 8-bit data width, 16-bit addresses, 32-entry hardware stack
- ❌ No FPGA implementation
- ❌ No physical hardware
- ❌ No multi-file compilation / linking
- ❌ No debugger

### FUTURE PLAN
- Phase 13+: Multi-file compilation, linker, debugger
- Hardware: RTL → FPGA synthesis → ASIC → physical chip
- Platform: Arabic IDE with live execution, classroom integration

## 1.6 Current Limits

- **Data width**: 8 bits (0–255 unsigned, -128 to 127 signed)
- **Address width**: 16 bits (64KB address space)
- **Hardware stack**: 32 entries, separate from memory frame
- **Register count**: 8 general (S0–S7) + ACC + FLAGS
- **Max program size**: ~56KB (0x0000–0xEFFF)
- **Frame size**: 256 bytes max (0xE000–0xE0FF)
- **No**: floating point, 16-bit data, dynamic memory, exceptions, multi-threading, linking

---

# PART 2 — PROJECT TIMELINE

## 2.1 Complete Phase History

### Phase 1–5: Foundation (C compiler core)
- Lexer, Parser, AST, Semantic Analysis
- Basic IR generation
- x86-64 code generation (host backend)

### Phase 6–8: DHAD Target
- DHAD ISA design (8-bit data, 16-bit address)
- DHAD assembler implementation
- DHAD CPU emulator implementation
- DHAD backend for compiler

### Phase 9–11: System Integration
- DhadSystem (CPU + Display + Keyboard + Timer + Interrupts)
- Runtime abstraction layer
- MMIO device bus

### Phase 12.1: DHAD Backend Hardening
- Prologue/Epilogue frame management
- Frame pointer (S7) and scratch register (S6) convention

### Phase 12.2: Register Allocation
- IR register → DHAD register mapping
- 6 allocatable registers (S0–S5)
- Fast-path for IR r1–r6

### Phase 12.3: Spill/Reload
- Frame-based spill slots at 0xE000+
- STRI/LDRI for memory access
- S6 as scratch for address high byte

### Phase 12.4: Pointer & Address Operations
- 16-bit address support (high/low byte pair)
- `&` (address-of) and `*` (dereference) operators
- LOAD_ELEMENT / STORE_ELEMENT IR ops

### Phase 12.5-A: Architecture Audit
- Full audit of DHAD backend
- Identified CMP collision, register classification, call convention issues

### Phase 12.5-B: CMP Collision Fix
- Fixed CMP immediate encoding collision with RET/NEG opcode (0x80–0x8F)
- CMP now uses EXT+register encoding (0xF0 0xE0–0xE7)

### Phase 12.5-C: 16-bit Address Support
- LDRI/STRI instructions for indirect memory access
- S6 as address high byte, register as address low byte

### Phase 12.5-D: Pointer Operations
- ADDRESS_OF → `mov sN, sM` (register copy)
- DEREF via LDRI/STRI

### Phase 12.5-E: Epilogue ACC Preservation
- Epilogue pushes ACC before restoring FP, pops after
- Ensures return value survives FP restoration

### Phase F.1: CALL Convention & Register Classification
- **Register classification**: S0–S5 allocatable, S6 dedicated scratch, S7 frame pointer
- **Spill/reload**: Frame-based with STRI/LDRI
- **Prologue**: Save old FP via `stri s7, s6, acc`; set `s7 = frame_base`
- **Epilogue**: `push; load s7 from frame; pop; ret`
- **CALL handler**: Save S7, load args to S0–S3, call, save return in S6, restore S7
- **RET handler**: Push ACC, restore FP, pop ACC, ret

### Phase F.2: Initial Verification
- 114/114 DHAD regression tests pass
- 12/12 E2E execution tests pass
- factorial(5) = 120 verified
- **Problem found**: deepest(8) returns 7 instead of 8

### Phase F.3: Register Allocation & Call Argument Bug Fixes

**Bug 1 — Dead Store Elimination (DSE)**: `pass_dead_store_elimination()` in `optimizer_pass.c` eliminated stores that were later loaded, converting `store r8, r5` to `nop r5, r8`. The backend emitted bare `nop` for NOP with two register operands.
- **Fix**: Added `loaded_between` check — store only eliminated if no load of the same target occurs between the two stores. Also added fallback in `IR_OP_NOP` handler to emit `mov dst, src` for two-register NOPs.

**Bug 2 — CALL Convention Double-Spill**: The CALL handler saved/restored S0–S5 via push/pop, AND the register allocator also generated spill/reload for the same registers. This caused the allocator to spill to frame AND the handler to push to stack, then the allocator would reload from the WRONG spill slot.
- **Fix**: Removed S0–S5 push/pop from CALL handler. Added proactive spill of live S0–S5 values before CALL. Added post-CALL invalidation of reg_map entries for S0–S5 (excluding dst register). The allocator's spill/reload is now the sole mechanism.

**Bug 3 — Label Collisions**: IR labels (if_then, if_merge, __cmp_true_4) used generic names that collided across functions.
- **Fix**: Added `g_func_label_prefix` and `func_label()`/`cmp_label()` helpers that prefix all labels with the function name (e.g., `factorial_if_then`, `deepest_if_then`).

**Results**: deepest(8) = 8 ✅, 114/114 DHAD tests ✅, 12/12 E2E tests ✅, 0 assembler warnings ✅

## 2.2 Known Regressions
- None after F.3 fixes (all previous tests continue to pass)

---

# PART 3 — COMPLETE REPOSITORY MAP

## 3.1 Project Structure

```
/home/m_hmoz/Documents/مشروع ض/
├── DAAD/                          # C compiler (NOT a git repo)
│   ├── compiler/                  # Compiler source (23,597 LOC C)
│   │   ├── main.c                 # CLI entry point
│   │   ├── ast/                   # Abstract Syntax Tree (6 files)
│   │   ├── backend/               # Code generation backends (2 files)
│   │   ├── cfg/                   # Control Flow Graph (2 files)
│   │   ├── codegen/               # Code generation (14 files)
│   │   ├── ir/                    # Intermediate Representation (26 files)
│   │   ├── lexer/                 # Tokenizer (6 files)
│   │   ├── optimizer/             # Optimization passes (2 files)
│   │   ├── parser/                # Syntax parser (6 files)
│   │   ├── semantic/              # Semantic analysis (20 files)
│   │   ├── ssa/                   # Static Single Assignment (2 files)
│   │   ├── token/                 # Token definitions (3 files)
│   │   └── tests/                 # Unit tests (11 test files)
│   ├── docs/                      # Language documentation (18 .md files)
│   ├── e2e/                       # End-to-end tests (~131 .daad files)
│   ├── tests/                     # Test suites
│   │   ├── phase12_5/             # Phase 12.5 specific tests (16 .daad + run_e2e.sh)
│   │   ├── abi/                   # ABI conformance tests
│   │   ├── conformance/           # Language conformance tests
│   │   └── ...                    # Other test categories
│   ├── build/                     # Build output
│   └── *.daad, *.s, *.md         # Root-level test/report files
│
├── cpu/                           # DHAD CPU emulator (NOT a git repo)
│   ├── src/                       # Source (16 .c + 1 .cpp)
│   │   ├── dhad_cpu.c             # CPU core (663 lines)
│   │   ├── dhad_asm.c             # Assembler (741 lines)
│   │   ├── dhad_disasm.c          # Disassembler (414 lines)
│   │   ├── dhad_runtime.c         # Runtime abstraction (189 lines)
│   │   ├── dhad_system.c          # Full system (CPU+Display+KB+Timer) (120 lines)
│   │   ├── dhad_device.c          # Device bus (140 lines)
│   │   ├── dhad_display.c         # Display device
│   │   ├── dhad_keyboard.c        # Keyboard device
│   │   ├── dhad_timer.c           # Timer device
│   │   ├── dhad_intc.c            # Interrupt controller
│   │   ├── dhad_backend_emulator.c# Emulator backend
│   │   ├── dhad_backend_host.c    # Host native backend
│   │   ├── dhad_console.c         # Console I/O
│   │   ├── dhad_debug.c           # Debug utilities
│   │   ├── visual_sim.c           # Visual simulator
│   │   ├── main.c                 # CLI entry point
│   │   └── dhad_live.cpp          # Live GUI (Qt)
│   ├── include/                   # Headers (17 files)
│   ├── tests/                     # Tests (~100 files)
│   ├── examples/                  # Assembly examples (6 .ضasm files)
│   ├── desktop/                   # Desktop distribution
│   ├── gui/                       # Qt GUI (main.cpp)
│   ├── docs/                      # Documentation (9 .md files)
│   ├── Makefile                   # Build system
│   ├── run_tests.sh               # Test runner (114 tests)
│   └── dhad_cpu                   # Compiled binary
│
├── daad-studio/dhad-studio/       # IDE + Web platform (git repo)
│   ├── frontend/                  # Qt/QML desktop IDE
│   ├── frontend-web/              # Web frontend (HTML/CSS/JS)
│   ├── server/                    # Node.js backend
│   │   ├── src/utils/dhad/        # JS transpiler (ast.js, lexer.js, parser.js, codegen.js)
│   │   ├── src/utils/dhadCompiler.js  # Compiler facade
│   │   ├── src/utils/dhadSandbox.js   # Sandboxed execution
│   │   └── tests/                 # Security + unit tests
│   ├── stdlib/                    # Standard library (.ض files, 100 files)
│   ├── stdlib_arabic/             # Arabic-named stdlib (100 files)
│   ├── vscode-extension/          # VS Code extension
│   ├── examples/                  # Example programs
│   ├── src/                       # C++ compiler (transpiler)
│   ├── include/Daad/              # C++ headers
│   ├── dist/                      # Distribution outputs
│   └── docs/                      # Documentation
│
└── PHASE_12_5_F*_REPORT.md        # Phase reports
```

## 3.2 File Counts

| Area | .c | .h | .js | .ts | .daad | .md | Total |
|------|----|----|-----|-----|-------|-----|-------|
| DAAD compiler | 61 | 47 | 0 | 0 | 165 | 33 | ~306 |
| DHAD CPU | 26 | 17 | 0 | 2 | 80 | 9 | ~134 |
| daad-studio | 0 | 0 | 200+ | 12 | 100+ | 10+ | ~322+ |

## 3.3 Lines of Code

| Component | Language | LOC |
|-----------|----------|-----|
| DAAD Compiler | C | 23,597 |
| DHAD CPU/Assembler/Emulator | C | ~4,500 |
| DHAD Qt GUI | C++ | ~800 |
| Web Transpiler | JavaScript | ~4,500 |
| Server Backend | JavaScript | ~8,000 |
| Standard Library | .ض | ~5,000 (est.) |
| **Total** | | **~46,000+** |

---

# PART 4 — BUILD SYSTEM

## 4.1 DAAD Compiler (C)

```bash
# Build command (single gcc invocation, no CMake currently used for this target)
cd "/home/m_hmoz/Documents/مشروع ض/DAAD"
gcc -o build/daad \
  compiler/main.c \
  compiler/token/token.c \
  compiler/lexer/lexer.c compiler/lexer/unicode.c compiler/lexer/keywords.c \
  compiler/parser/parser.c compiler/parser/parser_error.c compiler/parser/parser_recovery.c \
  compiler/ast/ast.c compiler/ast/ast_builder.c compiler/ast/ast_printer.c compiler/ast/ast_visitor.c \
  compiler/semantic/semantic_visitor.c compiler/semantic/semantic_error.c \
  compiler/semantic/symbol_table.c compiler/semantic/symbol.c \
  compiler/semantic/scope.c compiler/semantic/scope_stack.c \
  compiler/semantic/type_checker.c compiler/semantic/type_registry.c \
  compiler/semantic/type.c compiler/semantic/constant_folding.c \
  compiler/ir/ir_module.c compiler/ir/ir_function.c compiler/ir/ir_basic_block.c \
  compiler/ir/ir_instruction.c compiler/ir/ir_value.c compiler/ir/ir_types.c \
  compiler/ir/ir_printer.c compiler/ir/ir_builder.c compiler/ir/ir_visitor.c \
  compiler/ir/ir_validate.c compiler/ir/ir_verifier.c compiler/ir/debug_dumps.c \
  compiler/ir/liveness.c \
  compiler/ssa/ssa.c \
  compiler/optimizer/optimizer_pass.c \
  compiler/cfg/cfg.c \
  compiler/codegen/emitter.c compiler/codegen/expression_generator.c \
  compiler/codegen/function_generator.c compiler/codegen/label_manager.c \
  compiler/codegen/register_allocator.c compiler/codegen/stack_manager.c \
  compiler/codegen/statement_generator.c \
  compiler/backend/backend_interface.c compiler/backend/dhad_backend.c \
  -lm -I compiler -I include
```

**Compiler**: gcc
**Flags**: implicit `-O0` (debug), no explicit flags in the build command
**Libraries**: `-lm` (math)
**Include paths**: `-I compiler -I include`

## 4.2 DHAD CPU Emulator (C)

```bash
cd /home/m_hmoz/Documents/cpu
make          # Builds dhad_gui (Qt5 GUI application)
```

**Compiler**: gcc (C), g++ (C++17)
**Flags**: `-O2 -Wall -Wextra -D_REENTRANT -fPIC`
**Libraries**: Qt5Widgets, Qt5Gui, Qt5Core, GL, pthread
**Target**: `dhad_gui`

**Note**: The CLI binary `dhad_cpu` is compiled separately (not via the Makefile). The `dhad_cpu` binary used in testing is pre-compiled.

## 4.3 daad-studio (Node.js + Electron)

```bash
cd dhad-studio
npm install           # Install dependencies
npm run dev           # Development mode
npm run build         # Build for distribution
```

## 4.4 Test Commands

```bash
# DHAD CPU regression tests (114 tests)
cd /home/m_hmoz/Documents/cpu
bash run_tests.sh

# E2E compiler→assembler→emulator tests (12 tests)
cd "/home/m_hmoz/Documents/مشروع ض/DAAD"
bash tests/phase12_5/run_e2e.sh

# Manual E2E test
./build/daad test.daad --target=dhad -o test.s
/home/m_hmoz/Documents/cpu/dhad_cpu test.s
```

---

# PART 5 — لغة ض COMPLETE LANGUAGE

## 5.1 Lexical Layer

### Identifiers
- Must start with Arabic letter, English letter, or `_`
- Can contain letters, digits, `_`
- Max 256 characters
- Case-sensitive (Arabic)

### Keywords (Arabic)

| Keyword | Meaning | Usage |
|---------|---------|-------|
| `متغير` | variable | Variable declaration |
| `ثابت` | constant | Constant declaration |
| `نوع` | type | Type definition |
| `دالة` | function | Function declaration |
| `إذا` | if | Conditional |
| `وإلا` | else | Else branch |
| `طالما` | while | While loop |
| `لكل` | for | For loop |
| `ارجع` | return | Return statement |
| `اطبع` | print | Print output |
| `ادخل` | input | Read input |
| `توقف` | halt | Stop execution |
| `نهاية` | end/end | Block terminator |

### Operators

| Operator | Meaning | Precedence |
|----------|---------|------------|
| `+` `-` | Add/Sub | 4 |
| `*` `/` `%` | Mul/Div/Mod | 3 |
| `<<` `>>` | Shift | 5 |
| `&` `\|` `^` `~` | Bitwise | 6–8 |
| `==` `!=` `>` `<` `>=` `<=` | Comparison | 9 |
| `و` | Logical AND | 10 |
| `أو` | Logical OR | 11 |
| `=` `+=` `-=` `*=` `/=` `%=` | Assignment | 12 |
| `&x` | Address-of | 2 |
| `*x` | Dereference | 2 |

### Comments
- Single-line: `# comment`
- Multi-line: `#[ ... ]#`

## 5.2 Grammar (Key Constructs)

### Function Declaration
```
دالة: name(args) -> return_type :- body نهاية
```

### Variable Declaration
```
متغير: name = value
متغير: name: type = value
```

### If Statement
```
إذا (condition) :- body نهاية
إذا (condition) :- body وإلا :- body نهاية
```

### While Loop
```
طالما (condition) :- body نهاية
```

### Function Call
```
result = func_name(arg1, arg2)
```

### Return
```
ارجع(expression)
```

### Comments
```
# single line
#[
multi-line
]#
```

## 5.3 Types

| Type | Arabic | Size | Notes |
|------|--------|------|-------|
| Integer | `رقم` | 8-bit on DHAD | Maps to i8 |
| Float | `رقم_عشري` | Not on DHAD | Float not supported |
| Char | `حرف` | 8-bit | Maps to i8 |
| Bool | `منطق` | 8-bit | Maps to i8 |
| String | `نص` | Pointer | Maps to ptr |
| Pointer | `مؤشر_نوع(T)` | 16-bit | Maps to i16 |
| Array | `T[N]` | N × sizeof(T) | Fixed-size |
| Struct | `نوع: name = {...}` | Sum of fields | Sequential layout |
| Void | `فراغ` | 0 | No value |

## 5.4 Semantic Rules
- Type checking on assignments, comparisons, function arguments
- Scope-based symbol resolution
- Constant folding optimization
- Forward declarations supported
- No implicit type conversions (explicit `حوّل()` needed)

---

# PART 6 — لغة بسيطة (DHAD Assembly)

## 6.1 Instruction Syntax

```
mnemonic  operand1, operand2
```

Arabic and English mnemonics are both supported.

### Registers (Arabic → English)

| Arabic | English | Purpose |
|--------|---------|---------|
| `س0`–`س7` | `s0`–`s7` | General purpose |
| `مح` | `acc` | Accumulator |
| `أعلام` | `flags` | Flags register |

### Instructions

| Arabic | English | Opcode | Operands | Description |
|--------|---------|--------|----------|-------------|
| `停止` | `halt` | 0xF0 0xD3 | 0 | Stop CPU |
| `صفر` | `nop` | 0x00 | 0 | No operation |
| `تحمل` | `load` | 0x1n | reg, imm8 | Load immediate to register |
| `حرّك` | `mov` | 0x2n | dst, src | Move register to register |
| `اقرأ` | `ld` | 0x3n | reg, addr16 | Load from memory |
| `خزن` | `st` | 0x4n | reg, addr16 | Store to memory |
| `ادفع` | `push` | 0x50 | 0 | Push ACC to stack |
| `اسحب` | `pop` | 0x60 | 0 | Pop stack to ACC |
| `ادعُ` | `call` | 0x7n | addr16 | Call subroutine |
| `اعد` | `ret` | 0x80 | 0 | Return from subroutine |
| `اجمع` | `add` | 0x9n | reg | ACC += register |
| `اطرح` | `sub` | 0xAn | reg | ACC -= register |
| `اضرب` | `mul` | 0xBn | reg | ACC *= register |
| `اقسم` | `div` | 0xCn | reg | ACC /= register |
| `بقي` | `mod` | 0xDn | reg | ACC %= register |
| `اطبع` | `print` | 0xE0 | 0 | Print ACC as number |
| `اطبع_حرف` | `print_ch` | 0xF0 0x50 | 0 | Print ACC as char |
| `ادخل` | `input` | 0xF0 0x60 | 0 | Read input to ACC |
| `مقارنة` | `cmp` | 0xF0 0xEn | reg | ACC -= reg (set flags) |
| `اقفز` | `jmp` | 0xF0 0x0n | addr16 | Unconditional jump |
| `اقفز_صفر` | `jz` | 0xF0 0x1n | addr16 | Jump if Z=1 |
| `اقفز_غير_صفر` | `jnz` | 0xF0 0x2n | addr16 | Jump if Z=0 |
| `اقفز_حمل` | `jc` | 0xF0 0x3n | addr16 | Jump if C=1 |
| `اقفز_سالب` | `jn` | 0xF0 0x4n | addr16 | Jump if N=1 |
| `اذهب` | `ldri` | 0xF0 0xF1 | reg, s6, reg | Load indirect (16-bit addr) |
| `خزّن` | `stri` | 0xF0 0xF2 | reg, s6, acc | Store indirect (16-bit addr) |

## 6.2 Labels

```
label_name:
    instruction
```

## 6.3 Directives

| Directive | Meaning |
|-----------|---------|
| `org 0x0000` | Set origin address |
| `equ NAME value` | Define named constant |
| `#` comment | Single-line comment |

---

# PART 7 — معالج ض ISA (Complete)

## 7.1 Architecture Overview

| Property | Value |
|----------|-------|
| Name | داله (Dhad) |
| Version | 4.0.0 |
| Data width | 8-bit |
| Address width | 16-bit |
| General registers | 8 (S0–S7) |
| Accumulator | ACC (8-bit) |
| Flags | Z, N, C, V (4 bits) |
| Hardware stack | 32 entries, 8-bit values |
| Memory size | 65,536 bytes (64KB) |
| Max program | 0xF000 (61,440 bytes) |
| Endianness | Little-endian |

## 7.2 Registers

| Register | Width | Purpose | Compiler Use |
|----------|-------|---------|-------------|
| S0–S5 | 8-bit | General purpose | Allocatable (6) |
| S6 | 8-bit | Scratch / address high byte | Dedicated scratch |
| S7 | 8-bit | Frame pointer | Callee-saved |
| ACC | 8-bit | Accumulator / ALU / return value | Temporary |
| FLAGS | 4-bit | Z, N, C, V | Set by CMP/ALU |
| PC | 16-bit | Program counter | Hidden |
| SP | 5-bit | Stack pointer (0–31) | Hidden |

## 7.3 Memory Map

```
0x0000 ┌────────────────────┐
       │   Program Code     │  Up to 0xEFFF (~56KB)
0xE000 ├────────────────────┤
       │   Frame Area       │  256 bytes (compiler-managed)
0xE100 ├────────────────────┤
       │   Unused           │
0xF000 ├────────────────────┤
       │   MMIO I/O Ports   │  256 bytes
0xF100 ├────────────────────┤
       │   Interrupt Vector │  256 bytes
0xF200 ├────────────────────┤
       │   Reserved         │
0xFFFF └────────────────────┘
```

## 7.4 Instruction Encoding

### Base Instructions (1 byte)
```
[7:4] opcode (4 bits)
[3:0] operand (4 bits — register index or sub-opcode)
```

### Extended Instructions (2 bytes)
```
Byte 1: 0xF0 (extended prefix)
Byte 2: [7:4] extension code, [3:0] register
```

### Memory Instructions (2–4 bytes)
```
Load/Store: [opcode+reg] [addr_lo] [addr_hi]  (3 bytes)
LDRI/STRI: [0xF0] [0xF1/F2] [reg] [s6, reg]  (4 bytes)
```

## 7.5 Instruction Sizes

| Type | Bytes | Example |
|------|-------|---------|
| Base (no imm) | 1 | `add s0` → 0x90 |
| Immediate | 2 | `load s0, 42` → 0x10 0x2A |
| Register-to-register | 1 | `mov s1, s0` → 0x21 |
| Memory (load/store) | 3 | `ld s0, 0x1234` → 0x30 0x34 0x12 |
| Extended (2-byte) | 2 | `jmp 0x1234` → 0xF0 0x00 0x34 0x12 |
| LDRI/STRI | 4 | `ldri s0, s6, s1` → 0xF0 0xF1 0x01 |
| CALL | 3 | `call 0x1234` → 0x70 0x34 0x12 |
| PUSH/POP | 1 | `push` → 0x50 |
| HALT | 2 | `halt` → 0xF0 0xD3 |
| CMP register | 2 | `cmp s0` → 0xF0 0xE0 |

## 7.6 CMP Collision (Historical)

**Problem**: CMP with immediate was originally encoded as `0x8n` where `n` = immediate value (0–7). This collided with RET (0x80) and NEG (0x8F).

**Resolution**: CMP now uses extended encoding `0xF0 0xEn` for register comparison. CMP with immediate was removed from the ISA.

## 7.7 Flags Behavior

| Instruction | Z | N | C | V |
|------------|---|---|---|---|
| ADD | ✓ | ✓ | ✓ | ✓ |
| SUB | ✓ | ✓ | ✓ | ✓ |
| MUL | ✓ | ✓ | — | — |
| DIV | ✓ | ✓ | — | — |
| MOD | ✓ | ✓ | — | — |
| CMP | ✓ | ✓ | ✓ | — |
| AND/OR/XOR | ✓ | ✓ | — | — |
| SHL/SHR | — | ✓ | ✓ | — |
| NOT | ✓ | ✓ | — | — |
| INC/DEC | ✓ | ✓ | — | ✓ |
| NEG | ✓ | ✓ | ✓ | — |

## 7.8 Cycles

| Instruction | Cycles |
|------------|--------|
| NOP | 1 |
| HALT | 1 |
| LOAD_IMM | 2 |
| MOV_REG | 1 |
| LOAD_MEM | 3 |
| STORE_MEM | 3 |
| PUSH | 2 |
| POP | 2 |
| CALL | 4 |
| RET | 3 |
| ALU (ADD/SUB/etc.) | 2 |
| CMP | 2 |
| BRANCH (taken) | 3 |
| BRANCH (not taken) | 1 |
| PRINT | 2 |
| INPUT | 2 |
| LDRI | 4 |
| STRI | 4 |

---

# PART 8 — ISA DECISIONS

## 8.1 LDRI/STRI for 16-bit Addresses

**Problem**: 8-bit registers cannot directly hold 16-bit addresses.
**Options**: (a) Two-instruction sequences, (b) Register pair, (c) Dedicated S6 as high byte.
**Decision**: S6 is dedicated as address high byte. LDRI uses `S6:reg` as 16-bit address.
**Consequence**: S6 cannot be used for general computation during address operations.
**Limitation**: Only one 16-bit address can be active at a time (S6 is shared).

## 8.2 S7 as Frame Pointer

**Problem**: No hardware support for stack frames.
**Options**: (a) Use memory directly, (b) Use S7 as frame base.
**Decision**: S7 holds the current function's frame base address (0xE000 + offset).
**Consequence**: S7 is callee-saved; prologue/epilogue manage it.
**Limitation**: Frame size limited to 256 bytes (one S7 offset range).

## 8.3 Hardware Stack vs. Memory Frame

**Problem**: Need both a call stack (return addresses) and a frame (local variables, spill slots).
**Decision**: Hardware stack (32 entries) for return addresses only. Memory frame (0xE000–0xE0FF) for locals and spill.
**Consequence**: PUSH/POP only work with ACC, limited to 32 entries. Frame is separate and compiler-managed.
**Limitation**: Deep recursion (>32 levels) overflows the hardware stack.

## 8.4 8-bit Data Width

**Problem**: 8-bit limits the value range to 0–255.
**Decision**: Start with 8-bit for simplicity. 16-bit is a future expansion.
**Consequence**: factorial(5) = 120 is the maximum that fits. factorial(6) = 720 overflows.
**Limitation**: No 16-bit data operations; 16-bit addresses require S6 + register pair.

---

# PART 9 — CPU EMULATOR

## 9.1 Source: `cpu/src/dhad_cpu.c` (663 lines)

### Key Functions

| Function | Line | Purpose |
|----------|------|---------|
| `dhad_cpu_init()` | 43 | Zero CPU struct, set PC=0x0000, all IRQs enabled |
| `dhad_cpu_reset()` | 60 | Reset registers/flags/SP, keep program memory |
| `dhad_cpu_load_program()` | 87 | Copy binary to memory at 0x0000 |
| `dhad_cpu_step()` | 223 | **Main execution loop**: fetch-decode-execute + tick + interrupt dispatch |
| `dhad_cpu_run()` | 643 | Loop: step() until halted or max_cycles |
| `dhad_cpu_push()` | 139 | Push 8-bit value to hardware stack (max 32) |
| `dhad_cpu_pop()` | 147 | Pop 8-bit value from hardware stack |
| `dhad_cpu_update_flags_nz()` | 156 | Update Z and N flags from 8-bit result |

### Execution Model

1. **Fetch**: Read byte at PC, increment PC
2. **Decode**: Extract opcode (bits 7:4) and operand (bits 3:0)
3. **Execute**: Perform operation, update flags, advance PC as needed
4. **Tick**: Increment cycle count, tick all devices
5. **Interrupt**: Check pending interrupts, dispatch if enabled

### Memory Access

```c
uint8_t dhad_cpu_read_mem(DhadCPU* cpu, uint16_t addr) {
    if (addr >= DHAD_IO_BASE && addr <= DHAD_IO_END) {
        return dhad_bus_read_io(cpu->bus, addr);  // MMIO routing
    }
    return cpu->memory[addr];
}
```

## 9.2 Assembler: `cpu/src/dhad_asm.c` (741 lines)

Two-pass assembler:
- **Pass 1**: Parse all instructions, record label addresses
- **Pass 2**: Generate binary bytes using label addresses

Supports: labels, equates, Arabic/English mnemonics, hex/decimal literals, macros.

## 9.3 Disassembler: `cpu/src/dhad_disasm.c` (414 lines)

Decodes machine code back to Arabic assembly text. Used for debugging and inspection.

---

# PART 10 — DhadSystem

## 10.1 Architecture

```
┌─────────────────────────────────────┐
│            DhadSystem               │
│  ┌──────────┐  ┌───────────────┐   │
│  │  DhadCPU  │──│ DhadDeviceBus │   │
│  └──────────┘  └───────┬───────┘   │
│       │                │           │
│  ┌────┴────┐    ┌──────┴──────┐   │
│  │ Memory  │    │  Devices    │   │
│  │ 64KB    │    ├─────────────┤   │
│  └─────────┘    │ Display (0) │   │
│                 │ Keyboard (1)│   │
│                 │ Timer (2)   │   │
│                 │ GPIO (3)    │   │
│                 │ Serial (4)  │   │
│                 │ IntCtl (5)  │   │
│                 └─────────────┘   │
└─────────────────────────────────────┘
```

## 10.2 MMIO Address Decoding

```
Address format: 0xF0DD
  Device index = DD >> 4   (0–5)
  Register     = DD & 0xF  (0–15)
```

| Device | Index | Base Address |
|--------|-------|-------------|
| Display | 0 | 0xF000 |
| Keyboard | 1 | 0xF010 |
| Timer | 2 | 0xF020 |
| GPIO | 3 | 0xF030 |
| Serial | 4 | 0xF040 |
| IntCtl | 5 | 0xF050 |

## 10.3 Device Bus

```c
typedef struct {
    DhadDevice* devices[16];  // Max 16 devices
    int count;
} DhadDeviceBus;
```

Each device has a vtable: `init`, `reset`, `read`, `write`, `tick`, `destroy`.

---

# PART 11 — COMPILER INTERNAL ARCHITECTURE

## 11.1 Pipeline

```
Source (.ض)
    ↓
┌─────────┐
│  Lexer  │  → Token stream
└────┬────┘
     ↓
┌─────────┐
│ Parser  │  → AST
└────┬────┘
     ↓
┌──────────┐
│ Semantic │  → Type-checked AST + Symbol table
└────┬─────┘
     ↓
┌─────────┐
│ IR Gen  │  → IRModule (functions, basic blocks, instructions)
└────┬────┘
     ↓
┌──────────┐
│Optimizer │  → Optimized IR (11 passes, 3 rounds)
└────┬─────┘
     ↓
┌─────┐
│ SSA │  → SSA form (optional)
└──┬──┘
   ↓
┌──────────────┐
│ Reg Alloc    │  → Virtual→Physical register mapping
└──────┬───────┘
       ↓
┌──────────────┐
│ DHAD Backend │  → DHAD Assembly (.s)
└──────┬───────┘
       ↓
┌──────────────┐
│  Assembler   │  → Binary (.bin)
└──────┬───────┘
       ↓
┌──────────────┐
│  CPU Emulator│  → Execution
└──────────────┘
```

## 11.2 Key Data Structures

### IRModule
```c
typedef struct {
    char* name;
    IRFunction* functions[256];
    int function_count;
    IRGlobal globals[256];
    int global_count;
    IRStringEntry strings[1024];
    int string_count;
} IRModule;
```

### IRFunction
```c
typedef struct {
    char* name;
    IRType return_type;
    IRValue params[32];
    IRType param_types[32];
    char* param_names[32];
    int param_count;
    IRBasicBlock* blocks[256];
    int block_count;
    IRBasicBlock* entry_block;
    IRBasicBlock* exit_block;
    int next_reg;
    IRValue alloca_list[256];
    int alloca_count;
} IRFunction;
```

### IRBasicBlock
```c
typedef struct {
    char* label;
    IRInstruction instructions[256];
    int instruction_count;
    IRBasicBlock* successors[4];
    IRBasicBlock* predecessors[4];
    int successor_count;
    int predecessor_count;
    int visited;
    int block_id;
} IRBasicBlock;
```

### IRInstruction
```c
typedef struct {
    IROpcode opcode;
    IRValue result;
    IRValue operands[8];
    int operand_count;
    IRCompareOp compare_op;
    int flags;
    int line, col;
} IRInstruction;
```

---

# PART 12 — AST

## 12.1 Node Types (32 total)

| NodeType | Description |
|----------|-------------|
| `NODE_PROGRAM` | Root: entire program |
| `NODE_BLOCK` | Block of statements |
| `NODE_VARIABLE_DECL` | `متغير: x = 5` |
| `NODE_CONSTANT_DECL` | `ثابت: X = 10` |
| `NODE_FUNCTION_DECL` | `دالة: f(x) -> T :- ... نهاية` |
| `NODE_PARAMETER` | Function parameter |
| `NODE_RETURN_STATEMENT` | `ارجع(expr)` |
| `NODE_IF_STATEMENT` | `إذا(cond) :- ... نهاية` |
| `NODE_WHILE_STATEMENT` | `طالما(cond) :- ... نهاية` |
| `NODE_FOR_STATEMENT` | `لكل(init; cond; update) :- ... نهاية` |
| `NODE_BREAK_STATEMENT` | Break |
| `NODE_CONTINUE_STATEMENT` | Continue |
| `NODE_ASSIGNMENT_EXPRESSION` | `x = expr` or `x += expr` |
| `NODE_BINARY_EXPRESSION` | `a + b`, `a == b`, etc. |
| `NODE_UNARY_EXPRESSION` | `-x`, `!x`, `&x`, `*x` |
| `NODE_LITERAL` | Integer, float, string, char, bool |
| `NODE_IDENTIFIER` | Variable reference |
| `NODE_CALL_EXPRESSION` | `f(args)` |
| `NODE_ARRAY_EXPRESSION` | `{1, 2, 3}` |
| `NODE_STRUCT_EXPRESSION` | Struct initializer |
| `NODE_POINTER_EXPRESSION` | Pointer type |
| `NODE_MEMBER_EXPRESSION` | `obj.field` |
| `NODE_INDEX_EXPRESSION` | `arr[i]` |
| `NODE_CAST_EXPRESSION` | `convert(expr, type)` |
| `NODE_ARRAY_TYPE` | Array type node |
| `NODE_STRUCT_TYPE` | Struct type node |
| `NODE_STRUCT_FIELD` | Single struct field |
| `NODE_GOTO_STATEMENT` | Goto |
| `NODE_LABEL_STATEMENT` | Label |
| `NODE_TRY_STATEMENT` | Try (stub) |
| `NODE_CATCH_STATEMENT` | Catch (stub) |
| `NODE_THROW_STATEMENT` | Throw (stub) |

## 12.2 ASTNode Struct

```c
typedef struct ASTNode {
    NodeType type;
    Position start, end;
    char* filename;
    int error_flag;
    union {
        struct { ASTNodeList* declarations; } program;
        struct { ASTNodeList* statements; } block;
        struct { char* name; ASTNode* type; ASTNode* init; } var_decl;
        struct { char* name; ASTNode* value; } const_decl;
        struct { char* name; ASTNodeList* params; ASTNode* return_type; ASTNode* body; } func_decl;
        struct { char* name; ASTNode* type; } param;
        struct { ASTNode* value; } return_stmt;
        struct { ASTNode* condition; ASTNode* then_block; ASTNode* else_block; } if_stmt;
        struct { ASTNode* condition; ASTNode* body; } while_stmt;
        struct { ASTNode* init; ASTNode* condition; ASTNode* update; ASTNode* body; } for_stmt;
        struct { ASTNode* target; AssignmentOperator op; ASTNode* value; } assignment;
        struct { BinaryOperator op; ASTNode* left; ASTNode* right; } binary;
        struct { UnaryOperator op; ASTNode* operand; } unary;
        struct { LiteralKind kind; union { long long int_val; double float_val; char* string_val; char char_val; int bool_val; }; } literal;
        struct { char* name; } identifier;
        struct { ASTNode* callee; ASTNodeList* arguments; } call;
        // ... more variants
    } as;
} ASTNode;
```

---

# PART 13 — IR

## 13.1 Opcodes (34 total)

| Opcode | Operands | Result | Description |
|--------|----------|--------|-------------|
| `NOP` | — | — | No operation |
| `LOAD` | src | dst | Load value to register |
| `STORE` | addr, val | — | Store value to address |
| `MOV` | src | dst | Move/copy register |
| `ADD` | a, b | dst | dst = a + b |
| `SUB` | a, b | dst | dst = a - b |
| `MUL` | a, b | dst | dst = a * b |
| `DIV` | a, b | dst | dst = a / b |
| `MOD` | a, b | dst | dst = a % b |
| `NEG` | src | dst | dst = -src |
| `NOT` | src | dst | dst = ~src |
| `AND` | a, b | dst | dst = a & b |
| `OR` | a, b | dst | dst = a \| b |
| `XOR` | a, b | dst | dst = a ^ b |
| `SHL` | a, b | dst | dst = a << b |
| `SHR` | a, b | dst | dst = a >> b |
| `CMP` | a, b | dst | Compare, store boolean |
| `JMP` | label | — | Unconditional jump |
| `JE` | cond, true, false | — | Jump if equal |
| `JNE` | cond, true, false | — | Jump if not equal |
| `JG` | cond, true, false | — | Jump if greater |
| `JL` | cond, true, false | — | Jump if less |
| `JGE` | cond, true, false | — | Jump if greater or equal |
| `JLE` | cond, true, false | — | Jump if less or equal |
| `CALL` | func, args... | dst | Call function |
| `RET` | val | — | Return value |
| `PUSH` | val | — | Push to stack |
| `POP` | — | dst | Pop from stack |
| `PHI` | args... | dst | SSA phi function |
| `CAST` | src, type | dst | Type conversion |
| `ALLOCA` | size | dst | Allocate stack frame slot |
| `LOAD_ELEMENT` | base, index | dst | Load from array/struct |
| `STORE_ELEMENT` | base, index, val | — | Store to array/struct |
| `MEMBER_ACCESS` | base, field | dst | Access struct member |
| `LEA` | base, offset | dst | Load effective address |

## 13.2 IR Values

```c
typedef enum {
    IR_VALUE_REGISTER,      // Virtual register (SSA)
    IR_VALUE_CONSTANT_INT,  // Integer literal
    IR_VALUE_CONSTANT_FLOAT,// Float literal
    IR_VALUE_CONSTANT_STRING,// String label
    IR_VALUE_LABEL,         // Jump target label
    IR_VALUE_NULL           // Void/null
} IRValueKind;

typedef struct {
    IRValueKind kind;
    IRType type;
    int id;                 // Register ID or label name
    union {
        long long int_val;
        double float_val;
        char* string_val;
        char* label_val;
    } as;
    int line, col;
} IRValue;
```

---

# PART 14 — SSA / CFG / OPTIMIZER

## 14.1 SSA Support

Located in `compiler/ssa/ssa.c`:
- `ssa_convert_to_ssa()`: Convert register form to SSA
- `ssa_convert_from_ssa()`: Convert back for codegen
- `ssa_insert_phi_nodes()`: Insert PHI at dominance frontiers
- `ssa_rename_variables()`: Unique versioning per definition

**Status**: IMPLEMENTED but not used by default in DHAD backend.

## 14.2 CFG

Located in `compiler/cfg/cfg.c`:
- Build control flow graph from basic blocks
- Compute dominance, dominance frontiers
- Used by SSA and optimization passes

## 14.3 Optimization Passes (11 total)

| Pass | Description | Status |
|------|-------------|--------|
| `constant_folding` | Evaluate constant expressions at compile time | ✅ IMPLEMENTED |
| `constant_propagation` | Replace register uses with known constants | ✅ IMPLEMENTED |
| `copy_propagation` | Eliminate redundant MOV | ✅ IMPLEMENTED |
| `dead_code_elimination` | Remove unused instruction results | ✅ IMPLEMENTED |
| `dead_store_elimination` | Remove stores overwritten without load | ✅ IMPLEMENTED (F.3 fix) |
| `algebraic_simplification` | x+0→x, x*1→x, x*0→0, x-0→x | ✅ IMPLEMENTED |
| `strength_reduction` | Replace multiply by power-of-2 with SHL | ✅ IMPLEMENTED |
| `jump_optimization` | Constant-fold conditional jumps | ✅ IMPLEMENTED |
| `remove_empty_blocks` | Remove NOP-only blocks | ✅ IMPLEMENTED |
| `merge_blocks` | Merge single-successor JMP blocks | ✅ IMPLEMENTED |
| `peephole` | Local 3-instruction pattern matching | ✅ IMPLEMENTED |

Each pass is run 3 rounds by `optimizer_run_all()`.

---

# PART 15 — REGISTER ALLOCATION

## 15.1 Allocator Design

Located in `compiler/backend/dhad_backend.c`:

### Register Classification

| Register | Role | Compiler Use |
|----------|------|-------------|
| S0–S5 | Allocatable | User values (6 registers) |
| S6 | Dedicated scratch | Address high byte, spill/reload temp |
| S7 | Frame pointer | Callee-saved, managed by prologue/epilogue |
| ACC | ALU / return value | Temporary, not allocatable |

### Fast-Path Mapping

```c
static int ir_reg_to_dhad(int ir_reg) {
    if (ir_reg >= 1 && ir_reg <= 6) return ir_reg - 1;  // r1→S0, r2→S1, ..., r6→S5
    return -1;  // Slow path via ensure_in_reg
}
```

### Spill/Reload

- **Spill**: Store register to frame via `stri reg, s6, acc` at `0xE000 + offset`
- **Reload**: Load from frame via `ldri reg, s6, acc`
- **Spill slot allocation**: `alloc_spill_slot()` increments `g_frame_size` (up to 256 bytes)

### CALL Convention (F.3 Final)

1. **Before CALL**: Proactively spill all live S0–S5 values to frame (skip dst register)
2. **Save FP**: `mov acc, s7; push`
3. **Load arguments**: `mov s0, arg1; mov s1, arg2; ...`
4. **Call**: `call func_name`
5. **Save return**: `mov s6, acc`
6. **Restore FP**: `pop; mov s7, acc`
7. **Invalidate**: Mark S0–S5 reg_map entries as not loaded (skip dst)
8. **Move return**: `mov acc, s6; mov dst, acc`

### Known Bugs in Allocator

1. **Post-instruction reg_map staleness**: After ALU operations, the result register's `is_loaded` flag may not be set correctly. (Partially mitigated by spill/reload.)
2. **Victim selection heuristic**: Uses highest `ir_reg` ID as victim, not true liveness distance. Works but is suboptimal.

---

# PART 16 — STACK FRAME

## 16.1 Why Not Use Hardware Stack

The hardware stack has 32 entries and only supports PUSH/POP on ACC. It's designed for return addresses, not for local variables or spill slots. The compiler needs:
- Multiple simultaneous local variables
- Spill slots for register pressure
- Stable addresses for frame pointer relative access

## 16.2 Frame Layout

```
0xE000 ┌─────────────────────────┐ ← Frame base (S7 = 0xE000 + base_offset)
       │  Saved FP (old S7)     │  1 byte
0xE001 ├─────────────────────────┤
       │  Spill Slot 0          │  1 byte
0xE002 ├─────────────────────────┤
       │  Spill Slot 1          │  1 byte
  ...  │       ...              │
0xE0FF └─────────────────────────┘ ← Max frame end
```

## 16.3 Prologue

```asm
# Save old FP
load s6, 224          # S6 = 0xE0 (high byte of frame area)
load acc, <offset>    # ACC = frame base offset
stri s7, s6, acc      # mem[0xE000+offset] = old S7
load s7, <offset>     # S7 = new frame base
```

## 16.4 Epilogue

```asm
push                   # Save return value (ACC) to hardware stack
load s6, 224          # S6 = 0xE0
load acc, <offset>    # ACC = frame base offset
ldri s7, s6, acc      # S7 = old FP (restored from frame)
pop                    # Restore return value from hardware stack to ACC
ret                    # Return (pops return address from hardware stack)
```

## 16.5 Frame Size Tracking

```c
static int g_frame_size = 0;        // Cumulative across all functions
static int g_function_frame_base;    // Per-function base
static int g_spill_count = 0;        // Per-function spill slot count
```

Each function adds its spill slots to `g_frame_size`. The frame base for the next function starts after the previous one.

## 16.6 Limits

- **Max frame**: 256 bytes (`DHAD_FRAME_MAX_SIZE`)
- **Warning**: 240 bytes (`DHAD_FRAME_WARN_SIZE`)
- **Overflow**: If `g_frame_size > 256`, allocation fails silently

---

# PART 17 — ABI / CALLING CONVENTION

## 17.1 Argument Passing

| Argument | Register |
|----------|----------|
| 1st | S0 |
| 2nd | S1 |
| 3rd | S2 |
| 4th | S3 |

More than 4 arguments: NOT IMPLEMENTED (no stack-based argument passing).

## 17.2 Return Value

- Return value in ACC
- CALL handler saves ACC to S6 before restoring FP
- After restore, moves S6 → ACC → destination register

## 17.3 Caller-Saved vs Callee-Saved

| Category | Registers |
|----------|-----------|
| Caller-saved | S0–S5 (allocator saves via spill before CALL) |
| Callee-saved | S7 (FP, saved/restored by prologue/epilogue) |
| Scratch | S6 (never saved, dedicated to address high byte) |
| Return | ACC (not saved across CALL, preserved in S6 by handler) |

## 17.4 Limitations

- Max 4 arguments (no stack overflow for arguments)
- No variadic functions
- No struct passing (all via registers)
- No nested function support (closures)
- Recursion limited to ~30 levels (hardware stack = 32 entries)

---

# PART 18 — POINTERS

## 18.1 Representation

16-bit address: `HIGH BYTE : LOW BYTE`
- HIGH byte stored in S6 (scratch register)
- LOW byte stored in the pointer's DHAD register

## 18.2 Address-Of (`&`)

IR: `ADDRESS_OF` → Backend: `mov dst, src` (copy the register)

## 18.3 Dereference (`*`)

IR: `DEREF` → Backend: LDRI (load indirect) or STRI (store indirect)

```asm
# Load *ptr into dst:
load s6, <high_byte>      # S6 = high byte of address
ldri dst, s6, ptr_reg     # dst = mem[(S6 << 8) | ptr_reg]

# Store val to *ptr:
load s6, <high_byte>      # S6 = high byte of address
stri val_reg, s6, ptr_reg # mem[(S6 << 8) | ptr_reg] = val_reg
```

## 18.4 Pointer Spill

When a pointer is spilled to frame, the full 16-bit address is stored as two consecutive bytes. Reload reconstructs the pair.

---

# PART 19 — ARRAYS

## 19.1 Declaration

```daad
متغير: arr: رقم[10]           # 10-element integer array
متغير: arr2: رقم[5] = {1,2,3,4,5}  # Initialized
```

## 19.2 Element Access

IR: `LOAD_ELEMENT(base, index)` → `STORE_ELEMENT(base, index, value)`

Backend: Address calculation = `base + index * element_size`

For 8-bit data: address = `base + index` (element size = 1)

## 19.3 Limitations

- Fixed-size arrays only (no dynamic allocation)
- No bounds checking
- No 16-bit element support (8-bit data width)
- Runtime index requires multiplication in backend

---

# PART 20 — STRUCTS

## 20.1 Declaration

```daad
نوع: نقطة = { x: رقم, y: رقم }
```

## 20.2 Member Access

IR: `MEMBER_ACCESS(obj, field_name)`

Backend: Address = `base + field_offset`

## 20.3 Limitations

- No nested structs (flat layout only)
- No bitfields
- No alignment/padding management
- No struct copying (must access fields individually)

---

# PART 21 — MEMORY MAP

| Range | Purpose | Writable | Owner |
|-------|---------|----------|-------|
| 0x0000–0xEFFF | Program code + string data | W (at load) | Compiler/Assembler |
| 0xE000–0xE0FF | Compiler frame area | R/W | Compiler (spill/locals) |
| 0xF000–0xF0FF | MMIO I/O ports | R/W | Devices |
| 0xF100–0xF1FF | Interrupt Vector Table | R/W | System |
| 0xF200–0xFFFF | Reserved | — | — |

**Total RAM**: 65,536 bytes
**Usable for code**: ~56KB (0x0000–0xEFFF)
**Usable for frame**: 256 bytes (0xE000–0xE0FF)
**Hardware stack**: 32 entries (separate from memory)

---

# PART 22 — STRINGS

## 22.1 Handling

- String literals stored in the data section with labels
- Backend maintains a string table: `dhad_str_lbl[]`, `dhad_str_dat[]`
- Strings accessed via label references
- Long strings (>255 chars) use `.ds` data section

## 22.2 Print

```daad
اطبع("Hello")        # Print string literal
اطبع(variable)        # Print variable as number
```

## 22.3 Limitations

- No string manipulation functions in compiler
- No Unicode support on DHAD (8-bit ASCII only)
- Web transpiler supports Arabic strings (JavaScript)
- No dynamic string allocation

---

# PART 23 — BACKENDS

## 23.1 DHAD Backend (Primary)

**File**: `compiler/backend/dhad_backend.c` (~1,300 lines)
**Status**: ✅ IMPLEMENTED + VERIFIED (Phase F.3)

Output: DHAD assembly (`.s` files)
Execution: via assembler → binary → emulator

## 23.2 x86 Backend (Legacy)

**File**: `compiler/backend/backend_interface.c`
**Status**: 🟡 PARTIALLY IMPLEMENTED (stub)

Output: x86-64 assembly (not actively maintained)
Used for host-side testing only.

## 23.3 Web Transpiler (JavaScript)

**Files**: `server/src/utils/dhad/ast.js`, `lexer.js`, `parser.js`, `codegen.js`
**Status**: ✅ IMPLEMENTED

Transpiles لغة ض → JavaScript for browser execution.
Separate implementation (not using the C compiler).

## 23.4 C++ Transpiler

**Files**: `src/AST.cpp`, `CodeGen.cpp`, `Lexer.cpp`, `Parser.cpp`
**Status**: 🔵 PROTOTYPE

Transpiles لغة ض → C++. Environment-dependent tests.

---

# PART 24 — WEB / SANDBOX

## 24.1 Architecture

```
Browser Request → Node.js Server → dhadCompiler.js → dhadSandbox.js → Response
```

## 24.2 Security Model

- **VM isolation**: Node.js `vm` module with strict sandbox
- **Proxy protection**: Blocks `constructor`/`__proto__` escape
- **Limits**: 5s timeout, 64KB output, 50KB code, 32MB heap, 100K loop iterations
- **No**: `require`, `process`, `fs`, `child_process`, `http`, `eval`, `Function`

## 24.3 Security Tests

- `tests/security/serverSandbox.test.js` — Server-side sandbox escape tests
- `tests/security/webSandbox.test.js` — Web transpiler security tests
- `pentest.js`, `deep_pentest.js` — Penetration testing scripts

---

# PART 25 — C++ TRANSPILER

**Location**: `dhad-studio/src/`
**Files**: AST.cpp, CodeGen.cpp, Lexer.cpp, Parser.cpp + headers in `include/Daad/`

Status: 🔵 PROTOTYPE
- Transpiles to C++ code
- Requires external C++ compiler (g++)
- Environment-dependent tests
- Not used for DHAD target

---

# PART 26 — STANDARD LIBRARY

## 26.1 Location

- `dhad-studio/stdlib/` — English-named (100 files, 10 categories)
- `dhad-studio/stdlib_arabic/` — Arabic-named (100 files, 10 categories)

## 26.2 Categories

| Category | Files | Examples |
|----------|-------|----------|
| math/ | 10 | basic_operations, roots_powers, complex_numbers |
| text/ | 10 | search_replace, encryption_encoding, parsing |
| io/ | 10 | reading_writing, logging, compression |
| data_structures/ | 10 | linked_lists, binary_trees, graphs |
| time/ | 10 | calendar_dates, scheduling, timestamps |
| web/ | 10 | dom_elements, routing, css_styling |
| networking/ | 10 | http_https, tcp_udp, websockets |
| database/ | 10 | sql_commands, caching, transactions |
| graphics/ | 10 | drawing, animations, image_processing |
| concurrency/ | 10 | threads, semaphores, actor_pattern |

## 26.3 Status

- 🟠 IMPLEMENTED BUT NOT VERIFIED — These are `.ض` files that define the API but compilation/execution has not been systematically tested
- No DHAD compatibility testing
- No test coverage data available

---

# PART 27 — TOOLS

| Tool | Location | Language | Purpose |
|------|----------|----------|---------|
| Compiler CLI | `DAAD/compiler/main.c` | C | `./build/daad file.daad --target=dhad -o out.s` |
| Assembler | `cpu/src/dhad_asm.c` | C | Assembles `.ضb` to binary |
| Disassembler | `cpu/src/dhad_disasm.c` | C | Disassembles binary to assembly |
| CPU Emulator | `cpu/src/dhad_cpu.c` | C | Executes binary on virtual CPU |
| DhadSystem | `cpu/src/dhad_system.c` | C | Full system (CPU+Display+KB+Timer) |
| Qt GUI | `cpu/gui/main.cpp` | C++ | Visual CPU simulator |
| VS Code Extension | `vscode-extension/src/extension.ts` | TypeScript | Syntax highlighting + snippets |
| Electron App | `desktop-app/main.js` | JavaScript | Desktop IDE |
| Test Runner | `cpu/run_tests.sh` | Bash | Runs 114 DHAD tests |
| E2E Runner | `DAAD/tests/phase12_5/run_e2e.sh` | Bash | Runs 12 E2E tests |

---

# PART 28 — TESTING

## 28.1 Test Inventory

| Suite | Location | Count | Type | Status |
|-------|----------|-------|------|--------|
| DHAD CPU Tests | `cpu/tests/` | 114 | Unit + Integration | ✅ ALL PASS |
| DAAD Unit Tests | `DAAD/compiler/tests/` | 11 | Compile-only | ✅ All compile |
| E2E Execution | `DAAD/tests/phase12_5/` | 12 | Runtime E2E | ✅ ALL PASS |
| E2E B3 Tests | `DAAD/e2e/` | 131 | Compile + Execute | 🟡 Partially verified |
| Security Tests | `server/tests/security/` | 2 | Sandbox escape | ✅ PASS |
| Server Unit Tests | `server/tests/unit/` | 14 | Node.js unit | 🟡 Some verified |
| Integration Tests | `server/tests/integration/` | 8 | API integration | 🟡 Some verified |
| Stress Tests | `tests/` (root) | 10+ | Load/stress | 🟠 Run manually |
| Pentest | `tests/` (root) | 5+ | Security audit | 🟠 Run manually |

## 28.2 DHAD Test Categories (114 tests)

| Category | Tests | Description |
|----------|-------|-------------|
| ISA Core | ~30 | Arithmetic, logic, comparison |
| Control Flow | ~15 | Branches, jumps, calls |
| Memory | ~10 | Load, store, LDRI, STRI |
| Stack | ~8 | Push, pop, call, ret |
| System | ~15 | Display, keyboard, timer |
| Security | ~8 | NULL, bounds, overflow |
| Backend Equivalence | ~12 | Emulator vs Host |
| Runtime | ~9 | Init, backend, putchar |

---

# PART 29 — SECURITY

## 29.1 Attack Surfaces

1. **Web sandbox**: Student code execution in browser/server
2. **Parser**: Malformed Arabic input
3. **Generated code**: Compiler output execution
4. **MMIO**: Device register access

## 29.2 Defenses

- VM sandbox with Proxy-based prototype chain blocking
- Time, memory, and iteration limits
- No filesystem/network/process access in sandbox
- Input validation on parser

## 29.3 Known Limitations

- 8-bit data limits but doesn't prevent overflow attacks
- No ASLR/DEP (8-bit CPU has no memory protection)
- Hardware stack overflow not checked at runtime

---

# PART 30 — BUG DATABASE

## 30.1 Historical Bugs (Fixed)

| ID | Phase | Component | Symptom | Root Cause | Fix |
|----|-------|-----------|---------|------------|-----|
| B1 | F.2 | DSE | deepest(8)=7 | Store eliminated without checking intermediate load | Added `loaded_between` check |
| B2 | F.2 | CALL | Values corrupted | Double spill/reload (handler + allocator) | Removed handler push/pop, added proactive spill |
| B3 | F.2 | Labels | Assembler warnings | Generic labels across functions | Added function-name prefix |
| B4 | F.2 | NOP | Wrong value | Two-register NOP emitted bare `nop` | Added `mov dst, src` fallback |
| B5 | 12.5-B | CMP | Encoding collision | CMP imm overlapped RET/NEG | Changed to EXT encoding |

## 30.2 Known Current Bugs

| ID | Component | Description | Severity |
|----|-----------|-------------|----------|
| KB1 | Allocator | Post-instruction reg_map not updated after ALU | MEDIUM |
| KB2 | Allocator | Victim selection uses highest ir_reg, not true liveness | LOW |
| KB3 | Hardware | 32-entry stack limits recursion to ~30 levels | MEDIUM |
| KB4 | Compiler | No multi-file compilation/linking | HIGH |
| KB5 | Compiler | Max 4 function arguments | MEDIUM |

---

# PART 31 — TECHNICAL DEBT

## CRITICAL
- No linker for multi-file programs
- No runtime error checking (division by zero, stack overflow)

## HIGH
- Single-pass compilation (no separate compilation)
- No debug symbols / source mapping
- Frame size limit (256 bytes) not enforced at compile time

## MEDIUM
- Victim selection heuristic is suboptimal
- SSA not used in DHAD backend
- C++ transpiler not maintained
- Web transpiler is a separate implementation (code duplication)

## LOW
- Label counter resets could be cleaner
- Some header file conflicts (dhad_isa.h vs dhad_isa_constants.h)
- Test infrastructure is ad-hoc (shell scripts, not a framework)

---

# PART 32 — INCOMPLETE FEATURES

| Feature | State | Missing | Next Step |
|---------|-------|---------|-----------|
| Multi-file compilation | NOT IMPLEMENTED | Linker, separate compilation | Design linker |
| Debugger | NOT IMPLEMENTED | Step, breakpoints, watch | Implement GDB-like interface |
| Float on DHAD | NOT IMPLEMENTED | FPU or software float | Design FPU ISA extension |
| 16-bit data | NOT IMPLEMENTED | Extended ALU, register pairs | ISA v5.0 design |
| Dynamic allocation | NOT IMPLEMENTED | Heap manager | Design heap in memory map |
| Exceptions | STUB | Try/catch AST exists but no codegen | Implement exception mechanism |
| Struct passing | PARTIAL | Only field-by-field | Design ABI for structs |
| Arrays on DHAD | PARTIAL | LOAD_ELEMENT/STORE_ELEMENT exist but limited | Verify end-to-end |
| For loop | PARTIAL | IR exists, DHAD codegen works for simple cases | Verify complex cases |
| Strings on DHAD | PARTIAL | Print works, manipulation doesn't | Implement string ops |

---

# PART 33 — ARCHITECTURAL RISKS

| Risk | Severity | Description |
|------|----------|-------------|
| 8-bit data width | HIGH | Limits all computation to 0–255 |
| 32-entry stack | HIGH | Limits recursion depth |
| 256-byte frame | MEDIUM | Limits local variables per function |
| No memory protection | HIGH | Any pointer can corrupt any memory |
| Single address space | MEDIUM | Code + data + frame share 64KB |
| No hardware multiply | LOW | MUL is microcoded (emulated in CPU) |
| CMP collision history | LOW | Encoding space is tight |
| ABI immaturity | MEDIUM | No formal ABI spec for DHAD |
| No FPGA path | HIGH | Hardware transition is unverified |
| Web transpiler divergence | MEDIUM | Separate implementation may drift |

---

# PART 34 — COMPLETENESS DASHBOARD

| Component | Implemented | Verified | Production | Status |
|-----------|-------------|----------|------------|--------|
| لغة ض (Language) | ✅ | ✅ | Partial | 🟢 Solid |
| Lexer (C) | ✅ | ✅ | Yes | 🟢 Solid |
| Parser (C) | ✅ | ✅ | Yes | 🟢 Solid |
| AST (C) | ✅ | ✅ | Yes | 🟢 Solid |
| Semantic (C) | ✅ | ✅ | Yes | 🟢 Solid |
| IR (C) | ✅ | ✅ | Yes | 🟢 Solid |
| Optimizer | ✅ | ✅ | Yes | 🟢 Solid |
| SSA | ✅ | 🟡 | No | 🟡 Partial |
| DHAD Backend | ✅ | ✅ | Yes | 🟢 Solid |
| DHAD Assembler | ✅ | ✅ | Yes | 🟢 Solid |
| DHAD CPU | ✅ | ✅ | Yes | 🟢 Solid |
| DhadSystem | ✅ | ✅ | Partial | 🟢 Solid |
| disassembler | ✅ | ✅ | Yes | 🟢 Solid |
| Runtime | ✅ | ✅ | Partial | 🟡 Partial |
| Standard Library | 🟡 | ❌ | No | 🟠 Unverified |
| x86 Backend | 🟡 | ❌ | No | 🔵 Stub |
| Web Transpiler | ✅ | ✅ | Yes | 🟢 Solid |
| C++ Transpiler | 🟡 | 🟡 | No | 🔵 Prototype |
| VS Code Extension | ✅ | 🟡 | Yes | 🟡 Partial |
| Electron IDE | ✅ | 🟡 | Yes | 🟡 Partial |
| Security | ✅ | ✅ | Partial | 🟢 Solid |
| Documentation | ✅ | ✅ | Yes | 🟢 Solid |
| Hardware (FPGA) | ❌ | ❌ | No | ❌ Not started |
| Physical chip | ❌ | ❌ | No | ❌ Not started |

---

# PART 35 — HARDWARE PATH

## 35.1 Current State

- CPU emulator (`dhad_cpu.c`) is cycle-accurate
- ISA is well-defined in `dhad_isa_constants.h`
- Memory map and MMIO are designed

## 35.2 What Can Be Reused

- ISA specification (dhad_isa_constants.h)
- Instruction encoding (already binary-compatible)
- Memory map design
- MMIO device register definitions
- Assembler (output format can be adapted)

## 35.3 What Needs Redesign

- CPU step function → RTL behavioral model
- Memory array → SRAM/ROM blocks
- Device callbacks → Hardware peripheral modules
- Stack array → Register file with SP
- FLAGS → Status register with hardware flags

## 35.4 What Doesn't Represent Hardware

- `malloc` in emulator (real hardware uses SRAM)
- `printf` in emulator (real hardware uses UART/MMIO)
- `vm.Script` in web sandbox (pure software)
- JavaScript transpiler (no hardware equivalent)

---

# PART 36 — FUTURE ROADMAP

## NOW (Phase F.4+)
- Fix remaining allocator edge cases
- Add more E2E tests for nested calls
- Update ABI documentation
- Multi-file compilation design

## NEXT (Phase 13)
- Linker implementation
- Debug symbols
- Formal ABI specification
- 16-bit data width design (ISA v5)

## LATER (Phase 14+)
- RTL design for معالج ض
- FPGA implementation (Lattice iCE40 or similar)
- Physical ASIC design

## LONG TERM
- Arabic operating system
- Hardware prototype
- Educational deployment

---

# PART 37 — "HOW ONE PROGRAM TRAVELS"

## Simple Program

```daad
متغير: x = 5
```

### Trace

1. **Lexer**: Tokenizes `متغير` `:` `x` `=` `5`
2. **Parser**: Creates `NODE_VARIABLE_DECL(name="x", init=NODE_LITERAL(5))`
3. **Semantic**: Resolves type (inferred as `رقم`), adds to symbol table
4. **IR Gen**: `%1 = alloca 0` → `store 5, %1`
5. **Optimizer**: Constant folding (no change for simple store)
6. **DHAD Backend**: `load s0, 5` (or spill if S0 occupied)

## Complex Program

```daad
دالة: factorial(n: رقم) -> رقم :-
    إذا (n <= 1) :- ارجع(1) نهاية
    متغير: r = factorial(n - 1)
    ارجع(r * n)
نهاية
```

### Trace

1. **Lexer**: ~30 tokens
2. **Parser**: `NODE_FUNCTION_DECL` with `NODE_IF_STATEMENT`, recursive call, multiply
3. **Semantic**: Type-check params, verify recursion
4. **IR Gen**: Multiple basic blocks (entry, if_then, if_merge, exit), PHI nodes
5. **Optimizer**: Dead store elimination, constant folding
6. **Register Allocation**: S0=param, S1=local, spill/reload around CALL
7. **DHAD Backend**: Prologue → comparison → branch → recursive CALL → multiply → epilogue
8. **Assembly**: `load s1, 0; mov s1, s0; ... call factorial; ... mul s1; ... ret`
9. **Assembler**: Binary: `0x10 0x00 0x21 0x10 ...`
10. **CPU**: Fetch-decode-execute loop, 275 cycles for factorial(5)

---

# PART 38 — CODE LEVEL INDEX

## Key Functions

| Function | File | Purpose |
|----------|------|---------|
| `main()` | `compiler/main.c:46` | CLI entry, runs full pipeline |
| `lexer_next_token()` | `compiler/lexer/lexer.c` | Tokenize next token |
| `parser_parse()` | `compiler/parser/parser.c` | Parse tokens to AST |
| `ir_module_create()` | `compiler/ir/ir_module.c` | Create IR module |
| `ir_builder_*` | `compiler/ir/ir_builder.c` | IR instruction builders |
| `emit_dhad_function()` | `compiler/backend/dhad_backend.c:1216` | Emit full function |
| `emit_inst()` | `compiler/backend/dhad_backend.c:433` | Emit single IR instruction |
| `ensure_in_reg()` | `compiler/backend/dhad_backend.c:243` | Ensure value in register |
| `spill_to_frame()` | `compiler/backend/dhad_backend.c:208` | Spill register to memory |
| `dhad_cpu_step()` | `cpu/src/dhad_cpu.c:223` | Execute one CPU instruction |
| `dhad_asm()` | `cpu/src/dhad_asm.c:708` | Two-pass assembler |

## Key Structs

| Struct | File | Purpose |
|--------|------|---------|
| `IRModule` | `compiler/ir/ir_module.h` | Top-level IR container |
| `IRFunction` | `compiler/ir/ir_function.h` | Function IR |
| `IRBasicBlock` | `compiler/ir/ir_basic_block.h` | Basic block |
| `IRInstruction` | `compiler/ir/ir_instruction.h` | Single instruction |
| `IRValue` | `compiler/ir/ir_value.h` | Universal value |
| `ASTNode` | `compiler/ast/ast_node.h` | AST node (32 variants) |
| `DhadCPU` | `cpu/include/dhad_cpu.h` | Full CPU state |
| `DhadSystem` | `cpu/include/dhad_system.h` | Complete system |
| `DhadDeviceBus` | `cpu/include/dhad_device.h` | Device bus |
| `DHADRegMapping` | `compiler/backend/dhad_backend.c` | Register allocation entry |

## Key Constants

| Constant | File | Value |
|----------|------|-------|
| `DHAD_DATA_WIDTH` | `cpu/include/dhad_isa_constants.h:42` | 8 |
| `DHAD_ADDR_WIDTH` | `cpu/include/dhad_isa_constants.h:45` | 16 |
| `DHAD_STACK_SIZE` | `cpu/include/dhad_isa_constants.h:51` | 32 |
| `DHAD_MEM_SIZE` | `cpu/include/dhad_isa_constants.h:54` | 65536 |
| `DHAD_FRAME_BASE_ADDR` | `compiler/backend/dhad_backend.c:67` | 0xE000 |
| `DHAD_FRAME_MAX_SIZE` | `compiler/backend/dhad_backend.c:69` | 256 |
| `DHAD_NUM_REGS` | `compiler/backend/dhad_backend.c:62` | 6 |
| `DHAD_SCRATCH_REG` | `compiler/backend/dhad_backend.c:63` | 6 (S6) |
| `DHAD_FRAME_REG` | `compiler/backend/dhad_backend.c:64` | 7 (S7) |

---

# PART 39 — FILE-BY-FILE REFERENCE

## DAAD Compiler

| File | LOC | Role | Key Symbols |
|------|-----|------|-------------|
| `main.c` | 221 | CLI entry | `main()`, `read_file()` |
| `ast/ast_node.h` | 320 | AST definitions | `NodeType`, `ASTNode`, `BinaryOperator` |
| `ast/ast.c` | 73 | AST utilities | `ast_node_create()`, `ast_node_destroy()` |
| `ast/ast_builder.c/h` | — | AST construction | `ast_builder_*` |
| `ast/ast_printer.c/h` | — | AST pretty-print | `ast_printer_*` |
| `ast/ast_visitor.c/h` | — | AST traversal | `ast_visitor_*` |
| `backend/dhad_backend.c` | 1313 | DHAD codegen | `emit_dhad_function()`, `emit_inst()` |
| `backend/backend_interface.c/h` | — | Backend abstraction | `backend_create_dhad()`, `BackendTarget` |
| `cfg/cfg.c/h` | — | Control flow graph | `cfg_build()` |
| `codegen/emitter.c/h` | — | Code emission | `emitter_*` |
| `codegen/expression_generator.c/h` | — | Expression codegen | — |
| `codegen/function_generator.c/h` | — | Function codegen | — |
| `codegen/label_manager.c/h` | — | Label generation | `label_manager_generate()` |
| `codegen/register_allocator.c/h` | — | Register allocation (old) | — |
| `codegen/stack_manager.c/h` | — | Stack frame (old) | — |
| `codegen/statement_generator.c/h` | — | Statement codegen | — |
| `ir/ir_instruction.h` | 84 | IR instruction defs | `IROpcode`, `IRInstruction` |
| `ir/ir_value.h` | 47 | IR value defs | `IRValue`, `IRValueKind` |
| `ir/ir_types.h` | 42 | IR type defs | `IRType`, `IRTypeKind` |
| `ir/ir_module.h` | 43 | IR module | `IRModule`, `ir_module_create()` |
| `ir/ir_function.h` | 40 | IR function | `IRFunction`, `ir_function_create()` |
| `ir/ir_basic_block.h` | 34 | Basic block | `IRBasicBlock`, `ir_bb_create()` |
| `ir/ir_builder.c/h` | — | IR builders | `ir_builder_*` |
| `ir/ir_printer.c/h` | — | IR pretty-print | `ir_print_module()` |
| `ir/ir_validate.c/h` | — | IR validation | `ir_validate_*` |
| `ir/ir_verifier.c/h` | — | IR verification | `ir_verify_*` |
| `ir/liveness.c/h` | — | Liveness analysis | `liveness_*` |
| `lexer/lexer.c/h` | 147+ | Tokenizer | `lexer_create()`, `lexer_next_token()` |
| `lexer/unicode.c/h` | — | Unicode handling | `unicode_*` |
| `lexer/keywords.c/h` | — | Keyword table | `keywords_*` |
| `optimizer/optimizer_pass.c/h` | — | Opt passes | `optimizer_run_all()` |
| `parser/parser.c/h` | 56+ | Parser | `parser_create()`, `parser_parse()` |
| `parser/parser_error.c/h` | — | Error handling | `parser_error_*` |
| `parser/parser_recovery.c` | — | Error recovery | — |
| `semantic/semantic_visitor.c/h` | — | Semantic analysis | `semantic_visit_*` |
| `semantic/type_checker.c/h` | — | Type checking | `type_checker_*` |
| `semantic/symbol_table.c/h` | — | Symbol table | `symbol_table_*` |
| `semantic/scope.c/h` | — | Scope management | `scope_*` |
| `semantic/type_registry.c/h` | — | Type registry | `type_registry_*` |
| `semantic/type.c/h` | 154 | Type definitions | `DaadType`, `DaadTypeKind` |
| `semantic/symbol.c/h` | 32 | Symbol definitions | `Symbol`, `SymbolKind` |
| `semantic/constant_folding.c/h` | — | Constant folding | `constant_folding_*` |
| `ssa/ssa.c/h` | — | SSA conversion | `ssa_convert_to_ssa()` |
| `token/token.c/h` | — | Token types | `token_type_name()` |
| `token/token_types.h` | 216 | Token definitions | `TokenType` (206 values) |

## DHAD CPU

| File | LOC | Role | Key Symbols |
|------|-----|------|-------------|
| `src/dhad_cpu.c` | 663 | CPU core | `dhad_cpu_step()`, `dhad_cpu_run()` |
| `src/dhad_asm.c` | 741 | Assembler | `dhad_asm()` |
| `src/dhad_disasm.c` | 414 | Disassembler | `dhad_disasm()` |
| `src/dhad_runtime.c` | 189 | Runtime | `dhad_runtime_run()` |
| `src/dhad_system.c` | 120 | Full system | `dhad_system_init()` |
| `src/dhad_device.c` | 140 | Device bus | `dhad_bus_*` |
| `src/dhad_display.c` | — | Display device | `dhad_display_*` |
| `src/dhad_keyboard.c` | — | Keyboard device | `dhad_keyboard_*` |
| `src/dhad_timer.c` | — | Timer device | `dhad_timer_*` |
| `src/dhad_intc.c` | — | Interrupt controller | `dhad_intc_*` |
| `src/dhad_backend_emulator.c` | — | Emulator backend | — |
| `src/dhad_backend_host.c` | — | Host backend | — |
| `src/dhad_console.c` | — | Console I/O | — |
| `src/dhad_debug.c` | — | Debug utilities | — |
| `src/main.c` | — | CLI entry | — |
| `gui/main.cpp` | — | Qt GUI | — |
| `include/dhad_isa_constants.h` | 842 | ISA constants | Everything ISA-related |
| `include/dhad_isa.h` | 188 | ISA definitions | `DhadCPU`, opcodes |
| `include/dhad_cpu.h` | 250 | CPU interface | `DhadCPU`, `dhad_cpu_*` |
| `include/dhad_memory_map.h` | 131 | Memory map | Address regions, MMIO |
| `include/dhad_backend.h` | 117 | Backend interface | `DhadBackendVtable` |
| `include/dhad_runtime.h` | 125 | Runtime interface | `dhad_runtime_*` |
| `include/dhad_system.h` | 177 | System interface | `DhadSystem`, `dhad_system_*` |
| `include/dhad_device.h` | 166 | Device interface | `DhadDeviceBus`, `DhadDevice` |

---

# PART 40 — CONFIGURATION

## 40.1 Environment Variables

None required for standard build/test.

## 40.2 Build Flags

| Flag | Where | Effect |
|------|-------|--------|
| `-lm` | DAAD build | Links math library |
| `-O2` | CPU Makefile | Optimization level |
| `-Wall -Wextra` | CPU Makefile | All warnings |

## 40.3 Runtime Settings

| Setting | Value | Where |
|---------|-------|-------|
| `DHAD_MAX_CYCLES` | 100000 | `dhad_isa_constants.h:72` |
| `DHAD_FRAME_MAX_SIZE` | 256 | `dhad_backend.c:69` |
| `DHAD_STACK_SIZE` | 32 | `dhad_isa_constants.h:51` |
| `MAX_IR_REGS` | 256 | `dhad_backend.c:72` |

---

# PART 41 — DEPENDENCIES

## External Libraries

| Library | Where | Purpose |
|---------|-------|---------|
| libc | Everywhere | Standard C library |
| libm | DAAD compiler | Math functions |
| Qt5 | CPU GUI | Desktop GUI framework |
| Node.js | daad-studio server | Server runtime |
| SQLite | Server DB | Local development database |
| Prisma | Server DB ORM | Database access |

## No External C Dependencies (Compiler)

The DAAD compiler uses only libc and libm. No external libraries.

---

# PART 42 — RELEASE HISTORY

| Version | Tag | Commit | Date | Notes |
|---------|-----|--------|------|-------|
| v1.0.0 | `v1.0.0` | `330a7ee` | 2026-08 | First tagged release |
| Pre-release | `pre-phase5-checkpoint` | — | Earlier | Checkpoint tag |

**Git**: `daad-studio` repo has 17+ commits on `main` branch.
**DAAD compiler**: NOT a git repo.
**DHAD CPU**: NOT a git repo.

---

# PART 43 — REPRODUCIBILITY

## Build from Scratch

```bash
# 1. Clone daad-studio
git clone <repo>
cd dhad-studio

# 2. Install Node.js dependencies
cd server && npm install

# 3. Build DAAD compiler (requires gcc)
cd "../../DAAD"
mkdir -p build
gcc -o build/daad compiler/main.c \
  compiler/token/token.c \
  compiler/lexer/lexer.c compiler/lexer/unicode.c compiler/lexer/keywords.c \
  compiler/parser/parser.c compiler/parser/parser_error.c compiler/parser/parser_recovery.c \
  compiler/ast/ast.c compiler/ast/ast_builder.c compiler/ast/ast_printer.c compiler/ast/ast_visitor.c \
  compiler/semantic/semantic_visitor.c compiler/semantic/semantic_error.c \
  compiler/semantic/symbol_table.c compiler/semantic/symbol.c \
  compiler/semantic/scope.c compiler/semantic/scope_stack.c \
  compiler/semantic/type_checker.c compiler/semantic/type_registry.c \
  compiler/semantic/type.c compiler/semantic/constant_folding.c \
  compiler/ir/ir_module.c compiler/ir/ir_function.c compiler/ir/ir_basic_block.c \
  compiler/ir/ir_instruction.c compiler/ir/ir_value.c compiler/ir/ir_types.c \
  compiler/ir/ir_printer.c compiler/ir/ir_builder.c compiler/ir/ir_visitor.c \
  compiler/ir/ir_validate.c compiler/ir/ir_verifier.c compiler/ir/debug_dumps.c \
  compiler/ir/liveness.c \
  compiler/ssa/ssa.c \
  compiler/optimizer/optimizer_pass.c \
  compiler/cfg/cfg.c \
  compiler/codegen/emitter.c compiler/codegen/expression_generator.c \
  compiler/codegen/function_generator.c compiler/codegen/label_manager.c \
  compiler/codegen/register_allocator.c compiler/codegen/stack_manager.c \
  compiler/codegen/statement_generator.c \
  compiler/backend/backend_interface.c compiler/backend/dhad_backend.c \
  -lm -I compiler -I include

# 4. Build DHAD CPU (requires gcc + Qt5)
cd "../../cpu"
make

# 5. Run tests
bash run_tests.sh
cd "../مشروع ض/DAAD"
bash tests/phase12_5/run_e2e.sh

# 6. Sample program
./build/daad tests/phase12_5/12_recursion_depth.daad --target=dhad -o test.s
../cpu/dhad_cpu test.s
```

---

# PART 44 — TROUBLESHOOTING

## Build Failures

| Problem | Solution |
|---------|----------|
| `gcc: command not found` | Install gcc: `sudo apt install gcc` |
| `Qt5 not found` | Install Qt5: `sudo apt install qt5-default` |
| `math.h not found` | Add `-lm` flag |
| `undefined reference` | Missing .c file in build command |

## Runtime Failures

| Problem | Solution |
|---------|----------|
| `النتيجة: 0` when expecting non-zero | Check CALL convention, spill/reload |
| Assembler warnings about labels | Already fixed in F.3 |
| `تحذير 128` (label redefined) | Old assembly file, recompile |
| Deep recursion crashes | Limited to ~30 levels (hardware stack) |
| Frame overflow | Max 256 bytes, reduce locals/spills |

---

# PART 45 — GLOSSARY

| Term | Definition |
|------|-----------|
| لغة ض | The high-level Arabic programming language |
| ض بسيطة | The low-level Arabic assembly language |
| معالج ض | The custom CPU architecture |
| DhadSystem | The complete system: CPU + Display + Keyboard + Timer |
| DAAD | The C compiler project (legacy name) |
| DHAD | Dhad Hardware — refers to the CPU/ISA |
| IR | Intermediate Representation |
| SSA | Static Single Assignment |
| CFG | Control Flow Graph |
| ABI | Application Binary Interface |
| FP | Frame Pointer (S7) |
| ACC | Accumulator register |
| LDRI | Load Register Indirect (16-bit address) |
| STRI | Store Register Indirect (16-bit address) |
| MMIO | Memory-Mapped I/O |
| IVT | Interrupt Vector Table |
| spill | Save a register value to the frame in memory |
| reload | Load a spilled value from the frame back to a register |
| live range | The span of instructions where a value is needed |
| prologue | Code at function entry (save FP, set frame) |
| epilogue | Code at function exit (restore FP, return) |
| caller-saved | Registers the caller must preserve (S0–S5) |
| callee-saved | Registers the callee must preserve (S7) |
| scratch | Register dedicated to compiler use (S6) |

---

# PART 46 — MASTER LIMITATIONS

What this project **CANNOT** reliably do today:

1. **No 16-bit data operations** — All arithmetic is 8-bit (0–255)
2. **No deep recursion** — Hardware stack limited to 32 entries
3. **No multi-file programs** — No linker exists
4. **No dynamic memory** — No malloc/free
5. **No floating point** — Float type exists in high-level language but not on DHAD
6. **No struct passing** — Structs must be passed field-by-field
7. **No string manipulation** — Print only, no concat/compare/slice
8. **No error recovery at runtime** — Division by zero, stack overflow are unchecked
9. **No debugger** — No stepping, breakpoints, or watchpoints
10. **No formal ABI** — ABI is implicit in the backend code
11. **No test framework** — Tests are shell scripts, not a testing framework
12. **No CI/CD** — No automated build/test pipeline
13. **No FPGA implementation** — CPU is emulator-only
14. **No optimization reports** — No way to see what passes did
15. **No source mapping** — No mapping from generated assembly back to source

---

# PART 47 — MASTER TODO

## P0 (Critical)
1. Multi-file compilation + linker
2. Runtime error checking (div by zero, stack overflow)
3. Formal ABI specification document

## P1 (High)
4. 16-bit data width (ISA v5)
5. Debugger implementation
6. Comprehensive E2E test suite (100+ programs)
7. Frame overflow detection at compile time

## P2 (Medium)
8. SSA integration in DHAD backend
9. Better victim selection in allocator
10. Float support (software or FPU)
11. Struct passing ABI
12. String manipulation functions

## P3 (Low)
13. CI/CD pipeline
14. Test framework migration
15. Performance benchmarks
16. Documentation generation from source

---

# PART 48 — SOURCE OF TRUTH MATRIX

| Topic | Source of Truth |
|-------|----------------|
| ISA encoding | `cpu/include/dhad_isa_constants.h` |
| CPU execution | `cpu/src/dhad_cpu.c` |
| Assembler syntax | `cpu/src/dhad_asm.c` |
| Memory map | `cpu/include/dhad_memory_map.h` |
| Compiler IR | `compiler/ir/ir_instruction.h` |
| AST nodes | `compiler/ast/ast_node.h` |
| Token types | `compiler/token/token_types.h` |
| DHAD codegen | `compiler/backend/dhad_backend.c` |
| Register allocation | `compiler/backend/dhad_backend.c` (ensure_in_reg, spill_to_frame) |
| Frame layout | `compiler/backend/dhad_backend.c` (emit_dhad_function) |
| ABI | Implicit in `compiler/backend/dhad_backend.c` CALL handler |
| Type system | `compiler/semantic/type.h` |
| Optimization | `compiler/optimizer/optimizer_pass.c` |
| Device MMIO | `cpu/include/dhad_memory_map.h` |
| Language grammar | `docs/grammar.md` |
| Type system spec | `docs/type-system.md` |
| ABI spec (x86) | `docs/abi.md` |

---

# PART 49 — CONTRADICTIONS

## Found Contradictions

1. **`dhad_isa.h` vs `dhad_isa_constants.h`**: Two separate ISA definition files exist. `dhad_isa.h` defines a 4-bit/12-bit variant while `dhad_isa_constants.h` defines the 8-bit/16-bit version. The CPU uses `dhad_isa_constants.h`. The `dhad_isa.h` file appears to be legacy/unused.

2. **ABI doc describes x86**: `docs/abi.md` describes x86-64/ARM64/RISC-V calling conventions. The actual DHAD calling convention is implemented in `dhad_backend.c` but not documented in a separate DHAD ABI document.

3. **Frame base accumulation**: `g_frame_size` accumulates across ALL functions. Each function's frame starts where the previous one ended. This means deep call chains consume more frame space, but the doc doesn't mention this.

4. **`docs/type-system.md` says 8-byte types**: The type system doc says `رقم` is 8 bytes (64-bit). But on DHAD, it's actually 8-bit (1 byte). The doc describes the x86 target, not DHAD.

---

# PART 50 — COMPLETENESS AUDIT

## Document Coverage

| Area | Covered | Confidence |
|------|---------|------------|
| Architecture | ✅ | HIGH |
| Language syntax | ✅ | HIGH |
| ISA specification | ✅ | HIGH |
| CPU implementation | ✅ | HIGH |
| Compiler pipeline | ✅ | HIGH |
| IR design | ✅ | HIGH |
| Register allocation | ✅ | HIGH |
| Frame management | ✅ | HIGH |
| Backend codegen | ✅ | HIGH |
| Testing | ✅ | HIGH |
| Security | ✅ | MEDIUM |
| Bugs | ✅ | HIGH |
| Incomplete features | ✅ | HIGH |
| Hardware path | ✅ | MEDIUM |
| Standard library | ✅ | LOW (not verified) |
| Web transpiler | ✅ | MEDIUM |
| Desktop IDE | ✅ | LOW |
| CI/CD | ✅ | HIGH (none exists) |

## What's NOT Covered

- Individual line-by-line code comments
- Every test case output
- Performance benchmarks
- Detailed commit history analysis
- Browser compatibility testing

## Confidence Level: **HIGH**

The document covers all architectural, implementation, and testing aspects with evidence-based analysis. Known gaps are explicitly marked.

---

# FINAL STATUS SNAPSHOT

```
Project: لغة ض + ض بسيطة + معالج ض
Date: 2026-09-04
Compiler: ✅ Working (23,597 LOC C)
Assembler: ✅ Working (741 LOC C)
CPU Emulator: ✅ Working (663 LOC C)
DHAD Tests: 114/114 PASS
E2E Tests: 12/12 PASS
Deepest(8): Returns 8 ✅
Factorial(5): Returns 120 ✅
Label Collisions: Fixed ✅
Hardware: Emulator only
```

---

**END OF PROJECT MASTER REFERENCE**
