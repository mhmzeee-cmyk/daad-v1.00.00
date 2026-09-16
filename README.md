# ض ستوديو — Dhad Studio / DAAD

> **لغة برمجة عربية كاملة + منصة تعليمية + أدوات بناء وإصدار أصلية (Native).**
> مونوربو واحد يجمع ثلاثة مسارات لغوية (`.ض` و`.daad` و`.ضasm`)، ومنسق بناء موحد `dhad`،
> ومنصة EdTech (ويب + سيرفر + تطبيق مكتبي + إضافة VSCode)، وحزم Windows أصلية (MSI/PE).

---

## الفهرس

1. [الفكرة والفلسفة](#1-الفكرة-والفلسفة)
2. [خريطة المستودع](#2-خريطة-المستودع)
3. [المسارات اللغوية الثلاثة](#3-المسارات-اللغوية-الثلاثة)
4. [المنسق الموحد `dhad`](#4-المنسق-الموحد-dhad)
5. [عمل كل ملف ومكون](#5-عمل-كل-ملف-ومكون)
6. [طريقة التشغيل](#6-طريقة-التشغيل)
7. [الاختبارات والتحقق](#7-الاختبارات-والتحقق)
8. [الإصدارات والشحن](#8-الإصدارات-والشحن)
9. [الأمان](#9-الأمان)
10. [التوثيق](#10-التوثيق)
11. [المساهمة والترخيص](#11-المساهمة-والترخيص)

---

## 1- الفكرة والفلسفة

**الفكرة:** أن يكتب الطالب العربي برنامجه الأول بلغته الأم — من `طباعة("مرحبا بالعالم!")`
حتى أصناف `صنف` ووراثة `يرث` — ثم يرى كوده يتحول إلى برنامج أصلي يعمل على جهازه،
ويتعلم عبر 210 تحديًا تفاعليًا داخل المنصة نفسها.

**الفلسفة (5 مبادئ):**

1. **العربية أولًا، لا ترجمة:** الكلمات المفتاحية (`طباعة، إذا، طالما، دالة، صنف، جديد`)
   والتشخيصات والأمثلة عربية أصيلة، لا طبقة ترجمة فوق إنجليزية.
2. **ثلاث لهجات لثلاثة مستويات:** لهجة عالية `.ض` (تُترجم إلى C++ ثم أصلي)،
   ولهجة `.daad` (تُترجم إلى تجميع x86 أو إلى `.ضasm`)، ولهجة تجميع `.ضasm`
   (تُجمّع وتُحاكى على معالج تعليمي) — كل مستوى يعلّم طبقة من الحاسوب.
3. **منسق واحد للحقيقة:** كل البناء يمر عبر `dhad` وسجل الأهداف
   `Docs/Reference/targets.json` — لا أوامر متفرقة ولا تخمين.
4. **هندسة صادقة:** ما يعمل موثق بدليل تشغيل (`Tests/real_regression.sh` + `Operational-Evidence`)،
   وما لا يعمل موسوم `NOT_IMPLEMENTED` بخروج `3` بدل الادعاء (مثل macOS والهواتف).
5. **التقييم على الخادم لا على العميل:** حلول الطلاب تُنفذ في صندوق معزول
   (`worker_threads` + `vm` بحدود ذاكرة وزمن) وتُصحح server-side — العميل لا يُؤتمن على الدرجات.

---

## 2- خريطة المستودع

```
Dhad-Studio-Unified/
├── dhad                  ← المنسق الموحد (137 سطر bash): targets/build/run
├── Compiler/             ← المسار A: مترجم .ض → C++ (C++20, CMake)
├── DAAD/                 ← المسار B: مترجم .daad → x86/DHAD (C99, CMake)
├── CPU/                  ← المجمّع والمحاكي .ضasm (C11 + Makefile)
├── Studio/               ← المنصة: Web + Full + desktop-app + VSCode
├── Tests/                ← حزم الاختبار المرجعية (real_regression + run_all)
├── Examples/             ← أمثلة .ض/.daad/.ضasm (من 01_hello إلى الألعاب)
├── Demo/                 ← عروض بضغطة واحدة + بناء ويندوز (MinGW)
├── Docs/                 ← المواصفات (Specs) + المراجع + التعليمات + الأرشيف
├── Scripts/              ← سكربتات مساعدة (بناء/اختبار/توليد)
├── release/              ← حزم windows-7 (v1.0.1) و windows-10-11 (v1.1.0)
├── Operational-Evidence/ ← أدلة التشغيل الحية (CPU/MSI/WINE/…)
├── Sources-Archive/ + archive/ ← لقطات وأرشيف خارج خط البناء
├── PROJECT_UNDERSTANDING.md + PROJECT_INDEX.md ← شرح المشروع وفهارسه
└── README.md             ← هذا الملف
```

| المجلد | الدور | التقنية |
|---|---|---|
| `Compiler/` | مترجم اللهجة العالية `.ض` إلى C++ | C++20 + CMake + Makefile |
| `DAAD/` | مترجم اللهجة `.daad` إلى x86 أو DHAD | C99 + CMake + CTest |
| `CPU/` | مجمّع `.ضasm` + محاكي المعالج + واجهة Qt | C11 + Makefile |
| `Studio/Web/` | الواجهة الثابتة (المصدر المرجعي canonical) | HTML/JS خام بدون bundler |
| `Studio/Full/server/` | خادم API المرجعي (canonical) | Node 18+ + Express + Prisma |
| `Studio/desktop-app/` | تطبيق Electron (يغلف السيرفر والواجهة) | Electron 33 |
| `Studio/VSCode-Extension/` | إضافة لغة ض لـ VSCode | TypeScript |
| `Studio/Full/bridge/` | جسر تعليمي `DhadBridge` | C++ + Qt |
| `Tests/` | الانحدار المرجعي والفحص الشامل | bash |
| `Examples/` | 552 ملف `.ض` + 186 `.daad` + 153 `.ضasm` | لغة ض |
| `Demo/` | `run-demo.sh` + `build-windows.sh` | bash + MinGW |
| `Docs/Specs/` | عقود اللغة المشتقة من الكود | Markdown |
| `release/` | حزم MSI وPE لكل ويندوز | WiX (`wixl`) |

---

## 3- المسارات اللغوية الثلاثة

| # | اللهجة | الامتداد | المترجم | المخرجات | مثال |
|---|---|---|---|---|---|
| A | ض العالية | `.ض` | `Compiler/build/bin/daad-compiler` | `.cpp` ثم `g++` ← ELF أو `mingw` ← PE | `طباعة("مرحبا بالعالم!") ؛` |
| B | ضاد | `.daad` | `DAAD/build/daad` | خلفية `x86` ← `.s` ← ELF، أو خلفية `dhad` ← `.ضasm` | `دالة: الرئيسية() -> رقم :- … نهاية` |
| C | تجميع ض | `.ضasm` | `CPU/dhad_asm` + `CPU/dhad_cpu` | `.bin` ثم تنفيذ ومحاكاة | `حمّل مح، 10` |

الأهداف المتاحة عبر `dhad`: `linux-x64` و`linux-x64-cpp` و`windows-x64` و`dhad-cpu`
(و`macos/android/ios` معلنة `NOT_IMPLEMENTED` وتخرج `3` بصدق).

---

## 4- المنسق الموحد `dhad`

`dhad` سكربت واحد (137 سطرًا) يوجه كل شيء حسب سجل `Docs/Reference/targets.json`:

```bash
./dhad targets                                        # عرض الأهداف وحالتها
./dhad build Examples/01_hello.ض --target=linux-x64-cpp -o /tmp/hello
/tmp/hello                                            # مرحبا بالعالم!
./dhad build Examples/01_hello.ض --target=windows-x64 -o /tmp/hello.exe
file /tmp/hello.exe                                   # PE32+ executable
./dhad build DAAD/factorial.daad --target=linux-x64 -o /tmp/fact; /tmp/fact; echo $?  # 120
./dhad run Demo/loop5.daad --target=dhad-cpu          # 15
```

---

## 5- عمل كل ملف ومكون

### 5-1 الجذر

| الملف | العمل |
|---|---|
| `dhad` | المنسق: يحل الهدف من `targets.json` ويستدعي السائق الصحيح (`driver_cpp` / `driver_daad` / `driver_windows` / `driver_cpu`) ثم يتحقق من المخرج |
| `Docs/Reference/targets.json` | سجل الأهداف المرجعي (4 متاحة + 3 معلنة غير منفذة) — مصدر الحقيقة للبناء |
| `PROJECT_UNDERSTANDING.md` | شرح المشروع الكامل (8350 ملفًا موثقًا مرحلة بمرحلة) |
| `PROJECT_INDEX.md` | فهرس الأحجام والمسارات |
| `تقرير.md` | خط الأساس المعماري الحالي |
| `README.md` | هذا الفهرس والدليل |

### 5-2 `Compiler/` (المسار A — C++20)

| الملف | العمل |
|---|---|
| `src/main.cpp` | نقطة الدخول `:83` — يقرأ `.ض` ويقود السلسلة حتى C++ |
| `src/Lexer.cpp` + `include/Daad/Lexer.hpp` | المحلل المعجمي + الكلمات العربية + الترميز |
| `src/Parser.cpp` + `include/Daad/Parser.hpp` | المحلل النحوي: الأصناف والباني العاري والمصنعي والواجهات |
| `src/AST.cpp` | شجرة البناء (أصناف/دوال/بواني/وراثة) |
| `src/CodeGen.cpp` | مولد C++ (`emitClasses`) — لا وسيط IR، المخرج C++ مباشرة |
| `src/Optimizer.cpp` | تحسينات خفيفة قبل التوليد |
| `src/Compiler.cpp` | واجهة `compile(src,header,sourceFile)` + أسماء `#line` للتنقيح |
| `src/Diagnostics.cpp` | تشخيصات عربية بمواضع دقيقة |
| `src/Keywords.cpp` + `Token.cpp` | سجل الكلمات والرموز |
| `src/SandboxValidator.cpp` | فحص أمان الكود قبل التقييم في المنصة |
| `CMakeLists.txt` | بناء `DaadCore` ثم `daad-compiler` + حزم NSIS (الاختبارات معطلة افتراضيًا) |
| `Makefile` | اختصار تطوير (`build-make`) |
| `stdlib/` + `stdlib_arabic/` | ~200 وحدة قياسية عربية |
| `Daad/Runtime/DaadRuntime.hpp` | زمن التشغيل المرافق للمخرجات |
| `examples/` + `conversions/before→after` + `programs/{large,mega}` | عينات واختبارات تحويل |

### 5-3 `DAAD/` (المسار B — C99)

| الملف/المجلد | العمل |
|---|---|
| `compiler/main.c` | نقطة الدخول |
| `compiler/{lexer,parser,ast,semantic}` | تحليل `.daad` (أعداد/نصوص/دوال/شروط/حلقات) |
| `compiler/{ir,optimizer,cfg,ssa,codegen}` | تمثيل وسيط وتحسين وتوليد |
| `compiler/backend/` | خلفيتان: `x86` (تجميع GNU) و`dhad` (تجميع ض) |
| `compiler/token/` | الرموز وأنواعها |
| `CMakeLists.txt` | بناء `daad_lib` ثم `daad` ثم اختبارات `test_*` |
| `tests/` | اختبارات CTest (معجم/نحو/دلالة/توليد/ضغط/أداء) + `phase12_5/run_e2e.sh` |
| `docs/` (16 ملفًا) | توثيق المسار B |
| `*.daad` + `e2e/` | أمثلة وحالات طرف-لطرف |

### 5-4 `CPU/` (المجمّع + المحاكي — C11)

| الملف | العمل |
|---|---|
| `src/asm_main.c` | المجمّع `dhad_asm`: `.ضasm` ← `.bin` |
| `src/main.c` | المحاكي `dhad_cpu` (1360 سطرًا): تنفيذ + تنقيح |
| `src/dhad_asm.c` / `src/dhad_cpu.c` | منطق التجميع وتنفيذ التعليمات |
| `src/dhad_debug.c` | المنقح التفاعلي |
| `src/dhad_live.cpp` / `src/visual_sim.c` | المحاكاة الحية والعرض المرئي |
| `include/dhad_isa.h` | تعريف التعليمات (المرجع الفعلي: ذاكرة 4K nibble) |
| `include/memory_map.h` + `dhad_memory_map.h` | خريطتا ذاكرة (موثق تعارضهما للمراجعة) |
| `include/` (16 ترويسة) | المعالج والذاكرة والأجهزة والمقاطعات |
| `Makefile` | بناء `dhad_cpu` + `dhad_asm` (+ هدف Qt للواجهة) |
| `tests/` (153 ملفًا) | اختبارات تعليمات كاملة + `run_tests.sh` المرجعي |
| `gui/main.cpp` + `hardware/*.circ` | واجهة Qt وتجارب Logisim |

### 5-5 `Studio/` (المنصة التعليمية)

| الملف/المجلد | العمل |
|---|---|
| `Web/` | الواجهة الثابتة المرجعية (`index.html` + `js/api.js` + الصفحات) — المصدر الوحيد، والمرايا تُنسخ عند البناء |
| `Full/server/src/index.js` | الخادم المرجعي (470 سطرًا): Express + وسائط الأمان + `/api/v1` + خدمة الواجهة |
| `Full/server/src/routes/` | مسارات `auth` و`student` و`challenges` و`courses` و`analytics` و`schools` و`otp` |
| `Full/server/src/controllers/` | منطق الدخول والتحديات والتقييمات والفصول |
| `Full/server/src/middlewares/` | مصادقة JWT + CSRF مزدوج + حدود معدل + قفل تدريجي |
| `Full/server/src/utils/dhadSandbox.js` | صندوق التنفيذ المعزول + `dhadWorker.js` (`worker_threads`) |
| `Full/server/src/utils/serverEvaluator.js` | المصحح (مخرجات 60 + بنية 25 + سلامة 15، النجاح ≥60) |
| `Full/server/prisma/` | مخطط SQLite (افتراضيًا) وPostgres (للإنتاج) + البذور |
| `Full/server/scripts/generate-secrets.js` | توليد أسرار قوية لملف `.env` |
| `desktop-app/main.js` | غلاف Electron: يطلق السيرفر ويري الواجهة |
| `desktop-app/server/src` | رابط رمزي ← `Full/server/src` (مصدر واحد، لا نسخ) |
| `desktop-app/scripts/prepare-build.js` | نسخ الواجهة عند البناء + حارس الانحراف |
| `VSCode-Extension/src/extension.ts` | الإضافة: تمييز و snippets للغة ض |
| `Full/bridge/` | جسر `DhadBridge` التعليمي |
| `Full/frontend/{cpp,qml,i18n}` | واجهة Qt الاختيارية |
| `Windows-Launcher/` | مشغلات `.bat` العربية لويندوز |
| `tests/` (68 ملفًا) | اختبارات تكامل وضغط وتحديات (تاريخية/حمل) |

### 5-6 `Tests/` و`Examples/` و`Demo/`

| الملف | العمل |
|---|---|
| `Tests/real_regression.sh` | **المرجع القانوني**: 47 فحصًا (مترجم + DAAD + CPU + `dhad` + حدود) — الدليل أن النسخة تعمل |
| `Tests/run_all_tests.sh` | فحص دخاني شامل (نحو JS + TypeScript + مسارات) |
| `Examples/01_hello..07_fibonacci.ض` | المنهج الأساسي + مخرجاتها المتوقعة في الانحدار |
| `Examples/games/*.ض` | ألعاب تعليمية (ثعبان/رسم/جسيمات) |
| `Demo/run-demo.sh` | عرض بضغطة واحدة |
| `Demo/build-windows.sh` | `.ض` ← PE ويندوز عبر MinGW (نفس خط `dhad`) |
| `Demo/loop5.daad` | حلقة 1..5 = 15 (شاهد التكامل DAAD→CPU) |

### 5-7 `Docs/` و`Scripts/` و`release/`

| الملف/المجلد | العمل |
|---|---|
| `Docs/Specs/` | عقود اللغة: `LANGUAGE_SPEC` و`GRAMMAR_SPEC` و`SEMANTIC_RULES` و`STDLIB_SPEC` و`KEYWORDS_REGISTRY` |
| `Docs/Reference/` | `TARGETS` و`BUILD_PIPELINE` و`LANGUAGE_CONTRACT` و`ISA_COMPATIBILITY` |
| `Docs/Instructions/` | أدلة التثبيت والتشغيل (Win7/Win10-11) |
| `Docs/Archive/` | `bak` و`leaked-outputs` المؤرشفة + README |
| `Scripts/` | `run_tests.sh` (اختبارات CPU) + مولدات `wxs` و`launcher` |
| `release/windows-7/` | MSI الإصدار `v1.0.1` (خمس PE صحيحة) + `DhadStudio.wxs` + `SHA256SUMS` |
| `release/windows-10-11/` | MSI الإصدار `v1.1.0` (خمس PE صحيحة بعد الإصلاح) + `DhadStudio.wxs` + `SHA256SUMS` |
| `Operational-Evidence/` | أدلة حية: CPU (102/102) وجداول MSI وWINE والتكامل |
| `Sources-Archive/` + `archive/` | لقطات provenance خارج خط البناء |

---

## 6- طريقة التشغيل

### 6-1 المتطلبات

```bash
# أدوات البناء (لينكس)
sudo apt install cmake gcc g++ python3 mingw-w64 nodejs npm
# ويندوز: MinGW-w64 + Node 18+ + CMake
```

### 6-2 بناء سلاسل الأدوات (مرة واحدة)

```bash
cmake -S Compiler -B Compiler/build -DDAAD_BUILD_TESTS=OFF
cmake --build Compiler/build -j"$(nproc)"     # → Compiler/build/bin/daad-compiler
cmake -S DAAD -B DAAD/build && cmake --build DAAD/build -j  # → DAAD/build/daad
make -C CPU                                    # → CPU/dhad_asm + CPU/dhad_cpu
./dhad targets                                 # 4 متاحة
```

### 6-3 تشغيل برنامج (الطريقة المختصرة)

```bash
./dhad build Examples/01_hello.ض --target=linux-x64-cpp -o /tmp/hello && /tmp/hello
./dhad run Demo/loop5.daad --target=dhad-cpu
```

### 6-4 تشغيل المنصة (السيرفر + الواجهة)

```bash
cd Studio/Full/server
cp .env.example .env                            # ثم املأ القيم
node scripts/generate-secrets.js --write        # توليد أسرار قوية (لا يكتب فوق الموجود)
# (أول مرة) npx prisma migrate dev
node src/index.js                               # → http://localhost:3000
# الصحة: http://localhost:3000/health/live
```

الواجهة: افتح `Studio/Web/index.html` عبر السيرفر (تُخدم تلقائيًا)،
وسجل طالبًا جديدًا من صفحة التسجيل (التسجيل عام للطلاب).

### 6-5 تطبيق سطح المكتب وإضافة VSCode

```bash
cd Studio/desktop-app && npm install && npm start      # Electron (يطلق السيرفر تلقائيًا)
cd Studio/VSCode-Extension && npm install && npm run compile
```

### 6-6 بناء نسخة ويندوز

```bash
bash Demo/build-windows.sh release/Examples/01_hello.ض release/windows-10-11/bin/hello.exe
file release/windows-10-11/bin/hello.exe   # PE32+ executable
```

---

## 7- الاختبارات والتحقق

```bash
bash Tests/real_regression.sh     # المرجع: 47/47 يجب أن تنجح
bash Tests/run_all_tests.sh       # فحص دخاني موسع
(cd Studio/Full/server && npm test)  # اختبارات Jest للخادم
```

شهادة هذه النسخة: `real_regression` خضراء 47/47 + جناح CPU (102) + سيرفر حي
(صحة 200 وواجهة 200) + حزم PE الخمس سليمة لكل ويندوز.

---

## 8- الإصدارات والشحن

| الإصدار | الملف | المحتوى |
|---|---|---|
| v1.0.1 (Win7) | `release/windows-7/DhadStudio-Windows7.msi` | خمس PE + سكربتات تحقق + SHA |
| v1.1.0 (Win10/11) | `release/windows-10-11/DhadStudio-Windows-x64.msi` | خمس PE + سكربتات تحقق + SHA |

كل حزمة لها `DhadStudio.wxs` (مصدر WiX يُبنى بـ `wixl`) و`SHA256SUMS.txt` (تحقق بـ `sha256sum -c`).

---

## 9- الأمان

- دخول JWT مزدوج (`1h/7d`) في كوكيز `httpOnly` + CSRF مزدوج + إبطال عبر `tokenVersion` + قفل تدريجي بعد 5 محاولات.
- الخادم يرفض الإقلاع بأسرار ضعيفة (`assertStrongSecrets`) — ولّدها بـ `generate-secrets.js`.
- تنفيذ حلول الطلاب في `worker_threads` معزولة بحدود (64KB مخرجات/5s) وفشل مغلق.
- لا أسرار في المستودع: `.env` محجوب، والمتتبع فقط `.env.example`.

---

## 10- التوثيق

- ابدأ من `PROJECT_UNDERSTANDING.md` ثم `PROJECT_INDEX.md`.
- عقود اللغة: `Docs/Specs/` — وخط البناء: `Docs/Reference/BUILD_PIPELINE.md`.
- الأدلة الحية: `Operational-Evidence/`.

---

## 11- المساهمة والترخيص

- الرخصة: `Studio/LICENSE.txt`.
- أضف اختبارًا في `Tests/real_regression.sh` لكل إصلاح، ووثق الدليل في `Operational-Evidence/`.
- المصدر الوحيد للواجهة `Studio/Web` والسيرفر `Studio/Full/server` — لا تعدل المرايا يدويًا
  (`desktop-app` مربوطة برابط رمزي ونسخ بناء).
