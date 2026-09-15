# تقرير بحث التكرار والهيكل — Dhad-Studio-Unified
**المرحلة: بحث فقط — ممنوع الحذف/التعديل**
**التاريخ:** 2026-09-13 | **السياق الأساسي:** `ض.json` (143 رسالة) + الكود المصدري الخام
**المنهج:** قراءة مباشرة + `du -sh` + `find` + `md5sum` + `diff -q` + 3 وكلاء استكشاف متوازيين (بحث فقط). لم يُحذف أو يُعدل أي ملف كود. هذا الملف الجديد هو المخرج الوحيد.

---

## 0. الخلاصة التنفيذية (10 أسطر)

1. المشروع 5 طبقات حقيقية: (أ) لغات/مترجمات `Compiler+DAAD+CPU` (ب) منصة `Studio` (ج) توثيق مزدوج `Docs+docs` (د) إصدارات مثلثة `Releases/release/Program Files` (هـ) أمثلة متضخمة `Examples (271M)`.
2. أكبر تكرار مؤكد: السيرفر توأم `desktop-app/server (130 ملف) ≡ Full/server (153 ملف)` — الفرق فقط `aws/ (8) + .env.archived + 14 .bak`، والنواة متطابقة hash (محرك `dhad/*` + `rateLimitStore` + `dhadWorker` + `schema.prisma` + بنود 2-7 كلها حاضرة ومتطابقة).
3. الواجهة مثلثة: `Web (62 ملف) ≡ desktop/frontend-web (62) ≡ Full/frontend-web (62)` = 186 ملفاً، منها `index/login/register/style` متطابقة 100%، والانحراف الحقيقي 8 ملفات فقط (`api/dhad-*/settings/privacy/terms`) حيث `Web` الأحدث/الأكبر (`parser +7KB`, `codegen +3KB`).
4. المترجم C++ توأم مفترق: `Compiler/ (71M) vs Full/compiler/` — نفس `CMakeLists` حرفياً + 7 ملفات `cpp` متطابقة + 5 ملفات مختلفة (`CodeGen/Lexer/Parser/Keywords/SandboxValidator`) = عمل البند A موجود في طرف دون الآخر ويحتاج دمجاً انتقائياً لا اعتماد أعمى.
5. `CPU (11M)` ≠ `DAAD (27M)`: الأول عتاد/محاكي `C+Qt+.circ` والثاني لغة `.daad→.s→.exe` — لا توحيد بينهما، فقط أسماء أمثلة متشابهة.
6. `Docs/ (24M)` ≠ `docs/ (304K)`: الأول أرشيف عملياتي (`Archive 11M + Root-Reports 11M`) والثاني مواصفة معيارية (`LANGUAGE/GRAMMAR/SEMANTIC/STDLIB/KEYWORDS_REGISTRY`) — تكميل لا تكرار، لكن الاسمين بحساسية حالة خطران على ويندوز.
7. الإصدارات 3 نسخ متداخلة (`Releases 8.2M / release 29M / Program Files 12M`) + `Examples/data-pos وحدها 253M (756 .dat من 1999)` = 93% من تضخم الأمثلة لا علاقة له بالمترجم.
8. مخلفات بناء ضخمة: `build/ (95M: Compiler 27+40 + Full/compiler 21 + DAAD 6 + bridge 1.2)` + `node_modules (465M: server 217×2 + VSCode 27 + Electron 4.6)` + `32 .bak` + `output.cpp/hpp` متناثرة + `143 .exe + 322 .s في DAAD`.
9. المفاضلة العامة (حسب قاعدتك: نحو الأكثر تطوراً): `desktop` يتفوق في 5 (`cluster/create-accounts/dhad.js/codegen/isolation-tests`) و`Full` يتفوق في 6 (`auth/IP-block/health-timingSafe/csrf-H7/parser-finally/privacy-terms-UTF8`) و`Web` يتفوق في 2 (`parser/codegen` الكبيران) — فالصحيح دمج انتقائي لكل ملف لا ترجيح شجرة كاملة.
10. لا شيء حُذف في هذه المرحلة. كل ما يلي توصيات مرقمة للمرحلة التالية بانتظار أمرك.

---

## 1. فهرس المشروع — عمل كل مجلد حرفياً

### 1.1 المستوى العلوي

| المجلد | الحجم | الملفات | العمل الحرفي | أهم المحتويات |
|---|---|---:|---|---|
| `CPU/` | 11M | 352 | معالج عربي 16-بت منخفض المستوى: محاكي C + مجمّع `ضasm` + واجهة Qt | `src/dhad_cpu.c dhad_asm.c / include/dhad_isa.h dhad_memory_map.h / hardware/*.circ / gui/main.cpp+dhad_gui.pro / docs/HARDWARE_SPEC.md / *.bin / desktop_dhad_v*.zip` |
| `Compiler/` | 71M | 1213 | مترجم `ض → C++20` (`DaadStudio 1.0.0 C++20 + googletest`) | `src/{Lexer,Parser,CodeGen,Compiler}.cpp / include/Daad/*.hpp / stdlib_arabic / examples / daad-compiler.exe / build/ + build-make/` |
| `DAAD/` | 27M | 1197 | لغة `.daad → .s → .exe` + تقارير تدقيق متراكمة | `compiler/{lexer,parser,semantic,codegen,backend,ssa} / 182 .daad / 322 .s / 143 .exe / docs/{grammar,registers,type-system} / Testing/e2e` |
| `Demo/` | 2.4M | 14 | عرض مصغر قابل للتشغيل | `api-demo.js / run-demo.{sh,bat} / start-server.{sh,bat} / loop5.daad+.bin / دليل-العرض*.md` |
| `Docs/` | 24M | 187 | أرشيف عملياتي تاريخي | `Archive/ (60+ PHASE2/3/45/6) / Root-Reports/ (CODE_REVIEW/SECURITY_HARDENING + 9 .png) / Reference/{BUILDING,DEPLOY_FULL} / Instructions/README-ويندوز / Reports/` |
| `Examples/` | 271M | 889 | مدونة أمثلة مترجمة (كل مثال 5 صيغ) | `01_hello.{ض,cpp,hpp,exe,linux} ×~75 مثال / data-pos/ (253M) / dhad_cpu.exe / games/ / conversions/` |
| `Operational-Evidence/` | 388K | 59 | أدلة تشغيلية مصنفة | `CPU/Compiler/DAAD/Electron/MSI/Qt/Studio-Server/Web/VSCode/WIN64/WINE + logs` |
| `Releases/` | 8.2M | 333 | إصدار مجمد واحد | `Interview-Release/{DhadStudio-Setup-Win7.msi,SHA256SUMS,Documentation,Examples,Source,build-companions}` |
| `Scripts/` | 144K | 25 | أتمتة بناء/تشغيل عربية | `winbuild.sh / run_tests.sh / gen_wxs.py / تشغيل-المنصة-الكاملة.bat / ترجمة-ملف-ض.bat` |
| `Sources-Archive/` | 35M | 387 | أرشيف بارد (لا يُبنى) | `vsc-extensions 21M / compiler-sources 6.2M (=DAAD/compiler) / msi-extract 5.4M / binaries 1.7M / wix-templates / build-logs / dup_hashes.txt` |
| `Studio/` | 567M | 26121 | المنصة الكاملة (90% منها `node_modules`) | انظر 1.2 |
| `Tests/` | 52K | 2 | بوابتا اختبار فقط | `run_all_tests.sh / real_regression.sh` |
| `Program Files/` | 12M | 7 | صورة تثبيت ويندوز مصغرة | `DhadStudio/bin/{hello,arithmetic,loops,functions,factorial}.exe + README-WINDOWS7 + validate-windows7.bat` |
| `docs/` | 304K | 11 | المواصفة المعيارية للغة | `LANGUAGE_SPEC.md / GRAMMAR_SPEC.md / SEMANTIC_RULES.md / STDLIB_SPEC.md / KEYWORDS_REGISTRY.json / vscode/` |
| `release/` | 29M | 359 | مسرح إصدار نشط | `DhadStudio-Setup-Win7.msi 5.4M / windows-7/ 16M / windows-10-11/ 2.6M / Source/ 5.2M / Examples/ 84K / Documentation/` |
| ملفات جذرية | — | 24 `*.md` | تقارير المراحل | `STAGE1 (711 سطر) / STAGE2 (304) / SECURITY_FIX_PLAN (691) / HIGH_LEVEL_GAPS / CHALLENGES_* / OOP_* / PROJECT_UNDERSTANDING / ض.json (2.0M)` |

### 1.2 المستوى الثاني داخل `Studio/`

| المجلد | الحجم | الملفات | العمل الحرفي |
|---|---|---:|---|
| `Studio/Electron/` | 4.8M (منها 4.6M node_modules) | 4 فعلية | غلاف Electron قديم/مصغر: `main.js+preload.js+splash.html+titlebar` (`package:dhad-studio`) |
| `Studio/Full/` | 267M | 12768 | المونوريبو الكامل: `bridge+compiler+frontend+server` |
| `↳ Full/bridge` | 1.5M | 75 | أداة Qt للمدارس: `main.cpp/SchoolOnboarder.cpp/bridge_main.qml/DhadBridge` + `build/ 1.2M` |
| `↳ Full/compiler` | 21M | 347 | نسخة المترجم C++ (`src/ 264K + build/ 21M`) — نفس `CMakeLists` الجذري حرفياً |
| `↳ Full/frontend` | 3.3M | 134 | واجهة `Qt/QML (cpp/qml/i18n)` + `frontend-web/ (62 ملف)` |
| `↳ Full/server` | 242M (منها 217M node_modules) | 12211 مع node_modules / 130 بدونها | سيرفر EdTech `Express+Prisma+JWT` (`package:dhad-studio-server`) + `Dockerfile/prisma/nginx` |
| `Studio/Web/` | 2.9M | 82 (62 html/js) | واجهة ويب ستاتيكية (`index/login/register/book+css/js/pages` — `dhad-studio-frontend v2.0.0`) |
| `Studio/VSCode-Extension/` | 29M (منها 27M node_modules) | 3 فعلية | إضافة `daad`: `syntaxes/daad.tmLanguage.json/snippets/src/out` + `dhad-lang/` — أمر `F9` |
| `Studio/desktop-app/` | 262M (منها 217M node_modules) | 12282 معها / ~200 بدونها | تطبيق Electron الإنتاجي: `main.js/preload/splash + server/ + frontend-web/` منسوخة آلياً عبر `scripts/prepare-build.js` (يثبت آلية النسخ — انظر 4.3) |
| `Studio/Windows-Launcher/` | 64K | 10 | قائمة `.bat` عربية: `ض-ستوديو/مترجم-ض/معالج-ض/محرر-الويب + README-ويندوز` |
| `Studio/tests/` | 956K | 68 | مدونة تحديات/اختراق (`challenges_tier1-7/advanced_pentest/anti_cheat`) وليست unit-tests |
| ملفات `Studio/` الجذرية | — | — | `cli-login.js / create-accounts.js / Dockerfile / docker-compose / vercel.json / stdlib_index.json / slides_data.json / output.cpp+hpp (مخرجات مسربة)` |

---

## 2. الملفات المكررة — الدليل بالـ hash

### 2.1 التوأم الأكبر: السيرفر (مؤكد 100%)

- الهيكل: `desktop/server 130 ملف` vs `Full/server 153 ملف` = الثاني = الأول + 23 (`aws/ 8 + .env.archived + 14 .bak`). لا ملف حقيقي مفقود من أي طرف.
- عينة 20 ملفاً (`md5sum`):
  - ✅ متطابق (18): `dhad/{lexer 0868352d, parser 97a31701, codegen dcf07bd1, ast 7f5125f0}` + `dhadCompiler a72bbc38` + `dhadSandbox 57100f56` + `dhadWorker 0d5c1be2` + `serverEvaluator 7d5903c1` + `challengeController a8bfb8b2` + `courseController 334a4ea1` + `rateLimitStore 0b424cd9` + `auth 9cd33136` + `security 304947f3` + `schema.prisma 5df8d135` + `package.json e70fdecc` + `seed-student-default c4a9181e` + `migration 20260912 77eb8d85` + `challengeInputs unit 02cab91e + integration 7f351fb5`.
  - ❌ مختلف (2 في العينة + 8 إضافية من المسح الشامل = 10): انظر جدول المفاضلة 3.1.
- بنود 2→7 حاضرة ومتطابقة في الشجرتين (لا انقسام): `dhadWorker / rateLimitStore / challengeInputs×2 / seed-student-default / migration` كلها نفس الهاش.

### 2.2 المثلث الثاني: الواجهة الأمامية (مؤكد)

- العدد (بدون node_modules): `Web 62 = desktop/frontend-web 62 = Full/frontend-web 62` → 186 ملفاً. + `Electron 4 + VSCode 3` = **193 ملف html/js فعلياً**.
- `index.html c4738b.. / login 0a91c9.. / register 3d044b.. / style.css 0ea1c6..` متطابقة 100% في النسخ الثلاث.
- متطابقة ثلاثياً أيضاً: `dhad-libraries b4412318 (128K) / smart-evaluator bb47fdb8 / challenges-page bb109ad1 / auth 89179214`.
- مختلفة (8): `api / dhad-ast / dhad-codegen / dhad.js / dhad-lexer (33B) / dhad-parser (~7KB) / settings-page / privacy+terms` — التفصيل في 3.2.
- `Web` وحدها تحمل 12 `.bak`، و`Full/frontend` تحمل 4 `.bak` — كلها مرشحة أرشفة لا حذف مباشر.

### 2.3 المترجم C++ (توأم مفترق — ليس نسخة عمياء)

- `CMakeLists` الجذري + `src/CMakeLists` متطابقان حرفياً (`diff -q` صفر، `DaadStudio 1.0.0 C++20 + googletest v1.14.0`).
- `src/` نفس القائمة (12 ملفاً): 7 متطابقة hash (`AST 2a47 / Diagnostics 46ba / UnicodeUtils 5025 / Token 502d / main 5c51 / Compiler 837a / Optimizer afb9`) و5 مختلفة (`CodeGen ee15 vs 6a02 / Lexer c28f vs f848 / Parser bb94 vs 828e / Keywords fa01 vs 49c1 / SandboxValidator 7bd9 vs 7339`).
- `Full/compiler` بلا `include/` خاص (يعتمد على `../include` + `_deps`) — أي التطوير كان على طرف واحد دون مزامنة.
- نسخ ثالثة خارجية (اشتباه): `release/Source/Daad-Compiler/` + `Sources-Archive/compiler-sources/ (=DAAD/compiler نسخة 6.2M)` + `DAAD/compiler/` الأصلي (toolchain مختلف كلياً بلغة C).

### 2.4 الإصدارات المثلثة + الأمثلة المتضخمة

- `Releases/Interview-Release (8.2M) ≈ release/ (29M)` نفس الهيكل (`MSI/Documentation/Examples/Source/SHA256SUMS`) — تكرار شبه كامل، و`Program Files/DhadStudio (12M)` صورة منصبة مصغرة منهما.
- `Examples/ (271M)`: `data-pos/ وحدها 253M (756 .dat، مثال b1.dat 5.8M بتاريخ 1999)` = 93% من الحجم. + `7 .exe ×2.3M ≈16.7M` + `7 .linux` + `31 .cpp + 7 .hpp + 43 .ض`. مقابل `release/Examples/ 84K فقط (20 ملفاً)` — أي النسخة المسرحية أنظف بكثير.

### 2.5 ما ليس تكراراً (مهم لعدم التوحيد الخاطئ)

- `CPU ≠ DAAD`: عتاد (`dhad_cpu.c/dhad_asm.c/dhad_isa.h/memory_map/*.circ/Qt-gui/HARDWARE_SPEC`) vs لغة (`lexer/parser/semantic/codegen/ssa/182 .daad/322 .s/143 .exe`) — توحيدمها خطأ معماري.
- `Docs ≠ docs`: `comm` على الأسماء فارغ و`md5` بلا تقاطع و`grep GRAMMAR|LANGUAGE_SPEC في Docs` لا يعيد إلا `Archive/README`. العلاقة تكميلية (أرشيف عملياتي vs مواصفة normative).
- `Studio/tests/ (68 ملف تحديات)` ليست unit-tests — لا تدمج مع `server/tests/`.

---

## 3. المفاضلة: المتطور يكسب (حسب قاعدتك — ملفاً بملف لا شجرة بشجرة)

### 3.1 السيرفر — 10 ملفات مختلفة (الاتجاه مختلط فيثبت وجوب الدمج الانتقائي)

| الملف | الفائز | الدليل |
|---|---|---|
| `server/create-accounts.js` | **desktop** (أحدث 00:00، 3841B) | باسورد عشوائي `crypto` vs ثابت `Test1234!` في Full |
| `src/cluster.js` | **desktop** (1702B) | فيه إصلاح H8 (عامل واحد بدون Redis) vs 1290B |
| `src/config/swagger.js` | **Full** (8419B) | يوثق `httpOnly cookies` (أحدث معمارياً) vs `Bearer` |
| `controllers/authController.js` | **Full** (23127B) | فيه `M4 isIPBlocked + COOKIE_OPTIONS` أمنياً أكمل (desk أكبر حجماً 23995B لكن أقدم) |
| `controllers/healthController.js` | **Full** (4190B) | `timingSafeEqual (M9)` vs `===` بسيطة |
| `src/index.js` | تعادل فلسفي — يُحسم بقرارك | `desk ensureSecureSecrets (توليد تلقائي عملي)` vs `Full assertStrongSecrets (رمي خطأ صارم)` |
| `middlewares/csrf.js` | **Full** (4099B) | يشترط `CSRF_SECRET>=32 (H7)` vs اشتقاق من JWT |
| `services/authService.js` | **Full** (6322B) | يشترط `JWT_REFRESH_SECRET` vs سقوط على `JWT_SECRET` |
| `utils/logger.js` | **Full** (6877B) | يستثني `/health/*` vs `/health` فقط |
| `tests/unit/dhadIsolation.test.js` | **desktop** (3963B) | فيه اختبارات `مجرّد Item-5` vs 2417B بدونها |

### 3.2 الواجهة — 8 ملفات مختلفة

| الملف | الفائز | الدليل |
|---|---|---|
| `js/api.js` | **desktop** (22489B) | يجرّد `user→{name,role,displayName}` (أأمن) vs تخزين كامل 22674B |
| `js/dhad-ast.js` | **Full** (20050B) | يدعم `finallyBody` vs غيابه |
| `js/dhad-codegen.js` | **desktop** (49166B) | فيه `classFields` للكونستراكتر vs 48014B |
| `js/dhad.js` | **desktop** (9013B) | فيه `DANGEROUS_PATTERNS (+74 سطر)` vs 6602B |
| `js/dhad-parser.js` | **Full** (58693B) | يبني `finallyBody` vs تجاهل `finally` |
| `js/dhad-lexer.js` | يُفحص سطراً (فرق 33B فقط) | `Web 8ea0440d 27831B` vs `desk/full 0868352d 27798B` |
| `js/settings-page.js` | متكافئ | `toast.error` vs `Toast.show` (اختلاف API فقط) |
| `pages/privacy.html+terms.html` | **Full** (5941/5751B) | UTF-8 سليم vs `mojibake (ط³ظٹط§ط³ط©)` في desk (تالف ويحتاج الاستبدال من Full حتماً) |
| `Web/js/dhad-parser.js (65713B) + codegen (52291B)` | **Web الأحدث/الأكبر** | أكبر بـ 7KB و3KB من desktop — يُعتمد كمصدر عند الدمج |

### 3.3 ملفات محسومة سلفاً (لا مفاضلة)

- كل `dhad/*` في السيرفر متطابقة — أي مصدر يصلح.
- `dhad-libraries/smart-evaluator/challenges-page/auth` متطابقة ثلاثياً — أي مصدر يصلح.
- `privacy/terms` محسومة لـ Full (عطب ترميز في desktop).

---

## 4. تضخم الواجهات وتوحيد المجلدات (فرص بأرقام)

### 4.1 التضخم الحقيقي ليس في الـ html/js

- الـ 193 ملفاً حجمها `Web 2.9M + frontend-web 2.5M + Full/frontend 3.3M + Electron 4 فعلية + VSCode 3 فعلية` = ~9M فقط — أي **التكرار منطقي لا حجمي**.
- الحجم الحقيقي في `node_modules (465M)` و`build/ (95M)` — انظر 5.2.

### 4.2 آلية النسخ المثبتة (تفسر التوأم)

- `Studio/desktop-app/scripts/prepare-build.js:3` ينص حرفياً: `Copies server/ and frontend-web/ into desktop-app/ for electron-builder packaging` مع `exclude: [node_modules,.env,prisma/dev.db]` ثم نسخ انتقائي لـ `node_modules` و`.prisma` و`prisma/` وترقيع `@prisma/client`.
- النتيجة: `desktop-app/server + frontend-web` **مشتقة آلياً** وليست أصلاً مستقلاً — الأصل المفترض `Full/server + Full/frontend` (أو العكس حسب الأحدث لكل ملف من 3.1/3.2). التوحيد الصحيح: مصدر واحد + بناء آلي، لا حذف يدوي لأحد الطرفين الآن.

### 4.3 مجلدات مرشحة للتوحيد (مقترح فقط — لم يُنفذ)

1. `Studio/Web + */frontend-web` → مصدر واجهة واحد (`frontend-web/` فقط) + `Web/` تصبح مجرد `build/dist` أو تُحذف بعد التثبيت.
2. `Studio/Electron (مصغر) + Studio/desktop-app (إنتاجي)` → غلاف واحد (الثاني هو المتطور ويبقى).
3. `Compiler/ + Full/compiler/` → شجرة مترجم واحدة (الدمج الانتقائي للملفات الخمسة المختلفة أولاً).
4. `Releases/ + release/ + Program Files/` → مسرح إصدار واحد (`release/` الأنظف والأحدث).
5. `Docs/Reports/ + Docs/Root-Reports/` → مكتملة الأرشفة سابقاً في `Archive/` — الباقي `Reports/Root-Reports` الفارغة/المكررة تُراجع (البند 7 أرشف 75 ملفاً وبقيت الهياكل).
6. `Scripts/*.bat + Windows-Launcher/*.bat` → مجلد لانشر واحد (نفس الاسم `README-ويندوز.md` مكرر).
7. `Sources-Archive/compiler-sources + DAAD/compiler` → إبقاء الأصل وحذف النسخة من الأرشيف عند الاعتماد (يحتاج قرار provenance).

---

## 5. جرد المرشحات للحذف/الأرشفة لاحقاً (ذكر فقط — لم يُحذف شيء)

### 5.1 نسخ احتياطية `.bak` (32 ملفاً — آمنة الأرشفة أولاً)

- `Web: 12` (`api/course-roadmap/dhad-ast/dhad-codegen/dhad/dhad-libraries/dhad-parser/settings/web-editor + dhad-editor.html/web-editor.html/terms.html`)
- `Full/server: 14` (`auth/csrf/controllers×4/utils×2/routes×6`)
- `Full/frontend: 4` (`course-roadmap/web-editor/dhad-editor/web-editor`)
- `Full/bridge: 2` (`bridge_main.qml/SchoolOnboarder.cpp`)
- التوصية: `git mv → Archive/bak-YYYYMMDD/` لا `rm` مباشر (تحتفظ بالتاريخ).

### 5.2 مخلفات بناء (لا تُخزن في git — تُبنى في CI)

- `build/: Compiler/build 27M + build-make 40M + Full/compiler/build 21M + DAAD/build 6M + bridge/build 1.2M` = ~95M (منها `_deps/googletest-src/build/subbuild` مكررة 3 مرات).
- `node_modules: Full/server 217M + desktop/server 217M (مكرر حرفياً بنفس الحجم — تحقق لاحقاً بـ `diff -rq`) + VSCode 27M + Electron 4.6M + .opencode 63M` = ~528M.
- `.bin/: VSCode/node_modules/.bin + .opencode/node_modules/.bin`.
- مخرجات مسربة: `Studio/output.cpp+hpp+output_fixed.cpp + Examples/output.cpp + bridge-sources/*_out.cpp/hpp`.
- مولدات DAAD: `143 .exe + 322 .s` + `Examples 7 .exe + 7 .linux` + `data-pos 756 .dat` + `CPU/*.bin + Demo/windows-bin/*.exe`.

### 5.3 ملفات النشر الخاصة (ليست تكراراً حقيقياً)

- `Full/server/aws/ (8 ملفات) + .env.archived` — موجودة في Full دون desktop لسبب (نشر AWS) — لا تُحذف مع التوحيد، تُنقل لمجلد `deploy/` مستقل.

---

## 6. الخطة المقترحة للمرحلة التالية (بانتظار أمرك — لن تُنفذ وحدي)

1. **تثبيت المصادر:** اعتماد `Web/parser+codegen` الكبيرين + `Full/privacy+terms/auth/health/csrf/parser-finally` + `desktop/cluster/api/dhad.js/codegen-classFields/isolation-tests` حسب جداول 3.1/3.2 (دمج انتقائي بمراجعة سطرية لا نسخ شجرة).
2. **مصدر واحد للواجهة والسيرفر:** جعل `Full/` المصدر و`desktop-app/` ناتج `prepare-build.js` فقط (أو العكس إن قررت) + توثيق ذلك في `README`.
3. **أرشفة `.bak`:** `git mv` جماعي لـ 32 ملفاً → `Archive/` مع `README_ملاحظة.md`.
4. **توحيد المترجم:** دمج الملفات الخمسة المختلفة (`CodeGen/Lexer/Parser/Keywords/SandboxValidator`) نحو الأكثر تطوراً ثم حذف `Full/compiler/build` و`Compiler/build-make` المكررة.
5. **مسرح إصدار واحد:** اعتماد `release/` + أرشفة `Releases/` و`Program Files/` خارجه (أو LFS).
6. **تنظيف الأمثلة:** نقل `Examples/data-pos/` (253M) خارج الريبو أو LFS + `.gitignore` لـ `*.exe/*.s/*.dat/*.bin/build/node_modules/_deps/output.*`.
7. **فهرس دائم:** توليد `PROJECT_INDEX.md` (عمل كل مجلد حرفياً من §1) كملف حي يُحدث مع كل توحيد.
8. **تحقق بعد كل خطوة:** `md5sum + diff -q + node --check + cmake --build + حزم الاختبارات` (نفس صرامة البنود 2-7).

---

## 7. ملاحظات ومخاطر سُجلت (لم تُتجاهل — حسب القاعدة 6)

1. `Docs` vs `docs` حساسية حالة — ستكسر على ويندوز (نفس الاسم باختلاف الحالة). التوصية: إعادة تسمية لاحقاً (`Docs-Ops/` vs `docs-Spec/`) لا الآن.
2. `privacy/terms` في desktop تالفتا الترميز (`mojibake`) — الاستبدال من Full إلزامي لا اختياري عند الدمج.
3. `index.js` السيرفر فلسفتان (`توليد تلقائي` vs `رمي صارم`) — تحتاج قرارك الصريح قبل التوحيد.
4. `Examples/data-pos/*.dat` تواريخ 1999 — تحقق من provenance قبل النقل (قد تكون بيانات اختبار خارجية).
5. `Sources-Archive` بلا `CMakeLists` جذري — بارد ومُستبعد من البناء، لكن `dup_hashes.txt/phase2_dups.txt` فيه قد تفيد تدقيق التكرار لاحقاً.
6. `Web/dhad-lexer` فرق 33B فقط — يحتاج `diff` سطرياً قبل الترجيح (لم أرجح جزافاً).
7. `node_modules` المتطابقتان (217M×2) لم أجزم بتطابقهما الثنائي (`du` متساوٍ لا يكفي) — تحتاج `diff -rq` أو مقارنة `package-lock` لاحقاً.

---

## 8. المصادر والمنهج (قابل لإعادة التشغيل)

- `ls -1 + du -sh */ + find -path *node_modules -prune` للهيكل والعد.
- `md5sum + diff -q + ls -l` للعينات (20 سيرفر + 8 واجهة + 12 مترجم).
- `grep -n copy Studio/desktop-app/scripts/prepare-build.js` لآلية الاشتقاق.
- `find -name *.bak | wc -l (32)` + `du -sh */build + */node_modules` للمخلفات.
- `ls Docs/ + ls docs/ + comm + grep GRAMMAR` لنفي تكرار التوثيق.
- `ls Releases/ + release/ + Program Files/ + du -sh Examples/data-pos` للإصدارات.
- وكلاء الاستكشاف الثلاثة (بحث فقط): فهرس حرفي + توأم desktop-full + علوي/واجهات — نُقلت خلاصاتهم أعلاه بأرقامها.

**انتهى التقرير — بحث فقط. لم يُحذف أو يُعدل أي ملف. بانتظار أمرك للتنفيذ.**
