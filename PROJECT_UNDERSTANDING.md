# PROJECT_UNDERSTANDING.md — Dhad-Studio-Unified

> **المنهجية:** DISCOVER → READ → UNDERSTAND → MAP → REPORT — بدون تعديل أي ملف كود.
> **النطاق المقروء فعلًا:** جذر المشروع + `Compiler/` + `DAAD/` + `CPU/` + `Studio/` + `Docs/` + `Scripts/` + `Tests/` + `Examples/` + `Sources-Archive/` + `Releases/` + `.github/` + `.opencode/` + `opencode.json` + `.gitignore`.
> **أدوات التحقق:** `Read` مباشر للملفات الحرجة + `Grep` لـ `TODO|FIXME|STUB|unimplemented` + قراءة متوازية عبر 5 وكلاء استكشاف + فحص `CMakeLists/Makefile/package.json/Dockerfile/ci-cd.yml`.
> **قاعدة الدقة المستخدمة أدناه:** `FACT` = مثبت من ملف مقروء، `INFERENCE` = استنتاج منطقي، `UNKNOWN` = لا يمكن إثباته بالقراءة وحدها.

---

## 1. Project Overview — ما هو المشروع؟

**FACT:** المشروع في `/home/m_hmoz/Documents/Dhad-Studio-Unified` هو **مونوربو موحّد (Unified monorepo)** يجمع 4 عوالم كانت سابقًا مستودعات منفصلة:

```text
Dhad-Studio-Unified/
├── Compiler/          # مترجم C++20: عربي (.ض) → C++ (.cpp + .hpp) — مشروع DaadStudio v1.0.0
├── DAAD/              # مترجم C99: عربي منخفض المستوى (.daad) → x86-64 (.s) أو DHAD (.ضasm)
├── CPU/               # محاكي + مجمّع + IDE بصري لمعالج عربي مخصص (حاليًا 8-بت/64KB)
├── Studio/            # المنصة التعليمية: Web ثابت + Electron + Server (Node/Express/Prisma) + VSCode + Qt/QML
├── Docs/              # توثيق (Instructions/Reference/Reports/Root-Reports)
├── Examples/          # أمثلة موحدة (75 مدخلًا)
├── Scripts/           # سكربتات Windows (.bat عربية) + أدوات بناء/اختبار
├── Tests/             # harness موحد واحد (run_all_tests.sh)
├── Sources-Archive/   # أرشيف ثنائيات/مصادر/لوجز قديمة
├── Releases/          # إصدار Interview (MSI للويندوز 7)
├── .github/workflows/ # CI واحد (ci-cd.yml)
├── .opencode/ + opencode.json  # إعداد وكلاء opencode
└── .gitignore         # شامل (141 سطرًا)
```

**FACT:** الجذر يحتوي 15 مدخلًا فقط (مقروء مباشرة): `.git/ .github/ .gitignore .opencode/ Compiler/ CPU/ DAAD/ Docs/ Examples/ opencode.json Releases/ Scripts/ Sources-Archive/ Studio/ Tests/`. لا يوجد `README.md` جذري ولا `package.json` جذري ولا `CMakeLists.txt` جذري — كل بناء لامركزي داخل المجلد الفرعي.

**INFERENCE:** الاسم "Unified" يعني دمج `Daad-Compiler` + `cpu` + `Dhad-Studio-Full` + `daad-studio/dhad-studio` في شجرة واحدة، لكن الدمج **ميكانيكي (نسخ مجلدات) وليس معماريًا** — كل مكون ما زال له entry-point وبناء ولغة مختلفة ولا يوجد orchestrator جذري يبني الكل.

---

## 2. Project Purpose — ما الهدف منه؟

**FACT (من الملفات):**
- `Compiler/CMakeLists.txt:6` يصف نفسه: `"ض استديو: مترجم لغة البرمجة العربية 'ض' إلى C++20"`.
- `DAAD/README.md:9` يصف `ض Core` بأنها *"لغة برمجة منخفضة المستوى ... الجسر بين لغة التجميع واللغات عالية المستوى ... واجهة عربية كاملة للبرمجة على مستوى المعالج"`.
- `Docs/Reference/PROJECT_STRUCTURE.md:9` يصف `Dhad Studio` بأنها *"Arabic-first EdTech platform for teaching programming ... teacher dashboard, student management, gamification (XP, levels, streaks), AI-powered challenges, comprehensive programming book"*.
- `Docs/Instructions/README.md` (330 سطرًا) يصف *"حاسوب عربي كامل: لغة ض + مترجم → C++20 + معالج 8-بت + assembler + سيرفر تعليمي + واجهات Web/Electron/VSCode/terminal"*.
- `Studio/desktop-app/package.json:3` : `"ض ستوديو - منصة تعليم البرمجة"`.

**INFERENCE:** الهدف المزدوج هو: (1) **تمكين البرمجة بالعربية** عبر 3 واجهات لغوية متكاملة (عالية → C++، منخفضة → آلة، تجميعية → محاكي)، و(2) **منصة تعليمية EdTech** (مدارس/فصول/تحديات/نقاط/قيادة) تستهلك تلك اللغات في المتصفح وسطح المكتب. لا يوجد ملف واحد يربط الهدفين؛ الربط مستنتج من تجاور المجلدات والأمثلة (`snake.ض` بألعاب canvas، `calculator.daad` بمنطق حسابي).

---

## 3. Technology Stack — التقنيات المستخدمة

| الطبقة | التقنية | الدليل (FACT) |
|---|---|---|
| مترجم عالي المستوى | C++20، CMake ≥3.21، g++، MSVC `/utf-8`، FetchContent GTest v1.14.0، CPack/NSIS + WiX | `Compiler/CMakeLists.txt:1-35`، `Compiler/Makefile:4` |
| مترجم منخفض المستوى | C99، CMake ≥3.10، gcc، `-Wall -Wextra -Wpedantic -Werror`، CTest | `DAAD/CMakeLists.txt:1-8` |
| محاكي/مجمّع | C11 (`-std=c11`) + C++ (`gnu++1z`)، Qt5 (GUI)، Logisim Evolution 3.8.0 (عتاد)، moc | `CPU/Makefile:2-8`، `CPU/hardware/build_dhad_cpu.py`، `CPU/gui/main.cpp` |
| ويب أمامي | HTML/CSS/JS خام بدون bundler (static)، `npx serve`، Vercel static، PWA `sw.js`، jspdf/html2canvas | `Studio/Web/package.json:6-10` (لا dependencies)، `Studio/vercel.json` |
| خلفية | Node ≥18، Express 4.21، Prisma 5.22 (SQLite dev + Postgres variant)، ioredis، JWT (bcryptjs/jsonwebtoken)، helmet/compression/rate-limit/multer/xlsx/openai 6.46/winston/swagger، Jest 30 + supertest، PM2/cluster، Docker multi-stage (node:20-alpine، non-root daaduser)، nginx، Railway/Render | `Studio/desktop-app/server/package.json:6-62`، `Studio/Dockerfile:9-80`، `Studio/docker-compose.yml`، `Studio/railway.json` |
| سطح مكتب | Electron 33 + electron-builder 24 (NSIS، appId `com.dhad.studio`) | `Studio/desktop-app/package.json:33-77`، `Studio/Electron/package.json` |
| محرر | VSCode Extension (TypeScript 5، TextMate grammar، snippets، F9) | `Studio/VSCode-Extension/package.json:7-58`، `src/extension.ts` (26 سطرًا) |
| أصلي Qt | Qt5 QML (26 ملفًا) + C++ bridge + 10 ملفات i18n | `Studio/Full/frontend/qml/`، `Studio/Full/bridge/` |
| CI | GitHub Actions ubuntu-latest (cmake/gcc/make) | `.github/workflows/ci-cd.yml` (73 سطرًا) |
| وكلاء | opencode `mimo-v2.5-free` (orchestrator primary + local-reviewer subagent) + plugin 1.18.27 | `opencode.json`، `.opencode/package.json` |
| قواعد بيانات | SQLite (`dev.db` + wal/shm) للتطوير، Postgres schema للإنتاج، Redis 7 (LRU+AOF) للجلسات/rate-limit | `Studio/desktop-app/server/prisma/`، `Studio/docker-compose.yml` |
| لاشيء مركزي | لا يوجد root build/test/lint؛ كل مكون يبني وحده | غياب مثبت بالـ `Read` الجذري |

**INFERENCE:** العلاقة بين التقنيات: `Compiler (C++)` و`DAAD (C)` و`CPU (C/Qt)` ثلاث سلاسل أدوات مستقلة تشترك فقط في **الفكرة (عربية) والأمثلة**، بينما `Studio/Web/js/dhad-*.js` **إعادة تنفيذ رابعة بالجافاسكربت** لنفس اللغة تعمل داخل المتصفح، و`Server` لا يستدعي المترجمات الأصلية مباشرة بل يحتوي نسخة JS منها تحت `server/src/utils/dhad/` + sandbox VM.

---

## 4. Directory Structure — شرح المجلدات

### الجذر (FACT — مقروء مباشرة)
- `.git/` — مستودع git (لم تُفحص سجلاته؛ خارج نطاق القراءة المطلوبة).
- `.github/workflows/ci-cd.yml` — job واحد صالح + jobان بمسارات مكسورة (انظر §16).
- `.gitignore` (141 سطرًا) — يغطي `.env`، `node_modules/`، مخرجات `Compiler/build*/`، `DAAD/**/*.exe`، `CPU/**/*.o`، `*.exe/*.o/*.a/*.so`، `*.zip`، `*.db*`، `CMakeFiles/`، `dist/build/out/coverage` — **FACT:** رغم ذلك المجلدات `Compiler/build/ build-make/` والثنائيات `*.exe/*.o/*.bin/*.zip/dev.db` **موجودة فعليًا على القرص** (تجاهل غير مطبق أو ملفات committed قبل القاعدة).
- `.opencode/` — `agents/orchestrator.md` (23 سطرًا) + `local-reviewer.md` (24 سطرًا) + `node_modules/` + `package.json`.
- `opencode.json` (16 سطرًا) — model + تعريف الوكيلين.

### `Compiler/` (12 مدخلًا — FACT)
`CMakeLists.txt` (193 سطرًا) + `Makefile` (26) + `LICENSE` + `daad-compiler.exe` (2.3M) + `Daad/` (wrapper واحد) + `src/` (14: 12 cpp + CMakeLists + `_backup_original/` بست نسخ) + `include/Daad/` (11 hpp) + `stdlib/` (5 hpp + 10 مجلدات `.ض`) + `stdlib_arabic/` (10 مجلدات بأسماء عربية) + `examples/` (14: basic/classes/functions/loops/paint/snake/flappy/particles/space-invaders/zombie + conversions/programs) + `build/` + `build-make/` (مجلدا بناء مولدان، كلاهما يحوي `bin/daad-compiler` بحجمين مختلفين 584K مقابل 1.7M).

### `DAAD/` (118 مدخلًا — FACT)
`CMakeLists.txt` (55) + `README.md` (39) + `compiler/` (14: `main.c` 221 سطرًا + 12 مجلد مراحل + `tests/` بـ 11 c و10 exe + `test_semantic.exe` شارد) + `include/Daad/` (12 hpp — واجهة C++ بديلة/قديمة) + `compiler/*` المراحل + `tests/` (61: 6 cpp gtest + عشرات `.ض` + conformance/valid/invalid/runtime/types/memory/abi/lexer/parser) + `Testing/Temporary/` (سجل فارغ) + `e2e/` (499: ثلاثيات `.daad/.s/.exe`) + `tools/` (ملفان py) + `docs/` (16 md) + `build/` + عشرات `.daad/.s/.exe` مبعثرة في الجذر + تقارير تدقيق 14 md + سكربتات `e2e_*.ps1` + ملفات `NUL/output.s/test_input*`.

### `CPU/` (33 مدخلًا — FACT)
`src/` (9: `asm_main.c` 29 + `dhad_asm.c` 664 + `dhad_cpu.c` 650 + `dhad_debug.c` 677 + `dhad_live.cpp` 729 + `main.c` 1571 + `visual_sim.c` 762 + o) + `include/` (16 h) + `hardware/` (11: circ + مولد py 1266 + 5 md مواصفات + tests) + `gui/main.cpp` (1156) + `desktop/` (7: exe + gui + bat + ARCHITECTURE مكرر + 9 أمثلة) + `docs/ARCHITECTURE.md` (235) + `examples/` (6) + `tests/` (112 `.ضasm` + شاذ واحد `.dasm`) + `cpu-original/` (29: نسخة ظل شبه كاملة) + `Makefile` (72) + `dhad_gui.pro` (15) + `REPORT.md` (207) + `run_tests.sh` (334) + ثنائيات `dhad_cpu/dhad_asm/dhad_gui/dhad_live/dhad_vis` + `demo*.ضasm` + `*.bin/*.o/*.moc/.qmake.stash/*.zip`.

### `Studio/` (31 مدخلًا — FACT)
`Web/` (16: 7 html + css/4 + js/43 + pages/21 فيها 3 bak + examples + sw.js + package.json) + `desktop-app/` (13: main.js 215 + preload + splash + icon + package.json + scripts/ + server/31 + frontend-web/ مرآة Web + prisma_client/) + `Electron/` (10: main.js 215 + bak + preload + splash/titlebar + package.json + node_modules/) + `VSCode-Extension/` (9: package.json + src/extension.ts واحد + syntaxes/snippets/out/tsconfig) + `Full/` (bridge/compiler/frontend/server/Dhad-Studio.bat؛ frontend فيه qml/26 + i18n/10 + frontend-web مرآة ثالثة) + `Windows-Launcher/` (10 bat + md) + `Dockerfile` (80) + `docker-compose.yml` (165) + `vercel.json` + `railway.json` + `tests/` (70+ سكربت pentest/stress + تقارير json) + سكربتات جذرية (`build*.bat`, `cli-login.js`, `create-accounts.js`, `gen_*.py`, `check_braces.py`, `stdlib_index.json`, `output.cpp/hpp`) + `LICENSE.txt`.

### `Docs/` (4 مجلدات فقط — FACT)
`Instructions/` (11: odt + txt + bat + wxs + FILE_INDEX + 3 README + REPORT + SHA) + `Reference/` (13: 9 md + handover/ + reference/ بستة مراجع ضخمة) + `Reports/` (62 md: PHASE2/3/45/6 + manifests) + `Root-Reports/` (5: مخطط بوابات + 4 تقارير).

### `Examples/` (75 — FACT)
أزواج `01_hello`–`07_fibonacci` (كل واحد `.ض+.cpp+.exe+.hpp+.linux` = 35) + 7 `err_*.log` + مفردات (`add/basic/calculator/classes/countdown/factorial/flappy/function/functions/hello/logic/loops/output/paint/particles/snake/space-invaders/test_full/test_sum/zombie*`) + 8 مجلدات (`conversions/cpp-learning/cpu-examples/cpu-original-examples/daad_presenter/data-pos/games/programs`).

### `Scripts/` (25 — FACT)
12 bat عربية + 12 كود (`analyze_dups.py/detailed_analysis.py/phase2_dupclass.py/killsrv.py/gen_launcher.py/gen_wxs.py/auth_cycle.sh/build_demos.sh/hash_script.sh/phase2_hash.sh/run_tests.sh 334/winbuild.sh 10`) + `README-ويندوز.md` (147).

### `Tests/` (1 — FACT)
`run_all_tests.sh` فقط (612 سطرًا).

### `Sources-Archive/` (9 — FACT)
`binaries/bridge-sources/build-logs/compiler-sources/misc/msi-extract/test-files/vsc-extensions/wix-templates/`.

### `Releases/Interview-Release/` (6 — FACT)
`DhadStudio-Setup-Win7.msi` + `build-companions/` (3) + `Documentation/` (README فقط) + `Examples/` (20) + `SHA256SUMS.txt` + `Source/` (مجلدان: Daad-Compiler + cpu).

---

## 5. Important Files — أهم الملفات ووظيفة كل ملف

| الملف | الوظيفة (FACT من القراءة) |
|---|---|
| `Compiler/src/main.cpp` (130) | CLI `daad-compiler [خيارات] <مصدر>`: banner عربي + parse `-o/-h/-v` + قراءة الملف + `compileAndOutput` (يشتق `.hpp` من `.cpp` ويكتب الاثنين، `تم التحويل بنجاح`/`فشل التحويل` + diagnostics `line:col`) |
| `Compiler/src/Compiler.cpp` (111) | الواجهة `DaadCompiler::compile(source, headerName)`: Lexer→Parser loop→Optimizer→جمع أسماء الدوال→CodeGen (فصل global/main)→تحذيرات `new/delete`→Sandbox validate→`CompileResult{success,diagnostics,header,source}` |
| `Compiler/include/Daad/*.hpp` (11) | المفردات: Token (~30 نوعًا) / Lexer (UTF-8 + `؛/،`) / Keywords (~90 في 9 مجموعات عربي→C++) / AST (677 سطرًا: 17 تعبيرًا + ~30 عبارة + 18 عقدة صور) / Parser (توجيه نصي للكلمات) / CodeGen (C++20 + `mapType` + `[][]→vector` + includes) / Optimizer (طي ثوابت `+-*/%` فقط) / Diagnostics (عربي) / Sandbox (whitelist ~30 + regex blocklist) / Unicode (فك UTF-8 يدوي) |
| `Compiler/CMakeLists.txt` + `src/CMakeLists.txt` | بناء `DaadCore` static + `daad-compiler→bin/` + تثبيت 4 مكونات + GTest fetch + خيارا Qt OFF + CPack NSIS |
| `Compiler/Makefile` | بناء سريع `g++ -std=c++20 -Iinclude -O2 → build-make/bin/daad-compiler`؛ هدفه `test` يشير لمسار غير موجود (انظر §16) |
| `DAAD/compiler/main.c` (221) | CLI `daad <source.daad> [-o] [--ast] [--ir] [--target=x86\|dhad]`: read_file آمن + Lexer→Parser (بوابة أخطاء)→Semantic (بوابة أخطاء Phase1)→IR→Optimizer (كل الدوال)→إعادة تسمية `الرئيسية→main` (x86 فقط)→كتابة `.s` (DHAD: header+`_start: load s7,0; call الرئيسية; halt`+strings+module؛ x86: module+`__daad_print_int/__daad_read_int`+`_start: callq main; mov→rdi; syscall 60`) |
| `DAAD/CMakeLists.txt` (55) | C99 + warnings-as-errors + glob المراحل كـ `daad_lib` static + `daad` executable + test لكل `test_*.c` + CTest |
| `DAAD/compiler/{token,lexer,parser,ast,semantic,ir,optimizer,cfg,ssa,codegen,backend}/` | المراحل (التفصيل في §6)؛ `backend_interface.h` يعلن 4 أهداف لكن CLI يكشف 2 فقط |
| `DAAD/e2e_runner.ps1` + `e2e_batch2.ps1` | توليد `.daad` بترميز UTF-8 ثم `daad.exe -o .s` ثم `gcc .s -o .exe` ثم مقارنة exit-code |
| `CPU/src/dhad_cpu.c` (650) + `include/dhad_cpu.h` (250) | النواة الموحدة v4.0: `DhadCPU{regs[8]+acc+flags(Z,N,C)+pc16+sp+mem64K+stack64+bus+callbacks}` + `init/reset/load/step/run/reg/mem/push/pop/output`؛ opcodes قاعدة `0x0-0xF` + موسعة بعد `0xF` |
| `CPU/src/dhad_asm.c` (664) + `include/dhad_asm.h` (62) | مجمّع two-pass: سجلات عربية `س0-س7/مح` + إنكليزية، فواصل عربية، labels/equates/macros (64×32)/`.if` (32)/`.org/.db/.dw/.ds/.align/.include/.equ` + تعبيرات `label±offset` |
| `CPU/src/asm_main.c` (29) | CLI `dhad_asm <file.ضasm> [output.bin]` |
| `CPU/src/main.c` (1571) | CLI `dhad_cpu <file.ضasm\|.bin> [--debug/--hex/--state/--bin --out]` — **FACT من تعليق Makefile:** يحوي نسخة CPU+ASM مضمنة خاصة ولا يربط `dhad_cpu.c` |
| `CPU/Makefile` (72) + `dhad_gui.pro` (15) | الأول يبني الثلاثة بشكل صحيح؛ الثاني يحذف `dhad_cpu.c` (قديم) |
| `CPU/run_tests.sh` (334) | ~90 حالة T1-T16 (تفصيل §11) |
| `Studio/Web/js/dhad-lexer.js` (703) + `dhad-parser.js` (1572) + `dhad-ast.js` (639) + `dhad-codegen.js` (1065) + `dhad.js` (264) | سلسلة المتصفح: tokenizer UTF-16 (103 كلمة: 83+20 صور) → recursive-descent (32 نوع عقدة، رسائل عربية) → AST → مولد JS (collect ثم emit runtime/classes/functions/globals/main) → منسق `compile()` مع blocklist (`process/require/eval/Function/fetch/WebSocket/...`) |
| `Studio/desktop-app/server/src/index.js` (434) | دخول الـ API (JWT auto-regenerate، routes، middlewares، Prisma، Redis، Swagger، /health، cluster) |
| `Studio/desktop-app/package.json` + `server/package.json` | الأول Electron NSIS؛ الثاني API الكامل (scripts start/cluster/dev/prisma/jest/pm2/docker) |
| `Studio/VSCode-Extension/src/extension.ts` (26) | أمر واحد `daad.compileCurrentFile` (F9) يفتح terminal ويرسل `daad-compiler "file" -o "temp_daad.cpp"` — يتطلب المترجم في PATH |
| `Studio/Dockerfile` (80) + `docker-compose.yml` (165) + `vercel.json` + `railway.json` | إنتاج multi-stage non-root + خدمات api/redis/ollama/nginx + استضافة أمامية static + نشر Railway |
| `Tests/run_all_tests.sh` (612) | الموحد: 6 أقسام (تفصيل §11) + `ROOT` مثبت |
| `.github/workflows/ci-cd.yml` (73) | 3 jobs (تفصيل §12) |
| `Docs/Reference/BUILDING.md` (97) + `PROJECT_STRUCTURE.md` (270) | تعليمات وبنية — كلاهما قديم (انظر §16) |
| `Examples/01_hello.ض` (6) + `DAAD/hello.daad` (3) + `CPU/tests/full_T1a_add.ضasm` (5) | الحد الأدنى لكل لغة (مقتبسة حرفيًا في §7) |

---

## 6. Architecture — شرح Architecture الفعلية

**FACT:** لا توجد معمارية واحدة؛ توجد **4 معماريات متوازية** لا تستدعي بعضها:

```text
┌─ Track A: Compiler (C++20 transpiler) ─────────────┐
│ UnicodeUtils → Lexer → KeywordRegistry → Parser    │
│   → AST → OptimizerVisitor → CodeGenVisitor        │
│   → Diagnostics → SandboxValidator → .cpp+.hpp     │
└────────────────────────────────────────────────────┘
┌─ Track B: DAAD (C99 native compiler) ──────────────┐
│ Lexer → Pratt Parser → AST → Semantic (scope/      │
│  type_registry/checker/folding) → IRBuilder →      │
│  Optimizer (11) → Backend (X86/DHAD) → .s → gcc    │
│  [CFG/SSA/liveness/regalloc موجودة كمكتبات،        │
│   SSA/CFG لا يستدعيها main مباشرة]                 │
└────────────────────────────────────────────────────┘
┌─ Track C: CPU (8-bit emulator + assembler) ────────┐
│ .ضasm → DhadAsm two-pass → program[0xF000] →      │
│  memcpy→CPU.memory[0x0000] → step/run (ALU/        │
│  branch/stack/MMIO/IRQ) → output buf/callback     │
│  [GUI/Qt + visual_sim + live + hardware Logisim]   │
└────────────────────────────────────────────────────┘
┌─ Track D: Studio (EdTech platform) ────────────────┐
│ Web static (in-browser Dhad JS toolchain +        │
│  sandbox VM) ⇄ Express API (JWT/Prisma/Redis) ⇄   │
│  Electron wrapper (offline bundle) + VSCode shim   │
│  + Qt/QML native track (Full/)                     │
└────────────────────────────────────────────────────┘
```

- **Track A تفصيلًا (FACT من `Compiler.cpp` + الهيدرات):** `Lexer(string_view)` ينتج `Token{text,line,col}`؛ `Parser(Lexer&,Diag)` توجيه نصي (`صحيح→VarDecl`، `إذا→If`، `طالما/بينما→While`، `لكل/كرر→ForEachOrFor`، `افعل→DoWhile`، `اختر→Switch`، `دالة→Func`، `صنف/فئة→Class`، `انتقل` مرفوض بخطأ)؛ `AST.hpp` الأضخم (677)؛ `Optimizer` يطوي `Number op Number` فقط؛ `CodeGen` يفصل `isGlobalDeclaration()` عن جسم `main` المخلّق ويحقن includes (`DaadRuntime/stdlib/Math/DaadStdlib/iostream/string/vector/stdexcept`) ويتتبع `new/delete`؛ `SandboxValidator` قائمة بيضاء + regex (`system/popen/exec/remove/socket/fork/...`) + heuristics (`>20 funcs`، `new int[`).
- **Track B تفصيلًا (FACT من `main.c` + الهيدرات):** `TokenType` ~104 (كلمات عربية + رموز + `NEWLINE/ERROR` + مستقبلية `ASYNC/PARALLEL` غير مدعومة)؛ `Lexer` بايت-wise مع `buffer[1024]/errors[100]`؛ `Parser` Pratt غير شفاف (`parser_internal.h`)؛ `SemanticContext{ScopeStack,TypeRegistry,TypeChecker,ErrorList,in_loop,return_type}` مع `رقم→int/عشري→float/حرف→char/منطق→bool/فراغ→void/نص→string`؛ `IRBuilder{var_hash[128],loop_stack[32],struct_fields[64]}`؛ `Optimizer` 11 تمريرة معلنة؛ `Backend{target,name,emit_*}` + `BackendContext{frame,params,offsets,alloca,callee_saved[5]}` + SysV (`rdi,rsi,rdx,rcx,r8,r9`/`rax`/`rbp`/16B) + DHAD (S0-S5 alloc، S6 scratch، S7 FP، إطار `0xE000` 256B، ACC للـ ALU).
- **Track C تفصيلًا (FACT من `dhad_cpu.h/memory_map.h/isa_constants.h`):** خريطة `0x0000-0xEFFF برنامج/RAM (حتى 0xF000)` + `0xF000-0xF0FF MMIO` + `0xF100-0xF1FF IVT` + `0xF200-0xFFFF محجوز`؛ أجهزة `Display/Keyboard/Timer/GPIO/Serial/INTCTL`؛ أعلام Z/N/C؛ `MAX_CYCLES 100000`؛ `ASM_PROG_MAX 0xF000/LABELS 256/MACROS 64/IF 32`.
- **Track D تفصيلًا (FACT):** أمامي بلا بناء؛ خلفي `routes/` 13 + `middlewares/` 7 + `controllers/` 11 + `services/` 2 + `utils/` (منها `dhad/{lexer,parser,ast,codegen}` نسخة سيرفر + `dhadSandbox` 321 سطرًا بحدود 5s/64KB/50KB/32MB)؛ `Electron/main.js` مقابل `desktop-app/main.js` (الأول يشير لمسار قديم `../Dhad-Studio-Full/server`، الثاني صحيح)؛ `Full/frontend` مسار Qt منفصل.

**INFERENCE:** التقاطع الوحيد المثبت بين المسارات هو **اتجاه DAAD→CPU**: `main.c:132-144` يولد `_start: load s7,0; call الرئيسية; halt` المتوافق مع `dhad_cpu` (دخول `0x0000`)، وتقارير Phase12.5-F توثقه. لا يوجد استدعاء مثبت `Compiler→CPU` أو `Studio→Compiler/CPU` الأصليين (السيرفر يستخدم نسخته JS).

---

## 7. Execution Flow — كيف يعمل البرنامج من البداية للنهاية

### Track A — `daad-compiler` (FACT — `main.cpp` + `Compiler.cpp`)
```text
$ daad-compiler prog.ض -o out.cpp
  → printBanner → arg-parse → ifstream→stringstream
  → DaadCompiler::compile(src, "out.hpp")
      → Lexer(src) → Parser.parseStatement()* (تخطي null)
      → OptimizerVisitor.accept* → جمع userFns
      → CodeGenVisitor (global→مباشرة، executable→beginMainBody/endMainBody→int main)
      → warnings new/delete → Sandbox.validate
  → كتابة out.cpp + out.hpp → "تم التحويل بنجاح"
  → $ g++ out.cpp -o out && ./out   (يدوي، خارج الأداة)
```
- **مثال مثبت (FACT):** `Examples/01_hello.ض`:
  ```arabic
  طباعة("مرحبا بالعالم!") ؛
  ```
  و`Compiler/examples/conversions/.../01_hello.cpp` المقابل يحوي `daad::runtime::daad_print("مرحبا بالعالم!")` مع includes المشروع.
- **مثال نحوي (FACT من `basic.ض/functions.ض/loops.ض/classes.ض/snake.ض`):** `صحيح س = 10 ؛` + `صحيح مجموع(صحيح أ ، صحيح ب){ارجع أ+ب؛}` + `كرر/بينما/إذا/وإلا/فئة/نقطة.س`.

### Track B — `daad` (FACT — `main.c` 221 سطرًا)
```text
$ daad hello.daad -o hello.s [--ast] [--ir] [--target=x86|dhad]
  → lexer_create→lexer_tokenize → parser_create→parser_parse (بوابة)
  → semantic_context_create→semantic_analyze (بوابة Phase1)
  → ir_builder_create→ir_builder_build → optimizer (كل الدوال)
  → الرئيسية→main (x86 فقط)
  → fopen(out,"w") → backend_emit_module + helpers + _start → "Generated: ... (N functions, target=...)"
  → $ gcc hello.s -o hello.exe && ./hello.exe; echo $?   (خارج الأداة، تثبته e2e/*.ps1)
```
- **مثال مثبت (FACT):** `DAAD/hello.daad`:
  ```arabic
  دالة: الرئيسية() -> رقم :-
      ارجع(42)
  نهاية
  ```
  و`hello.s` (20 سطرًا) يحوي `main: push %rbp;...; mov $42,%rax; leave; ret` + `_start: callq main; ...; syscall`.

### Track C — `dhad_asm` + `dhad_cpu` (FACT — `asm_main.c` + `Makefile` + `main.c:1297-1416`)
```text
$ dhad_asm prog.ضasm prog.bin → "Assembled: prog.ضasm (N bytes)"
$ dhad_cpu prog.ضasm [--hex] [--state] [--bin --out f.bin] [--debug]
  → كشف الامتداد (.ضasm→تجميع داخلي، .bin→تحميل مباشر)
  → memcpy program→memory[0x0000] → run(max 100000)/step/debugger
  → طباعة النتيجة (ACC) / الحالة (متوقف/يعمل) / --hex/--state/--debug
```
- **مثال مثبت (FACT):** `CPU/tests/full_T1a_add.ضasm`:
  ```arabic
  حمّل مح، 10
  حمّل س0، 20
  جمع س0
  توقف
  ```
  المتوقع في `run_tests.sh:77`: `ACC=30، الحالة=متوقف`.

### Track D — Studio (FACT)
```text
المتصفح: Web/index.html → login/register → pages/* (teacher/student dashboards)
  → dhad-editor/web-editor → DhadLexer→DhadParser→DhadAST→DhadCodeGen (داخل المتصفح)
  → smart-evaluator/dhadSandbox VM → عرض الناتج
  → api.js (cookies + refresh) ⇄ Express :3000 (/auth/students/challenges/analytics/...)
  → Prisma (SQLite dev / Postgres prod) + Redis + JWT + Swagger + /health
سطح المكتب: Electron main.js → spawn server/src/index.js → انتظار /health → splash→app
  (حزمة offline: server/** + frontend-web/**)
VSCode: فتح .ض → F9 → terminal: daad-compiler "file" -o "temp_daad.cpp"
Docker: deps→builder→production (daaduser) → :3000/health
```

---

## 8. Data Flow — كيف تنتقل البيانات

- **A (FACT):** `string UTF-8` → `codepoints+byteOffsets (UnicodeUtils)` → `Token{text,line,col}` (`؛→;`، `،→,`، `// /* */`، أرقام/سلاسل مع `\n\t...` وثماني، معرفات عربية) → `KeywordRegistry (نص عربي→KeywordType)` → `AST nodes (unique_ptr)` → `C++ strings (source+header)` → ملفان. الأخطاء `DiagnosticsEngine{severity,line,col,message,sourceLine}` عربية.
- **B (FACT):** `bytes .daad` → `Token[] (count)` → `NODE_PROGRAM` → `SemanticContext (scopes+types+errors)` → `IRModule{functions{blocks{instructions{opcode,operands}}}}` → `optimized IR` → `text .s` → `ELF via gcc` → `exit-code (rax→rdi→syscall 60)`. السلاسل عبر `dhad_emit_module_strings` قبل الدوال (توافق 8-بت).
- **C (FACT):** `UTF-8 .ضasm` → `DhadAsm{labels[256]/equates[256]/macros[64]/if_stack[32]/program[0xF000]}` (two-pass) → `bytes` → `memory[65536]` → `regs/flags/pc/sp/output[4096]` → `stdout (النتيجة/الحالة)` أو callback GUI أو MMIO (`F00x/F01x/F02x`) أو `.bin`.
- **D (FACT):** `form/API JSON` → `Express routes→middlewares(auth/security/csrf/cache)→controllers→services/prisma` → `SQLite/Postgres + Redis sessions` → `cookies JWT (access 1h/refresh 7d)` → `frontend guards (TEACHER/STUDENT/SHARED)`. كود التحديات عبر `codeVerifier/serverEvaluator/dhadSandbox` بحدود زمن/ذاكرة.

---

## 9. Dependency Map — العلاقات بين المكونات

```text
ROOT (لا orchestrator)
├── Compiler/daad-compiler
│    ├── DaadCore (Lexer→Parser→AST→Optimizer→CodeGen→Diagnostics→Sandbox)
│    ├── include/Daad/*.hpp  [strong coupling: Parser↔Lexer↔Keywords↔AST↔CodeGen]
│    ├── stdlib/*.hpp + stdlib*/*.ض  [runtime للمخرجات، ليس للبناء]
│    └── GTest (fetch فقط، غير مستخدم) + Qt (OFF) + NSIS/WiX (تغليف)
│
├── DAAD/daad
│    ├── daad_lib (token→lexer→parser→ast→semantic→ir→optimizer→codegen→backend)
│    │    ├── cfg/ssa/liveness/regalloc/stack/label [مكتبات مساعدة، SSA غير موصولة بـ main]
│    │    └── libm فقط (-lm)
│    ├── include/Daad/*.hpp [معزول: واجهة C++ قديمة لا يستدعيها main.c]
│    └── gcc/as خارجي (ربط .s→.exe) + PowerShell e2e
│
├── CPU/dhad_cpu + dhad_asm + dhad_gui + dhad_live + dhad_vis
│    ├── dhad_cpu.c (موحدة) ← gui/main.cpp + dhad_live.cpp + visual_sim [shared core]
│    ├── main.c (مكررة مضمنة) [DUPLICATE: لا يربط الموحدة — strong divergence risk]
│    ├── dhad_asm.c ← asm_main.c + main.c + gui [shared assembler]
│    ├── dhad_isa.h (قديم 4-بت) [ORPHAN: لا يستخدمه الكود الحالي]
│    └── Qt5/Logisim [خارجي]
│
├── Studio/
│    ├── Web (static) ← js/dhad-{lexer,parser,ast,codegen} [self-contained, لا يعتمد على Compiler/DAAD/CPU]
│    ├── server ← routes→controllers→services→prisma/redis + utils/dhad/* (نسخة JS) [weak coupling مع Web عبر HTTP فقط]
│    ├── desktop-app ← server/** + frontend-web/** (bundle) [strong packaging coupling]
│    ├── Electron (legacy) [STALE: مسار قديم]
│    ├── VSCode-Extension → daad-compiler (خارجي في PATH) [external runtime dep]
│    └── Full/ (Qt/C++ track) [منفصل، يكرر compiler/server]
│
└── Shared/global state
     ├── لا توجد حالة globale بين المسارات الأربعة (معزولة كليًا)
     ├── داخل كل مسار: Diagnostics/SemanticContext/DhadCPU/DhadAsm/ApiClient state محلية
     └── أدوات مشتركة حقيقية: الأمثلة (.ض/.daad/.ضasm) + Docs + Scripts (فقط ملفات، لا كود)
```

- **Circular dependencies (FACT):** لا توجد حلقات مثبتة داخل أي مسار (التدفق أحادي). التكرار الحقيقي هو **نسخ متوازية** لا حلقات: `Compiler/src` مقابل `Studio/Full/compiler/src`؛ `Web/js` مقابل `frontend-web/js` مقابل `server/src/utils/dhad`؛ `CPU/src` مقابل `cpu-original/src`؛ `Scripts/run_tests.sh` مقابل `CPU/run_tests.sh`.
- **External dependencies (FACT):** STL/C-lib فقط للمترجمات؛ `node_modules` للويب/سيرفر (مثبتة جزئيًا: `Electron/node_modules` و`VSCode/node_modules` موجودة)؛ Qt/Logisim/GCC/NSIS/WiX/MSYS2 خارجية.

---

## 10. Core Components — شرح المكونات الأساسية

### Compiler — المترجم العالي (C++20)
- **Lexer (233 cpp + 37 hpp) — FACT:** `string_view→codepoints`، `save/restoreState`، `scanNumber/String/Identifier`، `؛/،`، تعليقات، سلاسل مع escapes. **INFERENCE:** كافٍ للأمثلة؛ مجهول سلوكه مع أرقام عربية مشرقية `١٢٣` (تختبرها `Tests/run_all_tests.sh` على Web وليس هنا — UNKNOWN لهذا المسار).
- **Keywords (~90 في 9 مجموعات) — FACT:** أنواع (`صحيح→int`، `عشري→double`، `نص→string`)، تحكم (`إذا/كرر/لكل/بينما/طالما`)، IO (`طباعة→daad_print`، `ادخل`)، GUI (14 `زر_أمر→QPushButton`)، صور (20 `حمّل_صورة→daad::image::load`). **INFERENCE:** القاموس أوسع من CodeGen الفعلي (GUI/Image قد تسقط لـ `// TODO` أو generic Call).
- **Parser (1683 — الأكبر) — FACT:** recursive-descent بتوجيه نصي + `زد/انقص` sugar + رفض `انتقل/GOTO` + مسار مزدوج لكلمات الصور. **INFERENCE:** الأكثر تعقيدًا والأكثر عرضة للانحراف عن التوثيق.
- **AST (677) — FACT:** 17 تعبيرًا + ~30 عبارة + صور؛ `Struct` اسم فقط، `Constructor` معلن لا يُبنى في Parser (مثبت من `_backup` + التقارير).
- **Optimizer (105) — FACT:** طي `Number op Number` فقط؛ 35/47 زائرًا فارغًا. **PARTIAL.**
- **CodeGen (840) — FACT:** فصل global/main + `mapType` + sanitize/escape + تتبع leak + `stdlibExportedNames` (30). **INFERENCE:** يعمل للأمثلة الإجرائية/OOP البسيطة؛ `__dhad.*` في `snake.ض` خارج نطاقه (يحتاج shims ويب).
- **Sandbox (247) — FACT:** whitelist/regex/heuristics سطحية (عد نصي لا تحليل CFG). **PARTIAL.**

### DAAD — المترجم المنخفض (C99)
- **Lexer (1077) + Token (~104) + Parser (1337 Pratt) + AST (10 ملفات) — FACT:** واجهة عربية كاملة + أرقام عربية-لاتينية + `؛` + `دالة: name(ن: رقم) -> رقم :- ... نهاية` + ذاكرة (`حمل/خزن/ادفع/اسحب`) + `اطبع/أدخل` + `حاول/امسك/أطلق`.
- **Semantic (20 ملفًا) — FACT:** زوار 20+ + registry/checker/scope/symbol/folding + بوابة أخطاء صارمة (Phase1).
- **IR (26) + Optimizer (11) + CFG + SSA (191) — FACT:** بنية حقيقية؛ SSA فيه `insert_phi/rename` لكن `main.c` لا يستدعيه صراحة (الـ backend يبني intervals/alloc داخليًا حسب تقارير Phase2).
- **Backend (1204 + 1494) — FACT:** x86 SysV كامل (prologue/epilogue/callee-save/CALL preserve/spill/PHI-lowering/cond-jump fix) + DHAD (إطار `0xE000`، S0-S5/S6/S7/ACC) + مُنشئا ARM/RISCV غير مكشوفين CLI.

### CPU — المحاكي (C11/Qt)
- **النواة الموحدة — FACT:** 8-بت/16-عنوان/64KB/8 سجلات+ACC/أعلام ZNC/ستاك 64/MMIO/IVT/interrupts/callbacks.
- **المجمّع — FACT:** two-pass عربي/إنكليزي + directives + macros + conditional.
- **الواجهات — FACT:** 4 `main` (cpu/asm/visual/live) + Qt IDE (1156) + Logisim (24 subcircuit).
- **العتاد — FACT:** `dhad_cpu.circ` مولد من `build_dhad_cpu.py` + specs + vectors (10 مفقودة HIGH).

### Studio — المنصة (JS/TS)
- **Web toolchain — FACT:** Lexer 103 كلمة + Parser 32 عقدة + CodeGen JS + sandbox VM بحدود + `DhadLibraries.getNames()` + `SmartEvaluator`.
- **Server — FACT:** 13 route + 7 middlewares + 11 controller + Prisma/Redis/JWT/Swagger/health/cluster/PM2.
- **Desktop — FACT:** Electron يغلّف السيرفر + الواجهة offline.
- **VSCode — FACT:** shim نحيف (26 سطرًا) بلا LSP/bundled compiler.
- **Full/Qt — FACT:** مسار أصلي موازٍ (QML 26 + i18n 10 + bridge CSV/XLSX).

---

## 11. Tests — حالة الاختبارات

| الحزمة | ماذا يُختبر؟ (FACT) | ماذا لا يُختبر؟ | النوع |
|---|---|---|---|
| `CPU/run_tests.sh` (334) | ~90: T1 حساب (6) T2 نقل (3) T3 قفز (5) T4 منطق (7) T5 جديد (4) T6 interrupts (1) T7 assembler (6) T8 recovery (1) T9 edge (10: div0/mod0/wrap/empty-stack/nested/loop) T10 جديد (11: PRINT_CH/org/include/align/hex/English) T11 stdin T12 CLI flags T13 GUI وجود T14 أخطاء T15 extra (6) T16 regressions (8) + 20 legacy Phase2-3 | عتاد Logisim مقابل C (vectors ناقصة)، أداء/فوضى، MMIO عميق | shell integration + regression |
| `CPU/tests/` (112) | مدخلات `.ضasm` لكل ما سبق + `ldri_*` + `bug_*` + `edge_*` | لا توجد asserts داخل الملفات نفسها (التوقع في الشل) | fixtures |
| `DAAD/compiler/tests/` (11 c + 10 exe) | lexer/parser/ast/semantic/codegen/precedence/phase2_fixes/performance/stress/fuzz/expansion | غير مؤكد التشغيل الحالي (LastTest فارغ) | C unit/integration |
| `DAAD/e2e/` (499) + `*.ps1` | ثلاثيات `.daad→.s→.exe` + مقارنة exit-code (b3/bit/cmp/func/mem/while/if/float/string/ret_42...) | float/string/IO محدودة (حسب تقارير Phase3) | e2e عبر gcc |
| `DAAD/tests/` (61) | 6 cpp (Compiler/Comprehensive/Lexer/Runtime/Optimizer/Stdlib/NewSyntax) + `.ض` (t_*/test_comprehensive_*) + conformance/valid/invalid/runtime/types/memory/abi | `daad test --all` المذكور في README غير موجود في `main.c` (توثيق قديم) | gtest-style + fixtures |
| `Compiler/` | **لا يوجد `tests/`** (مثبت `ls` فشل)؛ `build/Testing/LastTest.log` فارغ؛ `Makefile:test` يشير لـ `../build/bin/DaadTests` غير الموجود | كل شيء (لا unit/integration/e2e لهذا المسار) | غائب |
| `Tests/run_all_tests.sh` (612) | S1 CPU (ثنائيات + ADD 30 + hex/state + asm bin + empty/bad/Arabic regs) S2 Compiler (syntax-only لكل cpp + عد أمثلة) S3 Web (node --check 16 + pages + lexer/parser/codegen/full-pipeline/libraries/evaluator/10K سطر) S4 Electron/Desktop (node --check mains/routes/utils) S5 VSCode (tsc + package.json) S6 Server (عد tests + syntax) | S2 لا يشغّل المترجم فعلًا (فحص نحوي فقط)؛ S5 يفشل إن غابت typescript؛ المسار مثبت `ROOT=/home/...` غير محمول | meta-harness (syntax + smoke) |
| `Studio/desktop-app/server/tests/` (29) | unit (19) + integration (7) + security (2) عبر jest + supertest | تغطية فعلية UNKNOWN بدون تشغيل | jest |
| `Studio/tests/` (70+) | pentest/stress/seed/verify/fix + تقارير json (tier1-7/courses/pentest100/attack_45/mega_stress...) | ad-hoc بلا jest config موحد | manual/security/stress |
| `Docs/Reports/` (62) + `DAAD/*.md` (14) | توثق 998 assert و617 و533 و347 في مراحل مختلفة — **INFERENCE:** أرقام تاريخية لتشغيلات سابقة لا دليل على تكرارها الآن | الحالة الحالية | تقارير، ليست اختبارات قابلة للتشغيل مباشرة |
| CI | `daad-compiler` (7 suites + runtime input) صالح شكليًا؛ الآخران مكسوران (§16) | Studio/CPU/Web/Electron/VSCode/Docker | actions |

**الخلاصة (FACT):** وجود الاختبارات لا يعني نجاحها الآن — `DAAD/Testing/Temporary/LastTest.log` 3 أسطر فارغة (`Start/End` بلا حالات)، و`Compiler` بلا suite أصلًا، و`Tests/run_all_tests.sh` لم يُشغَّل ضمن هذه المهمة (ممنوع التنفيذ الخطر/التثبيت، والتشغيل وحده يحتاج node/gcc).

---

## 12. Build & Run — كيف يتم بناء وتشغيل المشروع

### Compiler (FACT)
```bash
# سريع (Makefile):
cd Compiler && make            # → build-make/bin/daad-compiler
# إنتاجي (CMake):
cmake -B build -DCMAKE_BUILD_TYPE=Release -DDAAD_BUILD_TESTS=OFF
cmake --build build -j$(nproc) # → build/bin/daad-compiler (584K مثبت)
# ملاحظة: -DDAAD_BUILD_TESTS=ON سيفشل (لا يوجد tests/ ليضيفه add_subdirectory)
./build/bin/daad-compiler prog.ض -o out.cpp   # → out.cpp + out.hpp
g++ -std=c++20 out.cpp -o out && ./out
```

### DAAD (FACT)
```bash
cd DAAD
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)  # → build/daad + test_* + ctest
./build/daad hello.daad -o hello.s [--ast] [--ir] [--target=x86|dhad]
gcc hello.s -o hello.exe && ./hello.exe; echo $?
```

### CPU (FACT)
```bash
cd CPU
make            # → dhad_cpu (من main.c+debug.c) + dhad_asm (من asm_main.c+dhad_asm.c)
make gui        # → dhad_gui (يحتاج Qt5 + moc)
./dhad_asm prog.ضasm prog.bin
./dhad_cpu prog.ضasm [--hex] [--state] [--bin --out f.bin] [--debug]
./run_tests.sh            # ~90 حالة (يحتاج ./dhad_cpu في نفس المجلد)
# لاحظ: Scripts/run_tests.sh نسخة مكررة تعمل فقط إذا كان CWD هو CPU/
```

### Studio (FACT)
```bash
# أمامي:
cd Studio/Web && npx serve .              # أو vercel --prod
# خلفي:
cd Studio/desktop-app/server
npm ci && npx prisma generate
npx prisma migrate deploy && node src/index.js        # :3000/health
npm test                              # jest (unit/integration/coverage)
# سطح مكتب:
cd Studio/desktop-app && npm ci && npx electron .    # أو electron-builder --win
# VSCode:
cd Studio/VSCode-Extension && npm ci && npx tsc --noEmit
# Docker:
cd Studio && docker build -t daad-studio-server . && docker-compose up -d
```

### Unified (FACT)
```bash
bash Tests/run_all_tests.sh   # يحتاج gcc/g++/node/npx + ثنائيات CPU مبنية + typescript
# ملاحظة: السطر 20 يثبت ROOT=/home/m_hmoz/Documents/Dhad-Studio-Unified (غير محمول)
```

### Releases (FACT)
`Releases/Interview-Release/DhadStudio-Setup-Win7.msi` + `SHA256SUMS.txt` + `Source/{Daad-Compiler,cpu}` + `Examples/` (20) — لقطة مقابلة، ليست بناءً متكررًا.

---

## 13. Completed Components — المكونات المكتملة

> **تعريف:** مكتملة وظيفيًا بدليل ملف + منطق + مخرجات مثبتة/تقارير e2e (وليس مجرد وجود اسم).

- **Compiler CLI + Pipeline الأساسي — COMPLETE (FACT):** `main.cpp` + `Compiler.cpp` + Lexer/Parser/CodeGen/Diagnostics/Sandbox يعمل كـ transpiler إجرائي/OOP بسيط (إثبات: ثنائيتان مبنيتان + أمثلة `conversions/*/after/*.cpp` المولدة + `Examples/*.cpp/.hpp`).
- **DAAD Frontend + x86 Backend للصحيح/الفروع/الدوال — COMPLETE (FACT):** Lexer/Pratt/Semantic-gate/IR/Optimizer/Backend مع `__daad_print_int/__daad_read_int/_start` + 499 ثلاثية e2e + تقارير Phase2-4 (ABI/spill/PHI/cond-jump).
- **DAAD DHAD Target — COMPLETE (FACT):** `dhad_backend.c` (1494) + `_start` + strings + إطار `0xE000` (يثبته `main.c:132-144` + `factorial.daad.s` 94 سطرًا بتكرار حقيقي).
- **CPU Assembler + Emulator Core + CLI flags — COMPLETE (FACT):** two-pass + 8-بت/64KB + `--hex/--state/--bin/--debug` + ~90 regression في الشل + ثنائيات حاضرة.
- **CPU GUI + Visual + Live — COMPLETE (FACT):** `gui/main.cpp` + `visual_sim.c` + `dhad_live.cpp` (ملفات ضخمة مكتملة البنية؛ التشغيل البصري UNKNOWN بدون display).
- **Web In-Browser Toolchain — COMPLETE (FACT):** Lexer/Parser/AST/CodeGen/Sandbox/evaluator/libraries تعمل (يثبتها `Tests/run_all_tests.sh` S3 كـ smoke؛ التشغيل الكامل UNKNOWN بدون node هنا).
- **Server API Skeleton + Auth/Classrooms/Challenges — COMPLETE (FACT):** routes/controllers/middlewares/prisma/jest موجودة بهيكل إنتاجي + Docker/Railway.
- **VSCode Grammar + Snippets + F9 — COMPLETE (FACT)** كنحيف shim (النحافة مقصودة، ليست عيبًا).
- **Release MSI — COMPLETE (FACT)** كلقطة (`Interview-Release/`).

---

## 14. Incomplete Components — المكونات الناقصة

- **Compiler `tests/` — PARTIAL→MISSING (FACT):** مجلد غائب + `Makefile:test` مكسور + `LastTest.log` فارغ. لا يمكن ادعاء تغطية.
- **Compiler Optimizer/Sandbox/Struct/Constructor/GOTO — PARTIAL (FACT):** طي ثوابت فقط؛ sandbox عدّ نصي؛ `Struct` اسم فقط؛ `Constructor` لا يُبنى؛ `انتقل` مرفوض صراحة.
- **DAAD Float/String/IO/Exceptions/Alloc — PARTIAL (FACT من تقارير Phase3):** 28/35 opcode تولد؛ CMP أُصلح لكن float بلا `%xmm`، ولا alloc/IO-strings/try-catch كاملة؛ ARM/RISCV معلنة بلا CLI.
- **DAAD SSA/CFG Integration — PARTIAL (FACT):** كود حقيقي لكن غير موصول من `main` (التحسين فقط موصول).
- **CPU Hardware Parity — PARTIAL (FACT):** `.circ` مولد + specs، لكن 10 vectors مفقودة (HIGH: multi-byte fetch/CALL-RET/NEG-INC-DEC-wrap) + `DATAPATH_DISCREPANCIES.md` يوثق فروقًا.
- **Studio Qt/Full Track — PARTIAL (FACT):** QML/bridge/compiler موجودة لكن تكاملها مع Node-server غير مثبت ستاتيكيًا + `build/` و`DhadBridge` ثنائي بلا سجل بناء متكرر.
- **Docs/CI/Scripts Portability — PARTIAL (FACT):** توثيق قديم + jobان مكسوران + مسارات مثبتة (`ROOT`، `/run/media/...`).

---

## 15. Stubs / Placeholders — جميع الأجزاء الوهمية أو المؤقتة

> **منهجية:** `Grep` مباشر (وليس تخمينًا). النتيجة الإجمالية: **نظيف بشكل لافت — 3 مواضع فقط في الكود المشحون.**

| الموقع (FACT) | النص الحرفي | التصنيف |
|---|---|---|
| `Studio/Web/js/dhad-codegen.js:428` (+ 4 نسخ في `frontend-web/server/Full` + `.bak`) | `default: this.line('// TODO: ' + stmt.type)` | **PLACEHOLDER** — fallback لعقد AST غير المعالجة (سياق image ops). أي لغة تستخدم تلك العقد ستولد تعليقًا لا كودًا. |
| `DAAD/compiler/codegen/register_allocator.c:221` | `TODO: مخصص نطاقات حياة صحيح داخل الكتلة ثم عطّل هذا الوضع.` | **STUB** — تعطيل مؤقت لنطاقات الحياة الدقيقة داخل الكتلة. |
| `DAAD/compiler/backend/backend_interface.c:978` | `fprintf(out,"  # unimplemented opcode %s\n",...)` في `default:` | **STUB** — fallback لطيف (تعليق asm) بدل crash للـ opcodes غير المدعومة. |
| `Compiler/src` + `include` + `CPU/src` + `include` | **صفر تطابق** لـ `TODO\|FIXME\|STUB` (مثبت بـ Grep) | **COMPLETE** بهذا المعيار. |
| `CPU/hardware/tests/DATAPATH_VECTORS.md:349` | `## MISSING/TODO Test Vectors` (10 صفوف) | **PLACEHOLDER** توثيقي — vectors مخططة لم تُكتب. |
| `DAAD` تقارير قديمة تذكر `SSA stub` | ادعاءات V2/V3 (2026-08-05/06) | **HISTORICAL STUB** — تجاوزها الكود الحالي (`ssa.c` حقيقي) لكن غير موصول. |
| ملفات `.bak` (~10 في `Web/js` + `Electron/main.js.bak` + `bridge/*.bak`) | نسخ احتياطية بجانب الأصل | **EXPERIMENTAL** — دليل refactoring جارٍ، ليست stubs لكنها تلوث الشجرة. |
| `cpu-original/` + `Sources-Archive/` + `NUL` + `output.s` المبعثرة | لقطات/مخلفات | **UNUSED** — ليست stubs لكنها dead weight. |

**ما ليس stub (توضيح مهم):** `demo.ضasm` الفارغ و`factorial.ضasm` المسمى خطأ (countdown) هما **BROKEN fixtures** لا stubs؛ و`dhad_isa.h` القديم هو **ORPHAN** لا stub.

---

## 16. Known Issues — المشاكل التي تم اكتشافها (مثبتة فقط)

1. **CI مكسور جزئيًا (FACT — `ci-cd.yml:47-73`):** job `studio` و`gate-stdlib` يستخدمان `working-directory: daad-studio/dhad-studio` غير الموجود (`ls` يفشل) ويستهدفان `DaadTests` غير الموجود بهذا المسار. **الأثر:** 2/3 jobs تفشل على checkout نظيف. job `daad-compiler` وحده صالح شكليًا.
2. **Compiler بلا اختبارات (FACT):** لا `Compiler/tests/`؛ `Makefile:19-20` يشير لـ `../build/bin/DaadTests`؛ `CMakeLists:45-48` سيفشل مع `DAAD_BUILD_TESTS=ON`. **الأثر:** `BUILDING.md:48` (`-DDAAD_BUILD_TESTS=ON` + `ctest`) لا يعمل حرفيًا.
3. **`.gitignore` غير مطبق على الموجود (FACT):** `Compiler/build*/` و`*.exe/*.o/*.bin/*.zip/*.db*` محظورة نظريًا لكنها حاضرة على القرص (بما فيها `daad-compiler.exe` و`dev.db*` و`desktop_dhad_*.zip`). **الأثر:** تلوث الشجرة + خطر تسريب أسرار/ثنائيات.
4. **ازدواج النواة في CPU (FACT — تعليق `Makefile:11`):** `dhad_cpu` يُبنى من `main.c` المضمنة لا من `dhad_cpu.c` الموحدة. **الأثر:** إصلاح في واحدة لا يصل للأخرى (strong divergence risk)؛ التقارير `run_tests.sh` تختبر المضمنة فقط.
5. **ثوابت متناقضة لحجم الستاك (FACT):** `dhad_cpu.h/memory_map.h=64` مقابل `isa_constants.h=32` مقابل `isa.h=16` مقابل `main.c=32`. **الأثر:** اختبار `stack_overflow` قد يمر على نسخة ويفشل على أخرى.
6. **`dhad_gui.pro` قديم (FACT):** يحذف `src/dhad_cpu.c` و`dhad_cpu.h` بينما `Makefile` يشملهما. **الأثر:** بناء QtCreator يختلف عن بناء Make.
7. **توثيق قديم (FACT):** `REPORT.md` + `docs/ARCHITECTURE.md` + `desktop/ARCHITECTURE.md` تصف 4-بت/4096 بينما الكود 8-بت/64KB v4.0/v4.5؛ `PROJECT_STRUCTURE.md` تصف `frontend-web/server/...` غير المطابقة لـ `Studio/*`؛ `BUILDING.md` تطلب `build.ps1/installer.nsi` غير الموجودين وتستخدم `;` اللاتينية بينما الأمثلة `؛`؛ `Docs/Instructions/README.md` أرقام أسطر قديمة. **الأثر:** مطور جديد سيتبع مسارات خاطئة.
8. **Fixtures مكسورة (FACT):** `CPU/demo.ضasm` فارغ (0 سطر)؛ `CPU/examples/factorial.ضasm` تسمية خاطئة (countdown 5→1)؛ `CPU/tests/test_expr2.dasm` امتداد لاتيني شاذ. **الأثر:** أمثلة مضللة + كسر أي glob يفترض `.ضasm` فقط.
9. **مسارات مثبتة غير محمولة (FACT):** `Tests/run_all_tests.sh:20` يثبت `ROOT=/home/m_hmoz/...`؛ `Scripts/winbuild.sh` يثبت `BASE="/run/media/m_hmoz/UBUNTU 26_0/ "`؛ `Studio/Electron/main.js:39` يشير لـ `../Dhad-Studio-Full/server` القديم. **الأثر:** تعمل على جهاز المؤلف فقط.
10. **تكرار الملفات (FACT):** `Scripts/run_tests.sh` ≡ `CPU/run_tests.sh` (334 سطرًا)؛ `Web/js` ≡ `frontend-web/js` ≡ `server/src/utils/dhad` (بإزاحة TODO)؛ `Compiler/src` ≡ `Full/compiler/src`؛ `CPU/src` ≡ `cpu-original/src` (ناقص `asm_main.c`)؛ `README-ويندوز.md` مكرر. **الأثر:** انحراف صامت + تضخم (`Docs/Reports` 62 + `DAAD/*.md` 14 تروي نفس القصة).
11. **`.bak` و`NUL` و`output.s` مبعثرة (FACT):** ~10 bak + ملف `DAAD/NUL` (بقايا Windows) + `.s/.exe` في جذر DAAD رغم `.gitignore`. **الأثر:** ضجيج + التباس أي مخرج هو "الحالي".
12. **VSCode يعتمد على خارجي (FACT — `extension.ts:20`):** `daad-compiler` يجب أن يكون في PATH ولا يُحزم مع الإضافة ولا يوجد LSP. **الأثر:** F9 يفشل على تثبيت نظيف.
13. **حدود Sandbox سطحية (FACT):** `SandboxValidator` عدّ نصي (`>20 funcs`، `new int[`) + regex؛ `dhad.js` blocklist نصية. **الأثر:** ليست ضمانًا أمنيًا (انظر §17).
14. **تجزئة اللغة (FACT):** 4 امتدادات (`.ض/.daad/.ضasm/.deff`) و4 نحويات (`صحيح س=10؛` مقابل `دالة: f()->رقم :-` مقابل `حمّل مح 5` مقابل `011-input.deff`) بلا جدول مواصفة موحد. **الأثر:** غير واضح أي frontend يقبل أي ملف.

**ما لم يُدَّعَ:** لا ادعاء بتسرب أسرار (`.env` محظور وغير مقروء هنا)، ولا بكسر بناء (البناء لم يُشغَّل ضمن هذه المهمة)، ولا بثغرة runtime محددة.

---

## 17. Technical Debt — الدين التقني الواضح

1. **توحيد مفقود:** 4 سلاسل أدوات + 3 نسخ JS + لقطات `cpu-original/Sources-Archive` — **الدين:** أي إصلاح لغوي يجب تكراره 3-4 مرات. **المقترح (تسجيل فقط):** جدول مواصفة واحد + مولد واحد + حزم مشتركة.
2. **اختبارات Compiler غائبة + سجلات فارغة:** **الدين:** لا شبكة أمان للمسار الأكثر استخدامًا تعليميًا. **المقترح:** استعادة `tests/` أو توثيق غيابها رسميًا وتصحيح `Makefile:test` و`BUILDING.md`.
3. **النواة المزدوجة + الثوابت المثلثة:** **الدين:** `main.c` مقابل `dhad_cpu.c` + 64/32/16. **المقترح:** حذف المضمنة أو توليدها من الموحدة + ثابت واحد `STACK_SIZE`.
4. **التوثيق كأرشيف لا كمرجع:** 62 + 14 تقريرًا تروي تاريخًا لا حالة. **الدين:** `PROJECT_STRUCTURE/BUILDING/ARCHITECTURE/REPORT` مضللة. **المقترح:** تجميد القديم تحت `Docs/Archive/` + صفحة واحدة حالية مولدة من الشجرة.
5. **الأمان بالheuristics:** sandbox عدّ نصي + أسرار JWT تُولَّد تلقائيًا (`index.js:16-56` حسب التقارير) + pentest ad-hoc بلا بوابة. **الدين:** غير صالح للإنتاج المدرسي الحقيقي. **المقترح:** VM حدود صارمة + إدارة أسرار + بوابة `npm test` في CI.
6. **البناء غير المتكرر:** `build*/node_modules/.exe/.db` ملوثة + `winbuild.sh` بمسار شخصي + CI مكسور. **الدين:** "يعمل على جهازي". **المقترح:** تنظيف + `SCRIPT_DIR` + إصلاح `ci-cd.yml` ليشير لـ `Compiler` و`Studio/...` الحقيقية.
7. **التغليف المزدوج:** NSIS (CMake) مقابل WiX (`dhadstudio.wxs` + MSI) مقابل electron-builder — **الدين:** 3 أنظمة تثبيت. **المقترح:** اختيار واحد وتوثيقه.

---

## 18. Unknowns — الأشياء التي لم يمكن إثباتها

- **هل يبني كل شيء الآن؟ UNKNOWN:** البناء لم يُشغَّل (ممنوع التثبيت/الأوامر الخطرة في هذه المرحلة)؛ الثنائيات الحاضرة بتورايخ Aug/Sep 2026 توحي بنجاح سابق لا حالي.
- **هل تنجح الاختبارات الآن؟ UNKNOWN:** `LastTest.log` فارغ + `run_all_tests.sh` smoke فقط + تقارير الـ 998/617 assert تاريخية.
- **أداء المترجمات (10K سطر/stress/fuzz)؟ UNKNOWN:** ملفات `test_stress/test_fuzz/test_performance` موجودة لكن نتائجها في تقارير لا سجلات حية.
- **سلوك الأرقام المشرقية `١٢٣` في Track A؟ UNKNOWN:** تُختبر على Web فقط؛ `UnicodeUtils` في C++ لم يُفحص سطريًا هنا.
- **تكامل Qt/QML مع السيرفر؟ UNKNOWN:** الملفات موجودة لكن لا استدعاء مثبت.
- **مخطط DB الكامل (25 جدولًا) وصلاحيات المدارس؟ UNKNOWN:** `schema.prisma` لم يُقرأ سطريًا (خارج عينة القراءة العميقة؛ التقرير اعتمد على `PROJECT_STRUCTURE.md` + أسماء controllers).
- **سجل git (من كتب ماذا ومتى)؟ UNKNOWN:** خارج نطاق القراءة المطلوبة.
- **الترخيص الفعلي؟ UNKNOWN جزئيًا:** `Compiler/LICENSE` + `Studio/LICENSE.txt` موجودان لكن نصهما لم يُقرأ؛ `PHASE6_LICENSE_AUDIT.md` موجود للمراجعة اللاحقة.

---

## 19. Final Architecture Map — خريطة نهائية للمشروع

```text
                        ┌─────────────────────────────────────────┐
                        │  Dhad-Studio-Unified (monorepo, no root │
                        │  build — 4 tracks + platform + archive) │
                        └──────────────┬──────────────────────────┘
        ┌──────────────┼───────────────┼────────────────┐
        ▼              ▼               ▼                ▼
 ┌─────────────┐ ┌───────────┐ ┌──────────────┐ ┌──────────────┐
 │ Compiler    │ │ DAAD      │ │ CPU          │ │ Studio       │
 │ .ض → C++    │ │ .daad→.s  │ │ .ضasm→emu    │ │ EdTech       │
 │ C++20       │ │ C99       │ │ C11/Qt/Logisim│ │ JS/TS/Node  │
 └──────┬──────┘ └─────┬─────┘ └──────┬───────┘ └──────┬───────┘
        │              │              │                │
   DaadCore       daad_lib      dhad_cpu/       Web◄──►Server
   CLI:daad-      CLI:daad      asm+gui         ▲    (Prisma/
   compiler       (x86|dhad)    CLIs:cpu/        │     Redis/JWT)
   examples       e2e:499       asm/vis/live     │         │
   stdlib×2       docs:16       tests:112    Electron    VSCode
   (no tests)     audits:14     hw:circ       (bundle)   (shim F9)
                                     │         Full/Qt
                                     ▼        (parallel)
                              ┌──────────────┐
                              │ Docs/Scripts │
                              │ Tests/Examples│
                              │ Archive/     │
                              │ Releases(MSI)│
                              │ CI (1/3 ok)  │
                              └──────────────┘

  Inter-track edges (proven only):
    DAAD --(.s/.ضasm _start)--> CPU   [documented Phase12.5]
    Examples --(fixtures)--> all tracks
    Docs/Scripts/Tests --(describe/invoke)--> all tracks
    Studio server --(JS re-impl, NOT native)--> Dhad language
  No edge: Compiler→CPU, Studio→native Compiler/DAAD/CPU binaries
```

---

## FINAL VERDICT — ملخص حالة المشروع الحالية

### What works (مثبت بالملفات)
- **Compiler كـ transpiler تعليمي:** CLI + pipeline كاملة + مخرجات `.cpp/.hpp` حقيقية + أمثلة مولدة + ثنائيتان مبنيتان. **FACT.**
- **DAAD كـ native compiler للصحيح/الفروع/الدوال:** frontend صارم + IR/optimizer/backend + 499 e2e + DHAD target المتوافق مع CPU. **FACT.**
- **CPU كـ teaching emulator:** assembler two-pass + نواة 8-بت + CLI flags + ~90 regression + GUI/visual/live + نموذج Logisim. **FACT.**
- **Studio كـ EdTech skeleton:** ويب static + toolchain داخل المتصفح + API بخارطة كاملة + Electron offline + VSCode F9 + Docker/Railway. **FACT.**
- **Release:** MSI للويندوز 7 + SHA + أمثلة + مصادر لقطة. **FACT.**

### What does not work (مثبت)
- **CI:** 2/3 jobs بمسارات `daad-studio/dhad-studio` غير الموجودة. **FACT (`ci-cd.yml`).**
- **`make test` في Compiler + `BUILDING.md` حرفيًا:** يشيران لـ `tests/` و`DaadTests` و`build.ps1/installer.nsi` غير الموجودة. **FACT.**
- **`Electron/main.js` القديم:** مسار `../Dhad-Studio-Full/server` لا يطابق الشجرة (الصحيح في `desktop-app/`). **FACT.**
- **`dhad_gui.pro`:** يبني GUI ناقصًا (بلا `dhad_cpu.c`). **FACT.**
- **Fixtures:** `demo.ضasm` فارغ + `factorial` مسمى خطأ + `.dasm` شاذ. **FACT.**

### What is incomplete (ناقص لا مكسور)
- Optimizer/Sandbox/Struct/Constructor/GOTO في Compiler؛ Float/String/IO/exceptions/ARM في DAAD؛ SSA/CFG غير موصولة؛ 10 hardware vectors؛ تكامل Qt؛ توثيق محدث؛ `Tests` harness محمول. **(تفصيل §14 — FACT.)**

### What is stubbed (وهمي/مؤقت — القائمة الكاملة)
- `Web/js/dhad-codegen.js:428` fallback `// TODO: stmt.type` (+نسخها). **PLACEHOLDER — FACT.**
- `DAAD/.../register_allocator.c:221` TODO عربي. **STUB — FACT.**
- `DAAD/.../backend_interface.c:978` `# unimplemented opcode`. **STUB — FACT.**
- 10 vectors في `DATAPATH_VECTORS.md:349`. **PLACEHOLDER — FACT.**
- ادعاءات `SSA stub` القديمة تجاوزها الكود لكن بقيت غير موصولة. **HISTORICAL — FACT.**
- **والباقي نظيف:** صفر TODO في `Compiler/src+include` و`CPU/src+include`. **FACT (Grep).**

### What is unknown (يحتاج تشغيلًا)
- نجاح البناء/الاختبارات **الآن**، الأداء/stress/fuzz، سلوك `١٢٣` في C++، تكامل Qt، مخطط DB الكامل، سجل git، نصوص الرخص. **(تفصيل §18.)**

### What should be investigated next (بالترتيب، بدون تعديل الآن)
1. **تشغيل آمن للقراءة فقط:** `cmake --build` + `ctest` في `DAAD/build`، و`node --check` في `Studio/Web/js`، و`./dhad_cpu tests/full_T1a_add.ضasm` — لتأكيد §11 قبل أي تغيير.
2. **حسم اللغة:** جدول واحد يربط `.ض/.daad/.ضasm/.deff` بالنحويات والأدوات (يحل §16.14).
3. **حسم المعمارية:** هل تُحذف `main.c` المضمنة أم `dhad_cpu.c`؟ وهل يُحذف `dhad_isa.h` و`cpu-original/`؟ (يحل §16.4-6).
4. **حسم الاختبارات:** استعادة `Compiler/tests/` أو توثيق غيابها + إصلاح `ci-cd.yml` + جعل `ROOT` نسبيًا (يحل §16.1-2,9).
5. **حسم التوثيق:** تجميد `REPORT.md/ARCHITECTURE.md/PROJECT_STRUCTURE.md/BUILDING.md` القديمة ونشر صفحة بناء واحدة من الشجرة الحالية.
6. **حسم الأمان:** مراجعة `PHASE6_SECRET_AUDIT.md` + `SECURITY_HARDENING_REPORT.md` + `prisma/schema.prisma` قبل أي نشر مدرسي.

> **قاعدة عدم اللمس:** لا تُمس الأجزاء `Compiler/src/Parser.cpp` (1683) و`DAAD/compiler/backend/*` (2700+) و`CPU/src/main.c` (1571) و`Studio/desktop-app/server/src/*` (50 ملفًا) دون فهم إضافي — فهي الأكثر coupling والأقل تغطية مثبتة، وأي تغيير فيها يكسر مسارًا كاملًا.
