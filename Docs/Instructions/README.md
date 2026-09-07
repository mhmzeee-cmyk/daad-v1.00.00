# ض ستوديو — Dhad Studio v1.0.0

## منصة تعليم البرمجة باللغة العربية

---

## نظرة عامة

**ض ستوديو** هو نظام كمبيوتر متكامل بالكامل مصمم للتعليم البرمجة بلغة عربية. يتضمن:

- **لغة ض (Dhad)** — لغة برمجة عربية كاملة الميزات (كائنية، تعاملات، قوالب، رسوميات)
- **مترجم ض** — يترجم الكود العربي إلى C++20
- **معالج ض** — معالج مخصص 8-بت مع مجمّع ومtokenizer عربية
- **سيرفر تعليمي** — إدارة مدارس، فصول، طلاب، تحديات، تقييم
- **واجهات متعددة** — ويب، سطح مكتب (Electron)، VS Code، تيرمنال

---

## المكونات

### 1. Daad-Compiler — مترجم لغة ض إلى C++20

مترجم كامل بلغة C++20 يترجم ملفات `.ض` إلى كود C++:

| الملف | الوصف |
|---|---|
| `src/main.cpp` | نقطة الدخول لسطر الأوامر |
| `src/Parser.cpp` | محلل هابط تكراري (1670 سطر) |
| `src/CodeGen.cpp` | مولّد الكود عبر نمط الزائر (840 سطر) |
| `src/Lexer.cpp` | محلل لفظي يدعم UTF-8 (233 سطر) |
| `src/Keywords.cpp` | 103 كلمة محجوزة عربية |
| `src/Optimizer.cpp` | طي الثوابت |
| `src/SandboxValidator.cpp` | فحص أمان الكود المولّد |
| `src/AST.cpp` | تعريفات شجرة الأooktor |
| `src/Diagnostics.cpp` | محرك الأخطاء |
| `src/UnicodeUtils.cpp` | أدوات Unicode |
| `CMakeLists.txt` | نظام البناء |
| `examples/` | 50+ مثال (.ض + .cpp) |

**مميزات لغة ض:**
- أنواع بيانات: صحيح (int)، نص (string)، عشري (double)، حرف (char)، منطقي (bool)
- هيكل تحكم: إذا/وإلا، لكل/طالما/كرر، اختر/حالة
- OOP: صنف/فئة، هيكل، نطاق، قالب، واجهة
- تعاملات: حاول/امسك/أخيراً
- رسوميات: 20 أمر مدمج (ارسم، دائرة، مستطيل، خط، نص)
- معالجة صور: حمّل، قص، غيّر حجم، فلتر، تراكب
- ملفات: افتح، اقرأ، اكتب، أغلق

### 2. CPU Assembler & Simulator — معالج ض 8-بت

محاكي معالج مخصص مع مجمّع عربية:

| الملف | الوصف |
|---|---|
| `src/main.c` | المعالج + المجمّع + المحاكي (1571 سطر) |
| `src/dhad_asm.c` | مكتبة المجمّع الثنائية (مرورين) |
| `src/dhad_cpu.c` | نواة المعالج (استخدام 4-بت) |
| `src/dhad_debug.c` | المصحح (نقاط توقف، مراقبة) |
| `src/dhad_live.cpp` | محاكي تيرمنال حي (C++) |
| `src/visual_sim.c` | محاكي تيرمنال مرئي |
| `gui/main.cpp` | واجهة Qt الرسومية |
| `tests/` | 96 اختبار تلقائي |
| `examples/` | أمثلة جاهزة |

**مواصفات المعالج:**
- 8-بت بيانات، 16-بت عناوين
- 65,536 بايت ذاكرة
- 8 سجلات عامة (س0-س7) + مُراكم (مح)
- أعلام: Z (صفر)، N (سالب)، C (حمل)
- 32 عنصر مكدس
- 30+ تعليمة (حسابية، منطقية، نقل، تحكم، مكدس، إدخال/إخراج)

### 3. Dhad-Studio-Full — المنصة الكاملة

سيرفر Node.js + واجهة ويب + جسر Qt:

```
server/
├── src/index.js              — نقطة دخول السيرفر (462 سطر)
├── src/router/index.js       — تسجيل المسارات
├── src/routes/               — مسارات API
│   ├── auth.js               — تسجيل الدخول/الخروج
│   ├── challenges.js         — إدارة التحديات
│   ├── student.js            — مسارات الطالب (685 سطر)
│   ├── security.js           — لوحة الأمان
│   ├── analytics.js          — التحليلات
│   └── ...                   — مسارات أخرى
├── src/middlewares/          — وسيطات الأمان
│   ├── auth.js               — JWT + تفويض (449 سطر)
│   └── strictSecurity.js    — حماية شاملة (554 سطر)
├── src/controllers/          — متحكمات المنطق
│   ├── authController.js     — مصادقة (712 سطر)
│   ├── challengeController.js — تحديات (747 سطر)
│   └── ...                   — تحكمات أخرى
├── src/utils/                — أدوات مساعدة
│   ├── dhadCompiler.js       — مترجم ض للسيرفر
│   ├── dhadSandbox.js        — تنفيذ محكوم في VM
│   ├── serverEvaluator.js    — تقييم ذكي (423 سطر)
│   └── codeVerifier.js       — فحص هيكل الكود
└── prisma/
    ├── schema.prisma         — نموذج قاعدة البيانات (623 سطر، 25 جدول)
    └── dev.db                — قاعدة SQLite
```

**amos الميزات:**
- مصادقة JWT مع إصدارات + إلغاء
- تنفيذ ض في VM معزول (5 ثوانٍ، 32MB heap)
- تقييم تلقائي: مخرجات (60) + هيكل (25) + سلامة (15)
- كشف غش: كود قصير، تنفيذ مستحيل، تثبيت صيغة
- Rate limiting + حظر حساب + حظر IP

### 4. Dhad-Studio-Web — واجهة الويب

واجهة HTML/CSS/JS كاملة (22 صفحة):

| الصفحة | الوصف |
|---|---|
| `index.html` | الصفحة الرئيسية |
| `login.html` | تسجيل الدخول |
| `register.html` | التسجيل |
| `pages/student-dashboard.html` | لوحة تحكم الطالب |
| `pages/teacher-dashboard.html` | لوحة تحكم المعلم |
| `pages/challenges.html` | صفحة التحديات (7 مستويات) |
| `pages/roadmap.html` | خارطة الطريق |
| `pages/dhad-editor.html` | محرر الكود |
| `pages/assessments.html` | الامتحانات |
| `pages/classrooms.html` | الفصول |
| `pages/students.html` | إدارة الطلاب |
| `pages/reports.html` | التقارير |
| `pages/leaderboard.html` | لوحة المتصدرين |
| `pages/profile.html` | الملف الشخصي |
| `pages/settings.html` | الإعدادات |
| `pages/achievements.html` | الإنجازات |

**محرك ض على الويب:**
- `js/dhad.js` — المترجم الرئيسي
- `js/dhad-lexer.js` — محلل لفظي (103 كلمة)
- `js/dhad-parser.js` — محلل هابط تكراري (32 نوع عقدة)
- `js/dhad-codegen.js` — مولّد JavaScript
- `js/dhad-ast.js` — تعريفات شجرة الأooktor
- `js/dhad-libraries.js` — مكتبة معيارية (600+ دالة)
- `js/dhad-highlight.js` — تلوين صيغة الكود

### 5. Dhad-Studio-Electron — تطبيق سطح المكتب

تطبيق Electron يجمع السيرفر + الواجهة:

| الملف | الوصف |
|---|---|
| `main.js` | نقطة الدخول (189 سطر) |
| `preload.js` | أمان الاتصال |
| `splash.html` | شاشة البداية |
| `titlebar.html/css` | شريط عنوان مخصص |
| `src/include/ChallengeSecurityManager.h` | فحص HMAC-SHA256 |

### 6. VSCode-Extension — إضافة VS Code

| الملف | الوصف |
|---|---|
| `package.json` | وصف الإضافة |
| `syntaxes/daad.tmLanguage.json` | قواعد التلوين |
| `snippets/daad.code-snippets` | قصاصات الكود (6 قصاصات) |
| `src/extension.ts` | نقطة تنشيط |
| `language-configuration.json` | إعدادات المحرر |

**القصاصات:**
- `صحيح` — تعريف متغير صحيح
- `كرر` — حلقة تكرارية
- `دالة` — تعريف دالة
- `فئة` — تعريف صنف
- `قالب` — قالب C++
- `طباعة` — طباعة

### 7. Desktop — نسخة ويندوز المحمولة

| الملف | الوصف |
|---|---|
| `dhad_cpu.exe` | المترجم والمحاكي |
| `شغّل..bat` | ملف التشغيل (سحب وإفلات) |
| `README.txt` | التوثيق |
| `examples/` | 9 أمثلة |

---

## مسارات API的主要

### المصادقة
| المسار | الوصف |
|---|---|
| `POST /api/v1/auth/login` | تسجيل الدخول |
| `POST /api/v1/auth/register` | التسجيل |
| `POST /api/v1/auth/refresh` | تحديث الرمز |
| `POST /api/v1/auth/logout` | الخروج |

### التحديات
| المسار | الوصف |
|---|---|
| `GET /api/v1/challenges` | قائمة التحديات |
| `POST /api/v1/student/challenge/submit` | تسليم حل |
| `POST /api/v1/challenge/verify` | توثيق HMAC |

### الطالب
| المسار | الوصف |
|---|---|
| `GET /api/v1/student/profile` | الملف الشخصي |
| `GET /api/v1/student/leaderboard` | لوحة المتصدرين |
| `GET /api/v1/student/achievements` | الإنجازات |
| `POST /api/v1/student/join-classroom` | الانضمام لفصل |

### الأمان
| المسار | الوصف |
|---|---|
| `GET /api/v1/security/dashboard` | لوحة الأمان |
| `GET /api/v1/security/audit` | سجل التدقيق |

---

## قاعدة البيانات

25 نموذج في SQLite عبر Prisma:

| النموذج | الوصف |
|---|---|
| `School` | المدارس |
| `User` | المستخدمون (مدير/معلم/طالب) |
| `Course` | الدورات |
| `Lesson` | الدروس |
| `Challenge` | التحديات (210 تحدي) |
| `Submission` | التسليمات |
| `Classroom` | الفصول (50 فصل) |
| `Assessment` | الامتحانات |
| `StudentProfile` | ملفات الطلاب (XP، مستوى) |
| `Achievement` | الإنجازات |
| `DailyXP` | XP اليومي |
| `ActivityLog` | سجل النشاط |
| `Invitation` | الدعوات |
| `SecurityAlert` | تنبيهات الأمان |
| `CloudWorkspace` | مساحات العمل السحابية |

---

## المتطلبات

### للسيرفر
- Node.js 18+
- npm
- SQLite (مضمونة)

### للواجهة
- متصفح حديث (Chrome، Firefox، Edge)

### للمترجم C++
- CMake 3.21+
- C++20 compiler
- Qt5 5.15+ (اختياري للـ IDE)

### للتطبيق Electron
- Node.js 18+
- Electron 33+

### لـ VS Code
- VS Code 1.80+

### للمعالج
- GCC/Clang (Linux) أو MSVC (Windows)

---

## التشغيل

### السيرفر الكامل
```bash
cd Dhad-Studio-Full/server
npm install --production
npx prisma generate
npx prisma db push
node src/index.js
# يفتح على http://localhost:3000
```

### المترجم C++
```bash
cd Daad-Compiler
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
# daad-compiler.exe ملف.ض -o output.cpp
```

### المعالج
```bash
cd cpu
make
# ./dhad_cpu examples/add.ضasm --state
```

### واجهة الويب
```bash
cd Dhad-Studio-Web
npx serve .
# http://localhost:3000
```

### التطبيق Electron
```bash
cd Dhad-Studio-Electron
npm start
```

---

## بيانات الدخول التجريبية

| الدور | البريد | كلمة المرور |
|---|---|---|
| مدير | admin_1@test.com | Test1234! |
| طالب | sara@test.com | Test1234! |

---

## رخصة

MIT License — محمد محمود الحموز

---

## التواصل

- البريد: support@daad-studio.com
- GitHub: https://github.com/your-repo/dhad-studio
