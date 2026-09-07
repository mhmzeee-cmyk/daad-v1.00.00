# فهرس الملفات — ض ستوديو

## دليل سريع

هذا الملف يشرح كل ملف في المشروع ووظيفته وعلاقته بالملفات الأخرى.

---

## 1. Daad-Compiler/ — مترجم لغة ض إلى C++20

### الملفات الرئيسية

| الملف | الأسطر | الوصف | الاستخدام |
|---|---|---|---|
| `CMakeLists.txt` | 193 | إعدادات CMake + CPack | بناء المشروع |
| `Makefile` | — | Makefile بديل | بناء بديل |
| `src/main.cpp` | 130 | نقطة الدخول CLI | `daad-compiler.exe ملف.ض -o output.cpp` |
| `src/Compiler.cpp` | ~150 | محرك الترجمة الرئيسي | يربط Lexer→Parser→CodeGen |
| `src/Lexer.cpp` | 233 | المحلل اللفظي | يحول النص إلى رموز (tokens) |
| `src/Parser.cpp` | 1670 | المحلل الهابط | يبني شجرة الأooktor (AST) |
| `src/CodeGen.cpp` | 840 | مولّد الكود | يحول AST إلى C++20 |
| `src/Keywords.cpp` | ~800 | السجلات المحجوزة | 103 كلمة عربية → أنواع C++ |
| `src/Optimizer.cpp` | 106 | المحسّن | طي الثوابت فقط |
| `src/SandboxValidator.cpp` | 235 | فحص الأمان | يحظر include:: خطرة واستدعاءات النظام |
| `src/AST.cpp` | ~50 | تعريفات AST | فئات شجرة الأooktor |
| `src/Diagnostics.cpp` | ~40 | محرك الأخطاء | يجمع الأخطاء والتحذيرات |
| `src/Token.cpp` | ~10 | تعريف الرموز | فئة الرمز الأساسية |
| `src/UnicodeUtils.cpp` | ~120 | أدوات Unicode | UTF-8 ↔ codepoints |

### المجلدات الفرعية

| المجلد | المحتوى |
|---|---|
| `src/_backup_original/` | نسخ احتياطية من الكود الأصلي |
| `examples/` | 50+ مثال (.ض + .cpp) |
| `examples/conversions/simple/` | أمثلة بسيطة (قبل/بعد الترجمة) |
| `examples/conversions/complex/` | أمثلة معقدة (قبل/بعد الترجمة) |
| `examples/programs/large/` | برامج كبيرة (5 ملفات) |
| `examples/programs/mega/` | برامج ضخمة (5 ملفات) |
| `examples/programs/cpp_large/` | نسخ C++ للبرامج الكبيرة |
| `examples/programs/cpp_mega/` | نسخ C++ للبرامج الضخمة |

### أمثلة بارزة

| الملف | الوصف | الميزات |
|---|---|---|
| `examples/basic.ض` | أساسي | متغيرات، طباعة |
| `examples/loops.ض` | حلقات | لكل، طالما، كرر |
| `examples/functions.ض` | دوال | تعريف + استدعاء |
| `examples/classes.ض` | فئات | OOP |
| `examples/flappy.ض` | لعبة Flappy Bird | رسوميات، فيزياء |
| `examples/snake.ض` | لعبة Snake | شبكة، تصادم |
| `examples/space-invaders.ض` | Space Invaders | أعداء، إطلاق |
| `examples/paint.ض` | برنامج رسم | أشكال، ألوان |
| `examples/particles.ض` | تأثيرات جسيمات | فيزياء، جاذبية |
| `examples/zombie-game.ض` | لعبة زومبي | RPG نصي كامل |

---

## 2. cpu/ — معالج ض + مجمّع + محاكي

### الملفات الرئيسية

| الملف | الأسطر | الوصف | الاستخدام |
|---|---|---|---|
| `src/main.c` | 1571 | المعالج + المجمّع + المحاكي + المصحح | `./dhad_cpu ملف.ضasm --state` |
| `src/dhad_asm.c` | ~900 | مكتبة المجمّع الثنائية | مروران: تسميات ثم توليد |
| `src/dhad_cpu.c` | ~500 | نواة المعالج (4-بت) | تنفيذ التعليمات |
| `src/dhad_debug.c` | ~800 | المصحح التفاعلي | نقاط توقف، مراقبة، تتبع |
| `src/dhad_live.cpp` | ~1200 | محاكي تيرمنال حي | C++، 8-بت، 64K ذاكرة |
| `src/visual_sim.c` | ~1000 | محاكي تيرمنال مرئي | ألوان ANSI |
| `gui/main.cpp` | ~2000+ | واجهة Qt الرسومية | IDE كامل |
| `dhad_gui.pro` | — | ملف مشروع Qt | qmake |
| `Makefile` | — | ملف البناء المولّد | qmake |

### ملفات الرأس

| الملف | الوصف |
|---|---|
| `include/dhad_isa_constants.h` | ثوابت ISA الحالية (8-بت، 64K ذاكرة) |
| `include/dhad_isa.h` | ISA القديمة (4-بت، 4096 ذاكرة) |
| `include/dhad_asm.h` | هياكل بيانات المجمّع |
| `include/dhad_debug.h` | أنواع المصحح |

### الاختبارات

| المجلد | العدد | الوصف |
|---|---|---|
| `tests/` | 96 ملف | اختبارات تلقائية |
| `tests/full_T*.ضasm` | ~70 | اختبارات تغطية شاملة |
| `tests/bug_*.ضasm` | ~26 | اختبارات إصلاح أخطاء |
| `run_tests.sh` | — | سكربت تشغيل الاختبارات |

### الأمثلة

| الملف | الوصف |
|---|---|
| `examples/add.ضasm` | جمع 3 + 5 = 8 |
| `examples/countdown.ضasm` | عد تنازلي 5 إلى 1 |
| `examples/factorial.ضasm` | مضروب (حلقة تكرارية) |
| `examples/function.ضasm` | نداء دالة (ضرب) |
| `examples/hello.ضasm` | طباعة أحرف ASCII |
| `examples/logic.ضasm` | عمليات منطقية (و، أو، ليس) |

### نسخة Desktop

| المجلد | المحتوى |
|---|---|
| `desktop/dhad_cpu.exe` | المترجم والمحاكي (ويندوز) |
| `desktop/شغّل..bat` | ملف التشغيل |
| `desktop/README.txt` | التوثيق |
| `desktop/examples/` | 9 أمثلة |

---

## 3. Dhad-Studio-Full/ — المنصة الكاملة

### السيرفر

| الملف | الأسطر | الوصف |
|---|---|---|
| `server/package.json` | 70 | التبعيات |
| `server/src/index.js` | 462 | نقطة الدخول + Security Stack |
| `server/src/cluster.js` | ~40 | وضع التجميع |
| `server/src/router/index.js` | 62 | تسجيل المسارات |
| `server/src/config/swagger.js` | ~300 | توثيق API |

### Routes (مسارات API)

| الملف | الأسطر | الوصف |
|---|---|---|
| `server/src/routes/auth.js` | 63 | مصادقة + حدود |
| `server/src/routes/challenges.js` | 70 | تحديات + HMAC |
| `server/src/routes/student.js` | 685 | مسارات الطالب الكاملة |
| `server/src/routes/security.js` | 81 | لوحة الأمان |
| `server/src/routes/analytics.js` | ~80 | التحليلات |
| `server/src/routes/teacher.js` | ~120 | أدوات المعلم |
| `server/src/routes/courses.js` | ~30 | الدورات |
| `server/src/routes/schools.js` | ~50 | المدارس |
| `server/src/routes/onboard.js` | ~40 | إعداد المدارس |
| `server/src/routes/otp.js` | ~30 | OTP |
| `server/src/routes/invitations.js` | ~25 | الدعوات |
| `server/src/routes/reports.js` | ~40 | التقارير |
| `server/src/routes/studentManagement.js` | ~100 | إدارة الطلاب |

### Middlewares (وسيطات الأمان)

| الملف | الأسطر | الوصف |
|---|---|---|
| `server/src/middlewares/auth.js` | 449 | JWT + تفويض + HMAC + كشف غش |
| `server/src/middlewares/strictSecurity.js` | 554 | Rate limit + حظر + تنظيف |
| `server/src/middlewares/apiSecurity.js` | ~250 | أمان API |
| `server/src/middlewares/security.js` | ~200 | أمان عام |
| `server/src/middlewares/cache.js` | ~200 | التخزين المؤقت |
| `server/src/middlewares/csrf.js` | ~130 | حماية CSRF |
| `server/src/middlewares/errorHandler.js` | ~80 | معالج الأخطاء |

### Controllers (تحكمات)

| الملف | الأسطر | الوصف |
|---|---|---|
| `server/src/controllers/authController.js` | 712 | تسجيل دخول + تحديث + إعادة تعيين |
| `server/src/controllers/challengeController.js` | 747 | تحديات + HMAC verification |
| `server/src/controllers/analyticsController.js` | ~1200 | تحليلات شاملة |
| `server/src/controllers/assessmentController.js` | ~1200 | امتحانات |
| `server/src/controllers/studentManagementController.js` | ~900 | إدارة الطلاب |
| `server/src/controllers/studentProfileController.js` | ~900 | ملفات الطلاب |
| `server/src/controllers/courseController.js` | ~300 | دورات |
| `server/src/controllers/reportsController.js` | ~400 | تقارير |
| `server/src/controllers/schoolController.js` | ~250 | مدارس |
| `server/src/controllers/onboardController.js` | ~350 | إعداد |
| `server/src/controllers/healthController.js` | ~100 | صحة السيرفر |
| `server/src/controllers/otpController.js` | ~100 | OTP |

### Utils (أدوات)

| الملف | الأسطر | الوصف |
|---|---|---|
| `server/src/utils/dhadCompiler.js` | 126 | مترجم ض للسيرفر |
| `server/src/utils/dhadSandbox.js` | 273 | تنفيذ محكوم في VM |
| `server/src/utils/serverEvaluator.js` | 423 | تقييم ذكي + كشف غش |
| `server/src/utils/codeVerifier.js` | 178 | فحص هيكل الكود |
| `server/src/utils/dhad/lexer.js` | ~800 | محلل لفظي (JavaScript) |
| `server/src/utils/dhad/parser.js` | ~1800 | محلل هابط (JavaScript) |
| `server/src/utils/dhad/codegen.js` | ~1500 | مولّد JavaScript |
| `server/src/utils/dhad/ast.js` | ~600 | تعريفات AST |
| `server/src/utils/encryption.js` | ~250 | تشفير |
| `server/src/utils/logger.js` | ~250 | تسجيل |
| `server/src/utils/redis.js` | ~350 | Redis |
| `server/src/utils/prisma.js` | ~50 | Prisma client |
| `server/src/utils/sessionStore.js` | ~250 | إدارة الجلسات |

### قاعدة البيانات

| الملف | الأسطر | الوصف |
|---|---|---|
| `server/prisma/schema.prisma` | 623 | 25 نموذج |
| `server/prisma/schema.postgresql.prisma` | ~600 | نسخة PostgreSQL |
| `server/prisma/dev.db` | — | قاعدة SQLite (24MB) |
| `server/prisma/seed.js` | ~120 | بيانات تجريبية |
| `server/prisma/seed_admin.js` | ~50 | حساب المدير |
| `server/prisma/seed_challenges.js` | ~80 | تحديات |
| `server/prisma/seed_ui_test.js` | ~150 | اختبار الواجهة |
| `server/prisma/fix_tiers.js` | ~100 | إصلاح المستويات |
| `server/prisma/migrations/` | — | ترحيلات قاعدة البيانات |

### الواجهة

| المجلد | المحتوى |
|---|---|
| `frontend/` | ملفات الواجهة (يُخدمها السيرفر) |
| `bridge/` | Qt bridge لإدارة المدارس |

---

## 4. Dhad-Studio-Web/ — واجهة الويب المستقلة

### ملفات الجذر

| الملف | الأسطر | الوصف |
|---|---|---|
| `index.html` | ~1000 | الصفحة الرئيسية |
| `login.html` | ~400 | تسجيل الدخول |
| `register.html` | ~300 | التسجيل |
| `welcome.html` | ~200 | ترحيب |
| `activate.html` | ~300 | تفعيل الحساب |
| `forgot-password.html` | ~150 | نسيان كلمة المرور |
| `book.html` | ~23000 | كتاب/info |
| `solutions.html` | ~700 | الحلول |
| `sw.js` | ~100 | Service Worker (PWA) |
| `package.json` | 15 | بيانات الحزمة |
| `_redirects` | 5 | قواعد إعادة التوجيه |
| `icons.svg` | ~300 | أيقونات SVG |

### CSS

| الملف | الوصف |
|---|---|
| `css/style.css` | التنسيقات الأساسية |
| `css/themes.css` | 5 موضوعات (terminal، purple، dark، light، ocean) |
| `css/animations.css` | رسوم متحركة |
| `css/decorations.css` | زخارف هندسية إسلامية |

### JavaScript الأساسي

| الملف | الأسطر | الوصف |
|---|---|---|
| `js/dhad.js` | ~300 | المترجم الرئيسي |
| `js/dhad-lexer.js` | ~800 | محلل لفظي (103 كلمة) |
| `js/dhad-parser.js` | ~1200 | محلل هابط (32 نوع عقدة) |
| `js/dhad-codegen.js` | ~1000 | مولّد JavaScript |
| `js/dhad-ast.js` | ~400 | تعريفات AST |
| `js/dhad-highlight.js` | ~200 | تلوين صيغة الكود |
| `js/dhad-libraries.js` | ~2000 | مكتبة معيارية (600+ دالة) |
| `js/dhad-images.js` | ~300 | معالجة صور |

### JavaScript البنية التحتية

| الملف | الوصف |
|---|---|
| `js/api.js` | عميل API + JWT |
| `js/auth.js` | مصادقة |
| `js/login-page.js` | صفحة الدخول |
| `js/sidebar.js` | الشريط الجانبي |
| `js/mobile-nav.js` | التنقل المحمول |
| `js/theme-switcher.js` | محوّل الموضوعات |
| `js/toast.js` | إشعارات |
| `js/watermark.js` | علامة مائية |
| `js/smart-evaluator.js` | تقييم ذكي (كلاينت) |

### الصفحات

| الصفحة | الوصف |
|---|---|
| `pages/dhad-editor.html` | محرر كود ض |
| `pages/web-editor.html` | محرر CSS/HTML |
| `pages/student-dashboard.html` | لوحة تحكم الطالب |
| `pages/teacher-dashboard.html` | لوحة تحكم المعلم |
| `pages/challenges.html` | التحديات (7 مستويات) |
| `pages/roadmap.html` | خارطة الطريق |
| `pages/course-roadmap.html` | خارطة الدورة |
| `pages/additional-courses.html` | دورات إضافية |
| `pages/assessments.html` | الامتحانات |
| `pages/classrooms.html` | الفصول |
| `pages/students.html` | إدارة الطلاب |
| `pages/reports.html` | التقارير |
| `pages/leaderboard.html` | لوحة المتصدرين |
| `pages/profile.html` | الملف الشخصي |
| `pages/settings.html` | الإعدادات |
| `pages/achievements.html` | الإنجازات |
| `pages/terms.html` | شروط الاستخدام |
| `pages/privacy.html` | سياسة الخصوصية |

### JavaScript الصفحات

| الملف | الأسطر | الوصف |
|---|---|---|
| `js/student-dashboard.js` | 253 | لوحة تحكم الطالب |
| `js/teacher-dashboard.js` | 140 | لوحة تحكم المعلم |
| `js/challenges-page.js` | 392 | صفحة التحديات |
| `js/roadmap-page.js` | 173 | خارطة الطريق |
| `js/course-roadmap-page.js` | 326 | خارطة الدورة |
| `js/assessments-page.js` | 472 | الامتحانات |
| `js/students-page.js` | 257 | إدارة الطلاب |
| `js/classrooms-page.js` | 95 | الفصول |
| `js/additional-courses-page.js` | 82 | دورات إضافية |
| `js/reports-page.js` | 161 | التقارير |
| `js/leaderboard-page.js` | 26 | لوحة المتصدرين |
| `js/profile-page.js` | 33 | الملف الشخصي |
| `js/settings-page.js` | 60 | الإعدادات |
| `js/achievements-page.js` | 26 | الإنجازات |

---

## 5. Dhad-Studio-Electron/ — تطبيق سطح المكتب

| الملف | الأسطر | الوصف |
|---|---|---|
| `main.js` | 189 | نقطة الدخول + السيرفر + النافذة |
| `preload.js` | 10 | أمان الاتصال |
| `splash.html` | 88 | شاشة البداية |
| `titlebar.html` | 22 | شريط عنوان |
| `titlebar.css` | 68 | تنسيقات شريط العنوان |
| `package.json` | 48 | التبعيات |
| `src/include/ChallengeSecurityManager.h` | 125 | فحص HMAC |

---

## 6. VSCode-Extension/ — إضافة VS Code

| الملف | الوصف |
|---|---|
| `package.json` | وصف الإضافة |
| `language-configuration.json` | إعدادات المحرر |
| `tsconfig.json` | إعدادات TypeScript |
| `syntaxes/daad.tmLanguage.json` | قواعد التلوين |
| `snippets/daad.code-snippets` | 6 قصاصات |
| `src/extension.ts` | نقطة التنشيط |

---

## 7. desktop/ — نسخة ويندوز المحمولة

| الملف | الوصف |
|---|---|
| `dhad_cpu.exe` | المترجم والمحاكي |
| `شغّل..bat` | ملف التشغيل |
| `README.txt` | التوثيق |
| `examples/add.ضasm` | جمع رقمين |
| `examples/countdown.ضasm` | عد تنازلي |
| `examples/factorial.ضasm` | مضروب |
| `examples/function.ضasm` | نداء دالة |
| `examples/hello.ضasm` | طباعة |
| `examples/logic.ضasm` | عمليات منطقية |
| `examples/test_full.ضasm` | اختبار شامل |

---

## 8. ملفات الجذر

| الملف | الوصف |
|---|---|
| `README.txt` | ملخص المشروع |
| `دليل-التثبيت-والتشغيل.txt` | دليل تفصيلي |
| `بحث_مشروع_معالج_16بت_بمواد_منخفضة_التكلفة.odt` | بحث أكاديمي |

---

## العلاقات بين الملفات

### تدفق الترجمة (Daad → C++)

```
ملف.ض
  ↓
UnicodeUtils.toCodepoints()    ← تحويل UTF-8
  ↓
Lexer.tokenize()               ← تحليل لفظي
  ↓
Keywords.get()                 ← التعرف على الكلمات
  ↓
Parser.parseProgram()          ← بناء AST
  ↓
Optimizer.optimize()           ← طي الثوابت
  ↓
CodeGen.generate()             ← توليد C++
  ↓
SandboxValidator.validate()    ← فحص الأمان
  ↓
output.cpp + output.hpp
```

### تدفق التنفيذ على السيرفر

```
كود الطالب (ض)
  ↓
dhadCompiler.compile()
  ├── Lexer.tokenize()
  ├── Parser.parse()
  └── CodeGen.generate()  → JavaScript
  ↓
dhadSandbox.executeInSandbox()
  ├── vm.createContext()  ← عالم محكوم
  ├── vm.Script.run()    ← تنفيذ (5 ثوانٍ)
  └── __print captured   ← التقاط المخرجات
  ↓
serverEvaluator.evaluate()
  ├── evaluateOutput()     ← مقارنة المخرجات (60)
  ├── evaluateCodeStructure() ← فحص الهيكل (25)
  └── evaluateIntegrity()  ← كشف الغش (15)
  ↓
نتيجة: { passed, score, xpAwarded }
```

### تدفق تنفيذ المعالج

```
ملف.ضasm
  ↓
المرور الأول (dhad_asm.c)
  ├── جمع التسميات (labels)
  ├── معالجة الماكرو
  └── معالجة التوجيهات (.data, .text)
  ↓
المرور الثاني
  ├── توليد الرموز الثنائية
  └── حفظ output.bin
  ↓
cpu_step() (main.c)
  ├── جلب التعليمة من الذاكرة
  ├── فك التشفير
  └── التنفيذ (ALU، نقل، تحكم)
  ↓
النتيجة: ACC + Status
```

---

## حجم الملفات التقريبي

| المكون | عدد الملفات | الحجم التقريبي |
|---|---|---|
| Daad-Compiler | ~25 ملف مصدري | ~200 KB |
| cpu/ | ~20 ملف مصدري | ~500 KB |
| Dhad-Studio-Full/server | ~60 ملف | ~1 MB |
| Dhad-Studio-Web | ~60 ملف | ~500 KB |
| Dhad-Studio-Electron | ~8 ملفات | ~50 KB |
| VSCode-Extension | ~8 ملفات | ~30 KB |
| desktop/ | ~10 ملفات | ~500 KB (exe) |
| **المجموع** | **~200 ملف** | **~3 MB** |
