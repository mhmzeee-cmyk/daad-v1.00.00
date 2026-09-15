# PROJECT_UNDERSTANDING.md — Dhad-Studio-Unified

> **المنهجية:** DISCOVER → READ → UNDERSTAND → MAP → REPORT — بدون تعديل أي ملف كود (يُستثنى إنشاء هذا التقرير نفسه).
> **التاريخ:** 2026-09-14 — إعادة مسح كاملة بعد تقرير 2026-09-12 (القديم 592 سطراً أصبح قديماً في ~10 نقاط).
> **النطاق المقروء فعلاً:** الجذر (48 مدخلاً) + `Compiler/` + `DAAD/` + `CPU/` + `Studio/` + `Docs/` + `docs/` + `Scripts/` + `Tests/` + `Examples/` + `Demo/` + `release/` + `Sources-Archive/` + `Operational-Evidence/` + `.github/` + `.opencode/` + `opencode.json` + `.gitignore` + `dhad` + `ض.json` (عينة) + التقارير الجذرية.
> **أدوات التحقق:** `Read` مباشر للملفات الحرجة + `Bash ls/du/file/head` للقراءة فقط + `Grep` لـ `TODO|FIXME|STUB|unimplemented` + قراءة متوازية عبر 5 وكلاء استكشاف (Compiler / DAAD / CPU / Studio / Support-dirs) + فحص `CMakeLists/Makefile/package.json/Dockerfile/ci-cd.yml/targets.json`.
> **قاعدة الدقة المستخدمة أدناه:** `FACT` = مثبت من ملف مقروء (مع مسار:سطر)، `INFERENCE` = استنتاج منطقي، `UNKNOWN` = لا يمكن إثباته بالقراءة وحدها.
> **ملاحظة عن التقرير القديم:** التقرير السابق (2026-09-12) كان دقيقاً في العمق لكنه قديم في الوصف السطحي: ادعى `15 مدخلاً في الجذر بلا README` و`Releases/ موجودة` و`Examples 75` و`Tests ملف واحد` و`Studio/Electron/` — كلها مخالفة للمقروء الآن (انظر §16.7). هذا التقرير يصححها.

---

## 1. Project Overview — ما هو المشروع؟

**FACT:** المشروع في `/home/m_hmoz/Documents/Dhad-Studio-Unified` هو **مونوربو موحّد (Unified monorepo)** يجمع 4 عوالم كانت سابقاً مستودعات منفصلة، زائد طبقة تنسيق وطبقة أدلة:

```text
Dhad-Studio-Unified/  (48 مدخلاً جذرياً — مثبت بـ ls -la 2026-09-14)
├── Compiler/          # مترجم C++20: عربي (.ض) → C++ (.cpp + .hpp) — DaadStudio v1.0.0
├── DAAD/              # مترجم C99: عربي منخفض المستوى (.daad) → x86-64 (.s) أو DHAD (.ضasm/.bin)
├── CPU/               # محاكي + مجمّع + IDE بصري لمعالج عربي مخصص (8-بت بيانات / 16-بت عناوين / 64KB)
├── Studio/            # المنصة التعليمية: Web ثابت + desktop-app (Electron) + Server (Node/Express/Prisma) + VSCode + Full/Qt/QML
├── dhad               # المنسق الموحد الوحيد (bash، 7.1K) — build/run dispatcher يقرأ targets.json
├── Docs/              # أرشيف عملياتي تاريخي (6 مجلدات: Archive/Instructions/Reference/Reports/Root-Reports/Specs/)
├── docs/              # المواصفة المعيارية (Single Source of Truth، 304K: LANGUAGE_SPEC/GRAMMAR/KEYWORDS/STDLIB)
├── Examples/          # مدونة أمثلة موحدة (72 مدخلاً مثبتاً، لا 75)
├── Scripts/           # سكربتات Windows (.bat عربية) + أدوات بناء/تحليل (25 مدخلاً)
├── Tests/             # بوابتان: run_all_tests.sh (612، شكلي) + real_regression.sh (149، حقيقي)
├── Demo/              # عرض مصغر بضغطة واحدة (11 مدخلاً: run-demo.sh + loop5.daad + دليل عربي)
├── release/           # مسرح إصدار نشط Win7 + Win10-11 (8 مدخلات: 2×MSI + vsix + SHA332 + Source + Examples)
├── Sources-Archive/   # أرشيف بارد لا يُبنى منه (9 مجلدات: binaries/bridge/build-logs/...)
├── Operational-Evidence/ # أدلة تشغيل مصنفة حسب المكون (17-19 مدخلاً: CPU/Compiler/DAAD/MSI/WINE/...)
├── .github/workflows/ci-cd.yml  # CI واحد (3 jobs: واحد صالح + اثنان مكسوران)
├── .opencode/ + opencode.json   # إعداد وكلاء opencode (mimo-v2.5-free: orchestrator + local-reviewer)
├── .vscode/settings.json + .wine-demo/ (prefix تجريبي)  # جديدان لم يوثقهما التقرير القديم
├── ض.json (2MB session dump) + ~27 تقرير *.md جذري + README.md (سطر واحد فقط)
└── .gitignore (141 سطراً)
```

**FACT:** لا يوجد `Releases/` ولا `Program Files/` الآن (كانا في `PROJECT_INDEX.md:18,20` والتقرير القديم). المثبت بـ `ls الجذر`: `release/` فقط. `git status` المسجل في `ض.json:2723` يظهر `D Releases/Interview-Release/...` — أي حُذفت. أي توثيق يذكر `Releases/` أصبح تاريخياً.

**FACT:** لا يوجد `package.json` جذري ولا `CMakeLists.txt` جذري — كل بناء لامركزي داخل المجلد الفرعي، وطبقة التنسيق الوحيدة هي سكربت `dhad` (ليس مترجماً).

**INFERENCE:** الاسم "Unified" يعني دمج `Daad-Compiler` + `cpu` + `Dhad-Studio-Full` + `daad-studio/dhad-studio` في شجرة واحدة، لكن الدمج **ميكانيكي (نسخ مجلدات) وليس معمارياً** — كل مكون ما زال له entry-point وبناء ولغة مختلفة. الجديد في هذا التقرير: المنسق `dhad` + سجل `targets.json` + المواصفة `docs/` + بوابة `real_regression.sh` هي أول محاولة جدية لجعل التوحيد وظيفياً لا شكلياً.

---

## 2. Project Purpose — ما الهدف منه؟

**FACT (من الملفات):**
- `Compiler/CMakeLists.txt:6` يصف نفسه: `"ض استديو: مترجم لغة البرمجة العربية 'ض' إلى C++20"`.
- `DAAD/README.md:9` يصف `ض Core` بأنها *"لغة برمجة منخفضة المستوى ... الجسر بين لغة التجميع واللغات عالية المستوى"*.
- `docs/LANGUAGE_SPEC.md:1-9` (الجديد، `الإصدار 1.0-factual — 2026-09-11 — من الكود الفعلي فقط`): النطاق `.ض→C++20/JS` فقط، و`.daad` خارج النطاق عمداً. جدول `C++=الأصل vs JS=السحابية` مع `✅/❌/⚠️` لكل ميزة.
- `Docs/Reference/PROJECT_STRUCTURE.md:9` يصف `Dhad Studio` بأنها *"Arabic-first EdTech platform ... teacher dashboard, gamification (XP/levels/streaks), AI challenges"*.
- `Docs/Instructions/README.md` (330 سطراً) يصف *"حاسوب عربي كامل: لغة ض + مترجم → C++20 + معالج 8-بت + assembler + سيرفر تعليمي + واجهات Web/Electron/VSCode"*.
- `Studio/desktop-app/package.json:3` : `"ض ستوديو - منصة تعليم البرمجة"`.
- `dhad:1-22` يعلن نفسه: `unified build orchestration (NOT a compiler) ... frontend → representation → TargetResolver → Driver → toolchain`.
- `Demo/دليل-العرض-بضغطة-واحدة.md:50-52,95-97` يعلن الغرض العملي: عرض 5 دقائق بأربع مشاهد لغير التقنيين + حساب جاهز `demo@example.com/Test1234!` + أمانة `لينكس 4/4 خضراء، ويندوز يحتاج تجربة على جهاز العرض`.

**INFERENCE:** الهدف المزدوج هو: (1) **تمكين البرمجة بالعربية** عبر 3 واجهات لغوية (عالية → C++، منخفضة → آلة، تجميعية → محاكي)، و(2) **منصة تعليمية EdTech** تستهلك تلك اللغات. الجديد: `dhad + targets.json + real_regression.sh + Demo/` تربط الهدفين عملياً (build→run→verify)، بينما التقرير القديم قال "لا يوجد ملف واحد يربط الهدفين" — هذا عاد صحيحاً جزئياً: الرابط الآن موجود لكنه نحيف (سكربت + سجل JSON + سكربتا اختبار).

---

## 3. Technology Stack — التقنيات المستخدمة

| الطبقة | التقنية | الدليل (FACT) |
|---|---|---|
| مترجم عالي المستوى | C++20، CMake ≥3.21، g++، MSVC `/utf-8`، FetchContent GTest v1.14.0، CPack/NSIS + WiX | `Compiler/CMakeLists.txt:1-35`، `Compiler/Makefile:4` |
| مترجم منخفض المستوى | C99، CMake ≥3.10، gcc، `-Wall -Wextra -Wpedantic -Werror`، CTest، libm فقط | `DAAD/CMakeLists.txt:1-8,33` |
| محاكي/مجمّع | C11 (`-std=c11`) + C++ (`gnu++1z`)، Qt5 (GUI)، Logisim Evolution 3.8.0 (عتاد)، moc | `CPU/Makefile:2-8`، `CPU/hardware/build_dhad_cpu.py`، `CPU/gui/main.cpp` |
| منسق موحد | bash + python3 (لقراءة JSON) + gcc/g++/MinGW — لا يترجم بنفسه | `dhad` (head 60) + `Docs/Reference/targets.json` |
| سجل الأهداف | JSON آلي: 4 أهداف AVAILABLE فقط (linux-x64 / linux-x64-cpp / windows-x64 / dhad-cpu)، الباقي Not Implemented | `Docs/Reference/targets.json` + `Docs/Reference/TARGETS.md:17-27` |
| ويب أمامي | HTML/CSS/JS خام بدون bundler (static)، `npx serve`، Vercel static، PWA `sw.js`، jspdf/html2canvas | `Studio/Web/package.json:6-10` (لا dependencies)، `Studio/vercel.json` |
| خلفية | Node ≥18، Express 4.21، Prisma 5.22 (SQLite dev + Postgres variant)، ioredis، JWT (bcryptjs/jsonwebtoken)، helmet/compression/rate-limit/multer/xlsx/openai 6.46/winston/swagger، Jest 30 + supertest، PM2/cluster، Docker multi-stage (node:20-alpine، non-root daaduser)، nginx، Railway/Render | `Studio/desktop-app/server/package.json:6-62`، `Studio/Dockerfile:9-80`، `Studio/docker-compose.yml`، `Studio/railway.json` |
| سطح مكتب | Electron 33 + electron-builder 24 (NSIS، appId `com.dhad.studio`) — الموجود `desktop-app/` فقط، لا `Electron/` مستقل | `Studio/desktop-app/package.json:33-77`، `ls Studio/` (لا Electron/) |
| محرر | VSCode Extension (TypeScript 5، TextMate grammar، snippets، F9) | `Studio/VSCode-Extension/package.json:7-58`، `src/extension.ts` (26 سطراً) |
| أصلي Qt | Qt5/6 QML (26 ملفاً) + C++ bridge + 10 ملفات i18n | `Studio/Full/frontend/qml/`، `Studio/Full/bridge/` |
| مواصفة اللغة | Markdown + JSON (`LANGUAGE_SPEC/GRAMMAR_SPEC/KEYWORDS_REGISTRY 116 كلمة/SEMANTIC/STDLIB`) | `docs/` (8 مداخل) |
| CI | GitHub Actions ubuntu-latest (cmake/gcc/make) — 3 jobs | `.github/workflows/ci-cd.yml` (73 سطراً) |
| وكلاء | opencode `mimo-v2.5-free` (orchestrator primary + local-reviewer subagent) + plugin 1.18.27 | `opencode.json`، `.opencode/package.json` |
| قواعد بيانات | SQLite (`dev.db` + wal/shm) للتطوير، Postgres schema للإنتاج، Redis 7 (LRU+AOF) للجلسات/rate-limit | `Studio/desktop-app/server/prisma/`، `Studio/docker-compose.yml` |
| عرض تجريبي | bash + mingw_compat.c + loop5 fixtures + WINE prefix (.wine-demo/) | `Demo/` (11) + `.wine-demo/` |
| لاشيء مركزي | لا يوجد root build/test/lint؛ كل مكون يبني وحده + `dhad` كمنسق نحيف | غياب مثبت بالـ `Read` الجذري |

**INFERENCE:** العلاقة بين التقنيات: `Compiler (C++)` و`DAAD (C)` و`CPU (C/Qt)` ثلاث سلاسل أدوات مستقلة تشترك فقط في **الفكرة (عربية) والأمثلة**، بينما `Studio/Web/js/dhad-*.js` **إعادة تنفيذ رابعة بالجافاسكربت** لنفس اللغة تعمل داخل المتصفح، و`Server` لا يستدعي المترجمات الأصلية مباشرة بل يحتوي نسخة JS منها تحت `server/src/utils/dhad/` + sandbox VM. الجديد: `dhad` يوحد الاستدعاء من الأعلى (build/run لكل هدف)، و`docs/` توحد التعريف من الأسفل (spec واحدة)، لكن التنفيذ ما زال مكرراً 3-4 مرات.

---

## 4. Directory Structure — شرح المجلدات

### الجذر — 48 مدخلاً (FACT — `ls -la` 2026-09-14)
- `.git/` — مستودع git (لم تُفحص سجلاته؛ خارج نطاق القراءة).
- `.github/workflows/ci-cd.yml` — job واحد صالح + jobان بمسارات مكسورة (انظر §16.1).
- `.gitignore` (141 سطراً) — يغطي `.env`، `node_modules/`، `Compiler/build*/`، `DAAD/**/*.exe`، `CPU/**/*.o`، `*.exe/*.o/*.a/*.so`، `*.zip`، `*.db*`، `CMakeFiles/`، `dist/build/out/coverage` — **FACT:** رغم ذلك `Compiler/build/ build-make/` والثنائيات `*.exe/*.o/*.bin/*.zip/dev.db` **موجودة فعلياً على القرص** (تجاهل غير مطبق أو ملفات committed قبل القاعدة؛ `git check-ignore -v Examples/01_hello.exe → .gitignore:60:*.exe` يثبت أنها متجاهلة نظرياً لكنها حاضرة).
- `.opencode/` — `agents/orchestrator.md` + `local-reviewer.md` + `node_modules/` + `package.json` (`@opencode-ai/plugin 1.18.27`) + `.gitignore` يتجاهل نفسه.
- `.vscode/settings.json` — جديد، لم يوثقه القديم.
- `.wine-demo/` — WINE prefix (`dosdevices/drive_c/*.reg`) — دليل تجربة ويندوز على لينكس، جديد.
- `opencode.json` (16 سطراً) — model + تعريف الوكيلين.
- `dhad` (7.1K، bash executable) — **المنسق الوحيد** (انظر §5).
- `ض.json` (2MB) — session dump لـ opencode (`info.id ses_f69f... title البحث عن ملف جلسة ض`)، يحتوي `messages[]` وليس كوداً.
- `README.md` — سطر واحد `# daad-v-1` فقط (بلا قيمة، انظر §16).
- `PROJECT_INDEX.md` (59 سطراً، 2026-09-13) — فهرس أحجام، لكنه قديم جزئياً (يذكر `Releases/ + Program Files/` غير الموجودين، وأحجاماً مخالفة للمقروء: `CPU 11M` مقابل `5.4M` الآن).
- `~27 تقرير *.md جذري` — أرشيف مراحل (`PHASE_C/D/E/FGH/I/J/K` + `STAGE1/2` + `SECURITY_FIX_PLAN 75K` + `HIGH_LEVEL_GAPS 49K` + `CHALLENGES_*` + `OOP_*` + `FINAL_*` + `DEDUP_*` + `DEEP_BUG_*` + `INTERVIEW_*`).

### `Compiler/` (11 مدخلاً — FACT)
`CMakeLists.txt` (193) + `Makefile` (26) + `LICENSE` + `daad-compiler.exe` (2.3M) + `Daad/` (wrapper واحد) + `src/` (12 cpp + CMakeLists + `_backup_original/` بست نسخ) + `include/Daad/` (11 hpp) + `stdlib/` + `stdlib_arabic/` (10 مجلدات بأسماء عربية لكل منهما) + `examples/` (14) + `build/` + `build-make/` (مجلدا بناء مولدان، كلاهما يحوي `bin/daad-compiler` بحجمين مختلفين 584K مقابل 1.7M).

### `DAAD/` (~10 مجلدات + ~30 ملفاً جذرياً مبعثراً — FACT)
`CMakeLists.txt` (55) + `README.md` (39) + `compiler/` (token/lexer/parser/ast/semantic/ir/optimizer/cfg/ssa/codegen/backend + `main.c` 293 + `tests/` بـ 12 c و10 exe) + `include/Daad/` (12 hpp — واجهة C++ بديلة/قديمة + `_backup_original/`) + `tests/` (conformance: valid/invalid/runtime/types/memory/abi/lexer/parser + 6 cpp gtest + عشرات `.ض`) + `Testing/` (بقايا ctest) + `e2e/` (~499: ثلاثيات `.daad/.s/.exe`) + `tools/` (ملفان py يثقبان `stdlib/` غير موجود) + `docs/` (17 md) + `build/` + عشرات `.daad/.s/.exe/.daad.s` مبعثرة في الجذر + تقارير تدقيق 14 md + سكربتات `e2e_*.ps1` + ملفات `NUL/output.s`. لا `backends/`, `runtime/`, `stdlib/` رغم توثيقها.

### `CPU/` (FACT)
`src/` (9: `main.c` 1634 + `dhad_cpu.c` 647 + `dhad_asm.c` 704 + `asm_main.c` 29 + `dhad_debug.c` 677 + `dhad_live.cpp` + `visual_sim.c` + o) + `include/` (16 h) + `hardware/` (circ + مولد py + 5 md مواصفات + tests) + `gui/main.cpp` (1156) + `desktop/` + `docs/ARCHITECTURE.md` (235، قديم) + `examples/` (6) + `tests/` (113 ملفاً: `full_T*.ضasm` + `bug_*` + `edge_*` + `ldri_*` + شاذ واحد `.dasm`) + `Makefile` (72) + `dhad_gui.pro` (15) + `REPORT.md` (207، قديم) + `run_tests.sh` (346) + ثنائيات `dhad_cpu/dhad_asm/dhad_gui/dhad_live/dhad_vis` + `demo*.ضasm` + `*.bin/*.o/*.moc/.qmake.stash/*.zip` (`desktop_dhad_v2.0/v3.0.zip`).

### `Studio/` (25 مدخلاً — FACT، `ls Studio/`)
`Web/` (js/9 فيها 8 `dhad-*.js` + pages/ + css/ + examples + sw.js + package.json بلا dependencies) + `desktop-app/` (main.js 215 + preload + splash + package.json + `frontend-web/` مرآة Web + `server/` 31 + `prisma_client/`) + `Full/` (bridge/compiler/frontend/server — frontend فيه qml/26 + i18n/10 + frontend-web مرآة ثالثة) + `VSCode-Extension/` (package.json + src/extension.ts واحد + syntaxes/snippets/out/tsconfig) + `Windows-Launcher/` (9 bat عربية + README-ويندوز) + `Dockerfile` (80) + `docker-compose.yml` (165) + `vercel.json` + `railway.json` + `tests/` (تحديات أمنية) + سكربتات جذرية (`build*.bat`, `cli-login.js`, `create-accounts.js`, `gen_*.py`, `check_braces.py`, `stdlib_index.json`, `test_example.ض`, `test_runtime.cpp`) + `LICENSE.txt`. **تصحيح مهم:** لا يوجد `Studio/Electron/` ولا `Studio/server/` ولا `Studio/frontend-web/` في الجذر — التقرير القديم أخطأ هنا. الإلكترون = `desktop-app/` فقط.

### `Docs/` (6 مجلدات — FACT، لا 4)
`Archive/` + `Instructions/` (11) + `Reference/` (18: BUILD_PIPELINE/TARGETS/targets.json/LANGUAGE_CONTRACT/... + handover/ + reference/) + `Reports/` (62: PHASE2/3/45/6 + manifests) + `Root-Reports/` (5) + `Specs/` (9 — نسخة من `docs/` + ملف زائد `README-الدمج.md`).

### `docs/` (8 مداخل — FACT)
المواصفة المعيارية: `GRAMMAR_SPEC.md` + `KEYWORDS_REGISTRY.json` (116 كلمة، سطر واحد ضخم) + `KEYWORD_IMPLEMENTATION_PACK.md` + `LANGUAGE_SPEC.md` (`1.0-factual — 2026-09-11`) + `SEMANTIC_RULES.md` + `STDLIB_SPEC.md` + `logo-ض.jpeg` + `vscode/`.

### `Examples/` (72 — FACT بـ `ls | wc -l`، لا 75)
أزواج `01_hello`–`07_fibonacci` (كل واحد `.ض+.cpp+.exe+.hpp+.linux` = 35) + مفردات (`snake/flappy/zombie/calculator/test_sum/...`) + 6-8 مجلدات (`conversions/cpp-learning/daad_presenter/data-pos/games/programs/`).

### `Scripts/` (25 — FACT)
12 bat عربية + 12 كود (`analyze_dups.py/detailed_analysis.py/phase2_dupclass.py/killsrv.py/gen_launcher.py/gen_wxs.py/auth_cycle.sh/build_demos.sh/hash_script.sh/phase2_hash.sh/run_tests.sh 334/winbuild.sh 10`) + `README-ويندوز.md` (147). `run_tests.sh` نسخة طبق الأصل من `CPU/run_tests.sh`.

### `Tests/` (2 — FACT، لا 1)
`run_all_tests.sh` (612) + `real_regression.sh` (149، الحقيقي — فات القديم).

### `Demo/` (11 — FACT، جديد كلياً على القديم)
`دليل-العرض-بضغطة-واحدة.md` + `run-demo.sh` + `run-demo.bat` + `api-demo.js` + `loop5.daad` + `loop5.bin` + `mingw_compat.c` + `build-windows.sh` + `start-server.sh/bat` + `windows-bin/`.

### `release/` (8 — FACT)
`DhadStudio-Setup-Win7.msi` + `windows-10-11/` (bin + vsix + MSI + wxs + README + SHA + validate) + `windows-7/` (نفس الهيكل) + `Documentation/` + `Examples/` (20) + `Source/` (cpu + Daad-Compiler) + `build-companions/` + `SHA256SUMS.txt` (332 سطراً).

### `Sources-Archive/` (9 — FACT)
`binaries/bridge-sources/build-logs/compiler-sources/misc/msi-extract/test-files/vsc-extensions/wix-templates/`.

### `Operational-Evidence/` (17-19 — FACT، جديد على القديم)
`Assembler/B1-Recursion/B2-DHAD-Loop/Compiler/CPU/DAAD/DAAD-DHAD/Electron/FLOAT-RODATA/Integration/MSI/Qt/Studio-Server/Studio-Web/VSCode/WIN64/WINE/` — كل مجلد يحوي مخرجات حقيقية (مثلاً `CPU/suite_summary.txt: الإجمالي 96 ناجح 96 فاشل 0`).

---

## 5. Important Files — أهم الملفات ووظيفة كل ملف

| الملف | الوظيفة (FACT من القراءة) |
|---|---|
| `dhad` (bash 7.1K) | **المنسق الوحيد:** `targets/build/run` + `TargetResolver` (يقرأ `targets.json`) + `driver_cpp/driver_linux_cpp/driver_windows/driver_daad` (validate→compile→link→verify). `.ض+linux-x64-cpp→daad-compiler→g++→ELF`، `.ض+windows-x64→MinGW→PE32+`، `.daad+linux-x64→daad→gcc -nostartfiles→ELF`، `.daad+dhad-cpu→daad --target=dhad→dhad_asm→.bin`. يرفض `NOT_IMPLEMENTED` بخروج `3` ولا يزيف ثنائيات. |
| `Docs/Reference/targets.json` + `TARGETS.md` | سجل الأهداف الآلي: 4 فقط `AVAILABLE` (linux-x64/runtime + linux-x64-cpp/runtime + windows-x64/generated + dhad-cpu/runtime)، الباقي `macos/android/ios/arm/riscv/GUI = Not Implemented`. |
| `Docs/Reference/BUILD_PIPELINE.md:4-18` | يوثق أن `dhad` هو المنسق، لا `.ضب` موجودة، `program.ض→C++→g++→ELF ✓` و`program.daad→BACKEND_X86→gcc→ELF ✓`. |
| `docs/LANGUAGE_SPEC.md` + `GRAMMAR_SPEC.md` + `KEYWORDS_REGISTRY.json` | المواصفة الوحيدة المولدة من الكود (`EBNF` من `Lexer.cpp + Parser.cpp + dhad-lexer/parser.js`؛ 116 كلمة مصنفة؛ فروق `0x/علمي/.5/# = JS-only` والثماني `= C++-only` والمشرقية `= غير مدعومة`). |
| `Compiler/src/main.cpp` (130) | CLI `daad-compiler [خيارات] <مصدر>`: banner عربي + parse `-o/-h/-v` + قراءة الملف + `compileAndOutput` (يشتق `.hpp` من `.cpp` ويكتب الاثنين). |
| `Compiler/src/Compiler.cpp` (111) | الواجهة `DaadCompiler::compile(source, headerName)`: Lexer→Parser loop→Optimizer→جمع أسماء الدوال→CodeGen (فصل global/main)→تحذيرات `new/delete`→Sandbox validate→`CompileResult`. |
| `Compiler/include/Daad/*.hpp` (11) | المفردات: Token (~100 نوع) / Lexer (UTF-8 + `؛/،`) / Keywords (~90 في 9 مجموعات) / AST (691: 17 تعبيراً + ~30 عبارة + 20 عقدة صور) / Parser / CodeGen (C++20) / Optimizer (طي ثوابت فقط) / Diagnostics (عربي) / Sandbox / Unicode. `AST.cpp/Token.cpp` شمان header-only مقصودان. |
| `Compiler/CMakeLists.txt` + `src/CMakeLists.txt` + `Makefile` | بناء `DaadCore` static + `daad-compiler→bin/` + GTest fetch + خيارا Qt OFF + CPack NSIS؛ وبناء سريع `g++ -std=c++20 -O2`. هدفه `test` يشير لمسار غير موجود (انظر §16). `add_subdirectory(tests)` مع `ON` افتراضياً سيفشل (لا `tests/`). |
| `DAAD/compiler/main.c` (293) | CLI `daad <source.daad> [-o] [--ast] [--ir] [--target=x86\|dhad]`: read_file + Lexer→Parser (بوابة)→Semantic (بوابة Phase1)→IR→Optimizer→إعادة تسمية `الرئيسية→main` (x86 فقط)→كتابة `.s` (DHAD: header+`_start`+strings؛ x86: module+helpers+`_start: callq main; syscall 60`). |
| `DAAD/CMakeLists.txt` (55) | C99 + warnings-as-errors + glob المراحل كـ `daad_lib` static + `daad` executable + test لكل `test_*.c` + CTest + `libm`. |
| `DAAD/compiler/{token,lexer,parser,ast,semantic,ir,optimizer,cfg,ssa,codegen,backend}/` | المراحل (التفصيل §6)؛ `backend_interface.h` يعلن 4 أهداف لكن CLI يكشف 2 فقط (ARM/RISCV stubs بـ NULL). |
| `DAAD/e2e_runner.ps1` + `e2e_batch2.ps1` | توليد `.daad` بترميز UTF-8 ثم `daad.exe -o .s` ثم `gcc .s -o .exe` ثم مقارنة exit-code (مسارات `C:\Projects\DAAD` صلبة). |
| `CPU/src/dhad_cpu.c` (647) + `include/dhad_cpu.h` (250) + `dhad_memory_map.h` (131) | النواة الموحدة v4.0: `DhadCPU{regs[8]+acc+flags(Z,N,C)+pc16+sp+mem64K+stack64+bus+callbacks}` + `init/reset/load/step/run`. opcodes قاعدة `0x0-0xF` + موسعة بعد `0xF`. |
| `CPU/src/dhad_asm.c` (704) + `include/dhad_asm.h` (62) | مجمّع two-pass: سجلات عربية `س0-س7/مح` + إنكليزية، فواصل عربية، labels/equates/macros (64×32)/`.if` (32)/directives + تعبيرات `label±offset`. |
| `CPU/src/asm_main.c` (29) | CLI `dhad_asm <file.ضasm> [output.bin]`. |
| `CPU/src/main.c` (1634) | CLI `dhad_cpu <file.ضasm\|.bin> [--debug/--hex/--state/--bin --out]` — **FACT من تعليق Makefile:** يحوي نسخة CPU+ASM مضمنة خاصة ولا يربط `dhad_cpu.c` (ازدواج مثبت). |
| `CPU/Makefile` (72) + `dhad_gui.pro` (15) | الأول يبني الثلاثة بشكل صحيح؛ الثاني يحذف `dhad_cpu.c` (قديم). |
| `CPU/run_tests.sh` (346) | ~96-110 حالة T1-T17 (تفصيل §11). `Scripts/run_tests.sh` نسخة مكررة تعمل فقط إذا كان CWD هو CPU/. |
| `Studio/Web/js/dhad-lexer.js` (704) + `dhad-parser.js` (1741) + `dhad-ast.js` (639) + `dhad-codegen.js` (1130) + `dhad.js` (264) + `dhad-libraries.js` (2765) + `dhad-images.js` (512) + `dhad-highlight.js` (280) + `dhad-editor-page.js` (1897) | سلسلة المتصفح: tokenizer UTF-16 (103-116 كلمة) → recursive-descent (32 عقدة، رسائل عربية) → AST → مولد JS (collect ثم emit) → منسق `compile()` مع blocklist (22 نمطاً خطيراً) + مكتبات (10 فئات/600+ دالة) + صور canvas (22 صيغة). |
| `Studio/desktop-app/server/src/index.js` (434) | دخول الـ API (JWT auto-regenerate، routes، middlewares، Prisma، Redis، Swagger، /health، cluster). |
| `Studio/desktop-app/package.json` + `server/package.json` | الأول Electron NSIS؛ الثاني API الكامل (scripts start/cluster/dev/prisma/jest/pm2/docker). |
| `Studio/VSCode-Extension/src/extension.ts` (26) | أمر واحد `daad.compileCurrentFile` (F9) يفتح terminal ويرسل `daad-compiler "file" -o "temp_daad.cpp"` — يتطلب المترجم في PATH، بلا LSP. |
| `Studio/Dockerfile` (80) + `docker-compose.yml` (165) + `vercel.json` + `railway.json` | إنتاج multi-stage non-root + خدمات api/redis/ollama/nginx + استضافة أمامية static + نشر Railway — لكن الثلاثة تشير لمسارات غير موجودة من الجذر (انظر §16). |
| `Tests/run_all_tests.sh` (612) | الموحد الشكلي: 6 أقسام (تفصيل §11) + `ROOT` مثبت غير محمول. عنوانه `تغطية 100% حقيقية` مضلل (S2 syntax-only). |
| `Tests/real_regression.sh` (149) | **الحقيقي:** BUILD→EXECUTE→VERIFY (Compiler 6 + DAAD int/float/bounds + DAAD→DHAD→CPU + dhad driver + CPU suite fail=0). يبني الثنائيات إن غابت (`-DDAAD_BUILD_TESTS=OFF` الصحيح). |
| `Demo/run-demo.sh` + `loop5.daad` + `api-demo.js` | العرض: يتطلب الثنائيات الثلاثة، ثم `01_hello→cpp→g++→run` + `factorial.daad→s→gcc→exit 120` + `loop5.daad--target=dhad→dhad_cpu→15` + `server :3000/health`. `loop5`: `م=0 ع=1 طالما(ع<=5) م=م+ع` → 15. |
| `.github/workflows/ci-cd.yml` (73) | 3 jobs (تفصيل §12) — 1 صالح + 2 مكسوران. |
| `Examples/01_hello.ض` (4) + `DAAD/hello.daad` + `CPU/tests/full_T1a_add.ضasm` (5) | الحدود الدنيا لكل لغة (انظر §7). |
| `Operational-Evidence/*/suite_summary.txt` + `release/SHA256SUMS.txt` (332) | أدلة التشغيل الوحيدة القابلة للاستشهاد (96/96 CPU + 332 hash). |

---

## 6. Architecture — شرح Architecture الفعلية

**FACT:** لا توجد معمارية واحدة؛ توجد **4 معماريات متوازية** + طبقتا تنسيق/أدلة لا تستدعيهما المعماريات نفسها:

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
│  desktop-app (Electron offline bundle) + VSCode shim│
│  + Full/Qt/QML native track                        │
└────────────────────────────────────────────────────┘
┌─ Layer E: Orchestration (NEW vs old report) ───────┐
│ dhad (bash) → targets.json → drivers → toolchains  │
│ Tests/real_regression.sh → BUILD→EXECUTE→VERIFY    │
│ Demo/run-demo.sh → 4-scene smoke                   │
└────────────────────────────────────────────────────┘
┌─ Layer F: Spec & Evidence (NEW) ───────────────────┐
│ docs/ (spec واحدة) + Operational-Evidence/ (proofs) │
│ + release/ (artifacts + SHA)                       │
└────────────────────────────────────────────────────┘
```

- **Track A تفصيلاً (FACT من `Compiler.cpp` + الهيدرات):** `Lexer(string_view)` ينتج `Token{text,line,col}`؛ `Parser(Lexer&,Diag)` توجيه نصي (`صحيح→VarDecl`، `إذا→If`، `طالما/بينما→While`، `لكل/كرر→ForEachOrFor`، `افعل→DoWhile`، `اختر→Switch`، `دالة→Func`، `صنف/فئة→Class`، `انتقل` مرفوض بخطأ)؛ `AST.hpp` الأضخم (691)؛ `Optimizer` يطوي `Number op Number` فقط؛ `CodeGen` يفصل `isGlobalDeclaration()` عن جسم `main` المخلّق ويحقن includes ويتتبع `new/delete`؛ `SandboxValidator` قائمة بيضاء + regex + heuristics.
- **Track B تفصيلاً (FACT من `main.c` + الهيدرات):** `TokenType` ~104؛ `Lexer` بايت-wise مع `buffer[1024]/errors[100]`؛ `Parser` Pratt؛ `SemanticContext{ScopeStack,TypeRegistry,TypeChecker,ErrorList,in_loop,return_type}` مع `رقم→int/عشري→float/حرف→char/منطق→bool/فراغ→void/نص→string`؛ `IRBuilder{var_hash[128],loop_stack[32],struct_fields[64]}`؛ `Optimizer` 11 تمريرة؛ `Backend{target,name,emit_*}` + SysV + DHAD (S0-S5 alloc، S6 scratch، S7 FP، إطار `0xE000` 256B، ACC للـ ALU). ARM/RISCV مُعلنان بـ NULL.
- **Track C تفصيلاً (FACT من `dhad_cpu.h/memory_map.h/isa_constants.h`):** خريطة `0x0000-0xEFFF برنامج/RAM` + `0xF000-0xF0FF MMIO` + `0xF100-0xF1FF IVT` + `0xF200-0xFFFF محجوز`؛ أجهزة `Display/Keyboard/Timer/GPIO/Serial/INTCTL`؛ أعلام Z/N/C فقط؛ `MAX_CYCLES 100000`؛ حدود `ASM_PROG_MAX 0xF000/LABELS 256/MACROS 64/IF 32`. المرجع الوحيد الموثوق هو `dhad_cpu.h+memory_map.h+isa_constants.h (8-bit v4.0)` — أما `ARCHITECTURE.md/REPORT.md/dhad_isa.h` فتصف 4-بت منتهية.
- **Track D تفصيلاً (FACT):** أمامي بلا بناء؛ خلفي `routes/` 13 + `middlewares/` 7 + `controllers/` 11 + `services/` 2 + `utils/` (منها `dhad/{lexer,parser,ast,codegen}` نسخة سيرفر + `dhadSandbox` بحدود 5s/64KB/50KB/32MB)؛ `desktop-app/main.js` صحيح (القديم `Electron/main.js` غير موجود أصلاً)؛ `Full/frontend` مسار Qt منفصل.
- **Layer E تفصيلاً (FACT — جديد):** `dhad` لا يحوي منطق ترجمة؛ كل هدف دالة `driver_*` (validate→compile→link→verify) تفشل بسبب واضح ولا تزيف مخرجات. `real_regression.sh` يبني الثنائيات إن غابت بالعلم الصحيح ثم يتحقق من exit-codes ومخرجات exact-bits. `Demo/run-demo.sh` نسخة تفاعلية مختصرة منه + خادم.
- **Layer F تفصيلاً (FACT — جديد):** `docs/` generated-from-code (يثبتها `GRAMMAR_SPEC.md:7-44` بالإحالة لأسطر الكود)، و`targets.json` machine-readable يعكس `TARGETS.md`، و`Operational-Evidence/` proofs مصنفة، و`release/SHA256SUMS.txt` يغطي 332 ملفاً.

**INFERENCE:** التقاطع الوحيد المثبت بين المسارات التنفيذية هو **اتجاه DAAD→CPU**: `main.c:132-144` يولد `_start` المتوافق مع `dhad_cpu` (دخول `0x0000`)، وتقارير Phase12.5-F توثقه، و`real_regression [4]` و`Demo loop5` يمارسانه حياً. لا يوجد استدعاء مثبت `Compiler→CPU` أو `Studio→binaries` الأصلية (السيرفر يستخدم نسخته JS). `dhad` هو أول جسر إجرائي (ليس معمارياً) بين الثلاثة.

---

## 7. Execution Flow — كيف يعمل البرنامج من البداية للنهاية

### الموحد — `./dhad` (FACT — `dhad:1-60` + drivers)
```text
$ ./dhad targets                        # يسرد targets.json مع status/proof
$ ./dhad build prog.ض --target=linux-x64-cpp [-o out]
$ ./dhad run prog.daad --target=dhad-cpu
  → TargetResolver (python3 JSON) → رفض NOT_IMPLEMENTED بخروج 3
  → driver_cpp / driver_daad (validate→compile→link→verify)
  → لا 0-byte binaries (فشل بصوت عالٍ)
```

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
  → $ g++ out.cpp -o out && ./out   (يدوي، خارج الأداة؛ أو عبر dhad)
```
- **مثال مثبت (FACT):** `Examples/01_hello.ض` (4 أسطر): `طباعة("مرحبا بالعالم!")؛` — يتحقق منه `real_regression.sh:39` بتوقع `مرحبا بالعالم!`.
- **مثال نحوي (FACT):** `صحيح س = 10 ؛` + `صحيح مجموع(صحيح أ ، صحيح ب){ارجع أ+ب؛}` + `كرر/بينما/إذا/وإلا/فئة/نقطة.س`.

### Track B — `daad` (FACT — `main.c` 293 سطراً)
```text
$ daad hello.daad -o hello.s [--ast] [--ir] [--target=x86|dhad]
  → lexer_create→lexer_tokenize → parser_create→parser_parse (بوابة)
  → semantic_context_create→semantic_analyze (بوابة Phase1)
  → ir_builder_create→ir_builder_build → optimizer (كل الدوال)
  → الرئيسية→main (x86 فقط)
  → fopen(out,"w") → backend_emit_module + helpers + _start → "Generated: ... (N functions, target=...)"
  → $ gcc -nostartfiles hello.s -o hello && ./hello; echo $?   (خارج الأداة، تثبته e2e/*.ps1 و real_regression)
```
- **مثال مثبت (FACT):** `DAAD/hello.daad`: `دالة: الرئيسية() -> رقم :- ارجع(42) نهاية` و`hello.s` يحوي `main: push %rbp; mov $42,%rax; ret` + `_start: callq main; syscall`.

### Track C — `dhad_asm` + `dhad_cpu` (FACT — `asm_main.c` + `Makefile` + `main.c:1376-1634`)
```text
$ dhad_asm prog.ضasm prog.bin → "Assembled: prog.ضasm (N bytes)"
$ dhad_cpu prog.ضasm [--hex] [--state] [--bin --out f.bin] [--debug/--debugger]
  → كشف الامتداد (.ضasm→تجميع داخلي، .bin→تحميل مباشر)
  → memcpy program→memory[0x0000] → run(max 100000)/step/debugger
  → طباعة النتيجة (ACC) / الحالة (متوقف/يعمل) / --hex/--state/--debug
```
- **مثال مثبت (FACT):** `CPU/tests/full_T1a_add.ضasm`: `حمّل مح، 10 / حمّل س0، 20 / جمع س0 / توقف` — المتوقع `ACC=30، الحالة=متوقف`.

### Track D — Studio (FACT)
```text
المتصفح: Web/index.html → login/register → pages/* (teacher/student dashboards)
  → dhad-editor/web-editor → DhadLexer→DhadParser→DhadAST→DhadCodeGen (داخل المتصفح)
  → smart-evaluator/dhadSandbox VM → عرض الناتج
  → api.js (cookies + refresh) ⇄ Express :3000 (/auth/students/challenges/analytics/...)
  → Prisma (SQLite dev / Postgres prod) + Redis + JWT + Swagger + /health
سطح المكتب: desktop-app/main.js → spawn server/src/index.js → انتظار /health → splash→app
  (حزمة offline: server/** + frontend-web/**)
VSCode: فتح .ض → F9 → terminal: daad-compiler "file" -o "temp_daad.cpp"
Docker: deps→builder→production (daaduser) → :3000/health
```

### العرض — `Demo/run-demo.sh` (FACT)
```text
$ bash Demo/run-demo.sh
  → يتطلب Compiler/build/bin/daad-compiler + DAAD/build/daad + CPU/dhad_cpu
  → 01_hello→cpp→g++→run + factorial.daad→s→gcc→exit 120 + loop5.daad--target=dhad→dhad_cpu→15
  → server/src/index.js :3000/health + api-demo.js
  (الدليل: لينكس 4/4 خضراء، ويندوز يحتاج تجربة على جهاز العرض)
```

---

## 8. Data Flow — كيف تنتقل البيانات

- **A (FACT):** `string UTF-8` → `codepoints+byteOffsets (UnicodeUtils)` → `Token{text,line,col}` (`؛→;`، `،→,`، `// /* */`، أرقام/سلاسل مع escapes وثماني، معرفات عربية) → `KeywordRegistry (نص عربي→KeywordType)` → `AST nodes (unique_ptr)` → `C++ strings (source+header)` → ملفان. الأخطاء `DiagnosticsEngine{severity,line,col,message,sourceLine}` عربية.
- **B (FACT):** `bytes .daad` → `Token[] (count)` → `NODE_PROGRAM` → `SemanticContext (scopes+types+errors)` → `IRModule{functions{blocks{instructions{opcode,operands}}}}` → `optimized IR` → `text .s` → `ELF via gcc -nostartfiles` → `exit-code (rax→rdi→syscall 60)`. السلاسل عبر `dhad_emit_module_strings` قبل الدوال. العوائم عبر `%xmm`/RODATA (انظر `FLOAT-RODATA` evidence).
- **C (FACT):** `UTF-8 .ضasm` → `DhadAsm{labels[256]/equates[256]/macros[64]/if_stack[32]/program[0xF000]}` (two-pass) → `bytes` → `memory[65536]` → `regs/flags/pc/sp/output[4096]` → `stdout (النتيجة/الحالة)` أو callback GUI أو MMIO (`F00x/F01x/F02x`) أو `.bin`.
- **D (FACT):** `form/API JSON` → `Express routes→middlewares(auth/security/csrf/cache)→controllers→services/prisma` → `SQLite/Postgres + Redis sessions` → `cookies JWT (access 1h/refresh 7d)` → `frontend guards (TEACHER/STUDENT/SHARED)`. كود التحديات عبر `codeVerifier/serverEvaluator/dhadSandbox` بحدود زمن/ذاكرة.
- **E (FACT — جديد):** `CLI args → targets.json status/proof → driver → toolchain stdout → file artifact → verify (size>0 + exit-code/output match)`. لا توجد بنية وسيطة رسمية (التعليق يقول `NOT a formal IR`).

---

## 9. Dependency Map — العلاقات بين المكونات

```text
ROOT (منسق نحيف dhad + spec docs/ + proofs Operational-Evidence/)
├── dhad ──reads──→ Docs/Reference/targets.json ──drives──→ {daad-compiler, daad, dhad_asm, gcc/g++/MinGW}
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
│    ├── Web (static) ← js/dhad-{lexer,parser,ast,codegen,libraries,images,highlight,editor} [self-contained]
│    ├── server ← routes→controllers→services→prisma/redis + utils/dhad/* (نسخة JS) [weak coupling مع Web عبر HTTP فقط]
│    ├── desktop-app ← server/** + frontend-web/** (bundle) [strong packaging coupling]
│    ├── VSCode-Extension → daad-compiler (خارجي في PATH) [external runtime dep]
│    └── Full/ (Qt/C++ track) [منفصل، يكرر compiler/server]
│
├── Demo/ ──invokes──→ {Compiler, DAAD, CPU, server} (smoke حي)
├── Tests/real_regression.sh ──builds+verifies──→ {Compiler, DAAD, CPU, dhad}
├── Tests/run_all_tests.sh ──syntax-checks──→ {CPU, Compiler, Web, Electron, VSCode, Server}
├── release/ (artifacts: 2×MSI + vsix + SHA332 + Source + Examples)
├── Operational-Evidence/ (proofs لكل مكون)
└── Shared/global state
     ├── لا توجد حالة globale بين المسارات الأربعة (معزولة كلياً)
     ├── داخل كل مسار: Diagnostics/SemanticContext/DhadCPU/DhadAsm/ApiClient state محلية
     └── أدوات مشتركة حقيقية: الأمثلة (.ض/.daad/.ضasm) + docs/ + dhad (فقط ملفات، لا كود مشترك)
```

- **Circular dependencies (FACT):** لا توجد حلقات مثبتة داخل أي مسار (التدفق أحادي). التكرار الحقيقي هو **نسخ متوازية** لا حلقات: `Compiler/src` مقابل `Studio/Full/compiler/src`؛ `Web/js` مقابل `desktop-app/frontend-web/js` مقابل `Full/frontend-web/js` مقابل `server/src/utils/dhad`؛ `CPU/src` مقابل `cpu-original/` (محذوفة؟ غير موجودة الآن — كانت في القديم)؛ `Scripts/run_tests.sh` مقابل `CPU/run_tests.sh`؛ `Docs/Specs/` مقابل `docs/`.
- **External dependencies (FACT):** STL/C-lib فقط للمترجمات؛ `node_modules` للويب/سيرفر؛ Qt/Logisim/GCC/MinGW/NSIS/WiX/wixl/MSYS2/WINE خارجية.

---

## 10. Core Components — شرح المكونات الأساسية

### المنسق `dhad` + `targets.json` (جديد — FACT)
- **الوظيفة:** dispatcher نحيف، ليس مترجماً. `TargetResolver` يقرأ `targets.json` عبر python3، و`driver_*` تنفذ validate→compile→link→verify. يرفض غير المدعوم بخروج `3` ولا يزيف مخرجات (`no fake binary`).
- **الحالة:** COMPLETE كنحيف (7.1K)، لكنه يعتمد على `python3 + gcc/g++` وثنائيات مبنية مسبقاً أو يبنيها `real_regression`.

### المواصفة `docs/` (جديد — FACT)
- **الوظيفة:** `LANGUAGE_SPEC 1.0-factual` + `GRAMMAR_SPEC EBNF` + `KEYWORDS_REGISTRY 116` + `SEMANTIC/STDLIB` — مولدة من الكود (إحالات `ملف:سطر`).
- **الحالة:** COMPLETE كوثيقة؛ INFERENCE: تحتاج اختبار تزامن آلي مع الكود وإلا ستتقادم مثل سابقاتها.

### Compiler — المترجم العالي (C++20)
- **Lexer (373 cpp + 37 hpp) — FACT:** `string_view→codepoints`، `save/restoreState`، `scanNumber/String/Identifier`، `؛/،`، تعليقات، سلاسل مع escapes. **INFERENCE:** كافٍ للأمثلة؛ سلوك المشرقية `١٢٣` UNKNOWN لهذا المسار (موثق `غير مدعومة` في `GRAMMAR_SPEC`).
- **Keywords (~90 في 9 مجموعات) — FACT:** أنواع (`صحيح→int`)، تحكم (`إذا/كرر/لكل/بينما/طالما`)، IO (`طباعة→daad_print`)، GUI (14 `زر_أمر→QPushButton`)، صور (20 `حمّل_صورة→daad::image::load`). **INFERENCE:** القاموس أوسع من CodeGen الفعلي.
- **Parser (1926 — الأكبر) — FACT:** recursive-descent بتوجيه نصي + `زد/انقص` sugar + رفض `انتقل/GOTO` + مسار مزدوج لكلمات الصور.
- **AST (691) — FACT:** 17 تعبيراً + ~30 عبارة + صور؛ `Struct` اسم فقط، `Constructor` معلن لا يُبنى.
- **Optimizer (105) — FACT:** طي `Number op Number` فقط؛ معظم الزوار no-op. **PARTIAL.**
- **CodeGen (901) — FACT:** فصل global/main + `mapType` + sanitize/escape + تتبع leak + ترويسة `DaadRuntime/stdlib/Math/DaadStdlib/iostream/...`.
- **Sandbox (247) — FACT:** whitelist/regex/heuristics سطحية (عد نصي). **PARTIAL.**

### DAAD — المترجم المنخفض (C99)
- **Lexer (1077) + Token (~104) + Parser (1337 Pratt) + AST — FACT:** واجهة عربية كاملة + `دالة: name(ن: رقم) -> رقم :- ... نهاية` + ذاكرة (`حمل/خزن/ادفع/اسحب`) + `اطبع/أدخل` + `حاول/امسك/أطلق`.
- **Semantic (20 ملفاً) — FACT:** زوار 20+ + registry/checker/scope/symbol/folding + بوابة أخطاء صارمة.
- **IR + Optimizer (11) + CFG + SSA (191) — FACT:** بنية حقيقية؛ SSA فيه `insert_phi/rename` لكن `main.c` لا يستدعيه صراحة.
- **Backend (1431 + 1494) — FACT:** x86 SysV كامل + DHAD (إطار `0xE000`) + مُنشئا ARM/RISCV بـ NULL (Stub).

### CPU — المحاكي (C11/Qt)
- **النواة الموحدة — FACT:** 8-بت/16-عنوان/64KB/8 سجلات+ACC/أعلام ZNC/ستاك 64 منفصل/MMIO/IVT/interrupts/callbacks.
- **المجمّع — FACT:** two-pass عربي/إنكليزي + directives + macros + conditional.
- **الواجهات — FACT:** 4 `main` (cpu/asm/visual/live) + Qt IDE (1156) + Logisim (circ مولد).
- **العتاد — FACT:** `dhad_cpu.circ` مولد من `build_dhad_cpu.py` + specs + vectors (10 مفقودة).

### Studio — المنصة (JS/TS)
- **Web toolchain (9 ملفات، ~8000 سطر) — FACT:** Lexer 103-116 كلمة + Parser 32 عقدة + CodeGen JS + sandbox VM + Libraries (10 فئات/600+ دالة) + Images canvas (22 صيغة) + Highlight + Editor-page.
- **Server — FACT:** 13 route + 7 middlewares + 11 controller + Prisma/Redis/JWT/Swagger/health/cluster/PM2.
- **desktop-app — FACT:** Electron يغلّف السيرفر + الواجهة offline (splash + spawn + CSP).
- **VSCode — FACT:** shim نحيف (26 سطراً) بلا LSP/bundled compiler.
- **Full/Qt — FACT:** مسار أصلي موازٍ (QML 26 + i18n 10 + bridge CSV/XLSX + compiler مكرر).

---

## 11. Tests — حالة الاختبارات

| الحزمة | ماذا يُختبر؟ (FACT) | ماذا لا يُختبر؟ | النوع |
|---|---|---|---|
| `Tests/real_regression.sh` (149) **(فات القديم — الأهم)** | BUILD→EXECUTE→VERIFY حقيقي: [1] Compiler (.ض→cpp→exe→run: مرحبا/15/أنت بالغ/26/120/5050) [2] DAAD int (exit 42/60/1/55/42/120/13 + S001 للسلبي) [3] float (%.6f) [3b] bounds (max32/min32/max64 + L011 للطفح) [4] DAAD→DHAD→CPU (loop15/fact120) [5] dhad driver (targets/build/run/neg mac=3) [6] CPU suite (fail=0). يبني الثنائيات إن غابت بالعلم الصحيح. | Web/Electron/VSCode/Server/Windows-boot (يقفز Web إن غاب node) | real regression (gold standard هنا) |
| `Tests/run_all_tests.sh` (612) | S1 CPU (ثنائيات + ADD 30 + hex/state + asm bin + empty/bad/Arabic regs) S2 Compiler (syntax-only لكل cpp + عد أمثلة) S3 Web (node --check 16 + pages + lexer/parser/codegen/full-pipeline/libraries/evaluator/10K سطر) S4 Electron/Desktop (node --check mains/routes/utils) S5 VSCode (tsc + package.json) S6 Server (عد tests + syntax). `ROOT` مثبت غير محمول. | S2 لا يشغّل المترجم فعلاً؛ S5 يفشل إن غابت typescript | meta-harness (syntax + smoke) — عنوان `تغطية 100% حقيقية` مضلل |
| `CPU/run_tests.sh` (346) | ~96: T1 حساب (6) T2 نقل (3) T3 قفز (5) T4 منطق (7) T5 جديد (4) T6 interrupts (1) T7 assembler (6) T8 recovery (1) T9 edge (10) T10 جديد (11) T11 stdin T12 CLI flags T13 GUI وجود T14 أخطاء T15 extra (6) T16 regressions (8) T17 جديد + 20 legacy. الدليل الحي: `Operational-Evidence/CPU/suite_summary.txt: 96 ناجح 0 فاشل`. | عتاد Logisim مقابل C (vectors ناقصة)، أداء/فوضى، MMIO عميق؛ 17 يتيماً غير منفذ (انظر §16) | shell integration + regression |
| `CPU/tests/` (113) | fixtures `.ضasm` لكل ما سبق + `ldri_*` + `bug_*` + `edge_*` | لا asserts داخل الملفات (التوقع في الشل) | fixtures |
| `DAAD/compiler/tests/` (12 c + exe) | lexer/parser/ast/semantic/codegen/precedence/phase2_fixes/performance/stress/fuzz/expansion | التشغيل الحالي UNKNOWN (LastTest فارغ) | C unit/integration |
| `DAAD/e2e/` (~499) + `*.ps1` | ثلاثيات `.daad→.s→.exe` + مقارنة exit-code | float/string/IO محدودة | e2e عبر gcc (مسارات Windows صلبة) |
| `DAAD/tests/` | 6 cpp (Compiler/Comprehensive/Lexer/Runtime/Optimizer/Stdlib/NewSyntax) + `.ض` + conformance/valid/invalid/runtime/types/memory/abi. `daad test --all` المذكور في README غير موجود في `main.c`. | الأمثلة `*_out.cpp` تستورد `stdlib/` الغائب | gtest-style + fixtures |
| `Compiler/` | **لا يوجد `tests/`** (مثبت `ls` فشل)؛ `Makefile:test` يشير لمسار غير موجود؛ `-DDAAD_BUILD_TESTS=ON` سيفشل | كل شيء | غائب |
| `Studio/desktop-app/server/tests/` (29) | unit (19) + integration (7) + security (2) عبر jest + supertest | تغطية فعلية UNKNOWN بدون تشغيل | jest |
| `Studio/tests/` | pentest/stress/seed/verify/fix + تقارير json | ad-hoc بلا jest config موحد | manual/security/stress |
| `Demo/` | smoke رباعي (hello/factorial/loop5/server) | تغطية ضحلة عمداً (عرض لا اختبار) | demo |
| `Docs/Reports/` (62) + `DAAD/*.md` (14) | توثق 998/617/533/347 assert تاريخية — **INFERENCE:** تشغيلات سابقة لا دليل على تكرارها الآن | الحالة الحالية | تقارير، ليست اختبارات قابلة للتشغيل |
| CI | `daad-compiler` (7 suites + runtime input) صالح شكلياً؛ الآخران مكسوران (§16) | Studio/CPU/Web/Electron/VSCode/Docker | actions |

**الخلاصة (FACT):** المعيار الذهبي الآن هو `real_regression.sh` (حقيقي)، لا `run_all_tests.sh` (شكلي). `Compiler` بلا suite أصلاً. وجود الاختبارات لا يعني نجاحها الآن — التشغيل ممنوع في هذه المرحلة، والدليل الحي الوحيد هو `Operational-Evidence/` بتواريخ سابقة.

---

## 12. Build & Run — كيف يتم بناء وتشغيل المشروع

### الموحد (الجديد — FACT)
```bash
./dhad targets
./dhad build prog.ض --target=linux-x64-cpp [-o out]
./dhad run prog.daad --target=dhad-cpu
bash Tests/real_regression.sh            # يبني الناقص ويتحقق (الموصى به أولاً)
bash Demo/run-demo.sh                   # عرض 5 دقائق (يتطلب الثنائيات + node للخادم)
# ملاحظة: Tests/run_all_tests.sh سطر 20 يثبت ROOT=/home/... (غير محمول)
```

### Compiler (FACT)
```bash
cd Compiler && make            # → build-make/bin/daad-compiler (سريع)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DDAAD_BUILD_TESTS=OFF   # OFF إجباري: لا tests/
cmake --build build -j$(nproc) # → build/bin/daad-compiler
./build/bin/daad-compiler prog.ض -o out.cpp   # → out.cpp + out.hpp
g++ -std=c++20 out.cpp -o out && ./out
```

### DAAD (FACT)
```bash
cd DAAD
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)  # → build/daad + test_* + ctest
./build/daad hello.daad -o hello.s [--ast] [--ir] [--target=x86|dhad]
gcc -nostartfiles hello.s -o hello && ./hello; echo $?
```

### CPU (FACT)
```bash
cd CPU
make            # → dhad_cpu (من main.c+debug.c) + dhad_asm (من asm_main.c+dhad_asm.c)
make gui        # → dhad_gui (يحتاج Qt5 + moc)
./dhad_asm prog.ضasm prog.bin
./dhad_cpu prog.ضasm [--hex] [--state] [--bin --out f.bin] [--debug/--debugger]
./run_tests.sh            # ~96 حالة (يحتاج ./dhad_cpu في نفس المجلد)
```

### Studio (FACT)
```bash
cd Studio/Web && npx serve .              # أو vercel --prod (vercel.json يشير لمسار خاطئ — انظر §16)
cd Studio/desktop-app/server
npm ci && npx prisma generate
npx prisma migrate deploy && node src/index.js        # :3000/health
npm test                              # jest
cd Studio/desktop-app && npm ci && npx electron .    # أو electron-builder --win
cd Studio/VSCode-Extension && npm ci && npx tsc --noEmit
cd Studio && docker build -t daad-studio-server . && docker-compose up -d  # Dockerfile يشير لمسار خاطئ — انظر §16
```

### Releases (FACT)
`release/windows-10-11/DhadStudio-Windows-x64.msi` + `release/windows-7/DhadStudio-Windows7.msi` + `release/DhadStudio-Setup-Win7.msi` (جذر release) + `SHA256SUMS.txt` + `Source/{cpu,Daad-Compiler}` + `Examples/` (20) + `*.vsix` + `validate-*.bat` — لقطات قابلة للتحقق (`wixl` + hash)، لكن `boot/install على ويندوز حقيقي = pending` (أمانة `TARGETS.md` و`Demo/دليل`).

---

## 13. Completed Components — المكونات المكتملة

> **تعريف:** مكتملة وظيفيًا بدليل ملف + منطق + مخرجات مثبتة/تقارير e2e (وليس مجرد وجود اسم).

- **المنسق `dhad` + سجل `targets.json` — COMPLETE كنحيف (FACT):** يوزع build/run على 4 أهداف حقيقية ويرفض الباقي بسبب واضح.
- **المواصفة `docs/` — COMPLETE كوثيقة (FACT):** spec واحدة مولدة من الكود بإحالات سطرية.
- **البوابة الحقيقية `real_regression.sh` — COMPLETE (FACT):** تغطي Compiler + DAAD int/float/bounds + DAAD→DHAD→CPU + driver + CPU suite.
- **العرض `Demo/` — COMPLETE (FACT):** 4 مشاهد حية + دليل عربي + حساب تجريبي.
- **الأدلة `Operational-Evidence/` — COMPLETE كمجلد (FACT):** proofs مصنفة (CPU 96/96 + Compiler outputs + MSI/WINE/...).
- **المسرح `release/` — COMPLETE كلقطة (FACT):** 2×MSI + vsix + SHA332 + Source + Examples + validate scripts.
- **Compiler CLI + Pipeline الأساسي — COMPLETE (FACT):** transpiler إجرائي/OOP بسيط (إثبات: ثنائيتان + أمثلة مولدة + `Examples/*.cpp/.hpp`).
- **DAAD Frontend + x86 Backend — COMPLETE (FACT):** Lexer/Pratt/Semantic-gate/IR/Optimizer/Backend + 499 ثلاثية e2e.
- **DAAD DHAD Target — COMPLETE (FACT):** `dhad_backend.c` (1494) + `_start` + إطار `0xE000` (يثبته `factorial.daad.s` 94 سطراً).
- **CPU Assembler + Emulator Core + CLI — COMPLETE (FACT):** two-pass + 8-بت/64KB + flags + ~96 regression + ثنائيات حاضرة.
- **CPU GUI + Visual + Live — COMPLETE (FACT كبنية):** `gui/main.cpp` + `visual_sim.c` + `dhad_live.cpp` (التشغيل البصري UNKNOWN بدون display).
- **Web In-Browser Toolchain — COMPLETE (FACT كبنية):** Lexer/Parser/AST/CodeGen/Sandbox/evaluator/libraries (التشغيل الكامل UNKNOWN بدون node هنا).
- **Server API Skeleton — COMPLETE (FACT كبنية):** routes/controllers/middlewares/prisma/jest + Docker/Railway.
- **VSCode Grammar + Snippets + F9 — COMPLETE كنحيف shim (FACT).**

---

## 14. Incomplete Components — المكونات الناقصة

- **Compiler `tests/` — MISSING (FACT):** مجلد غائب + `Makefile:test` مكسور + `LastTest.log` فارغ. `real_regression` يغطي 6 حالات فقط — لا شبكة أمان حقيقية.
- **Compiler Optimizer/Sandbox/Struct/Constructor/GOTO — PARTIAL (FACT):** طي ثوابت فقط؛ sandbox عدّ نصي؛ `Struct` اسم فقط؛ `Constructor` لا يُبنى؛ `انتقل` مرفوض صراحة.
- **DAAD Float/String/IO/Exceptions/Alloc — PARTIAL (FACT):** CMP أُصلح لكن float بلا `%xmm` كامل، ولا alloc/IO-strings/try-catch كاملة؛ ARM/RISCV معلنة بلا CLI.
- **DAAD SSA/CFG Integration — PARTIAL (FACT):** كود حقيقي لكن غير موصول من `main`.
- **CPU Hardware Parity — PARTIAL (FACT):** `.circ` مولد + specs، لكن 10 vectors مفقودة (HIGH: multi-byte fetch/CALL-RET/NEG-INC-DEC-wrap) + `DATAPATH_DISCREPANCIES.md` يوثق فروقاً + `CONTROL_UNIT EXECUTE` فارغة.
- **Studio Qt/Full Track — PARTIAL (FACT):** QML/bridge/compiler موجودة لكن تكاملها مع Node-server غير مثبت + `bridge/build/` ملوث بمخرجات `CMakeCache`.
- **النشر الجذري (Docker/Vercel/Railway) — PARTIAL→BROKEN (FACT):** تشير لمسارات غير موجودة (انظر §16.5).
- **Docs/CI/Scripts Portability — PARTIAL (FACT):** توثيق قديم + jobان مكسوران + مسارات مثبتة (`ROOT`، `/run/media/...`، `C:\Projects\DAAD`).

---

## 15. Stubs / Placeholders — جميع الأجزاء الوهمية أو المؤقتة

> **منهجية:** `Grep` مباشر. النتيجة: **نظيف بشكل لافت — 3 مواضع كود فقط + placeholders توثيقية.**

| الموقع (FACT) | النص الحرفي | التصنيف |
|---|---|---|
| `Studio/Web/js/dhad-codegen.js:473` (+ نسخ `frontend-web/server/Full` + `.bak`) | `default: this.line('// TODO: ' + stmt.type)` | **PLACEHOLDER** — fallback لعقد AST غير المعالجة. أي لغة تستخدم تلك العقد ستولد تعليقاً لا كوداً. |
| `DAAD/compiler/codegen/register_allocator.c:221` | `TODO: مخصص نطاقات حياة صحيح داخل الكتلة ثم عطّل هذا الوضع.` | **STUB** — تعطيل مؤقت لنطاقات الحياة الدقيقة (`g_stack_mode=1`). |
| `DAAD/compiler/backend/backend_interface.c:1192` (978 في القديم — تزحزح سطري) | `fprintf(out,"  # unimplemented opcode %s\n",...)` في `default:` | **STUB** — fallback لطيف (تعليق asm) بدل crash. |
| `DAAD/compiler/backend/backend_interface.c:195-217` | `emit=NULL, reg_name=NULL` لـ ARM64/RISCV | **STUB** — هدفان معلنان بلا تنفيذ (`test_codegen` يفحص الاسم فقط). |
| `Compiler/src` + `include` + `CPU/src` + `include` | **صفر تطابق** لـ `TODO\|FIXME\|STUB` (مثبت بـ Grep) | **COMPLETE** بهذا المعيار. |
| `CPU/hardware/tests/DATAPATH_VECTORS.md:349` | `## MISSING/TODO Test Vectors` (10 صفوف) | **PLACEHOLDER** توثيقي. |
| `Studio/desktop-app/server/package.json:28` | `"deploy": "echo 'Deploy script not configured yet'"` | **PLACEHOLDER** سكربت. |
| `Studio/desktop-app/server/src/utils/redis.js:328` | `Redis not configured, running without cache` | **GRACEFUL-DEGRADE** (مقصود، ليس عيباً). |
| ملفات `.bak` (~10 في `Web/js` + `bridge/*.bak`) | نسخ احتياطية بجانب الأصل | **EXPERIMENTAL** — دليل refactoring جارٍ. |
| `Sources-Archive/` + `NUL` + `output.s` المبعثرة + `DAAD/*.exe` الملتزمة | لقطات/مخلفات | **UNUSED** — dead weight، ليست stubs. |

**ما ليس stub (توضيح مهم):** `demo.ضasm` الفارغ و`factorial.ضasm` المسمى خطأ (countdown) هما **BROKEN fixtures** لا stubs؛ و`dhad_isa.h` القديم هو **ORPHAN** (4-بت ميت) لا stub؛ و`include/Daad/*.hpp` في DAAD هو **LEGACY** (واجهة C++ معزولة) لا stub.

---

## 16. Known Issues — المشاكل التي تم اكتشافها (مثبتة فقط)

1. **CI مكسور جزئياً (FACT — `ci-cd.yml:47-73`):** job `studio` و`gate-stdlib` يستخدمان `working-directory: daad-studio/dhad-studio` غير الموجود (`ls` يفشل) ويستهدفان `DaadTests` غير الموجود بهذا المسار. **الأثر:** 2/3 jobs تفشل على checkout نظيف. job `daad-compiler` وحده صالح شكلياً (7 suites + runtime input `011-input.deff` — لاحظ الامتداد `.deff` مقابل `.daad` الفعلي).
2. **Compiler بلا اختبارات (FACT):** لا `Compiler/tests/`؛ `Makefile:19-20` يشير لـ `../build/bin/DaadTests`؛ `CMakeLists:44-48` سيفشل مع `DAAD_BUILD_TESTS=ON` (الافتراضي). **الأثر:** `BUILDING.md:48` (`-DDAAD_BUILD_TESTS=ON` + `ctest`) لا يعمل حرفياً. `real_regression.sh` يتجاوزها بـ `OFF` الصحيح.
3. **`.gitignore` غير مطبق على الموجود (FACT):** `Compiler/build*/` و`*.exe/*.o/*.bin/*.zip/*.db*` محظورة نظرياً لكنها حاضرة على القرص (بما فيها `daad-compiler.exe` و`dev.db*` و`desktop_dhad_*.zip`). **الأثر:** تلوث الشجرة + خطر تسريب أسرار/ثنائيات + تضخم (`Examples 271M + Studio 541M`).
4. **ازدواج النواة في CPU (FACT — تعليق `Makefile:34`):** `dhad_cpu` يُبنى من `main.c` المضمنة لا من `dhad_cpu.c` الموحدة. **الأثر:** إصلاح في واحدة لا يصل للأخرى؛ `run_tests.sh` يختبر المضمنة فقط.
5. **ثوابت متناقضة لحجم الستاك (FACT):** `dhad_cpu.h/memory_map.h=64` مقابل `isa_constants.h=32` مقابل `isa.h=16` مقابل `main.c=32` (وتعليق `dhad_cpu.c:13` يقول 32 بينما يستخدم 64). **الأثر:** اختبار `stack_overflow` قد يمر على نسخة ويفشل على أخرى.
6. **`dhad_gui.pro` قديم (FACT):** يحذف `src/dhad_cpu.c` بينما `Makefile` يشمله. **الأثر:** بناء QtCreator يختلف عن Make.
7. **التقرير القديم نفسه قديم + `PROJECT_INDEX.md` قديم جزئياً (FACT — جديد):** القديم ادعى `الجذر 15 مدخلاً + Examples 75 + Tests ملف واحد + Releases موجودة + Studio/Electron/` بينما المقروء `48 + 72 + ملفان + لا Releases + لا Electron/`. `PROJECT_INDEX.md` يذكر `Releases/ + Program Files/` غير الموجودين وأحجاماً مخالفة (`CPU 11M` مقابل `5.4M` بـ `du -sh`). **الأثر:** أي أتمتة تعتمد عليهما ستفشل. هذا التقرير يصححهما.
8. **توثيق قديم (FACT):** `REPORT.md` + `docs/ARCHITECTURE.md` + `desktop/ARCHITECTURE.md` تصف 4-بت/4096 بينما الكود 8-بت/64KB v4.0؛ `PROJECT_STRUCTURE.md` تصف `frontend-web/server/...` غير المطابقة لـ `Studio/*`؛ `BUILDING.md` تطلب `build.ps1/installer.nsi` غير الموجودين؛ `Docs/Instructions/README.md` أرقام أسطر قديمة؛ `README.md` الجذري سطر واحد `# daad-v-1` (بلا قيمة). **الأثر:** مطور جديد سيتبع مسارات خاطئة. المرجع الصحيح الجديد: `docs/` + `dhad --help` + `Demo/دليل`.
9. **Fixtures مكسورة/يتيمة (FACT):** `CPU/demo.ضasm` فارغ (0 سطر)؛ `CPU/examples/factorial.ضasm` تسمية خاطئة (countdown 5→1)؛ `CPU/tests/test_expr2.dasm` امتداد لاتيني شاذ؛ **17 يتيماً غير منفذ** (`display_test/edge_* (10)/inc_data/test_expr/...`)؛ `full_T10c_reti + full_T10j_error_msg` موجودان لكن `run_tests.sh:174-186` يتخطاهما. **الأثر:** أمثلة مضللة + ادعاء تغطية ناقص.
10. **تكرار الملفات (FACT):** `Scripts/run_tests.sh` ≡ `CPU/run_tests.sh`؛ `Web/js` ≡ `desktop-app/frontend-web/js` ≡ `Full/frontend-web/js` (مثلث 62 ملفاً بانحراف 8) + `server/src/utils/dhad` نسخة رابعة؛ `Compiler/src` ≡ `Full/compiler/src` (افتراق 5 ملفات)؛ `README-ويندوز.md` مكرر؛ `Docs/Specs/` ≡ `docs/` (+ ملف زائد). **الأثر:** انحراف صامت + تضخم.
11. **`.bak` و`NUL` و`output.s` مبعثرة (FACT):** ~10 bak + `DAAD/NUL` (بقايا Windows) + `.s/.exe` في جذر DAAD رغم `.gitignore`. **الأثر:** التباس أي مخرج هو "الحالي".
12. **VSCode يعتمد على خارجي (FACT — `extension.ts:20`):** `daad-compiler` يجب أن يكون في PATH ولا يُحزم مع الإضافة ولا يوجد LSP/diagnostics. **الأثر:** F9 يفشل على تثبيت نظيف.
13. **حدود Sandbox سطحية (FACT):** `SandboxValidator` عدّ نصي + regex؛ `dhad.js` blocklist نصية (22 نمطاً). **الأثر:** ليست ضماناً أمنياً (انظر §17).
14. **تجزئة اللغة (FACT):** 4 امتدادات (`.ض/.daad/.ضasm/.deff`) و4 نحويات (`صحيح س=10؛` مقابل `دالة: f()->رقم :-` مقابل `حمّل مح 5` مقابل `011-input.deff`) بلا جدول موحد — `docs/` بدأت الحل (تفصل `.ض` عن `.daad`) لكن الجدول الشامل مفقود.
15. **مسارات نشر جذرية مكسورة (FACT — جديد، فحص `ls`):** `Studio/vercel.json:4-9` يشير لـ `frontend-web/**` (لا يوجد `Studio/frontend-web`)؛ `Studio/railway.json:4-6` يشير لـ `server/Dockerfile` (لا يوجد `Studio/server`)؛ `Studio/Dockerfile:14-15,60-61` يفعل `COPY package.json prisma src` من الجذر (هي داخل `desktop-app/server/`)؛ `docker-compose.yml:139-140` يعلق `nginx/nginx.conf` (لا مجلد `nginx/`). **الأثر:** النشر من `Studio/` كسياق سيفشل؛ السياق الصحيح `desktop-app/server` و`Web`.
16. **سكربتات ميتة بمسارات شخصية (FACT):** `Scripts/build_demos.sh:3-8` و`winbuild.sh:1-10` يثبتان `BASE="/run/media/m_hmoz/UBUNTU 26_0/..."`؛ `Tests/run_all_tests.sh:20` يثبت `ROOT=/home/...`؛ `DAAD/e2e_*.ps1` تثبت `C:\Projects\DAAD`؛ `dhad-libraries.js:4` يعلق `Based on C:\Projects\DaadStudio\stdlib`. **الأثر:** تعمل على جهاز المؤلف فقط.
17. **`Docs/` مقابل `docs/` يتصادمان على ويندوز (FACT):** الاسمان يختلفان بحساسية الحالة فقط — `PROJECT_INDEX.md:48` يقر أنهما "تكميل لا تكرار" (أرشيف 25M مقابل spec 304K)، لكن Windows سيصهرهما. `Docs/Specs/` نسخة شبه مطابقة من `docs/` تزيد الالتباس.

**ما لم يُدَّعَ:** لا ادعاء بتسرب أسرار (`.env` محظور وغير مقروء هنا)، ولا بكسر بناء (البناء لم يُشغَّل ضمن هذه المرحلة)، ولا بثغرة runtime محددة.

---

## 17. Technical Debt — الدين التقني الواضح

1. **توحيد مفقود:** 4 سلاسل أدوات + 4 نسخ JS + `Docs/Specs` مكررة — **الدين:** أي إصلاح لغوي يجب تكراره 3-4 مرات. **المقترح (تسجيل فقط):** جدول مواصفة واحد (`docs/` بدأت) + مولد واحد + حزم مشتركة + حذف `Docs/Specs/`.
2. **اختبارات Compiler غائبة:** **الدين:** لا شبكة أمان للمسار الأكثر استخداماً تعليمياً (6 حالات `real_regression` لا تكفي). **المقترح:** استعادة `tests/` أو توثيق غيابها رسمياً وتصحيح `Makefile:test` و`BUILDING.md`.
3. **النواة المزدوجة + الثوابت المثلثة:** **الدين:** `main.c` مقابل `dhad_cpu.c` + 64/32/16 + `V` المحذوف/الموجود. **المقترح:** حذف المضمنة أو توليدها من الموحدة + ثابت واحد `STACK_SIZE` + حسم علم `V`.
4. **التوثيق كأرشيف لا كمرجع:** 62 + 14 + 27 تقريراً تروي تاريخاً لا حالة. **الدين:** `PROJECT_STRUCTURE/BUILDING/ARCHITECTURE/REPORT/PROJECT_INDEX` مضللة جزئياً. **المقترح:** تجميد القديم تحت `Docs/Archive/` + صفحة واحدة حالية (هذا التقرير + `docs/` + `dhad targets`).
5. **الأمان بالheuristics:** sandbox عدّ نصي + pentest ad-hoc بلا بوابة + `HMAC secret not configured → 500`. **الدين:** غير صالح للإنتاج المدرسي الحقيقي. **المقترح:** VM حدود صارمة + إدارة أسرار (`assertStrongSecrets`) + بوابة `npm test` في CI.
6. **البناء غير المتكرر:** `build*/node_modules/.exe/.db` ملوثة + `winbuild.sh` بمسار شخصي + CI مكسور + `ROOT` مثبت. **الدين:** "يعمل على جهازي". **المقترح:** تنظيف + `SCRIPT_DIR` نسبي + إصلاح `ci-cd.yml` ليشير لـ `Compiler` و`Studio/desktop-app/server` و`Studio/Web` الحقيقية.
7. **التغليف المزدوج/المثلث:** NSIS (CMake) مقابل WiX (`wxs` + MSI بـ `wixl`) مقابل electron-builder + تضارب `Docs/ vs docs/` على ويندوز. **المقترح:** اختيار واحد وتوثيقه + توحيد الاسم.
8. **النشر الجذري المكسور:** Dockerfile/vercel/railway بمسارات خاطئة. **المقترح:** توحيد السياق إلى `desktop-app/server` و`Web` واختباره في CI.

---

## 18. Unknowns — الأشياء التي لم يمكن إثباتها

- **هل يبني كل شيء الآن؟ UNKNOWN:** البناء لم يُشغَّل (ممنوع التثبيت/الأوامر الخطرة في هذه المرحلة)؛ الثنائيات الحاضرة بتواريخ Aug/Sep 2026 توحي بنجاح سابق لا حالي. الاستثناء: `real_regression.sh` مصمم ليبني الناقص تلقائياً — لكنه لم يُشغَّل هنا.
- **هل تنجح الاختبارات الآن؟ UNKNOWN:** `LastTest.log` فارغ + `run_all_tests.sh` smoke فقط + تقارير الـ 998/617 assert تاريخية + `suite_summary.txt` (96/96) بتاريخ سابق.
- **أداء المترجمات (10K سطر/stress/fuzz)؟ UNKNOWN:** ملفات `test_stress/test_fuzz/test_performance` موجودة لكن نتائجها في تقارير لا سجلات حية.
- **سلوك الأرقام المشرقية `١٢٣` في Track A؟ UNKNOWN جزئياً:** `GRAMMAR_SPEC` تدعي `غير مدعومة في الطرفين` — لكن `UnicodeUtils` في C++ لم يُفحص سطرياً هنا.
- **تكامل Qt/QML مع السيرفر؟ UNKNOWN:** الملفات موجودة لكن لا استدعاء مثبت.
- **مخطط DB الكامل وصلاحيات المدارس؟ UNKNOWN:** `schema.prisma` لم يُقرأ سطرياً (خارج عينة القراءة العميقة؛ التقرير اعتمد على أسماء controllers + `PROJECT_STRUCTURE.md`).
- **سجل git (من كتب ماذا ومتى)؟ UNKNOWN:** خارج نطاق القراءة المطلوبة.
- **الترخيص الفعلي؟ UNKNOWN جزئياً:** `Compiler/LICENSE` + `Studio/LICENSE.txt` موجودان لكن نصاهما لم يُقرأ؛ `PHASE6_LICENSE_AUDIT.md` موجود للمراجعة اللاحقة.
- **إقلاع ويندوز الحقيقي للـ MSI؟ UNKNOWN (مقر به):** `TARGETS.md` و`Demo/دليل` يصرحان `generated + wixl-verified + boot pending` — أمانة توثيقية، ليست عيباً خفياً.

---

## 19. Final Architecture Map — خريطة نهائية للمشروع

```text
                    ┌──────────────────────────────────────────────┐
                    │  Dhad-Studio-Unified (monorepo, 48 entries)  │
                    │  no root build — dhad (thin) + docs/ (spec)  │
                    │  + Tests/real_regression (gold gate)         │
                    └───────────────────┬──────────────────────────┘
        ┌───────────────┬───────────────┼───────────────┬──────────────┐
        ▼               ▼               ▼               ▼              ▼
 ┌─────────────┐ ┌───────────┐ ┌──────────────┐ ┌──────────────┐ ┌───────────┐
 │ Compiler    │ │ DAAD      │ │ CPU          │ │ Studio       │ │ release/  │
 │ .ض → C++    │ │ .daad→.s  │ │ .ضasm→emu    │ │ EdTech       │ │ 2×MSI+vsix│
 │ C++20       │ │ C99       │ │ C11/Qt/Logis.│ │ JS/TS/Node   │ │ +SHA332   │
 └──────┬──────┘ └─────┬─────┘ └──────┬───────┘ └──────┬───────┘ └───────────┘
        │              │              │                │
   DaadCore       daad_lib      dhad_cpu/       Web◄──►Server
   CLI:daad-      CLI:daad      asm+gui         ▲    (Prisma/
   compiler       (x86|dhad)    CLIs:cpu/        │     Redis/JWT)
   examples       e2e:~499      asm/vis/live     │         │
   stdlib×2       docs:17       tests:113    desktop-app   VSCode
   (no tests)     audits:14     hw:circ       (bundle)   (shim F9)
                                       │         Full/Qt
                                       ▼        (parallel)
                                ┌──────────────┐
                                │ Demo/ (smoke)│
                                │ Tests/ (2)   │
                                │ Docs/ (arch) │
                                │ docs/ (spec) │
                                │ Op-Evidence  │
                                │ CI (1/3 ok)  │
                                └──────────────┘

  Inter-track edges (proven only):
    DAAD --(.s/.ضasm _start @0x0000)--> CPU   [main.c + loop5 + Phase12.5]
    dhad --(drivers)--> {Compiler, DAAD, CPU}  [orchestration, NOT compilation]
    real_regression --(build+verify)--> {Compiler, DAAD, CPU, dhad}
    Demo --(smoke)--> {Compiler, DAAD, CPU, server}
    Examples --(fixtures)--> all tracks
    docs/ --(spec)--> {.ض tracks}   (.daad out of scope by design)
    Studio server --(JS re-impl, NOT native)--> Dhad language
  No edge: Compiler→CPU, Studio→native binaries, Qt→Node-server
  Name collision risk: Docs/ vs docs/ (Windows), .ض vs .daad vs .ضasm vs .deff
```

---

## FINAL VERDICT — ملخص حالة المشروع الحالية

### What works (مثبت بالملفات)
- **المنسق `dhad`:** يوزع build/run على 4 أهداف ويرفض الباقي بسبب واضح. **FACT.**
- **المواصفة `docs/`:** spec واحدة مولدة من الكود بإحالات سطرية (116 كلمة). **FACT.**
- **البوابة `real_regression.sh`:** BUILD→EXECUTE→VERIFY حقيقي (6 Compiler + int/float/bounds + DAAD→DHAD→CPU + driver + CPU suite). **FACT (لم يُشغَّل هنا، لكن منطوقه حقيقي).**
- **العرض `Demo/`:** 4 مشاهد حية + دليل عربي أمين (لينكس 4/4، ويندوز pending). **FACT.**
- **الأدلة `Operational-Evidence/`:** proofs مصنفة (CPU 96/96 + Compiler outputs + MSI/WINE). **FACT.**
- **المسرح `release/`:** 2×MSI (Win7 + Win10-11) + vsix + SHA332 + Source + Examples + validate. **FACT.**
- **Compiler كـ transpiler تعليمي:** CLI + pipeline كاملة + مخرجات `.cpp/.hpp` حقيقية + أمثلة مولدة + ثنائيتان مبنيتان. **FACT.**
- **DAAD كـ native compiler:** frontend صارم + IR/optimizer/backend + ~499 e2e + DHAD target المتوافق مع CPU. **FACT.**
- **CPU كـ teaching emulator:** assembler two-pass + نواة 8-بت + CLI flags + ~96 regression + GUI/visual/live + نموذج Logisim هيكلي. **FACT.**
- **Studio كـ EdTech skeleton:** ويب static + toolchain داخل المتصفح (9 ملفات) + API بخارطة كاملة + desktop-app offline + VSCode F9 + Docker/Railway (ملفات موجودة). **FACT.**

### What does not work (مثبت)
- **CI:** 2/3 jobs بمسارات `daad-studio/dhad-studio` غير الموجودة. **FACT (`ci-cd.yml:47-73`).**
- **`make test` في Compiler + `BUILDING.md` حرفياً:** يشيران لـ `tests/` و`DaadTests` و`build.ps1/installer.nsi` غير الموجودة. **FACT.**
- **النشر من `Studio/` كسياق:** Dockerfile/vercel/railway تشير لمسارات غير موجودة (`frontend-web`/`server`/`prisma`/`nginx`). **FACT (فحص `ls`).**
- **`dhad_gui.pro`:** يبني GUI ناقصاً (بلا `dhad_cpu.c`). **FACT.**
- **Fixtures:** `demo.ضasm` فارغ + `factorial` مسمى خطأ + `.dasm` شاذ + 17 يتيماً غير منفذ + تخطي T10c/T10j. **FACT.**
- **سكربتات المسارات الشخصية:** `build_demos.sh/winbuild.sh` (`/run/media/...`) + `e2e_*.ps1` (`C:\Projects\DAAD`) + `ROOT` مثبت. **FACT.**

### What is incomplete (ناقص لا مكسور)
- Optimizer/Sandbox/Struct/Constructor/GOTO في Compiler؛ Float/String/IO/exceptions/ARM في DAAD؛ SSA/CFG غير موصولة؛ 10 hardware vectors + `EXECUTE` فارغة؛ تكامل Qt؛ `Tests` harness محمول؛ جدول اللغة الموحد (4 امتدادات). **(تفصيل §14 — FACT.)**

### What is stubbed (وهمي/مؤقت — القائمة الكاملة)
- `Web/js/dhad-codegen.js:473` fallback `// TODO: stmt.type` (+نسخها). **PLACEHOLDER — FACT.**
- `DAAD/.../register_allocator.c:221` TODO عربي. **STUB — FACT.**
- `DAAD/.../backend_interface.c:1192` `# unimplemented opcode`. **STUB — FACT.**
- `ARM64/RISCV` بـ NULL. **STUB — FACT.**
- 10 vectors في `DATAPATH_VECTORS.md:349` + `"deploy": "echo 'not configured'"`. **PLACEHOLDER — FACT.**
- **والباقي نظيف:** صفر TODO في `Compiler/src+include` و`CPU/src+include`. **FACT (Grep).**

### What is unknown (يحتاج تشغيلاً)
- نجاح البناء/الاختبارات **الآن**، الأداء/stress/fuzz، سلوك `١٢٣` سطرياً في C++، تكامل Qt، مخطط DB الكامل، سجل git، نصوص الرخص، إقلاع ويندوز الحقيقي (مقر به pending). **(تفصيل §18.)**

### What should be investigated next (بالترتيب، بدون تعديل الآن)
1. **تشغيل آمن للقراءة فقط:** `bash Tests/real_regression.sh` (يبني الناقص بـ `OFF` الصحيح) ثم `bash CPU/run_tests.sh` ثم `node --check Studio/Web/js/dhad-*.js` — لتأكيد §11 قبل أي تغيير.
2. **حسم اللغة:** جدول واحد يربط `.ض/.daad/.ضasm/.deff` بالنحويات والأدوات (يُبنى على `docs/` ويحل §16.14).
3. **حسم المعمارية:** هل تُحذف `main.c` المضمنة أم `dhad_cpu.c`؟ وهل يُحذف `dhad_isa.h` (4-بت) و`Docs/Specs/` المكررة؟ (يحل §16.4-5,10,17).
4. **حسم الاختبارات/CI:** استعادة `Compiler/tests/` أو توثيق غيابها + إصلاح `ci-cd.yml` (يشير لـ `DAAD` + `Compiler` + `Studio/desktop-app/server` + `Studio/Web` الحقيقية) + جعل `ROOT` نسبياً (`SCRIPT_DIR`). (يحل §16.1-2,16).
5. **حسم النشر:** توحيد سياق Docker/Vercel/Railway (`desktop-app/server` + `Web`) واختباره في CI (يحل §16.15).
6. **حسم الأمان/التوثيق:** مراجعة `SECURITY_FIX_PLAN.md` + `schema.prisma` قبل أي نشر مدرسي + تجميد `REPORT/ARCHITECTURE/PROJECT_STRUCTURE/BUILDING/PROJECT_INDEX` القديمة تحت `Docs/Archive/`.

> **قاعدة عدم اللمس:** لا تُمس الأجزاء `Compiler/src/Parser.cpp` (1926) و`DAAD/compiler/backend/*` (2700+) و`CPU/src/main.c` (1634) و`Studio/desktop-app/server/src/*` و`Studio/Web/js/dhad-parser.js` (1741) و`dhad` نفسه دون فهم إضافي — فهي الأكثر coupling والأقل تغطية مثبتة، وأي تغيير فيها يكسر مساراً كاملاً. ولا يُحذف `release/` أو `Operational-Evidence/` أو `docs/` — فهي مصادر الحقيقة الوحيدة القابلة للاستشهاد.

> **الملفات المقروءة فعلاً في هذه الدورة (عينة قابلة للتدقيق):** الجذر `ls -la + du -sh` + `dhad (head 60 + file)` + `Docs/Reference/targets.json + TARGETS.md + BUILD_PIPELINE.md` + `docs/LANGUAGE_SPEC + GRAMMAR_SPEC + KEYWORDS_REGISTRY` + `Tests/run_all_tests.sh (612) + real_regression.sh (149)` + `Scripts/run_tests.sh + build_demos.sh + winbuild.sh + README-ويندوز` + `Demo/run-demo.sh + loop5.daad + دليل-العرض` + `Examples/01_hello.ض` + `release/SHA256SUMS.txt` + `Operational-Evidence/CPU/suite_summary.txt` + `.github/workflows/ci-cd.yml` + `opencode.json + .gitignore + .opencode/package.json` + `ض.json (عينة)` + `README + PROJECT_INDEX + FINAL_SUMMARY` + (عبر الوكلاء) `Compiler/CMakeLists + Makefile + main.cpp + Compiler.cpp + 16 هيدر + CodeGen/Lexer جزئياً` + `DAAD/CMakeLists + README + main.c + lexer/parser/backend/dhad_backend/emitter/register_allocator/semantic/optimizer/ir_builder + architecture/project-structure/tests-README + e2e_runner/batch2 + ret_42/b3_arith` + `CPU/Makefile + main.c + asm_main + dhad_cpu/asm + dhad_cpu.h/memory_map/isa/isa_constants/asm.h + ARCHITECTURE + REPORT + run_tests.sh + gui/main + HARDWARE_SPEC/ARCHITECTURE/DISCREPANCIES/TEST_REPORT + full_T1a/T10c/add` + `Studio/Web/package.json + desktop-app/package.json + server/package.json + extension.ts + Dockerfile/compose/vercel/railway + desktop-app/main.js + dhad-*.js (8+1، أول 80 سطراً لكل) + index.html + server/index.js + Full/frontend/main.cpp`.
