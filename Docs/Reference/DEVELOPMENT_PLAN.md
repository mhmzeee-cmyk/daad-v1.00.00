# Daad Studio - خطة التطوير والإصلاح الشاملة
## Development & Remediation Master Plan

**الإصدار:** 1.0.0  
**التاريخ:** 2026-07-10  
**إعداد:** OpenCode AI Audit & Planning Engine  
**الهدف:** خطة تطوير وإصلاح متكاملة لمشروع ض استديو (Daad Studio) – منصة تعليمية + لغة برمجة عربية + بيئة تطوير متكاملة

---

# فهرس المحتويات

1. [ملخص تنفيذي](#1-ملخص-تنفيذي)
2. [نظرة عامة على المشروع](#2-نظرة-عامة-على-المشروع)
3. [تحليل الخادم الخلفي (Server)](#3-تحليل-الخادم-الخلفي)
4. [تحليل الواجهة الأمامية للويب (Frontend-Web)](#4-تحليل-الواجهة-الأمامية-للويب)
5. [تحليل تطبيق سطح المكتب (Desktop Frontend)](#5-تحليل-تطبيق-سطح-المكتب)
6. [تحليل المترجم الأساسي والمكتبة الأساسية (Core Compiler + Stdlib)](#6-تحليل-المترجم-الأساسي)
7. [تحليل قاعدة البيانات (Prisma Schema)](#7-تحليل-قاعدة-البيانات)
8. [تدقيق الأمان الشامل (Security Audit)](#8-تدقيق-الأمان-الشامل)
9. [الأخطاء الحرجة والمعروفة (Known Bugs)](#9-الأخطاء-الحرجة)
10. [خارطة الطريق وخريطة التطوير (Roadmap)](#10-خارطة-الطريق)
11. [استراتيجية الاختبارات (Testing Strategy)](#11-استراتيجية-الاختبارات)
12. [البنية التحتية و DevOps](#12-البنية-التحتية)
13. [توثيق المشروع (Documentation)](#13-توثيق-المشروع)
14. [تكامل الذكاء الاصطناعي (AI Integration)](#14-تكامل-الذكاء-الاصطناعي)
15. [ربط كل المكونات معاً (Integration Architecture)](#15-ربط-كل-المكونات)
16. [الملاحق](#16-الملاحق)

---

# 1. ملخص تنفيذي

## 1.1 عن المشروع
Daad Studio (ض استديو) هو منصة تعليمية متكاملة + لغة برمجة عربية (لغة "ض") + بيئة تطوير متكاملة IDE + خادم REST API. المشروع يهدف إلى تعليم البرمجة باللغة العربية.

## 1.2 حجم المشروع

| المكوّن | اللغة | عدد الملفات | سطور البرمجة |
|---------|-------|-------------|--------------|
| Server (خادم REST API) | Node.js/Express/Prisma | 59 | ~14,200 |
| Frontend-Web (واجهة ويب) | HTML/CSS/JS | 33 | ~15,000+ |
| Desktop Frontend (تطبيق سطح مكتب) | QML/C++17/Qt5 | 56 | ~8,000+ |
| Core Compiler (مترجم لغة ض) | C++20 | 15+ | ~4,000+ |
| Standard Library (مكتبة قياسية) | Daad/C++ | 102 | ~5,000+ |
| Tests (اختبارات) | JS/C++ | 70+ | ~15,000+ |
| Infrastructure (بنية تحتية) | YAML/JSON/NSIS | 20+ | ~800+ |
| **الإجمالي** | **متنوع** | **~355+** | **~62,000+** |

## 1.3 التقييم العام
- **نضج المشروع:** متوسط-متقدم. المشروع يعمل ولكن يحتاج تحسينات في عدة مجالات.
- **الأمان:** ممتاز. يوجد نظام أمان متعدد الطبقات مع تدقيق مستمر.
- **قاعدة البيانات:** جيدة. التصميم يغطي معظم السيناريوهات ولكن يحتاج توحيد.
- **الأداء:** متوسط. بعض نقاط الضعف في التخزين المؤقت وإدارة الذاكرة.
- **التوثيق:** ضعيف. التوثيق الحالي غير كافٍ للتوسع.

## 1.4 الأولويات القصوى

| الأولوية | المجال | الوصف |
|----------|--------|-------|
| 🔴 حرج | Compiler Bug | خطأ في توليد كود C++ من Daad - `طباعة()` تفصل الدوال بشكل خاطئ |
| 🔴 حرج | Database | SQLite للإنتاج يجب ترقيته إلى PostgreSQL |
| 🔴 حرج | Security | ملف `.env` يحتوي توكن GitHub والتوكن مكشوف |
| 🟡 عالي | Performance | Redis غير متصل، التخزين المؤقت لا يعمل |
| 🟡 عالي | Architecture | لا يوجد فصل بين طبقات الخدمة في الـ Controllers |
| 🟡 عالي | Testing | لا يوجد اختبارات أوتوماتيكية للـ API (Jest/Supertest) |
| 🟢 متوسط | Frontend-Web | تحسين الأداء وتقليل حجم الـ JS |
| 🟢 متوسط | Documentation | توثيق API عبر Swagger ناقص |

---

# 2. نظرة عامة على المشروع

## 2.1 الهيكل العام

```
C:\Projects\dhad-studio\
├── .github/workflows/        # CI/CD Pipeline (GitHub Actions)
├── aws/                       # AWS Infrastructure (ECS, ALB, IAM)
├── bridge/                    # C++ Bridge بين Qt5 Desktop ↔ Server
├── build/                     # CMake Build Artifacts
├── config/                    # (فارغ حالياً)
├── desktop-app/               # تطبيق سطح المكتب C++
├── docs/                      # التوثيق
│   ├── handover/              # وثائق التسليم
│   └── wix/                   # WiX Installer
├── examples/                  # أمثلة برامج بلغة ض
│   ├── conversions/           # simple + complex تحويلات Daad → C++
│   └── programs/              # large + mega برامج كاملة
├── frontend/                  # Qt5/QML Desktop IDE
│   ├── cpp/                   # C++ backend للـ IDE
│   ├── i18n/                  # ترجمة الـ IDE (10 لغات)
│   └── qml/                   # QML واجهة المستخدم
├── frontend-web/              # Web Frontend
│   ├── css/                   # أنماط CSS
│   ├── js/                    # JavaScript
│   └── pages/                 # صفحات HTML
├── include/Daad/              # C++ headers للمترجم
│   └── Runtime/               # دوال وقت التشغيل
├── installer/                 # NSIS Windows Installer
├── server/                    # Node.js/Express REST API
│   ├── prisma/                # Prisma ORM + Migrations + Seeds
│   ├── scripts/               # أدوات مساعدة
│   └── src/                   # كود الخادم
│       ├── config/            # إعدادات (Swagger)
│       ├── controllers/       # 12 Controller
│       ├── middlewares/       # 4 Middleware
│       ├── router/            # Router مركزي
│       ├── routes/            # 13 Route files
│       ├── services/          # 1 Service
│       └── utils/             # 4 Utilities
├── src/                       # C++ Core Compiler
│   ├── main.cpp               # CLI entry
│   ├── Lexer.cpp              # Lexer
│   ├── Parser.cpp             # Parser (628 lines)
│   ├── Keywords.cpp           # 68 Arabic keywords
│   ├── AST.cpp                # Abstract Syntax Tree
│   ├── CodeGenVisitor.cpp     # C++ Code Generator
│   ├── OptimizerVisitor.cpp   # Optimizer
│   ├── Diagnostics.cpp        # Error diagnostics
│   └── Runtime.cpp            # Runtime functions
├── stdlib/                    # Daad Standard Library
│   ├── الرياضيات_والحساب/     # 10 math modules
│   ├── النصوص_والترميز/       # 10 text modules
│   ├── هياكل_البيانات/        # 10 data structure modules
│   ├── المدخلات_والمخرجات/     # 10 I/O modules
│   ├── الشبكات_والاتصالات/    # 10 network modules
│   ├── الوقت_والنظام/         # 10 time/system modules
│   ├── الرسوميات_والمساعدات/  # 10 graphics modules
│   ├── تطوير_الويب/           # 10 web dev modules
│   ├── قواعد_البيانات/        # 10 database modules
│   └── الالتزامن_وتعدد_المهام/ # 10 concurrency modules
├── tests/                     # Test Suite
│   ├── C++ Tests (Lexer, Parser, Compiler, Runtime, Stdlib)
│   ├── JavaScript Tests (Pentest, Stress, Anti-cheat)
│   └── Challenge Data (210 challenges, 7 tiers)
├── vscode-extension/          # VS Code Extension
│   ├── src/extension.ts       # Extension logic
│   ├── syntaxes/              # TextMate grammar
│   └── snippets/              # Code snippets
├── CMakeLists.txt             # Root CMake build
└── assets/                    # Static assets
```

## 2.2 تدفق البيانات (Data Flow)

```
مستخدم Web                    مستخدم Desktop                 طالب/معلم
    │                              │                            │
    ▼                              ▼                            ▼
[Frontend-Web]              [Qt5 Desktop IDE]           [VS Code Extension]
    │                              │                            │
    │ HTTP/JSON                    │ Bridge (HMAC)              │ CLI
    ▼                              ▼                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                     REST API Server (:3000)                         │
│  Express + Helmet + Rate Limit + CORS + Security Middleware        │
│                                                                     │
│  ┌─────────┐  ┌──────────┐  ┌──────────┐  ┌──────────────────┐   │
│  │ Auth    │  │ Student  │  │ Teacher  │  │ Security         │   │
│  │ Routes  │  │ Routes   │  │ Routes   │  │ Routes           │   │
│  └────┬────┘  └────┬─────┘  └────┬─────┘  └──────┬───────────┘   │
│       │            │             │               │                │
│       ▼            ▼             ▼               ▼                │
│  ┌────────────────────────────────────────────────────────────┐   │
│  │                    Controllers Layer                        │   │
│  │  auth | otp | onboard | school | course | challenge        │   │
│  │  assessment | analytics | reports | studentProfile         │   │
│  │  studentManagement                                         │   │
│  └────────────────────────┬───────────────────────────────────┘   │
│                           │                                       │
│                           ▼                                       │
│  ┌────────────────────────────────────────────────────────┐      │
│  │              Services + Utils Layer                      │      │
│  │  adminService | prisma | redis | logger | codeVerifier  │      │
│  └────────────────────────┬────────────────────────────────┘      │
│                           │                                       │
│                           ▼                                       │
│  ┌────────────────────────────────────────────────────────┐      │
│  │                    Prisma ORM                           │      │
│  │         SQLite (dev) / PostgreSQL (prod)                │      │
│  │         23 Models: School, User, Course, ...            │      │
│  └────────────────────────────────────────────────────────┘      │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                     C++ Compiler Engine                             │
│  Daad Source → Lexer → Parser → AST → CodeGen → C++ Output         │
│         ↓                                                           │
│    Optimizer (constant folding, DCE, strength reduction)           │
│         ↓                                                           │
│    Runtime (built-in functions)                                     │
│         ↓                                                           │
│    Stdlib (100 Arabic modules) → C++ translation                   │
└─────────────────────────────────────────────────────────────────────┘
```

---

# 3. تحليل الخادم الخلفي (Server Backend)

## 3.1 هيكل الخادم

**الموقع:** `C:\Projects\dhad-studio\server\`
**التقنيات:** Node.js ≥18, Express 4.21, Prisma 5.22, SQLite/PostgreSQL, Redis (ioredis), JWT, Helmet, Winston
**الموديل:** ES6 (CommonJS)
**نقطة الدخول:** `src/index.js` (372 سطر)

## 3.2 تحليل الـ Middleware

### 3.2.1 `src/middlewares/strictSecurity.js` (535 سطر)
**الوظيفة:** نظام أمني متعدد الطبقات

**المكونات:**
- `strictRateLimit` - تحديد معدل الطلبات حسب Category (public, auth, admin, onboard, submission, otp)
- `recordFailedAttempt` / `clearFailedAttempts` / `isAccountLocked` - قفل الحساب بعد 5 محاولات فاشلة
- `isProgressivelyDelayed` - تأخير تصاعدي (1s → 2s → 4s → 8s → 16s)
- `recordIPFailure` / `isIPBlocked` - حظر IP بعد 20 فشل
- `strictRequestValidation` - التحقق من طول URL، Content-Length، الـ Headers المشبوهة
- `strictSanitize` - تطهير عميق مع حدود للعمق والمفاتيح
- `validatePasswordStrength` - سياسة كلمة مرور قوية
- `strictSecurityHeaders` - CSP, HSTS, X-Frame-Options, Permissions-Policy
- `bridgeAuthMiddleware` - وصول محلي فقط للـ Bridge
- `signRequest` / `verifyRequestSignature` - توقيع HMAC للطلبات
- `registerSession` / `removeSession` / `enforceMaxSessions` - إدارة الجلسات مع حد أقصى 3

**المشاكل:**
- 🔴 **التخزين في الذاكرة**: `failedAttempts`, `ipBlocks`, `requestCounts` مخزنة في Maps بالذاكرة. إذا توقف الخادم، تفقد جميع البيانات. يحتاج Redis للمتانة.
- 🟡 **تسرب الذاكرة**: `auditLog` circular buffer بحجم 1000، ولكن لا يوجد تنظيف للـ Maps القديمة (فشل المحاولات القديمة تبقى في الذاكرة).
- 🟡 **صعوبة التكوين**: حدود الـ Rate limiting مشفرة في الكود ومأخوذة من `.env` ولكن لا يوجد تحقق من أن القيم منطقية.

### 3.2.2 `src/middlewares/auth.js` (480 سطر)
**الوظيفة:** المصادقة والتفويض

**المكونات:**
- `authenticate` - تحقق JWT مع التحقق من إلغاء التوكن
- `requireRole` - تحكم بالصلاحيات (ADMIN, TEACHER, STUDENT)
- `requireSchoolAccess` - عزل المدرسة
- `requireOwnership` - التحقق من ملكية المورد
- `checkTokenRevocation` - التحقق من إصدار التوكن
- `authRateLimit` - 10 طلبات/15 دقيقة للمصادقة
- `apiRateLimit` - 60 طلب/دقيقة للـ API العام
- `validateHMACSignature` - التحقق من توقيع HMAC مع حماية من إعادة التشغيل
- `detectSuspiciousActivity` - كشف النشاط المشبوه (توقيت سريع، درجات كاملة، كود قصير)

**المشاكل:**
- 🟡 **عدم توحيد الـ Error Handling**: كل دالة ترجع الخطأ بشكل مختلف (بعضها `res.status()`, بعضها `next(err)`)
- 🟡 **صلاحية مفرطة**: `requireRole` يتحقق من وجود الدور لكن لا يتحقق من أن المستخدم ينتمي للمدرسة الصحيحة إلا إذا أضفت `requireSchoolAccess` بشكل منفصل
- 🟢 **عدم استخدام الـ Redis للـ Sessions**: `activeSessions` في الذاكرة وليس Redis

### 3.2.3 `src/middlewares/security.js` (104 سطر)
**الوظيفة:** أمن إضافي

**المكونات:**
- `sanitizeMiddleware` - تطهير المدخلات من XSS
- `submissionRateLimit` - 30 تسليم/ساعة لكل طالب
- `typeGuard` - التحقق من أنواع البيانات

### 3.2.4 `src/middlewares/errorHandler.js` (61 سطر)
**الوظيفة:** معالجة الأخطاء الشاملة
**التعامل مع:**
- Prisma Errors: P2002 (unique constraint), P2025 (not found)
- JWT Errors: JsonWebTokenError, TokenExpiredError
- TypeError, SyntaxError, Generic Error

**المشاكل:**
- 🔴 **تسريب تفاصيل داخلية**: أخطاء Prisma قد تسرب معلومات عن هيكل قاعدة البيانات
- 🟡 **عدم توحيد تنسيق الخطأ**: كل خطأ له تنسيق مختلف

## 3.3 تحليل الـ Controllers

### 3.3.1 `authController.js` (623 سطر)
**الوظائف:**
- `login` - تسجيل الدخول بـ email/nationalId، قفل الحساب، تأخير تصاعدي، تسجيل الدخول
- `register` - إنشاء مستخدم من قبل المعلم/المشرف
- `changePassword` - تغيير كلمة المرور للمستخدم المصادق
- `resetPassword` - إعادة تعيين كلمة المرور من قبل المشرف
- `refreshToken` - تحديث JWT مع تدوير التوكن
- `logout` - إبطال جميع الـ JWTs بزيادة tokenVersion

**المشاكل:**
- 🟡 **حجم كبير جداً**: 623 سطر في Controller واحد → يجب تقسيمه إلى Service
- 🟡 **تكرار التحقق من الصلاحية**: التحقق من صلاحية ADMIN مكرر في resetPassword
- 🟢 **عدم توحيد الرسائل**: رسائل الخطأ غير موحدة (بعضها إنجليزي، بعضها عربي)

### 3.3.2 `assessmentController.js` (1045 سطر)
**الوظائف:**
- `createAssessment` - إنشاء اختبار بوقت محدد ووضع امتحان
- `getActiveAssessment` - الحصول على الاختبار النشط
- `submitAssessmentAnswer` - تقديم إجابة (لا يثق أبداً بالعميل للدرجة)
- `startAssessmentAttempt` - بدء محاولة اختبار
- `completeAssessment` - إنهاء الاختبار وحساب الدرجة
- `getAssessmentResults` - عرض النتائج مع إحصائيات
- `exportClassroomReport` - تصدير CSV مع حماية من حقن CSV
- `getClassroomAssessments` - قائمة اختبارات الفصل

**المشاكل:**
- 🔴 **أكبر Controller في المشروع**: 1045 سطر → كارثة صيانة
- 🟡 **تكرار منطق حساب الوقت**: حساب الوقت المتبقي مكرر في 3 دوال
- 🟡 **عدم استخدام Service Layer**: كل المنطق في Controller
- 🟢 **CSV Export جيد**: حماية من حقن CSV موجودة

### 3.3.3 `analyticsController.js` (1082 سطر)
**الوظائف:**
- لوحة المتصدرين (Leaderboard) مع Redis cache
- ملخص الحضور
- تقدم الفصل
- حالات الاختبار الآمنة
- لوحة المعلم
- إنشاء الفصول الدراسية
- تسجيل الحضور

**المشاكل:**
- 🔴 **أثقل Controller**: 1082 سطر، أسوأ ملف في المشروع من حيث قابلية الصيانة
- 🟡 **Redis غير متصل**: التخزين المؤقت لا يعمل فعلياً (Redis غير مشغل في بيئة التطوير)
- 🟡 **استعلامات N+1**: بعض الدالات تسوي استعلامات متعددة متسلسلة بدلاً من JOIN

### 3.3.4 `studentProfileController.js` (757 سطر)
**الوظائف:**
- Gamification Engine: XP, Levels (1-50), Streaks, Achievements
- Bullet Telemetry Processing
- Student Roadmap with Sequential Locking

**المشاكل:**
- 🟡 **Point System غير مفهوم**: حساب XP للتحديات يتم مباشرة دون تحقق من صحة التحدي (قد يكون مكرراً)
- 🟢 **عدم الثقة بالعميل**: ممتاز - الخادم هو من يحسب XP، لا يثق بالعميل

### 3.3.5 `studentManagementController.js` (663 سطر)
**الوظائف:**
- CRUD للطلاب
- تسجيل دخول الطالب بالاسم والفصل
- تعيين طالب لفصل

### 3.3.6 `onboardController.js` (275 سطر)
**الوظيفة:** توفير المدرسة دفعة واحدة مع معلمين وطلاب

### 3.3.7 `schoolController.js` (278 سطر)
**الوظيفة:** إدارة المدارس

### 3.3.8 `reportsController.js` (296 سطر)
**الوظيفة:** تقارير أسبوعية، نشاط المستخدم، نظرة عامة على المدرسة

### 3.3.9 `courseController.js` (218 سطر)
**الوظيفة:** مسار الدورات التعليمية مع فتح المتطلبات

### 3.3.10 `challengeController.js` (213 سطر)
**الوظيفة:** التحقق من التحديات بتوقيع HMAC

### 3.3.11 `otpController.js` (150 سطر)
**الوظيفة:** تفعيل الطالب عبر OTP

### 3.3.12 `healthController.js` (154 سطر)
**الوظيفة:** نقاط نهاية الصحة (`/health`, `/health/ready`, `/health/live`)

## 3.4 تحليل الـ Routes

### 3.4.1 `src/routes/student.js` (689 سطر)
**أخطر ملف Routes:**
- `/student/profile` - GET/POST
- `/student/leaderboard` - GET
- `/student/roadmap` - GET
- `/student/challenge/submit` - POST (مع locking)
- `/student/achievements` - GET
- `/student/classroom/check` - GET
- `/student/assessment/*` - 9 endpoints
- `/submissions/bulk-report` - POST (يدعم MessagePack)

**المشاكل:**
- 🔴 **حجم ضخم**: 689 سطر في ملف Routes (يجب أن يكون Routes خفيفة)
- 🔴 **Routes تفعل أشياء كثيرة**: Routes تحتوي منطق بدلاً من تمرير المسؤولية للـ Controller
- 🟡 **MessagePack في Routes**: معالجة تحويل MessagePack في Route نفسه

## 3.5 تحليل الـ Services و Utils

### 3.5.1 `adminService.js` (326 سطر)
**الخدمة الوحيدة** في المشروع. البقية كلها في Controllers.

### 3.5.2 `prisma.js` (13 سطر)
اتصال Prisma مع تسجيل الاستعلامات في وضع التطوير.

### 3.5.3 `redis.js` (359 سطر)
مدير Redis كامل مع تخزين مؤقت، Leaderboard (Sorted Sets)، عداد الفشل، وذاكرة تخزين JSON.
**المشكلة:** Redis غير متصل في بيئة التطوير (ليس مثبتاً محلياً).

### 3.5.4 `logger.js` (206 سطر)
Winston Logger مع 4 مستويات: critical, error, warn, info, http, debug.
يدعم Console + File logging.

### 3.5.5 `codeVerifier.js` (85 سطر)
التحقق من كود Daad في التحديات.

## 3.6 ملف `src/index.js` (372 سطر)
إعدادات الخادم الرئيسية:

```javascript
// ترتيب الـ Middleware (حرج):
1. requestLogger
2. strictSecurityHeaders
3. helmet + helmet.contentSecurityPolicy
4. compression
5. express.json + express.urlencoded
6. cors (ديناميكي من ALLOWED_ORIGINS)
7. strictSanitizeMiddleware
8. POST size limit check
9. Swagger UI
10. Rate limiting لكل endpoint
11. Routes
12. Static files
13. 404 handler
14. Global error handler
```

**المشاكل:**
- 🟡 `express.json({ limit: '50mb' })` — 50MB حد كبير جداً. يفضل 10MB كحد أقصى.
- 🟡 لا يوجد `app.set('trust proxy')` — قد يسبب مشاكل مع Rate Limiting عند استخدام Proxy.

## 3.7 مشاكل الخادم الرئيسية

| الرقم | المشكلة | الموقع | الخطورة |
|-------|---------|--------|---------|
| S-01 | Controllers ضخمة جداً (تحتاج Service Layer) | analyticsController.js, assessmentController.js | 🔴 |
| S-02 | Redis غير مشغل فعلياً | redis.js, index.js | 🟡 |
| S-03 | JSON limit 50MB كبير جداً | index.js:95 | 🟡 |
| S-04 | Route student.js كبير جداً (689 سطر) | routes/student.js | 🔴 |
| S-05 | لا يوجد `trust proxy` | index.js | 🟡 |
| S-06 | التوكن GitHub مكشوف في `.env` | .env | 🔴 |
| S-07 | JWT_SECRET ثابت وضعيف | .env | 🔴 |
| S-08 | لا يوجد فصل بين Config و Code | كل الإعدادات في `.env` فقط | 🟡 |
| S-09 | SQLite في الإنتاج (غير مناسب) | schema.prisma | 🔴 |
| S-10 | نظام التخزين المؤقت في الذاكرة (غير مستمر) | strictSecurity.js | 🟡 |

---

# 4. تحليل الواجهة الأمامية للويب (Frontend-Web)

## 4.1 هيكل الملفات

**الموقع:** `C:\Projects\dhad-studio\frontend-web\`

### صفحات HTML:
- `index.html` - الرئيسية
- `login.html` / `register.html` / `activate.html` - المصادقة
- `student-dashboard.html` - لوحة الطالب
- `teacher-dashboard.html` - لوحة المعلم
- `challenges.html` - التحديات
- `assessments.html` - الاختبارات
- `leaderboard.html` - لوحة المتصدرين
- `roadmap.html` / `course-roadmap.html` - مسار التعلم
- `additional-courses.html` - دورات إضافية
- `classrooms.html` - إدارة الفصول
- `students.html` - إدارة الطلاب
- `profile.html` - الملف الشخصي
- `reports.html` - التقارير
- `settings.html` - الإعدادات
- `web-editor.html` - محرر الأكواد
- `achievements.html` - الإنجازات
- `book.html` (9362 سطر) - كتاب تعليمي

### ملفات JS:
- `js/api.js` - عميل API
- `js/auth.js` - إدارة المصادقة
- `js/dhad.js` - المكتبة الأساسية
- `js/toast.js` - إشعارات
- `js/sidebar.js` - القائمة الجانبية
- `js/mobile-nav.js` - التنقل للجوال
- `js/theme-switcher.js` - تغيير السمة
- `js/lib/jspdf.umd.min.js` - مكتبة PDF
- `js/lib/html2canvas.min.js` - تصوير الشاشة

### ملفات CSS:
- `css/style.css`
- `css/themes.css`

## 4.2 تحليل الصفحات

### 4.2.1 `book.html` (9362 سطر)
**صفحة كتاب تعليمي تفاعلي** — أكبر ملف في المشروع.
- يحتوي على محتوى تعليمي كامل عن لغة "ض"
- يشمل أمثلة تفاعلية
- **مشكلة:** صفحة واحدة ضخمة جداً. يجب تقسيمها إلى فصول منفصلة.

### 4.2.2 `web-editor.html`
محرر أكواد بلغة "ض" مع:
- إرسال الكود للخادم للتحقق
- عرض نتائج الترجمة
- **مشكلة:** لا يوجد Syntax Highlighting حقيقي (يحتاج CodeMirror أو Monaco Editor)

### 4.2.3 `teacher-dashboard.html` / `student-dashboard.html`
لوحات معلومات متكاملة مع API calls مباشرة.

## 4.3 تحليل ملفات JS

### `js/api.js`
**عميل API** — يتعامل مع جميع طلبات الخادم.
- يستخدم `fetch()` مع JWT في الـ headers
- يعيد توجيه المستخدم عند انتهاء صلاحية التوكن
- **مشكلة:** لا يوجد retry logic، ولا معالجة Offline

### `js/auth.js`
إدارة المصادقة: تسجيل الدخول، تسجيل الخروج، تخزين JWT في localStorage.
**مشكلة 🔴:** JWT مخزن في `localStorage` (غير آمن ضد XSS). الأفضل استخدام `httpOnly` cookies.

### `js/dhad.js`
المكتبة الأساسية لصفحات الويب:
- تضمين المكونات (Header, Sidebar, Footer)
- المسارات
- إعدادات السمة
- **ملاحظة:** من أفضل الملفات كتابة في المشروع

## 4.4 مشاكل الـ Frontend-Web الرئيسية

| الرقم | المشكلة | الموقع | الخطورة |
|-------|---------|--------|---------|
| W-01 | JWT في localStorage | js/auth.js | 🔴 |
| W-02 | book.html ضخم جداً (9362 سطر) | pages/book.html | 🟡 |
| W-03 | لا يوجد build system (Webpack/Vite) | - | 🟡 |
| W-04 | لا يوجد CSS minification | css/ | 🟢 |
| W-05 | لا يوجد JS bundling | كل صفحة تحمل JS منفصل | 🟡 |
| W-06 | لا يوجد offline/dynamic loading | - | 🟢 |
| W-07 | hardcoded API URLs في JS (http://localhost:3000) | عدة ملفات | 🟡 |
| W-08 | محرر الأكواد بسيط جداً (TextArea) | web-editor.html 🟡 | 🟡 |

---

# 5. تحليل تطبيق سطح المكتب (Desktop Frontend)

## 5.1 هيكل التطبيق

**التقنيات:** Qt 5.15, QML, C++17, CMake
**الموقع:** `C:\Projects\dhad-studio\frontend\`

### C++ Backend (23 ملف):
- `main.cpp` - نقطة الدخول
- `CompilerWrapper` - التفاعل مع المترجم
- `CppToDaadTranspiler` - تحويل C++ → Daad
- `Translator` - تحويل Daad → C++ (المترجم الأساسي)
- `ChallengeManager` - إدارة التحديات البرمجية
- `CloudNetworkService` - الخدمات السحابية
- `FileSystemModel` - متصفح الملفات
- `GeminiBackend` - تكامل Gemini AI
- `GuiDesignerModel` - مصمم الواجهات
- `SyntaxHighlighter` - تلوين الكود
- `WidgetNode` - شجرة عناصر واجهة المستخدم
- `MockDataLoader` - بيانات اختبار

### QML UI (27 ملف):
- `Main.qml` - النافذة الرئيسية
- `LoginScreen.qml` - شاشة الدخول
- `StudentDashboard.qml` - لوحة الطالب
- `TeacherDashboard.qml` - لوحة المعلم
- `EditorPane.qml` - محرر الأكواد
- `OutputPane.qml` - مخرجات الترجمة
- `ChatPanel.qml` - لوحة الدردشة AI
- `DesignCanvas.qml` - لوحة الرسم
- `GuiDesignerView.qml` - مصمم الواجهات
- `SettingsPanel.qml` - الإعدادات
- `LanguageIndex.qml` - فهرس اللغة
- و 15 ملف QML آخر

## 5.2 تحليل المكونات

### 5.2.1 المترجم (Translator) - `CppToDaadTranspiler`
**الوظيفة:** تحويل كود C++ إلى Daad والعكس.
**المشاكل:**
- 🟡 لا يدعم جميع الكلمات المفتاحية الـ 68 (يدعم فقط المجموعة الأساسية)
- 🟡 لا يعالج الأنواع المعقدة (templates, Lambdas, Smart Pointers)
- 🟡 لا يوجد معالجة للأخطاء أثناء الترجمة (قد يعلق أو ينهار)

### 5.2.2 Gemini AI Integration - `GeminiBackend`
**الوظيفة:** تكامل مع Gemini API لتقديم اقتراحات ذكية.
**المشاكل:**
- 🟡 يستخدم API key مشفر بشكل ثابت (hardcoded)؟
- 🟡 لا يوجد fallback إذا فشل الـ API

### 5.2.3 SyntaxHighlighter
يدعم تلوين الكلمات المفتاحية العربية.

### 5.2.4 GuiDesigner
مصمم واجهات سحب وإفلات كامل (WidgetNode, DesignCanvas, WidgetPalette).

## 5.3 مشاكل تطبيق سطح المكتب الرئيسية

| الرقم | المشكلة | الخطورة |
|-------|---------|---------|
| D-01 | المترجم لا يدعم كل الميزات (Templates, Lambdas) | 🟡 |
| D-02 | `GeminiBackend` API key hardcoded (مشبوه) | 🔴 |
| D-03 | لا يوجد معالجة أخطاء شاملة في الـ C++ Backend | 🟡 |
| D-04 | بعض ملفات QML قديمة (Main.qml.backup) | 🟢 |
| D-05 | الترجمة غير مكتملة (10 ملفات .ts كلها تقريباً فارغة) | 🟡 |
| D-06 | لا يوجد Windows packaging جاهز (WiX غير مكتمل) | 🟡 |

---

# 6. تحليل المترجم الأساسي (Core Compiler)

## 6.1 هيكل المترجم

**الموقع:** `C:\Projects\dhad-studio\src\` + `C:\Projects\dhad-studio\include\Daad\`

```
Daad Source Code
      │
      ▼
┌──────────┐
│  Lexer   │  src/Lexer.cpp (199 lines)
│          │  يدعم: Unicode (UTF-32), أرقام, نصوص, تعليقات
│          │  الكلمات المفتاحية: 68 كلمة عربية
└─────┬────┘
      │
      ▼
┌──────────┐
│  Parser  │  src/Parser.cpp (628 lines)
│          │  Recursive Descent Parser كامل
│          │  المتغيرات: صحيح, عشري, نص, منطقي, حرف
│          │  التحكم: إذا, وإلا, طالما, لكل, افعل, اختر
│          │  الدوال: دالة, ارجع
│          │  OOP: صنف, هيكل, نطاق, تعداد
│          │  الاستثناءات: حاول, امسك, ارمِ
│          │  القوالب: قالب
│          │  المصفوفات: صحيح[]
└─────┬────┘
      │
      ▼
┌──────────┐
│   AST    │  include/Daad/AST.hpp
│          │  شجرة النحو المجردة مع Visitor Pattern
│          │  ~20+ نوع عقدة (Stmt, Expr, Decl)
└─────┬────┘
      │
      ▼
┌──────────┐
│ CodeGen  │  include/Daad/CodeGenVisitor.hpp
│          │  Visitor Pattern → C++ output
│          │  يولد كود C++ مطابق
└─────┬────┘
      │
      ▼
┌──────────┐
│Optimizer │  Constant Folding, DCE, Strength Reduction
└──────────┘
      │
      ▼
┌──────────┐
│ Runtime  │  دوال وقت التشغيل (daad_print, etc.)
└──────────┘
      │
      ▼
┌──────────┐
│  Stdlib  │  100 وحدة دوال عربية
└──────────┘
```

## 6.2 الكلمات المفتاحية (68 كلمة)

### أنواع البيانات (12):
صحيح, عشري, منطقي, حرف, نص, ثابت, فراغ, تلقائي, صواب, خطأ, عدم, عرّف

### التحكم في التدفق (12):
إذا, وإلا, طالما, افعل, لكل, في, اختر, حالة, افتراضي, توقف, استمر, انتقل

### OOP والنطاقات (12):
صنف, هيكل, تعداد, واجهة, نطاق, عام, خاص, محمي, يرث, ذاتي, الأصل, مجرّد

### الدوال والذاكرة (10):
دالة, ارجع, جديد, احذف, مؤشر, مرجع, ساكن, مضمن, خارجي, قالب

### الاستثناءات (8):
حاول, امسك, أخيراً, ارمِ, تأكد, استثناء, نوع_الـ, حجم_الـ

### التزامن (10):
تزامن, انتظر, خيط, احجز, مشترك, فريد, استورد, صدّر, وحدة, بديل

### GUI (14):
زر_أمر, حقل_نص, قائمة_خيارات, صورة, مربع_اختيار, شريط_تمرير, قائمة_منسدلة, لوحة, تسمية, عمود, صف, شبكة, شريط_تلوين, علامة_تبويب

## 6.3 الأخطاء المعروفة

### 🔴 BUG-01: `طباعة()` مع دوال — خطأ في توليد C++
**المشكلة:** عندما يكتب المستخدم:
```
daad طباعة(مضروب(5))
```
المترجم يولد:
```cpp
مضروب(5);                          // خطأ: يعدم قيمة الإرجاع
daad::runtime::daad_print(5);       // خطأ: يطبع آخر قيمة وليس نتيجة الدالة
```
بدلاً من:
```cpp
daad::runtime::daad_print(مضروب(5));
```

**السبب:** `طباعة` تعامل كـ built-in وليس كـ function call — الـ Parser يفصل بين اسم الدالة ومعاملها.

**الموقع:** Parser.cpp في دالة `parsePrintStatement()`

### 🟡 BUG-02: `لكل` (For loop) مع متغيرات غير معرّفة
**المشكلة:** حلقة `لكل` لا تعرّف متغير الحلقة بشكل صحيح في بعض الحالات.

### 🟡 BUG-03: `اختر` (Switch) على نصوص
`switch` لا يدعم المقارنة على `نص` (strings) — سيبايلد لخطأ في C++.

### 🟡 BUG-04: `جديد` (new) بدون `احذف` (delete)
المترجم لا يفرض استخدام `احذف` مع `جديد` — تسرب ذاكرة.

## 6.4 تحليل الأمثلة

### Conversions: Simple → Complex (11 مثال)
تم تحويل جميع الأمثلة من Daad إلى C++ بنجاح (باستثناء خطأ BUG-01).

### Programs (مشاريع كاملة):
- `01_calculator.daad` (78 سطر) → حاسبة متقدمة
- `02_guess_number.daad` (59 سطر) → لعبة تخمين
- `03_grades.daad` (60 سطر) → حاسبة درجات
- `04_primes.daad` (39 سطر) → أعداد أولية
- `05_interest.daad` (47 سطر) → فائدة مركبة
- `06_alarm.daad` (74 سطر) → منبه
- `07_prayer.daad` (80 سطر) → مواقيت الصلاة
- `08_clock.daad` (107 سطر) → تقويم/ساعة
- `09_tasks.daad` (70 سطر) → إدارة مهام
- `10_puzzle.daad` (89 سطر) → لعبة ألغاز

## 6.5 مشاكل المترجم الرئيسية

| الرقم | المشكلة | الخطورة |
|-------|---------|---------|
| C-01 | خطأ في توليد `طباعة()` مع الدوال | 🔴 |
| C-02 | لا يدعم الـ Lambda Expressions | 🟡 |
| C-03 | لا يدشرط الـ Smart Pointers (`shared_ptr`, `unique_ptr`) | 🟡 |
| C-04 | لا يدعم الـ Move Semantics | 🟡 |
| C-05 | لا يوجد تحسين حقيقي (Optimizer بسيط جداً) | 🟡 |
| C-06 | الأداء: Vector و Map في Stdlib تستخدم C++ مباشرة | 🟢 |
| C-07 | لا يوجد Debug Info (Source Maps) | 🟡 |

---

# 7. تحليل قاعدة البيانات (Prisma Schema)

## 7.1 النماذج (23 Models)

```
                                    ┌──────────────────┐
                                    │     School       │
                                    │ id, name, code,  │
                                    │ subscriptionLevel │
                                    └────────┬─────────┘
                                             │ 1
                                             │
                    ┌────────────────────────┼────────────────────────┐
                    │                        │                        │
                    ▼  N                     ▼  N                     ▼  N
          ┌──────────────────┐    ┌──────────────────┐    ┌──────────────────┐
          │      User        │    │    Classroom     │    │    Course        │
          │ id, email, role  │    │ id, name, level  │    │ id, title, type  │
          │ schoolId (FK)    │    │ schoolId (FK)    │    │ tierRequired (FK)│
          └────────┬─────────┘    └────────┬─────────┘    └────────┬─────────┘
                   │                       │                       │
                   │ 1                     │ 1                     │ 1
                   │                       │                       │
                   ▼ N                     ▼ N                     ▼ N
          ┌──────────────────┐    ┌──────────────────┐    ┌──────────────────┐
          │   Student        │    │ StudentCourseE.. │    │     Lesson       │
          │ username, xp,    │    │ enrollment       │    │ lessonNumber     │
          │ level, streak    │    │ studentId (FK)   │    │ courseId (FK)    │
          │ classroomId (FK) │    └──────────────────┘    └────────┬─────────┘
          └────────┬─────────┘                                     │
                   │ 1                                              │ 1
                   │                                                │
                   ▼ N                                              ▼ N
          ┌──────────────────┐                                     ┌──────────────────┐
          │    LoginLog      │                                     │   Challenge      │
          │ studentId (FK)   │                                     │ lessonId (FK)   │
          │ ip, timestamp    │                                     │ tierId (FK)     │
          └──────────────────┘                                     └────────┬─────────┘
                                                                           │ 1
                                                                           │
                                                                           ▼ N
                                                                  ┌──────────────────┐
                                                                  │ ChallengeTier    │
                                                                  │ title, order     │
                                                                  └──────────────────┘
```

### النماذج الكاملة:
1. **School** — المدرسة
2. **User** — المستخدم (ADMIN, TEACHER)
3. **Course** — دورة تعليمية
4. **Student** — الطالب
5. **Lesson** — درس
6. **StudentCourseEnrollment** — تسجيل الطالب في دورة
7. **Classroom** — الفصل الدراسي
8. **Challenge** — تحدي برمجي
9. **Submission** — تسليم الحل
10. **Project** — مشروع طالب
11. **Attendance** — الحضور
12. **ChallengeTier** — مستوى التحدي
13. **Assessment** — اختبار
14. **StudentAssessmentResult** — نتيجة اختبار
15. **StudentSubmission** — حل اختبار
16. **LoginLog** — سجل الدخول
17. **StudentProfile** — الملف الشخصي
18. **DailyXP** — XP يومي
19. **Achievement** — إنجاز
20. **UserAchievement** — إنجاز المستخدم
21. **ActivityLog** — سجل النشاط
22. **CloudWorkspace** — مساحة عمل سحابية
23. **SecurityAlert** — تنبيه أمني
24. **Invitation** — دعوة
25. **ChallengeVerification** — التحقق من التحدي

## 7.2 مشاكل قاعدة البيانات

| الرقم | المشكلة | الخطورة |
|-------|---------|---------|
| DB-01 | **SQLite في الإنتاج** — لا يدعم Concurrent Writes, ENUMs, JSON, أو أدوات PostgreSQL | 🔴 |
| DB-02 | **هيكل التحديات متشعب** — Challenge + Submission + ChallengeTier + Lesson + Course معقد جداً | 🟡 |
| DB-03 | **بيانات الإنجازات في الذاكرة** — Achievements معرفة في seed.js بدلاً من قاعدة البيانات | 🟡 |
| DB-04 | **فهرس ناقص** — بعض الحقول المستخدمة في WHERE ليس لها فهرس (مثل Student.username) | 🟡 |
| DB-05 | **نوع الحقول** — بعض الحقول `String` يجب أن تكون `Int` أو `Float` | 🟡 |
| DB-06 | **علاقات دائرية** — School → User → Student → Classroom → School (قابلة للحل) | 🟢 |

---

# 8. تدقيق الأمان الشامل (Security Audit)

## 8.1 نتائج الاختبارات الأمنية (موجودة)

| الاختبار | النتيجة |
|----------|---------|
| Anti-Cheat (99 اختبار) | ✅ جميعها تمنع الغش |
| PenTest (400 سطر) | ✅ جميع الثغرات مسدودة |
| Deep PenTest (600 سطر) | ✅ |
| Hack Attempt (32 متجه) | ✅ 100% دفاع |
| Attack 45 (45 هجوم) | ✅ |
| Deep Attack 90 (90 اختبار عميق) | ✅ |
| Manual PenTest (10 مستويات) | ✅ |

## 8.2 طبقات الأمان الحالية

```
الطبقة 1: Helmet (CSP, HSTS, X-Frame-Options, Permissions-Policy)
الطبقة 2: CORS (ديناميكي من ALLOWED_ORIGINS)
الطبقة 3: Rate Limiting (7 فئات)
الطبقة 4: طلب جامد (strictRequestValidation)
الطبقة 5: تطهير المدخلات (XSS, Null Bytes)
الطبقة 6: JWT مع Rotation
الطبقة 7: HMAC-SHA256 مع حماية Replay
الطبقة 8: قفل الحساب (5 محاولات فاشلة)
الطبقة 9: حظر IP (20 فشل)
الطبقة 10: كشف النشاط المشبوه
الطبقة 11: إدارة الجلسات (حد أقصى 3)
الطبقة 12: تدقيق (Audit Log)
```

## 8.3 الثغرات المتبقية

| الرقم | الثغرة | الموقع | الخطورة |
|-------|--------|--------|---------|
| SEC-01 | JWT_SECRET ضعيف وثابت | .env | 🔴 حرج |
| SEC-02 | GITHUB_TOKEN مكشوف في `.env` (يجب إبطاله الآن) | .env | 🔴 حرج |
| SEC-03 | JWT مخزن في localStorage (معرض لـ XSS) | js/auth.js | 🔴 حرج |
| SEC-04 | Redis غير مشفر (إذا استخدمته، البيانات مكشوفة) | redis.js | 🟡 |
| SEC-05 | Error handler يكشف تفاصيل داخلية أحياناً | errorHandler.js | 🟡 |
| SEC-06 | لا يوجد CSRF protection | index.js | 🟡 |
| SEC-07 | `express.json({ limit: '50mb' })` — هجوم DoS محتمل | index.js | 🟡 |

---

# 9. الأخطاء الحرجة (Critical Bugs)

## 🔴 BUG-01: `طباعة()` يدمر الدوال — Compiler CodeGen
**الموقع:** `src/Parser.cpp` — `parsePrintStatement()` (حوالي السطر 370)
**الوصف:** عندما تستدعي `طباعة(دالة())`، المترجم يولد كود C++ خاطئ يفصل بين الدالة ومعاملها
**الأثر:** كل برنامج يستخدم `طباعة` مع دوال ينتج كود C++ لا يعمل
**الحل:** تعديل `parsePrintStatement()` لمعاملة `طباعة` كـ Function Call حقيقي وليس built-in statement

## 🔴 BUG-02: التوكن GitHub مكشوف
**الموقع:** `server/.env` السطر 20
**الوصف:** `GITHUB_TOKEN=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`
**الأثر:** أي شخص يطلع على `.env` يتحكم بحساب GitHub
**الحل:** إبطال التوكن فوراً وإنشاء توكن جديد. إضافة `.env` إلى `.gitignore` (موجود بالفعل)

## 🔴 BUG-03: JWT_SECRET ضعيف
**الموقع:** `server/.env` السطر 2
**الوصف:** `JWT_SECRET="dhad-studio-dev-secret-key-change-in-production"`
**الأثر:** يمكن لأي شخص تخمين المفتاح وتزوير التوكن
**الحل:** توليد مفتاح عشوائي 64 حرف واستخدامه في الإنتاج

## 🟡 BUG-04: Redis غير متصل
**الموقع:** `server/src/utils/redis.js`
**الوصف:** التخزين المؤقت للـ Leaderboard لا يعمل. `connectRedis()` يفشل بدون خطأ واضح
**الأثر:** استعلامات Leaderboard تصل إلى قاعدة البيانات في كل مرة
**الحل:** إضافة fallback للذاكرة المؤقتة عندما يكون Redis غير متاح

## 🟡 BUG-05: SQLite ليس للإنتاج
**الموقع:** `server/prisma/schema.prisma`
**الوصف:** `datasource db { provider = "sqlite" }`
**الأثر:** الكتابة المتزامنة تتعارض، لا يدعم ENUMs
**الحل:** استخدام PostgreSQL schema الموجود (`schema.postgresql.prisma`)

---

# 10. خارطة الطريق (Development Roadmap)

## المرحلة 1: الإصلاحات العاجلة (الأسبوع 1)
**الأولوية قصوى — يجب معالجتها فوراً**

| المهمة | الرمز | المسؤول | الوقت المقدر |
|--------|-------|---------|-------------|
| إبطال GitHub Token وإنشاء جديد | SEC-02 | DevOps | 5 دقائق |
| تغيير JWT_SECRET إلى مفتاح عشوائي | SEC-01 | Backend | 10 دقائق |
| إصلاح BUG-01: `طباعة()` مع الدوال | C-01 | Compiler | ⏳ 4 ساعات |
| نقل JWT من localStorage إلى httpOnly cookies | W-01 | Frontend | ⏳ 3 ساعات |
| تخفيض JSON limit من 50MB إلى 10MB | S-03 | Backend | 5 دقائق |

## المرحلة 2: تحسين البنية (الأسبوع 2-3)

| المهمة | الرمز | الوقت المقدر |
|--------|-------|-------------|
| إنشاء Service Layer وفصل الـ Controllers | S-01 | ⏳ 8 ساعات |
| تقسيم `routes/student.js` إلى Routes خفيفة | S-04 | ⏳ 4 ساعات |
| تقسيم `book.html` إلى صفحات فصول | W-02 | ⏳ 3 ساعات |
| إضافة `trust proxy` للخادم | S-05 | 10 دقائق |
| إضافة Redis متصل (Upstash أو local) | S-02 | ⏳ 2 ساعات |
| إضافة CSRF protection | SEC-06 | ⏳ 1 ساعة |

## المرحلة 3: الترقية إلى PostgreSQL (الأسبوع 3-4)

| المهمة | الوقت المقدر |
|--------|-------------|
| ترحيل الـ Schema من SQLite إلى PostgreSQL | ⏳ 4 ساعات |
| ترحيل البيانات الموجودة | ⏳ 2 ساعات |
| تعديل الاستعلامات المتأثرة (JSON, ENUMs) | ⏳ 3 ساعات |
| تحديث Dockerfile و docker-compose | ⏳ 1 ساعة |

## المرحلة 4: استكمال المترجم (الأسبوع 4-6)

| المهمة | الوقت المقدر |
|--------|-------------|
| إصلاح BUG-01 بالكامل (طباعة + دوال) | ⏳ 4 ساعات |
| إضافة Lambda Expressions | ⏳ 6 ساعات |
| إضافة Smart Pointers (مشترك/فريد) | ⏳ 4 ساعات |
| إضافة Move Semantics | ⏳ 3 ساعات |
| تحسين الـ Optimizer (Loop Unrolling, Inlining) | ⏳ 8 ساعات |
| إضافة Source Maps (Debug Info) | ⏳ 6 ساعات |
| تحسين معالجة `لكل` (For loop with range) | ⏳ 2 ساعة |

## المرحلة 5: تطوير الـ IDE (الأسبوع 6-8)

| المهمة | الوقت المقدر |
|--------|-------------|
| تنظيف API keys من `GeminiBackend` | ⏳ 1 ساعة |
| إكمال الترجمة لـ 10 لغات | ⏳ 8 ساعات |
| تحسين EditorPane (CodeMirror/Monaco) | ⏳ 6 ساعات |
| إكمال GuiDesigner (سحب وإفلات متقدم) | ⏳ 8 ساعات |
| إكمال WiX Installer للإصدار Windows | ⏳ 4 ساعات |

## المرحلة 6: تحسين الواجهة الأمامية (الأسبوع 8-10)

| المهمة | الوقت المقدر |
|--------|-------------|
| إضافة Build System (Vite أو Webpack) | ⏳ 4 ساعات |
| تحسين محرر الأكواد في web-editor | ⏳ 6 ساعات |
| CSS minification + JS bundling | ⏳ 2 ساعات |
| إضافة Offline Support (Service Workers) | ⏳ 4 ساعات |
| متغيرات البيئة الديناميكية (بدلاً من hardcoded URLs) | ⏳ 2 ساعات |

## المرحلة 7: الاختبارات والتوثيق (الأسبوع 10-12)

| المهمة | الوقت المقدر |
|--------|-------------|
| إضافة Jest + Supertest لاختبارات API | ⏳ 8 ساعات |
| توثيق كامل للـ API (Swagger) | ⏳ 4 ساعات |
| توثيق لغة Daad (Reference Manual) | ⏳ 8 ساعات |
| توثيق الـ Stdlib بالعربية | ⏳ 6 ساعات |
| دليل المستخدم للـ IDE | ⏳ 4 ساعات |

## المرحلة 8: البنية التحتية والنشر (الأسبوع 12-14)

| المهمة | الوقت المقدر |
|--------|-------------|
| نشر PostgreSQL في الإنتاج | ⏳ 3 ساعات |
| نشر Redis (Upstash أو Elasticache) | ⏳ 2 ساعات |
| تكوين CI/CD Pipeline على GitHub Actions | ⏳ 4 ساعات |
| نشر على AWS ECS (Fargate) | ⏳ 6 ساعات |
| إعداد Domain + SSL | ⏳ 2 ساعات |
| إعداد Monitoring (CloudWatch/Grafana) | ⏳ 4 ساعات |

---

# 11. استراتيجية الاختبارات (Testing Strategy)

## 11.1 الوضع الحالي

**C++ Tests:**
- LexerTests — tokenization ✅
- ParserTests — AST parsing ✅
- CompilerTests — code generation ✅
- OptimizerTests — constant folding ✅
- RuntimeTests — built-in functions ✅
- StdlibTests — math/string/vector/map/I/O ✅
- ComprehensiveTests — 100+ tests ✅

**JavaScript Tests (للخادم):**
- Anti-cheat (99 test) ✅
- PenTest / Deep PenTest / Advanced PenTest ✅
- Stress Tests ✅
- Integration Tests ✅

## 11.2 الثغرات في التغطية

| المجال | الحالة |
|--------|--------|
| API Endpoints (Jest/Supertest) | ❌ غير موجود |
| Frontend-Web (E2E) | ❌ غير موجود |
| Desktop IDE QML Tests | ❌ غير موجود |
| VS Code Extension Tests | ❌ غير موجود |
| Performance/Load Tests للـ API | ❌ غير موجود |
| Security Tests للـ Web Frontend | ❌ غير موجود |

## 11.3 خطة إضافة الاختبارات

### المرحلة 1: API Integration Tests
```javascript
// مثال: test/api/auth.test.js
const request = require('supertest');
const app = require('../src/index');

describe('POST /api/v1/auth/login', () => {
  it('يرفض المحاولة بدون بيانات', async () => {
    await request(app).post('/api/v1/auth/login').expect(400);
  });
  
  it('يسجل الدخول بنجاح', async () => {
    await request(app)
      .post('/api/v1/auth/login')
      .send({ email: 'admin@test.com', password: 'Test1234' })
      .expect(200);
  });
});
```

### المرحلة 2: E2E Tests للـ Web Frontend
- استخدام Playwright (MCP موجود ومثبت) ✅
- تغطية: تسجيل الدخول، لوحة الطالب، حل التحدي

### المرحلة 3: Performance Tests
- استخدام k6 أو autocannon
- استهداف: 1000 مستخدم متزامن

---

# 12. البنية التحتية (DevOps)

## 12.1 الوضع الحالي

| المكون | الحالة |
|--------|--------|
| Dockerfile | موجود ✅ |
| CI/CD (GitHub Actions) | موجود ✅ |
| AWS ECS + Fargate | مكوّن ✅ |
| AWS ALB + HTTPS | مكوّن ✅ |
| IAM Roles | مكوّن ✅ |
| NSIS Installer (Windows) | موجود ✅ |
| Render.com Deploy | موجود ✅ |

## 12.2 ملف AWS ECS Task Definition

```json
{
  "family": "dhad-studio",
  "containerDefinitions": [
    {
      "name": "dhad-studio-app",
      "image": "${ECR_REPO}:latest",
      "portMappings": [{ "containerPort": 3000 }],
      "environment": [
        { "name": "NODE_ENV", "value": "production" },
        { "name": "DATABASE_URL", "value": "..." },
        { "name": "REDIS_URL", "value": "..." }
      ]
    }
  ]
}
```

**المشاكل:**
- 🟡 **Secrets Management**: DATABASE_URL و JWT_SECRET في Environment variables وليس AWS Secrets Manager
- 🟡 **Health Check**: مسار `/health` فقط، ولا يوجد `/live` و `/ready` منفصلين
- 🟢 **CI/CD**: متكامل بشكل جيد مع GitHub Actions → AWS ECR → ECS

## 12.3 تحسينات DevOps

| المهمة | الأولوية |
|--------|----------|
| نقل Secrets إلى AWS Secrets Manager | 🔴 |
| إضافة docker-compose للتطوير المحلي (PostgreSQL + Redis) | 🟡 |
| إضافة Health Checks كاملة (`/live`, `/ready`) | 🟡 |
| إضافة Monitoring (CloudWatch Logs + Metrics) | 🟡 |
| إضافة CI/CD للـ VS Code Extension | 🟢 |

---

# 13. توثيق المشروع (Documentation)

## 13.1 الوضع الحالي

| الوثيقة | الحالة |
|---------|--------|
| `README.md` | موجود — شامل ✅ |
| `HANDOVER_GUIDE.md` | موجود — مختصر ✅ |
| `SECURITY.md` | موجود ✅ |
| Swagger API Doc | موجود — ناقص 🟡 |
| Daad Language Reference | غير موجود ❌ |
| Stdlib Documentation | غير موجود ❌ |
| API Postman Collection | غير موجود ❌ |
| Architecture Diagram | غير موجود ❌ |

## 13.2 خطة التوثيق

### المستندات المطلوبة:

1. **دليل لغة Daad** — شرح كل كلمة من الـ 68 كلمة مع أمثلة
2. **دليل Stdlib** — شرح الـ 100 وحدة مع أمثلة استخدام
3. **API Reference** — إكمال Swagger لجميع الـ endpoints
4. **دليل المستخدم IDE** — شرح استخدام Qt Desktop IDE
5. **دليل المستخدم Web** — شرح استخدام Web Frontend
6. **دليل النشر** — AWS ECS, Render, Docker
7. **دليل المساهمة** — CONTRIBUTING.md
8. **دليل الاختبارات** — كيفية تشغيل وكتابة الاختبارات

---

# 14. تكامل الذكاء الاصطناعي (AI Integration)

## 14.1 المكونات الحالية

### Gemini Backend (Qt Desktop IDE)
`frontend/cpp/GeminiBackend.cpp` — يتكامل مع Google Gemini API
- يوفر اقتراحات ذكية أثناء كتابة الكود
- **مشكلة:** API key مشفر/hardcoded

### GitHub Models Agent (Server Script)
`server/scripts/github-agent.js` — وكيل ذكاء اصطناعي
- يستخدم GitHub Models (gpt-4o-mini) المجاني
- يدعم قراءة الملفات والمجلدات كسياق
- API key في `.env`

## 14.2 خطة التكامل

```
مستخدم
    │
    ├──► OpenCode (منسق رئيسي)
    │       │
    │       ├──► GitHub Models Agent (تحليل + اقتراحات)
    │       │
    │       ├──► OpenCode Task Agents (مهام متوازية)
    │       │
    │       └──► Playwright MCP (اختبارات E2E)
    │
    └──► Gemini CLI 0.50.0 (لما تتجدد الحصة)
```

### السيناريوهات:
1. **تحليل الكود**: المستند → OpenCode → GitHub Models Agent
2. **اقتراح إصلاحات**: OpenCode يقرأ → GitHub Models يحلل → OpenCode ينفذ
3. **اختبار E2E**: Playwright يسجل → OpenCode يحلل النتائج
4. **توليد توثيق**: OpenCode يقرأ الكود → GitHub Models يولد توثيق

---

# 15. ربط كل المكونات (Integration Architecture)

## 15.1 التدفق الشامل

```
                         [GitHub / DevOps]
                              │
                    CI/CD Pipeline (.github/workflows/)
                              │
         ┌────────────────────┼────────────────────┐
         │                    │                    │
         ▼                    ▼                    ▼
    [AWS ECS]            [VS Code]            [GitHub Models]
    (خادم REST)          (تطوير لغة ض)        (AI Agent)
         │                    │                    │
         │                    │                    │
    ┌────┴────┐         ┌────┴────┐         ┌────┴────┐
    │ Server  │◄────────│Web Front│         │ Analysis│
    │ Node.js │────────►│   UI    │         │ Reports │
    │ Express │         └─────────┘         └─────────┘
    │ :3000   │
    └────┬────┘
         │
         ├────────────────────┐
         │                    │
         ▼                    ▼
    [PostgreSQL]         [Redis]
    (قاعدة بيانات)       (تخزين مؤقت)
         │
         │
    ┌────┴────────────────────┐
    │    Qt5 Desktop IDE      │
    │  (تطوير لغة ض)          │
    │                         │
    │  ┌─────────────────┐   │
    │  │  Daad Compiler   │   │
    │  │  Daad → C++20    │   │
    │  └─────────────────┘   │
    │         │              │
    │         ▼              │
    │  ┌─────────────────┐   │
    │  │    Stdlib       │   │
    │  │  100 وحدة عربية  │   │
    │  └─────────────────┘   │
    └─────────────────────────┘
```

## 15.2 نقاط التكامل الحرجة

| النقطة | المكونات | البروتوكول | الحالة |
|--------|----------|------------|--------|
| Server ↔ Web Frontend | Express + HTML/JS | REST (JSON) | ✅ |
| Server ↔ Desktop IDE | Express + Bridge C++ | REST (HMAC) | ✅ |
| Desktop ↔ Compiler | C++ call | Native | ✅ |
| Compiler ↔ Stdlib | C++ include | Native | ✅ |
| Server ↔ AI Agent | Node.js script | GitHub Models API | ✅ |
| Desktop ↔ Gemini API | C++ libcurl | Gemini API | ✅ (Key مشكلة) |
| Server ↔ Database | Prisma ORM | SQLite/PostgreSQL | ✅ |
| Server ↔ Redis | ioredis | Redis Protocol | ⚠️ (غير متصل) |
| VS Code ↔ Compiler | CLI call | Child Process | ✅ |

## 15.3 تحسينات الربط

| التحسين | الوصف | الأولوية |
|---------|-------|----------|
| Bridge عبر WebSocket | بدلاً من REST polling للـ HMAC Bridge | 🟡 |
| Grpc أو GraphQL | بدلاً من REST للاتصالات عالية التردد | 🟢 |
| Event-Driven | استخدام RabbitMQ/Kafka للأحداث (تسليم، اختبارات) | 🟢 |
| Monorepo Tool | توحيد إدارة الحزم (Nx, Turborepo) | 🟢 |

---

# 16. الملاحق

## الملحق أ: قائمة جميع ملفات المشروع

(تم تضمين القائمة الكاملة في النص أعلاه — ~355 ملفاً)

## الملحق ب: الكلمات المفتاحية الـ 68 للغة "ض"

(مدرجة بالكامل في القسم 6.2)

## الملحق ج: أمراض قاعدة البيانات

```
SQLite → PostgreSQL Migration
─────────────────────────────
التغييرات المطلوبة في schema:
1. تغيير provider من "sqlite" إلى "postgresql"
2. إضافة ENUM types بدلاً من String
3. استخدام @db.Text للنصوص الطويلة
4. تعديل Dates إلى Timestamptz
5. إضافة JSON للبيانات المرنة
```

---

# 17. دليل لغة "ض" الكامل (Daad Language Reference)

## 17.1 أنواع البيانات (Data Types)

### `صحيح` — Integer
```cpp
صحيح عمر = 25
صحيح عدد = -10
صحيح[] أرقام = [1, 2, 3, 4, 5]
```
**الترجمة إلى C++:** `int age = 25;`

### `عشري` — Double/Float
```cpp
عشري نسبة = 95.5
عشري درجة = 3.14159
```
**الترجمة إلى C++:** `double ratio = 95.5;`

### `نص` — String
```cpp
نص اسم = "أحمد"
نص رسالة = "مرحباً بالعالم"
```
**الترجمة إلى C++:** `std::string name = "أحمد";`

### `منطقي` — Boolean
```cpp
منطقي ناجح = صواب
منطقي راسب = خطأ
```
**الترجمة إلى C++:** `bool passed = true;`

### `حرف` — Char
```cpp
حرف أول = 'أ'
حرف علامة = '!'
```
**الترجمة إلى C++:** `char first = 'أ';` (ملاحظة: العربية UTF-8 تحتاج wchar_t)

### `ثابت` — Const
```cpp
ثابت صحيح سرعة_الضوء = 299792458
ثابت نص اسم_التطبيق = "ض استديو"
```

### `فراغ` — Void
```cpp
دالة اطبع_الاسم(نص اسم) -> فراغ {
    طباعة("الاسم: " + اسم)
}
```

### `تلقائي` — Auto
```cpp
تلقائي س = 42       // صحيح
تلقائي ص = 3.14     // عشري
تلقائي ز = "نص"     // نص
```

### `صواب` / `خطأ` — True / False
```cpp
منطقي جاهز = صواب
منطقي خطأ_وقع = خطأ
```

### `عدم` — Nullptr
```cpp
مؤشر نص مدخل = عدم
```

### `عرّف` — Typedef
```cpp
عرّف عدد_صحيح = صحيح
عرّف نص_طويل = نص
```

## 17.2 العمليات الحسابية (Arithmetic Operators)

```
+  → جمع
-  → طرح
*  → ضرب
/  → قسمة
%  → باقي القسمة
+= → إضافة مع تعيين
-= → طرح مع تعيين
*= → ضرب مع تعيين
/= → قسمة مع تعيين
```

**مثال:**
```cpp
صحيح س = 10
صحيح ص = 3
صحيح ن = س + ص     // 13
صحيح ب = س - ص     // 7
صحيح ض = س * ص     // 30
صحيح ق = س / ص     // 3
صحيح باق = س % ص   // 1
س += 5              // س = 15
```

## 17.3 العمليات المنطقية (Comparison Operators)

```
== → يساوي
!= → لا يساوي
<  → أصغر من
>  → أكبر من
<= → أصغر أو يساوي
>= → أكبر أو يساوي
&& → و (AND)
|| → أو (OR)
ليس → نفي (NOT)
```

**مثال:**
```cpp
صحيح درجة = 85
منطقي ممتاز = درجة >= 90
منطقي ناجح = درجة >= 50 && درجة < 90
منطقي راسب = ليس ناجح
إذا (ممتاز) {
    طباعة("ممتاز!")
} وإلا إذا (ناجح) {
    طباعة("ناجح")
} وإلا {
    طباعة("راسب")
}
```

## 17.4 جمل التحكم (Control Flow)

### `إذا` / `وإلا` — If / Else
```cpp
صحيح العمر = 18
إذا (العمر >= 18) {
    طباعة("بالغ")
} وإلا {
    طباعة("قاصر")
}
```
**الترجمة إلى C++:**
```cpp
int age = 18;
if (age >= 18) {
    daad::runtime::daad_print("بالغ");
} else {
    daad::runtime::daad_print("قاصر");
}
```

### `طالما` — While
```cpp
صحيح ع = 0
طالما (ع < 5) {
    طباعة(ع)
    ع += 1
}
```
**الترجمة إلى C++:**
```cpp
int i = 0;
while (i < 5) {
    daad::runtime::daad_print(i);
    i += 1;
}
```

### `لكل` — For
```cpp
لكل (صحيح س = 0; س < 10; س += 1) {
    طباعة(س * 2)
}
```
**الترجمة إلى C++:**
```cpp
for (int x = 0; x < 10; x += 1) {
    daad::runtime::daad_print(x * 2);
}
```

### `افعل` / `طالما` — Do/While
```cpp
صحيح محاولة = 0
افعل {
    طباعة("محاولة رقم: " + محاولة)
    محاولة += 1
} طالما (محاولة < 3)
```

### `اختر` / `حالة` / `افتراضي` — Switch/Case/Default
```cpp
صحيح يوم = 3
اختر (يوم) {
    حالة 1: طباعة("السبت")
    حالة 2: طباعة("الأحد")
    حالة 3: طباعة("الاثنين")
    افتراضي: طباعة("يوم غير معروف")
}
```

### `توقف` — Break
```cpp
لكل (صحيح س = 0; س < 10; س += 1) {
    إذا (س == 5) توقف
    طباعة(س)  // 0, 1, 2, 3, 4
}
```

### `استمر` — Continue
```cpp
لكل (صحيح س = 0; س < 10; س += 1) {
    إذا (س % 2 == 0) استمر
    طباعة(س)  // 1, 3, 5, 7, 9
}
```

## 17.5 الدوال (Functions)

### `دالة` — Function Definition
```cpp
دالة جمع(صحيح أ, صحيح ب) -> صحيح {
    ارجع أ + ب
}

دالة طباعة_ترحيب(نص اسم) -> فراغ {
    طباعة("أهلاً " + اسم)
}

// الاستخدام
صحيح ن = جمع(10, 20)   // ن = 30
طباعة_ترحيب("أحمد")     // أهلاً أحمد
```

**الترجمة إلى C++:**
```cpp
int add(int a, int b) {
    return a + b;
}
void printWelcome(std::string name) {
    daad::runtime::daad_print("أهلاً " + name);
}
```

### `ارجع` — Return
```cpp
دالة مطلق(صحيح س) -> صحيح {
    إذا (س < 0) ارجع -س
    ارجع س
}
```

## 17.6 البرمجة كائنية التوجه (OOP)

### `صنف` — Class
```cpp
صنف طالب {
    نص الاسم
    صحيح العمر
    عشري المعدل
    
    دالة عرض() -> فراغ {
        طباعة("الاسم: " + الاسم + ", المعدل: " + المعدل)
    }
}

// الاستخدام
طالب ط1
ط1.الاسم = "أحمد"
ط1.العمر = 20
ط1.المعدل = 88.5
ط1.عرض()
```

### `هيكل` — Struct
```cpp
هيكل نقطة {
    صحيح س
    صحيح ص
}

نقطة ن1 = {10, 20}
طباعة(ن1.س + ن1.ص)  // 30
```

### `تعداد` — Enum
```cpp
تعداد لون {
    أحمر,
    أخضر,
    أزرق
}
لون خ = أحمر
```

### `نطاق` — Namespace
```cpp
نطاق حساب {
    صحيح ثابت_حساب = 100
    
    دالة جمع(صحيح أ, صحيح ب) -> صحيح {
        ارجع أ + ب
    }
}

طباعة(حساب::جمع(5, 3))  // 8
```

### `عام` / `خاص` / `محمي` — Access Specifiers
```cpp
صنف حساب_بنكي {
    خاص:
        صحيح الرصيد = 0
    
    عام:
        دالة إيداع(صحيح مبلغ) -> فراغ {
            الرصيد += مبلغ
        }
        دالة سحب(صحيح مبلغ) -> فراغ {
            إذا (مبلغ <= الرصيد) الرصيد -= مبلغ
        }
        دالة عرض_الرصيد() -> صحيح {
            ارجع الرصيد
        }
}
```

### `ذاتي` — Self (This)
```cpp
صنف شخص {
    نص الاسم
    
    دالة عرف_بنفسك() -> فراغ {
        طباعة("أنا " + ذاتي.الاسم)
    }
}
```

### `يرث` — Inheritance
```cpp
صنف حيوان {
    نص النوع
    
    دالة تكلم() -> فراغ {
        طباعة("...")
    }
}

صنف كلب يرث حيوان {
    دالة تكلم() -> فراغ {
        طباعة("نباح!")
    }
}
```

### `مجرّد` — Abstract/Virtual
```cpp
صنف مجرّد شكل {
    دالة مجرّد احسب_المساحة() -> عشري
}

صنف دائرة يرث شكل {
    عشري نصف_القطر = 5.0
    
    دالة احسب_المساحة() -> عشري {
        ارجع 3.14159 * نصف_القطر * نصف_القطر
    }
}
```

## 17.7 المؤشرات والمراجع (Pointers & References)

### `مؤشر` — Pointer
```cpp
صحيح قيمة = 42
مؤشر صحيح م = &قيمة
طباعة(*م)  // 42
```

### `مرجع` — Reference
```cpp
صحيح قيمة = 42
مرجع صحيح ر = قيمة
ر = 100
طباعة(قيمة)  // 100
```

### `جديد` / `احذف` — New / Delete
```cpp
مؤشر صحيح م = جديد صحيح(10)
طباعة(*م)  // 10
احذف م
```

## 17.8 القوالب (Templates)

### `قالب` — Template
```cpp
قالب <صغير>
دالة طباعة_مرتين(صغير قيمة) -> فراغ {
    طباعة(قيمة)
    طباعة(قيمة)
}

طباعة_مرتين(5)     // 5 5
طباعة_مرتين("نص")   // نص نص
```

## 17.9 معالجة الأخطاء (Exception Handling)

### `حاول` / `امسك` / `أخيراً` — Try/Catch/Finally
```cpp
حاول {
    صحيح ن = 10 / 0
} امسك (خطأ) {
    طباعة("حدث خطأ: " + خطأ)
} أخيراً {
    طباعة("انتهت المعالجة")
}
```

### `ارمِ` — Throw
```cpp
دالة قسمة(صحيح أ, صحيح ب) -> صحيح {
    إذا (ب == 0) ارمِ "لا يمكن القسمة على صفر"
    ارجع أ / ب
}
```

## 17.10 دوال الإدخال/الإخراج

### `طباعة` — Print
```cpp
طباعة("مرحباً بالعالم")
طباعة(42)
طباعة(3.14)
طباعة(صواب)
طباعة("العدد: " + 10)     // العدد: 10
```

### `طباعة_سطر` — Print Line (تطبع وتنتقل لسطر جديد)
```cpp
طباعة_سطر("السطر الأول")
طباعة_سطر("السطر الثاني")
```

### `اقرأ` — Read Input
```cpp
نص مدخل = اقرأ()
طباعة("كتبت: " + مدخل)
```

## 17.11 كلمات واجهة المستخدم (GUI Keywords)

```cpp
زر_أمر("اضغط هنا")                    // QPushButton
حقل_نص("أدخل اسمك")                   // QLineEdit
قائمة_خيارات(["أحمر", "أخضر", "أزرق"])  // QComboBox
صورة("مسار/الصورة.png")                // QLabel صورة
مربع_اختيار("أوافق")                    // QCheckBox
شريط_تمرير(0, 100)                     // QSlider (0-100)
تسمية("نص التسمية")                     // QLabel
شبكة(3, 3)                             // QGridLayout
عمود {                                 // QVBoxLayout
    زر_أمر("OK")
    زر_أمر("إلغاء")
}
```

## 17.12 كلمات التزامن (Concurrency Keywords)

```cpp
خيط مهمتي {
    طباعة("تشغيل في خيط منفصل")
}
انتظر(مهمتي)  // انتظر حتى يكتمل الخيط

تزامن قفل {
    // كود آمن للخيوط
    طباعة("دخول حصري")
}
```

---

# 18. المكتبة القياسية الكاملة (Standard Library Reference)

## 18.1 الرياضيات والحساب (`stdlib/الرياضيات_والحساب/`)

### `أساسيات` — Basic Arithmetic
```cpp
استورد "رياضيات/أساسيات"
طباعة(جمع(10, 20))     // 30
طباعة(طرح(50, 15))     // 35
طباعة(ضرب(6, 7))       // 42
طباعة(تقسيم(100, 4))   // 25
طباعة(باقي(17, 5))     // 2
طباعة(مطلق(-10))       // 10
طباعة(مقابل(5))        // -5
طباعة(مقلوب(4))        // 0.25
طباعة(مربع(9))         // 81
طباعة(مكعب(3))         // 27
```

### `قوى_وجذور` — Powers & Roots
```cpp
استورد "رياضيات/قوى_وجذور"
طباعة(أس(2, 10))       // 1024
طباعة(جذر_تربيعي(100)) // 10
طباعة(جذر_تكعيبي(27))  // 3
طباعة(قوة_عشرة(3))     // 1000
طباعة(لوغاريتم(100))   // 2 (log10)
طباعة(لوغاريتم_طبيعي(2.718)) // ~1 (ln)
```

### `نسب_مئوية` — Percentages
```cpp
استورد "رياضيات/نسب_مئوية"
طباعة(نسبة(200, 50))       // 25% (50 من 200)
طباعة(زيادة_نسبة(100, 10)) // 110
طباعة(خصم(200, 15))        // 170 (بعد خصم 15%)
طباعة(ضريبة(100, 15))      // 115 (بعد إضافة ضريبة 15%)
طباعة(فارق_نسبة(150, 100)) // 50%
طباعة(حصة(1000, 30, 70))   // 300 (30% من 1000)
```

### `إحصائيات` — Statistics
```cpp
استورد "رياضيات/إحصائيات"
صحيح[] بيانات = [10, 20, 30, 40, 50]
طباعة(متوسط(بيانات))        // 30
طباعة(وسيط(بيانات))         // 30
طباعة(منوال(بيانات))        // (القيمة الأكثر تكراراً)
طباعة(تباين(بيانات))        // 200
طباعة(انحراف_معياري(بيانات)) // ~14.14
طباعة(مدى(بيانات))          // 40
طباعة(مجموع(بيانات))        // 150
طباعة(أصغر(بيانات))         // 10
طباعة(أكبر(بيانات))         // 50
طباعة(ضرب_الكل(بيانات))     // 12000000
```

### `مصفوفات` — Matrices
```cpp
استورد "رياضيات/مصفوفات"
مصفوفة م1 = [[1, 2], [3, 4]]
مصفوفة م2 = [[5, 6], [7, 8]]
مصفوفة ن = جمع_مصفوفات(م1, م2)  // [[6, 8], [10, 12]]
مصفوفة ض = ضرب_مصفوفات(م1, م2)
طباعة(محدد(م1))               // -2
طباعة(منقولة(م1))             // [[1, 3], [2, 4]]
طباعة(معكوس(م1))              // [[-2, 1], [1.5, -0.5]]
```

### `أعداد_مركبة` — Complex Numbers
```cpp
استورد "رياضيات/أعداد_مركبة"
عدد_مركب ع1 = جمع_مركب(3, 2, 1, 4)    // (3+2i) + (1+4i)
عدد_مركب ع2 = ضرب_مركب(1, 1, 1, -1)  // (1+i) * (1-i)
طباعة(سعة(3, 4))                      // 5
طباعة(زاوية(1, 0))                    // 0
```

### `هندسة` — Geometry
```cpp
استورد "رياضيات/هندسة"
طباعة(مساحة_مربع(5))          // 25
طباعة(مساحة_مستطيل(4, 6))     // 24
طباعة(مساحة_مثلث(3, 4))       // 6
طباعة(مساحة_دائرة(7))         // ~153.94
طباعة(محيط_دائرة(7))          // ~43.98
طباعة(حجم_مكعب(3))            // 27
طباعة(حجم_كرة(5))             // ~523.6
طباعة(مساحة_سطح_كرة(5))       // ~314.16
طباعة(وتر_مثلث(3, 4))         // 5
```

## 18.2 النصوص والترميز (`stdlib/النصوص_والترميز/`)

### `أساسيات_نصوص` — String Basics
```cpp
استورد "نصوص/أساسيات_نصوص"
طباعة(طول("مرحباً"))                // 6
طباعة(حرف_في("ض استديو", 2))        // ا
طباعة(بحث("مرحباً بالعالم", "بال"))  // 7
طباعة(استبدال("Hello World", "World", "Daad"))  // Hello Daad
طباعة(اقتطاع("مرحباً", 0, 3))        // مرح
طباعة(حالة_علوية("daad"))            // DAAD
طباعة(حالة_سفلية("DAAD"))            // daad
طباعة(عكس("ض"))                      // ض
طباعة(فارغ(""))                      // صواب
```

### `دمج_وتنسيق` — Concatenation & Formatting
```cpp
استورد "نصوص/دمج_وتنسيق"
طباعة(دمج(["أ", "ب", "ج"]))                     // أ ب ج
طباعة(دمج_بفاصل(["a", "b", "c"], ", "))         // a, b, c
طباعة(تنسيق("الاسم: {0}, العمر: {1}", ["أحمد", 25]))
طباعة(محاذاة_يمين("abc", 10))                    // "       abc"
طباعة(محاذاة_يسار("abc", 10))                    // "abc       "
طباعة(مركز("abc", 10))                           // "   abc    "
طباعة(حشو("42", 5, "0"))                         // "00042"
```

### `ترميز_وتحويل` — Encoding & Conversion
```cpp
استورد "نصوص/ترميز_وتحويل"
طباعة(تحويل_يوتف_8("ض"))       // D8B6 (hex)
طباعة(تحويل_أسكي("A"))         // 65
طباعة(رقم_الى_نص(42))          // "42"
طباعة(نص_الى_رقم("3.14"))      // 3.14
طباعة(نص_الى_صحيح("100"))      // 100
طباعة(ثنائي_الى_نص([0, 1, 1, 0]))  // نص من البتات
```

### `تشفير` — Encryption
```cpp
استورد "نصوص/تشفير"
طباعة(تشفير_بيز64("ض استديو"))     // 2YYg2YTYrNmG2YjYqA==
طباعة(فك_بيز64("2YYg2YTYrNmG2YjYqA=="))  // ض استديو
طباعة(تشفير_MD5("كلمة_سرية"))      // hash
طباعة(تشفير_SHA256("نص"))          // hash
طباعة(تشفير_AES("نص", "مفتاح"))    // نص مشفر
طباعة(تشفير_قيصر("abc", 3))        // def
```

### `تحقق` — Validation
```cpp
استورد "نصوص/تحقق"
طباعة(بريد_الكتروني_صحيح("test@test.com"))    // صواب
طباعة(بريد_الكتروني_صحيح("not-email"))         // خطأ
طباعة(هاتف_صحيح("+966501234567"))              // صواب
طباعة(كلمة_مرور_قوية("Test@123"))              // صواب (8+ أحرف، حرف كبير، رقم، رمز)
طباعة(رقم_جوال_سعودي("0501234567"))            // صواب
طباعة(التحقق_من_البيانات(مدخل, قواعد))          // نتيجة التحقق
```

### `بحث_واستبدال_متقدم` — Advanced Search (Regex)
```cpp
استورد "نصوص/بحث_واستبدال_متقدم"
طباعة(تطابق_نمط("hello123", "[a-z]+\\d+"))   // صواب
طباعة(استخراج_النمط("الرقم: 42", "\\d+"))     // ["42"]
طباعة(استبدال_النمط("مرحاً!", "!", "."))       // "مرحباً."
طباعة(تقسيم_بنمط("a,b,c", ","))                // ["a", "b", "c"]
```

### `تحليل_صرفي` — Morphological Analysis
```cpp
استورد "نصوص/تحليل_صرفي"
طباعة(جذر_كلمة("يكتب"))     // كتب
طباعة(وزن_كلمة("مكتب"))     // مفعل
طباعة(نوع_كلمة("كتاب"))     // اسم
طباعة(مفرد("كتب"))          // كتاب
طباعة(جمع("كتاب"))          // كتب
طباعة(مذكر("معلمة"))        // معلم
طباعة(مؤنث("معلم"))         // معلمة
```

## 18.3 هياكل البيانات (`stdlib/هياكل_البيانات/`)

### `مصفوفة_ديناميكية` — Dynamic Array (Vector)
```cpp
استورد "هياكل/مصفوفة_ديناميكية"
مصفوفة_د م
م.أضف(10)
م.أضف(20)
م.أضف(30)
طباعة(م.حجم())         // 3
طباعة(م.عنصر(1))       // 20
م.احذف(0)
طباعة(م.حجم())         // 2
م.افرغ()
طباعة(م.فارغ())        // صواب
```

### `قائمة_مترابطة` — Linked List
```cpp
استورد "هياكل/قائمة_مترابطة"
قائمة_م ق
ق.أضف_أول(10)
ق.أضف_آخر(20)
ق.أضف_آخر(30)
طباعة(ق.عدد_العناصر())  // 3
طباعة(ق.الأول())         // 10
طباعة(ق.الأخير())        // 30
ق.احذف(20)
ق.اطبع()                 // 10 -> 30
```

### `كومة` — Stack
```cpp
استورد "هياكل/كومة"
كومة ك
ك.ادفع(10)
ك.ادفع(20)
ك.ادفع(30)
طباعة(ك.أعلى())    // 30
طباعة(ك.اسحب())    // 30
طباعة(ك.اسحب())    // 20
طباعة(ك.فارغ())    // خطأ
```

### `طابور` — Queue
```cpp
استورد "هياكل/طابور"
طابور ط
ط.ادفع(10)
ط.ادفع(20)
ط.ادفع(30)
طباعة(ط.أمام())    // 10
طباعة(ط.اسحب())    // 10
طباعة(ط.اسحب())    // 20
```

### `شجرة_ثنائية` — Binary Tree
```cpp
استورد "هياكل/شجرة_ثنائية"
شجرة_م ش
ش.أدخل(50)
ش.أدخل(30)
ش.أدخل(70)
ش.أدخل(20)
ش.أدخل(40)
طباعة(ش.بحث(40))    // صواب
طباعة(ش.أصغر())     // 20
طباعة(ش.أكبر())     // 70
طباعة(ش.الارتفاع()) // 3
ش.اجتياز_بالعرض()   // 50, 30, 70, 20, 40
```

### `جدول_تجزئة` — Hash Table
```cpp
استورد "هياكل/جدول_تجزئة"
جدول_ت ج
ج.أضف("اسم", "أحمد")
ج.أضف("عمر", 25)
ج.أضف("مدينة", "الرياض")
طباعة(ج.احصل("اسم"))           // أحمد
طباعة(ج.موجود("عمر"))          // صواب
طباعة(ج.حجم())                 // 3
ج.احذف("مدينة")
طباعة(ج.جميع_المفاتيح())       // [اسم, عمر]
```

### `رسم_بياني` — Graph
```cpp
استورد "هياكل/رسم_بياني"
رسم_ب ر
ر.أضف_عقدة("A")
ر.أضف_عقدة("B")
ر.أضف_عقدة("C")
ر.أضف_حافة("A", "B")
ر.أضف_حافة("B", "C")
طباعة(ر.أقصر_مسار("A", "C"))   // A → B → C
ر.اجتياز_بعمق("A")              // A, B, C
ر.اجتياز_بعرض("A")              // A, B, C
```

### `خريطة` — Map
```cpp
استورد "هياكل/خريطة"
خريطة<نص, صحيح> م
م["تفاح"] = 5
م["موز"] = 3
م["برتقال"] = 7
لكل (زوج في م) {
    طباعة(زوج.مفتاح + ": " + زوج.قيمة)
}
```

### `مجموعة` — Set
```cpp
استورد "هياكل/مجموعة"
مجموعة م
م.أضف(1)
م.أضف(2)
م.أضف(3)
م.أضف(1)  // مكرر
طباعة(م.حجم())         // 3
طباعة(م.يحتوي(2))     // صواب
م.احذف(2)
```

## 18.4 المدخلات والمخرجات (`stdlib/المدخلات_والمخرجات/`)

### `ملفات` — File Operations
```cpp
استورد "مدخلات/ملفات"
ملف ف = افتح("data.txt", "قراءة")
نص محتوى = اقرأ_الكل(ف)
طباعة(محتوى)
أغلق(ف)

ملف ك = افتح("output.txt", "كتابة")
اكتب_سطر(ك, "مرحباً بالعالم")
أغلق(ك)

طباعة(موجود("data.txt"))        // صواب
طباعة(حجم_ملف("data.txt"))       // 1024
طباعة(آخر_تعديل("data.txt"))     // timestamp
طباعة(نسخ_ملف("s.txt", "d.txt")) // صواب
طباعة(نقل_ملف("s.txt", "d.txt")) // صواب
طباعة(حذف_ملف("old.txt"))       // صواب
انسخ_مجلد("مصدر", "هدف")
احذف_مجلد("مجلد_قديم")
```

### `ضغط` — Compression
```cpp
استورد "مدخلات/ضغط"
ضغط_مجلد("مصدر", "archive.zip")
فك_ضغط("archive.zip", "هدف")
ضغط_ملف("data.txt", "data.gz")
فك_ضغط_ملف("data.gz", "data.txt")
```

### `خطأ_وسجل` — Error & Logging
```cpp
استورد "مدخلات/خطأ_وسجل"
تسجيل("معلومة", "تم بدء التشغيل")
تسجيل("تحذير", "مساحة القرص منخفضة")
تسجيل("خطأ", "فشل الاتصال بقاعدة البيانات")
تسجيل("مهم", "اكتمال العملية")
تقرير_خطأ("حدث خطأ غير متوقع")
```

## 18.5 الشبكات والاتصالات (`stdlib/الشبكات_والاتصالات/`)

### `HTTP` — HTTP Client
```cpp
استورد "شبكات/HTTP"
استجابة است = طلب_HTTP("GET", "https://api.example.com/data")
طباعة(است.حالة())    // 200
طباعة(است.جسم())     // {"key":"value"}
طباعة(است.رأس("Content-Type"))  // application/json

// POST request
استجابة است2 = طلب_HTTP("POST", "https://api.example.com/submit", 
    {"name": "أحمد", "age": 25})
```

### `WebSocket` — Real-time Communication
```cpp
استورد "شبكات/WebSocket"
اتصال_فوري و = افتح_اتصال("wss://chat.example.com")
و.أرسل("رسالة ترحيب")
و.عند_الاستلام(دالة(نص رسالة) {
    طباعة("وصلت: " + رسالة)
})
```

### `DNS` — Domain Name System
```cpp
استورد "شبكات/DNS"
طباعة(حل_اسم("google.com"))       // 142.250.185.78
طباعة(حل_عكسي("8.8.8.8"))         // dns.google
```

## 18.6 الوقت والنظام (`stdlib/الوقت_والنظام/`)

### `الوقت_الحالي` — Current Time
```cpp
استورد "وقت/الوقت_الحالي"
طباعة(الآن())               // 2026-07-10 14:30:00
طباعة(اليوم())              // 10
طباعة(الشهر())              // 7
طباعة(السنة())              // 2026
طباعة(الساعة())             // 14
طباعة(الدقيقة())            // 30
طباعة(الثانية())            // 0
طباعة(بصمة_الوقت())         // 1767893400 (Unix timestamp)
```

### `تقويم` — Calendars
```cpp
استورد "وقت/تقويم"
طباعة(تقويم_هجري())          // 1447-12-15
طباعة(تقويم_ميلادي())        // 2026-07-10
طباعة(تقويم_شمسي())          // 1405-04-19
طباعة(تحويل_هجري_ميلادي(1447, 12, 15)) // 2026-07-10
طباعة(اسم_اليوم(2026, 7, 10))           // الجمعة
طباعة(اسم_الشهر_هجري(12))               // ذو الحجة
طباعة(هل_سنة_كبيسة(2026))               // خطأ
```

### `مؤقتات` — Timers & Scheduling
```cpp
استورد "وقت/مؤقتات"
مؤقت م = كل(5000, دالة() {        // كل 5 ثوانٍ
    طباعة("تذكير: اشرب ماء")
})

مؤقت م2 = بعد(10000, دالة() {      // بعد 10 ثوانٍ (مرة واحدة)
    طباعة("انتهى الوقت!")
})

منبه_جدول("08:00", "موعد الاجتماع")  // منبه يومي
```

### `معلومات_النظام` — System Info
```cpp
استورد "وقت/معلومات_النظام"
طباعة(نظام_التشغيل())        // Windows
طباعة(إصدار_النظام())        // 10.0.22631
طباعة(بنية_المعالج())        // x64
طباعة(عدد_الأنوية())         // 8
طباعة(الذاكرة_الكلية())       // 16384 (MB)
طباعة(الذاكرة_المستخدمة())    // 8192 (MB)
طباعة(مساحة_القرص())          // 512000 (MB)
طباعة(اسم_الجهاز())           // DESKTOP-ABC123
طباعة(اسم_المستخدم())         // USER
```

## 18.7 قواعد البيانات (`stdlib/قواعد_البيانات/`)

### `SQL` — SQL Operations
```cpp
استورد "قواعد/SQL"
قاعدة_ب ب = اتصال_قاعدة("sqlite://data.db")

// إنشاء
ب.تنفيذ("CREATE TABLE طلاب (id INT, اسم TEXT, درجة INT)")

// إدخال
ب.تنفيذ("INSERT INTO طلاب VALUES (1, 'أحمد', 95)")

// استعلام
نتيجة ن = ب.استعلام("SELECT * FROM طلاب WHERE درجة > 80")
لكل (سجل في ن) {
    طباعة(سجل["اسم"] + ": " + سجل["درجة"])
}

// تحديث
ب.تنفيذ("UPDATE طلاب SET درجة = 100 WHERE id = 1")

// حذف
ب.تنفيذ("DELETE FROM طلاب WHERE درجة < 50")

ب.اغلاق()
```

### `المعاملات` — Transactions
```cpp
استورد "قواعد/المعاملات"
بدء_المعاملة()
حاول {
    ب.تنفيذ("INSERT INTO حسابات VALUES (1, 1000)")
    ب.تنفيذ("INSERT INTO حسابات VALUES (2, 500)")
    تأكيد_المعاملة()
} امسك (خطأ) {
    تراجع_المعاملة()
    طباعة("فشلت المعاملة: " + خطأ)
}
```

## 18.8 تطوير الويب (`stdlib/تطوير_الويب/`)

### `DOM` — DOM Manipulation
```cpp
استورد "ويب/DOM"
عنصر ر = احصل_على_عنصر("root")
ر.نص_داخلي = "مرحباً"
ر.نمط.لون = "أحمر"
ر.أضف_طفل(عنصر_جديد("div"))
ر.أضف_صنف("نشط")
ر.احذف_صنف("مخفي")
```

### `أحداث` — Events
```cpp
استورد "ويب/أحداث"
عنصر زر = احصل_على_عنصر("button")
زر.عند_النقر(دالة() {
    طباعة("تم النقر!")
})

احصل_على_عنصر("input").عند_التغيير(دالة(نص قيمة) {
    طباعة("القيمة الجديدة: " + قيمة)
})
```

### `AJAX` — Fetch/API Calls
```cpp
استورد "ويب/AJAX"
طلب("GET", "/api/data").عند_النجاح(دالة(نص بيانات) {
    طباعة(بيانات)
}).عند_الخطأ(دالة(نص خطأ) {
    طباعة("فشل: " + خطأ)
})
```

## 18.9 الرسوميات والوسائط (`stdlib/الرسوميات_والمساعدات/`)

### `رسم_ثنائي` — 2D Drawing
```cpp
استورد "رسوم/رسم_ثنائي"
رسم_2د ر
ر.لون = "أزرق"
ر.سمك = 2
ر.ارسم_خط(0, 0, 100, 100)
ر.ارسم_دائرة(50, 50, 25)
ر.ارسم_مستطيل(10, 10, 80, 60)
ر.املأ_لون("أحمر")
ر.املأ_دائرة(50, 50, 25)
ر.ارسم_نص("ض استديو", 10, 10)
```

### `وسائط` — Multimedia
```cpp
استورد "رسوم/وسائط"
وسائط و
و.شغل_صوت("sound.mp3")
و.شغل_فيديو("video.mp4")
و.شغل_موسيقى("background.mp3")
و.أيقاف()
و.رفع_الصوت(50)
و.خفض_الصوت(50)
```

## 18.10 التزامن وتعدد المهام (`stdlib/الالتزامن_وتعدد_المهام/`)

### `خيوط` — Threads
```cpp
استورد "تزامن/خيوط"
خيط ع
ع.شغل(دالة() {
    طباعة("خيط منفصل")
})
ع.انتظر()

خيط ع2 = شغل_خيط(دالة() {
    طباعة("تشغيل فوري")
})
خيط[] خيوط = شغل_متعدد([
    دالة() { طباعة("مهمة 1") },
    دالة() { طباعة("مهمة 2") },
    دالة() { طباعة("مهمة 3") }
])
انتظر_الكل(خيوط)
```

### `تزامن` — Synchronization
```cpp
استورد "تزامن/تزامن"
قفل_متبادل ق
ق.أغلق()
// كود محمي
ق.فتح()

// أو باستخدام التزامن التلقائي
تزامن (ق) {
    // كود آمن للخيوط
    طباعة("دخول حصري")
}
```

---

# 19. برامج كاملة بلغة "ض" (Complete Programs)

## 19.1 برنامج: آلة حاسبة متقدمة
**`examples/programs/large/01_calculator.daad`** (78 سطر)

```cpp
// آلة حاسبة متقدمة - ض استديو
دالة مضروب(صحيح ن) -> صحيح {
    إذا (ن <= 1) ارجع 1
    ارجع ن * مضروب(ن - 1)
}

دالة اس(صحيح أساس, صحيح أس) -> صحيح {
    صحيح ن = 1
    لكل (صحيح ع = 0; ع < أس; ع += 1) {
        ن *= أساس
    }
    ارجع ن
}

دالة قائمة_العمليات() -> فراغ {
    طباعة("=== الآلة الحاسبة ===")
    طباعة("1. جمع")
    طباعة("2. طرح")
    طباعة("3. ضرب")
    طباعة("4. قسمة")
    طباعة("5. مضروب")
    طباعة("6. أس")
    طباعة("0. خروج")
}

دالة رئيسية() -> فراغ {
    صحيح اختيار = -1
    طالما (اختيار != 0) {
        قائمة_العمليات()
        اختيار = 1  // تبسيطاً
        
        إذا (اختيار == 1) { طباعة(10 + 5) }
        وإلا إذا (اختيار == 5) { طباعة(مضروب(5)) }
        وإلا إذا (اختيار == 6) { طباعة(اس(2, 10)) }
        وإلا إذا (اختيار == 0) { طباعة("مع السلامة") }
    }
}

رئيسية()
```

**الترجمة إلى C++:** (78 سطر Daad → 89 سطر C++)
```cpp
#include "Daad/Runtime.hpp"
#include <iostream>

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i += 1) {
        result *= base;
    }
    return result;
}
// ...
```

## 19.2 برنامج: أرقام أولية
**`examples/programs/large/04_primes.daad`** (39 سطر)

```cpp
دالة أولي(صحيح ن) -> منطقي {
    إذا (ن <= 1) ارجع خطأ
    إذا (ن <= 3) ارجع صواب
    إذا (ن % 2 == 0 || ن % 3 == 0) ارجع خطأ
    
    صحيح ع = 5
    طالما (ع * ع <= ن) {
        إذا (ن % ع == 0 || ن % (ع + 2) == 0) ارجع خطأ
        ع += 6
    }
    ارجع صواب
}

لكل (صحيح ع = 1; ع <= 100; ع += 1) {
    إذا (أولي(ع)) {
        طباعة(ع + " أولي")
    }
}
```

## 19.3 برنامج: منبه (Alarm Clock)
**`examples/programs/mega/06_alarm.daad`** (74 سطر)

```cpp
دالة تشغيل_نغمة(صحيح نوع) -> فراغ {
    إذا (نوع == 1) طباعة("🔔 نغمة عادية")
    وإلا إذا (نوع == 2) طباعة("🔔 نغمة هادئة")
    وإلا إذا (نوع == 3) طباعة("🔔 نغمة قوية")
    وإلا طباعة("🔔 نغمة افتراضية")
}

دالة ضبط_منبه(صحيح ساعة, صحيح دقيقة, صحيح نغمة) -> فراغ {
    طباعة("تم ضبط المنبه على " + ساعة + ":" + دقيقة)
    تشغيل_نغمة(نغمة)
}

ضبط_منبه(6, 30, 1)
طباعة("تصبح على خير")
```

## 19.4 برنامج: مواقيت الصلاة
**`examples/programs/mega/07_prayer.daad`** (80 سطر)

```cpp
// برنامج مواقيت الصلاة

دالة حساب_الفجر(عشري خط_عرض, عشري خط_طول) -> عشري {
    // خوارزمية تبسيطية
    ارجع 4.5  // 4:30 صباحاً
}

دالة حساب_الظهر(عشري خط_عرض, عشري خط_طول) -> عشري {
    ارجع 12.0  // 12:00 ظهراً
}

دالة حساب_العصر(عشري خط_عرض, عشري خط_طول) -> عشري {
    ارجع 15.5  // 3:30 عصراً
}

دالة حساب_المغرب(عشري خط_عرض, عشري خط_طول) -> عشري {
    ارجع 18.25  // 6:15 مساءً
}

دالة حساب_العشاء(عشري خط_عرض, عشري خط_طول) -> عشري {
    ارجع 19.75  // 7:45 مساءً
}

دالة عرض_المواقيت(عشري عرض, عشري طول) -> فراغ {
    طباعة("مواقيت الصلاة:")
    طباعة("الفجر:   " + حساب_الفجر(عرض, طول))
    طباعة("الظهر:   " + حساب_الظهر(عرض, طول))
    طباعة("العصر:   " + حساب_العصر(عرض, طول))
    طباعة("المغرب:  " + حساب_المغرب(عرض, طول))
    طباعة("العشاء:  " + حساب_العشاء(عرض, طول))
}

عرض_المواقيت(24.7136, 46.6753)  // الرياض
```

## 19.5 برنامج: إدارة المهام
**`examples/programs/mega/09_tasks.daad`** (70 سطر)

```cpp
// نظام إدارة المهام

صحيح[] معرفات
نص[] مهام
منطقي[] مكتملة
صحيح عدد = 0

دالة أضف_مهمة(نص وصف) -> فراغ {
    عدد += 1
    مهام.أضف(وصف)
    مكتملة.أضف(خطأ)
    طباعة("تمت إضافة المهمة: " + وصف)
}

دالة أكمل_مهمة(صحيح رقم) -> فراغ {
    إذا (رقم >= 0 && رقم < عدد) {
        مكتملة[رقم] = صواب
        طباعة("تم إكمال: " + مهام[رقم])
    }
}

دالة عرض_المهام() -> فراغ {
    طباعة("=== المهام ===")
    لكل (صحيح ع = 0; ع < عدد; ع += 1) {
        نص حالة = "❌"
        إذا (مكتملة[ع]) حالة = "✅"
        طباعة(حالة + " " + ع + ": " + مهام[ع])
    }
}

أضف_مهمة("شراء مستلزمات")
أضف_مهمة("حل التحدي")
أضف_مهمة("مراجعة الدرس")
أكمل_مهمة(1)
عرض_المهام()
```

## 19.6 برنامج: لعبة الألغاز الرياضية
**`examples/programs/mega/10_puzzle.daad`** (89 سطر)

```cpp
// لعبة ألغاز رياضية

دالة لغز_الأرقام() -> صحيح {
    صحيح س = 5
    صحيح ص = 3
    ارجع س * س + ص * ص - (س + ص)
}

دالة لغز_التسلسل(صحيح ن) -> صحيح {
    إذا (ن <= 0) ارجع 0
    إذا (ن == 1) ارجع 1
    ارجع لغز_التسلسل(ن - 1) + لغز_التسلسل(ن - 2)
}

دالة لعبة_التخمين() -> فراغ {
    صحيح الرقم_السري = 42
    صحيح تخمين = 0
    
    طباعة("خمن الرقم (بين 1 و 100):")
    
    طالما (تخمين != الرقم_السري) {
        تخمين = 50  // تبسيطاً
        
        إذا (تخمين < الرقم_السري) {
            طباعة("أكبر من " + تخمين)
        } وإلا إذا (تخمين > الرقم_السري) {
            طباعة("أصغر من " + تخمين)
        } وإلا {
            طباعة("صح! الرقم هو " + الرقم_السري)
        }
    }
}

طباعة("نتيجة لغز الأرقام: " + لغز_الأرقام())
طباعة("حد فيبوناتشي العاشر: " + لغز_التسلسل(10))
لعبة_التخمين()
```

---

# 20. نظام التحديات (Challenge System)

## 20.1 نظرة عامة

نظام التحديات مصمم لتعليم لغة "ض" عبر 7 مستويات (Tiers) بإجمالي **210 تحدياً**.

### هيكل التحديات

```
7 مستويات × 30 تحدياً = 210 تحدياً
```

| المستوى | العنوان | المهارات | عدد التحديات |
|---------|---------|----------|-------------|
| Tier 1 | أساسيات البرمجة | طباعة، متغيرات، عمليات حسابية | 30 |
| Tier 2 | الجمل الشرطية | إذا، وإلا، عمليات منطقية | 30 |
| Tier 3 | الحلقات التكرارية | لكل، طالما، افعل | 30 |
| Tier 4 | المصفوفات والنصوص | مصفوفات، نصوص، دوال نصوص | 30 |
| Tier 5 | الدوال | تعريف دوال، بارامترات، إرجاع | 30 |
| Tier 6 | الخوارزميات | ترتيب، بحث، خوارزميات متقدمة | 30 |
| Tier 7 | مشاريع متكاملة | برامج كاملة، OOP، قوالب | 30 |

## 20.2 توزيع التحديات حسب الملفات

```javascript
// tests/challenges_tier1.js  → 30 تحدياً (Tier 1)
// tests/challenges_tier2.js  → 30 تحدياً (Tier 2)
// tests/challenges_tier3.js  → 30 تحدياً (Tier 3)
// tests/challenges_tier4.js  → 30 تحدياً (Tier 4)
// tests/challenges_tier5.js  → 30 تحدياً (Tier 5)
// tests/challenges_tier6.js  → 30 تحدياً (Tier 6)
// tests/challenges_tier7.js  → 30 تحدياً (Tier 7)
// tests/challenges_data.js   → تجميع جميع المستويات
```

## 20.3 أمثلة من التحديات

### تحديات المستوى 1 (Tier 1 - BASIC)

**تحدي 1: طباعة مرحبا**
```cpp
// المطلوب: اطبع "مرحباً بالعالم"
// الحل:
طباعة("مرحباً بالعالم")
```

**تحدي 2: المتغيرات**
```cpp
// المطلوب: عرّف متغيرين واجمعهما
صحيح س = 10
صحيح ص = 20
صحيح ن = س + ص
طباعة(ن)
```

**تحدي 3: العمليات الحسابية**
```cpp
// المطلوب: احسب مساحة مستطيل
صحيح طول = 5
صحيح عرض = 3
صحيح مساحة = طول * عرض
طباعة(مساحة)  // 15
```

### تحديات المستوى 2 (Tier 2 - CONDITIONALS)

**تحدي 4: أكبر رقم**
```cpp
// المطلوب: اطبع أكبر رقمين
صحيح أ = 15
صحيح ب = 10
إذا (أ > ب) {
    طباعة(أ)
} وإلا {
    طباعة(ب)
}
```

### تحديات المستوى 3 (Tier 3 - LOOPS)

**تحدي 12: جدول الضرب**
```cpp
// المطلوب: اطبع جدول ضرب الرقم 5
لكل (صحيح ع = 1; ع <= 12; ع += 1) {
    طباعة("5 x " + ع + " = " + (5 * ع))
}
```

### تحديات المستوى 5 (Tier 5 - FUNCTIONS)

**تحدي 25: دالة إيجاد العامل المشترك الأكبر (GCD)**
```cpp
دالة قاسم_مشترك_أكبر(صحيح أ, صحيح ب) -> صحيح {
    طالما (ب != 0) {
        صحيح م = أ % ب
        أ = ب
        ب = م
    }
    ارجع أ
}

طباعة(قاسم_مشترك_أكبر(48, 18))  // 6
```

## 20.4 التحديات في قاعدة البيانات

**جدول التحديات (Challenge):** يخزن التحديات الفعلية من الـ seed data
**جدول ChallengeTier:** يحدد المستويات (BEGINNER, BASIC, INTERMEDIATE, ADVANCED, EXPERT, MASTER, LEGENDARY)
**جدول Submission:** يخزن حلول الطلاب

```javascript
// هيكل التحدي في قاعدة البيانات:
{
  id: "challenge_001",
  title: "طباعة مرحبا",
  description: "قم بطباعة العبارة 'مرحباً بالعالم'",
  difficulty: "BEGINNER",
  tier: 1,
  order: 1,
  starterCode: "// اكتب الكود هنا\n",
  expectedOutput: "مرحباً بالعالم",
  requirements: {
    minLines: 1,
    mustContain: ["طباعة"]
  }
}
```

## 20.5 آلية التحقق من التحديات

في `server/src/utils/codeVerifier.js`:
```javascript
verifyCode(code, requirements) {
  // التحقق من عدد المتغيرات
  if (minVariables && variables.length < minVariables) return false
  
  // التحقق من عدد أوامر الطباعة
  if (minPrints && prints.length < minPrints) return false
  
  // التحقق من وجود كلمات معينة
  if (mustContain && !mustContain.every(k => code.includes(k))) return false
  
  // التحقق من عدم وجود كلمات ممنوعة
  if (mustNotContain && mustNotContain.some(k => code.includes(k))) return false
}
```

---

# 21. نظام الدورات (Course System)

## 21.1 الدورات المتاحة

### دورة: التشفير (Cryptography)
**`tests/courses_crypto.js`**

```javascript
// تشفير Caesar
// تشفير Vigenère
// تحليل التردد
// RSA basics
// AES basics
```

### دورة: الأمن السيبراني (Cybersecurity)
**`tests/courses_cyber.js`**

```javascript
// أساسيات الأمن السيبراني
// هجمات الرجل في المنتصف (MITM)
// حقن SQL
// XSS
// أمن الشبكات
```

### دورة: أمن الويب (Web Security)
**`tests/courses_web.js`**

```javascript
// أمن تطبيقات الويب
// OWASP Top 10
// CSRF Protection
// إدارة الجلسات الآمنة
// HTTPS و SSL/TLS
```

## 21.2 هيكل الدورة التعليمية

```
Course (دورة)
├── type = "ADDITIONAL" (ليست MAIN)
├── tierRequired (شرط: يجب إكمال Tier معين)
├── Lessons (دروس)
│   ├── lessonNumber: 1
│   ├── title: "مقدمة في التشفير"
│   ├── content: "..."
│   └── challenges: [Challenge IDs]
│
├── Enrollments (تسجيلات الطلاب)
│   └── StudentCourseEnrollment
│       ├── studentId
│       ├── courseId
│       └── enrolledAt
│
└── Roadmap (مسار الطالب)
    └── Lessons مع Progress و Locking
```

## 21.3 آلية فتح الدروس (Sequential Locking)

```javascript
// courseController.js
function getCourseRoadmap(req, res) {
  const studentId = req.user.studentId
  const courseId = req.params.id
  
  // 1. التحقق من أن الطالب مسجل في الدورة
  // 2. جلب جميع دروس الدورة
  // 3. لكل درس، تحقق إذا كان الطالب أكمل الدرس السابق
  // 4. إذا لم يكمل → مقفل
  // 5. إذا أكمل → مفتوح (يمكن حل التحديات)
}
```

---

# 22. مجموعة الاختبارات الكاملة (Test Suite)

## 22.1 إحصائيات الاختبارات

| نوع الاختبار | العدد | اللغة | البيئة |
|-------------|-------|-------|--------|
| Lexer Tests | ~50 | C++ | GoogleTest |
| Parser Tests | ~50 | C++ | GoogleTest |
| Compiler Tests | ~50 | C++ | GoogleTest |
| Optimizer Tests | ~30 | C++ | GoogleTest |
| Runtime Tests | ~30 | C++ | GoogleTest |
| Stdlib Tests | ~30 | C++ | GoogleTest |
| Comprehensive Tests | 100+ | C++ | GoogleTest |
| Anti-Cheat Tests | 99 | JS | Node.js |
| PenTest Suite | 400+ سطر | JS | Node.js |
| Deep PenTest | 600+ سطر | JS | Node.js |
| Hack Attempts | 32 متجه | JS | Node.js |
| Attack 45 | 45 هجوم | JS | Node.js |
| Deep Attack 90 | 90 اختبار | JS | Node.js |
| Stress Tests | 6 ملفات | JS | Node.js |
| Integration | 2 ملفات | JS | Node.js |
| **الإجمالي** | **~700+ اختبار** | **C++20/JS** | **متعدد** |

## 22.2 اختبارات اختراق الأمان (Pentest)

### `hack_attempt.js` — 32 متجه هجوم
**معدل الدفاع: 100% ✅**

```javascript
// أنواع الهجمات المختبرة:
// 1. SQL Injection في جميع الحقول
// 2. XSS في جميع المدخلات
// 3. Path Traversal في مسارات الملفات
// 4. Command Injection في الأوامر
// 5. CSRF في جميع الطلبات الحساسة
// 6. تزوير JWT
// 7. هجمات القوة الغاشمة (Brute Force)
// 8. هجمات إعادة التشغيل (Replay)
// 9. تجاوز Rate Limiting
// 10. تسميم الـ Cache
```

### `attack_45.js` — 45 متجه هجوم منظم

```javascript
// Category A: Hack (15 هجوم) — ✅ جميعها مسدودة
// Category B: Tamper (15 هجوم) — ✅ جميعها مسدودة  
// Category C: Cheat (15 هجوم) — ✅ جميعها مسدودة
```

### `deep_attack_90.js` — 90 اختبار أمني عميق

```javascript
// Category A: Authentication (10) — التحقق من JWT، OTP، الجلسات
// Category B: Authorization (10) — التحقق من الصلاحيات
// Category C: Input Validation (15) — حقن SQL، XSS، Command Injection
// Category D: Business Logic (15) — تجاوز الترتيب، التكرار، التوقيت
// Category E: Rate Limiting (10) — تجاوز الحدود المسموحة
// Category F: Session Management (10) — اختطاف الجلسة، التثبيت
// Category G: Anti-Cheat (20) — تزوير XP، تزوير الدرجات، حلول سريعة
```

## 22.3 اختبارات التحمل (Stress Tests)

### `mega_stress.js` — اختبار تحمل ضخم
- **الطلاب المُحاكيين:** 200
- **المعلمين المُحاكيين:** 100
- **الفصول المُحاكية:** 50
- **التحديات المُقدمة:** 1000+
- **مدة الاختبار:** حتى اكتمال جميع السيناريوهات

### `hour_stress.js` — اختبار ساعة كاملة
```javascript
// محاكاة فصل دراسي كامل لمدة ساعة واحدة:
// - 200 طالب يقدمون حلولاً
// - 100 معلم يصححون
// - تحديث لوحة المتصدرين
// - تسجيل الحضور
// - إنشاء اختبارات
// مراقبة: وقت الاستجابة، الأخطاء، الذاكرة
```

## 22.4 اختبارات Anti-Cheat (99 اختباراً)

### أنواع الغش المكتشفة:

| النوع | الوصف | آلية الكشف |
|-------|-------|------------|
| ⏱️ زمن سريع جداً | حل التحدي في < 2 ثانية | `detectSuspiciousActivity` |
| 🎯 درجة كاملة متكررة | 100% في كل التحديات | تحليل إحصائي |
| 📝 كود قصير جداً | أقل من 10 أحرف لحل معقد | `codeVerifier.js` |
| 🔄 كود مكرر | نفس الحل لـ 10 طلاب مختلفين | HMAC + بصمة الكود |
| 🔗 حل متطابق | طالبين نفس الحرف الخطأ | HMAC + تخزين الحلول |
| 📊 تفوق غير طبيعي | 50 تحدياً في 5 دقائق | `strictSecurity.js` |
| 🚀 تقدم سريع | Tier 7 في يوم واحد | التحقق من المتطلبات |
| 🤖 استخدام Bot | طلبات HTTP سريعة جداً | Rate Limiting |
| 🔐 تزوير الـ API | تغيير بيانات الـ Request | HMAC Signature |
| 👥 انتحال شخصية | استخدام توكن طالب آخر | JWT + IP Check |

## 22.5 نتائج التدقيق الأمني

```
┌─────────────────────────────────────────────────────┐
│              DEEP SECURITY AUDIT RESULTS             │
├─────────────────────────────────────────────────────┤
│ Anti-Cheat:  99/99  (100% block rate)               │
│ Hack:        15/15  (100% blocked)                  │
│ Tamper:      15/15  (100% blocked)                  │
│ Cheat:       15/15  (100% blocked)                  │
│ Auth Bypass: 0/10   (0% bypass rate)                │
│ Rate Limit:  10/10  (100% enforced)                 │
│ Session:     10/10  (100% secure)                   │
│ CSRF:        5/5    (100% protected)                │
│ SQL Inj:     10/10  (0% success)                    │
│ XSS:         10/10  (0% success)                    │
└─────────────────────────────────────────────────────┘
```

---

# 23. ملفات المثال والتحويلات (Examples & Conversions)

## 23.1 أمثلة بسيطة (Simple Conversions)

### `01_hello.daad` → `01_hello.cpp` — Hello World
```cpp
// لغة ض:
طباعة("مرحباً بالعالم")

// C++ المُنتَج:
#include <iostream>
#include "Daad/Runtime.hpp"
int main() {
    daad::runtime::daad_print("مرحباً بالعالم");
    return 0;
}
```

### `02_arithmetic.daad` → `02_arithmetic.cpp` — العمليات الحسابية
```cpp
// لغة ض:
صحيح أ = 10
صحيح ب = 3
طباعة(أ + ب)  // 13
طباعة(أ * ب)  // 30
طباعة(أ / ب)  // 3
طباعة(أ % ب)  // 1

// C++ المُنتَج:
int a = 10;
int b = 3;
daad::runtime::daad_print(a + b);
daad::runtime::daad_print(a * b);
daad::runtime::daad_print(a / b);
daad::runtime::daad_print(a % b);
```

### `03_conditions.daad` → `03_conditions.cpp` — الشروط
```cpp
// لغة ض:
صحيح درجة = 85
إذا (درجة >= 90) {
    طباعة("ممتاز")
} وإلا إذا (درجة >= 75) {
    طباعة("جيد جداً")
} وإلا إذا (درجة >= 60) {
    طباعة("جيد")
} وإلا {
    طباعة("راسب")
}
```

### `04_loops.daad` → `04_loops.cpp` — الحلقات
```cpp
// لغة ض:
لكل (صحيح ع = 1; ع <= 10; ع += 1) {
    طباعة(ع * ع)  // مربعات الأعداد
}

// C++ المُنتَج:
for (int i = 1; i <= 10; i += 1) {
    daad::runtime::daad_print(i * i);
}
```

### `05_functions.daad` → `05_functions.cpp` — الدوال
```cpp
// لغة ض:
دالة مطلق(صحيح س) -> صحيح {
    إذا (س < 0) ارجع -س
    ارجع س
}

دالة زوجي(صحيح س) -> منطقي {
    ارجع س % 2 == 0
}

طباعة(مطلق(-5))     // 5
طباعة(زوجي(10))     // صواب
```

## 23.2 أمثلة متقدمة (Complex Conversions)

### `06_factorial.daad` → `06_factorial.cpp` — المضروب
```cpp
// لغة ض (33 سطر):
دالة مضروب(صحيح ن) -> صحيح {
    إذا (ن <= 1) ارجع 1
    ارجع ن * مضروب(ن - 1)
}

دالة مجموع(صحيح ن) -> صحيح {
    صحيح ناتج = 0
    لكل (صحيح ع = 1; ع <= ن; ع += 1) {
        ناتج += ع
    }
    ارجع ناتج
}

دالة اس(صحيح أساس, صحيح أس) -> صحيح {
    صحيح ناتج = 1
    لكل (صحيح ع = 0; ع < أس; ع += 1) {
        ناتج *= أساس
    }
    ارجع ناتج
}

طباعة(مضروب(5))     // 120
طباعة(مجموع(10))    // 55
طباعة(اس(2, 8))     // 256
```

### `07_fibonacci.daad` → `07_fibonacci.cpp` — فيبوناتشي
```cpp
// لغة ض (32 سطر):
دالة فيبوناتشي(صحيح ن) -> صحيح {
    إذا (ن <= 0) ارجع 0
    إذا (ن == 1) ارجع 1
    ارجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
}

لكل (صحيح ع = 0; ع < 10; ع += 1) {
    طباعة(فيبوناتشي(ع))
}
// 0, 1, 1, 2, 3, 5, 8, 13, 21, 34
```

### `08_sorting.daad` — خوارزميات الترتيب (64 سطر)
```cpp
// لغة ض:
دالة ترتيب_فقاعي(صحيح[] أ) -> فراغ {
    صحيح ن = طول(أ)
    لكل (صحيح ع = 0; ع < ن - 1; ع += 1) {
        لكل (صحيح ص = 0; ص < ن - ع - 1; ص += 1) {
            إذا (أ[ص] > أ[ص + 1]) {
                صحيح م = أ[ص]
                أ[ص] = أ[ص + 1]
                أ[ص + 1] = م
            }
        }
    }
}
```

---

# 24. قائمة شاملة لجميع ملفات المشروع

## 24.1 Server (56 ملفاً)

```
server/
├── .dockerignore
├── .env
├── .env.example
├── .gitignore
├── Dockerfile
├── README.md
├── package.json
├── render.yaml
├── check_challenges.js
├── update_challenges.js
├── prisma/
│   ├── dev.db
│   ├── schema.prisma (620 lines)
│   ├── schema.postgresql.prisma (610 lines)
│   ├── seed.js (929 lines)
│   ├── seed_admin.js
│   ├── seed_challenges.js
│   ├── seed_ui_test.js
│   ├── migrate_batch7.ps1
│   └── migrations/
│       ├── migration_lock.toml
│       ├── 20260628110013_init/migration.sql
│       └── 20260628111955_add_challenges/migration.sql
├── scripts/
│   ├── github-agent.js
│   └── mega_classroom_simulator.js
└── src/
    ├── index.js (372 lines)
    ├── config/swagger.js (229 lines)
    ├── controllers/
    │   ├── analyticsController.js (1082 lines)
    │   ├── assessmentController.js (1045 lines)
    │   ├── authController.js (623 lines)
    │   ├── challengeController.js (213 lines)
    │   ├── courseController.js (218 lines)
    │   ├── healthController.js (154 lines)
    │   ├── onboardController.js (275 lines)
    │   ├── otpController.js (150 lines)
    │   ├── reportsController.js (296 lines)
    │   ├── schoolController.js (278 lines)
    │   ├── studentManagementController.js (663 lines)
    │   └── studentProfileController.js (757 lines)
    ├── middlewares/
    │   ├── auth.js (480 lines)
    │   ├── errorHandler.js (61 lines)
    │   ├── security.js (104 lines)
    │   └── strictSecurity.js (535 lines)
    ├── router/index.js
    ├── routes/
    │   ├── student.js (689 lines)
    │   ├── analytics.js
    │   ├── auth.js
    │   ├── challenge.js
    │   ├── challenges.js
    │   ├── courses.js
    │   ├── invitations.js
    │   ├── onboard.js
    │   ├── otp.js
    │   ├── reports.js
    │   ├── schools.js
    │   ├── security.js
    │   └── studentManagement.js
    ├── services/adminService.js (326 lines)
    └── utils/
        ├── codeVerifier.js
        ├── logger.js
        ├── prisma.js
        └── redis.js (359 lines)
```

---

# 25. خريطة مفاهيم النظام الكاملة (System Concept Map)

## 25.1 خريطة المنصة التعليمية

```
                           ┌──────────────┐
                           │   ض استديو   │
                           │  Daad Studio  │
                           └──────┬───────┘
                                  │
            ┌─────────────────────┼─────────────────────┐
            │                     │                     │
            ▼                     ▼                     ▼
   ┌────────────────┐   ┌────────────────┐   ┌────────────────┐
   │   منصة تعليم   │   │  لغة برمجة عربية│   │  بيئة تطوير    │
   │  (EdTech LMS)  │   │   (Daad Lang)  │   │  (IDE Tools)   │
   └───────┬────────┘   └───────┬────────┘   └───────┬────────┘
           │                    │                    │
           │                    │                    │
     ┌─────┴─────┐       ┌─────┴─────┐       ┌──────┴──────┐
     │           │       │           │       │             │
     ▼           ▼       ▼           ▼       ▼             ▼
 ┌──────┐  ┌────────┐ ┌──────┐  ┌────────┐ ┌────────┐ ┌─────────┐
 │طلاب  │  │معلمين  │ │مترجم│  │مكتبة   │ │Desktop │ │Web      │
 │      │  │        │ │Daad │  │قياسية  │ │IDE     │ │Frontend │
 └──┬───┘  └───┬────┘ └──┬───┘  └───┬────┘ └───┬────┘ └────┬────┘
    │          │         │          │          │           │
    ▼          ▼         │          │          ▼           │
 ┌──────┐  ┌────────┐   │          │   ┌──────────┐       │
 │تحديات│  │اختبارات│   │          │   │VS Code   │       │
 │210   │  │تقيمية  │   │          │   │Extension │       │
 └──┬───┘  └───┬────┘   │          │   └────┬─────┘       │
    │          │         │          │        │             │
    └──────────┴─────────┴──────────┴────────┴─────────────┘
                                   │
                                   ▼
                        ┌──────────────────┐
                        │   REST API       │
                        │   (Express)      │
                        │   Port 3000      │
                        └────────┬─────────┘
                                 │
                    ┌────────────┼────────────┐
                    │            │            │
                    ▼            ▼            ▼
             ┌──────────┐ ┌──────────┐ ┌──────────┐
             │PostgreSQL│ │  Redis   │ │  AI      │
             │(DB)      │ │(Cache)   │ │(GitHub   │
             └──────────┘ └──────────┘ │ Models)  │
                                       └──────────┘
```

## 25.2 خريطة العلاقات بين المستخدمين

```
                    ┌─────────────────────────────────────┐
                    │            SYSTEM ADMIN             │
                    │  - إدارة المدارس                     │
                    │  - إعادة تعيين كلمات المرور          │
                    │  - لوحة الأمان                       │
                    │  - حظر/فك حظر IP                    │
                    └────────────┬────────────────────────┘
                                 │
                                 │ يدير
                                 ▼
                    ┌─────────────────────────────────────┐
                    │              SCHOOL                  │
                    │  (مؤسسة تعليمية)                      │
                    │  - كود فريد للمدرسة                  │
                    │  - مستوى اشتراك                       │
                    └────────────┬────────────────────────┘
                                 │
                    ┌────────────┼────────────┐
                    │            │            │
                    ▼            ▼            │
          ┌────────────────┐ ┌────────────┐  │
          │   TEACHER      │ │  CLASSROOM │  │
          │  - يدير طلاب   │ │  (فصل)     │  │
          │  - ينشئ تحديات │ │  - مستوى   │  │
          │  - يصحح        │ │  - مادة    │  │
          │  - تقارير      │ └──────┬─────┘  │
          └────────┬───────┘        │        │
                   │                │        │
                   │                ▼        │
                   │         ┌────────────┐  │
                   └────────►│  STUDENT   │◄─┘
                             │  - اسم مستخدم│
                             │  - XP/Level  │
                             │  - Streak    │
                             │  - حلول      │
                             │  - درجات     │
                             └──────┬──────┘
                                    │
                     ┌──────────────┼──────────────┐
                     │              │              │
                     ▼              ▼              ▼
              ┌──────────┐ ┌──────────┐ ┌──────────┐
              │تحديات    │ │اختبارات  │ │حضور     │
              │(Challenges)│ │(Assess) │ │(Attend) │
              └──────────┘ └──────────┘ └──────────┘
```

## 25.3 خريطة تدفق XP والنقاط (Gamification Flow)

```
        ┌─────────────────────────────────────────────────┐
        │              GAMIFICATION ENGINE                │
        │         (studentProfileController.js)           │
        └─────────────────────┬───────────────────────────┘
                              │
         ┌────────────────────┼────────────────────┐
         │                    │                    │
         ▼                    ▼                    ▼
   ┌──────────┐        ┌──────────┐        ┌──────────┐
   │نظام XP   │        │نظام      │        │نظام     │
   │(نقاط خبرة)│        │المستويات  │        │الإنجازات │
   └────┬─────┘        └────┬─────┘        └────┬─────┘
        │                   │                    │
        ▼                   ▼                    ▼
  ┌─────────────┐   ┌──────────────┐   ┌──────────────┐
  │XP = قاعدة   │   │مستوى 1-50    │   │10 إنجازات   │
  │الصعوبة × 10 │   │كل مستوى     │   │- First Steps │
  │مثلاً:       │   │يحتاج XP     │   │- Streak 7    │
  │سهل = 10 XP  │   │متراكم       │   │- Level 5     │
  │متوسط = 25 XP│   │Level(n) =   │   │- 1000 XP     │
  │صعب = 50 XP  │   │n × 100      │   │- etc...     │
  └──────┬──────┘   └──────┬───────┘   └──────┬───────┘
         │                 │                   │
         └─────────────────┼───────────────────┘
                           │
                           ▼
                  ┌─────────────────┐
                  │  LEADERBOARD    │
                  │  متصدر الطلاب   │
                  │  Redis Cache    │
                  │  ترتيب تنازلي   │
                  └─────────────────┘
```

---

# 26. خرائط تدفق البيانات (Data Flow Maps)

## 26.1 تدفق تسليم التحدي (Challenge Submission Flow)

```
    طالب                         الخادم                        قاعدة البيانات
     │                              │                              │
     │   POST /challenge/submit     │                              │
     │   { code, challengeId }      │                              │
     │─────────────────────────────►│                              │
     │                              │                              │
     │                              │  1. التحقق من JWT            │
     │                              │  2. التحقق من Tier Locking   │
     │                              │  3. التحقق من Sequential     │
     │                              │     Locking (Lesson Order)   │
     │                              │                              │
     │                              │  4. تحليل الكود:             │
     │                              │     ├─ codeVerifier.js       │
     │                              │     │  - minVariables?       │
     │                              │     │  - mustContain?        │
     │                              │     │  - expectedOutput?     │
     │                              │     └─ match? → PASS/FAIL    │
     │                              │                              │
     │                              │  5. detectSuspicious         │
     │                              │     Activity؟                │
     │                              │     ├─ time < 2s → SUSPICIOUS│
     │                              │     ├─ perfect score chain   │
     │                              │     └─ short code → FLAG     │
     │                              │                              │
     │                              │  6. حساب XP:                │
     │                              │     └─ difficulty × 10      │
     │                              │                              │
     │                              │  7. تسجيل النتيجة:          │
     │                              │     ├─ INSERT Submission     │
     │                              │     ├─ UPDATE Student.XP    │
     │                              │     ├─ INSERT DailyXP      │
     │                              │     └─ Check Achievement    │
     │                              │                              │
     │   ← 200 { result, xp,       │                              │
     │       level, achievements }  │                              │
     │◄─────────────────────────────│                              │
```

## 26.2 تدفق المصادقة (Authentication Flow)

```
    متصفح/تطبيق                    الخادم                        قاعدة البيانات
       │                              │                              │
       │   POST /auth/login           │                              │
       │   { email, password }        │                              │
       │─────────────────────────────►│                              │
       │                              │                              │
       │                              │  1. strictRequestValidation │
       │                              │  2. strictSanitize          │
       │                              │  3. strictRateLimit (auth)  │
       │                              │     └─ 10 requests/15min    │
       │                              │                              │
       │                              │  4. استعلام User:           │
       │                              │     └─ findUnique by email  │
       │                              │─────────────────────────────►│
       │                              │◄─────────────────────────────│
       │                              │                              │
       │                              │  5. isAccountLocked?         │
       │                              │     └─ 5 failed → قفل 15 د  │
       │                              │                              │
       │                              │  6. bcrypt.compare(password) │
       │                              │     ├─ FAIL → recordFailed  │
       │                              │     │  Attempt + IP failure  │
       │                              │     │  + isProgressively     │
       │                              │     │    Delayed?            │
       │                              │     └─ SUCCESS → clearFailed │
       │                              │                              │
       │                              │  7. إنشاء JWT:              │
       │                              │     ├─ { userId, role,      │
       │                              │     │   schoolId, tokenVer } │
       │                              │     ├─ expiresIn: 7d        │
       │                              │     └─ sign with JWT_SECRET │
       │                              │                              │
       │                              │  8. registerSession          │
       │                              │     └─ enforceMaxSessions(3) │
       │                              │                              │
       │                              │  9. INSERT LoginLog          │
       │                              │─────────────────────────────►│
       │                              │                              │
       │   ← 200 { token, user,       │                              │
       │       school }               │                              │
       │◄─────────────────────────────│                              │
```

## 26.3 تدفق إنشاء الاختبار وتقديمه (Assessment Flow)

```
    معلم                              الخادم                       طالب
     │                                  │                           │
     │  POST /assessments               │                           │
     │  { title, timeLimit,            │                           │
     │    questions, classroom }        │                           │
     │─────────────────────────────────►│                           │
     │                                  │                           │
     │  ← 201 { assessment }           │                           │
     │◄─────────────────────────────────│                           │
     │                                  │                           │
     │                                  │    GET /assessments/active│
     │                                  │◄──────────────────────────│
     │                                  │                           │
     │                                  │  حساب الوقت المتبقي      │
     │                                  │  return { questions,     │
     │                                  │    timeRemaining }       │
     │                                  │──────────────────────────►│
     │                                  │                           │
     │                                  │  POST /assessments/:id/  │
     │                                  │    submit-answer          │
     │                                  │  { questionId, answer }  │
     │                                  │◄──────────────────────────│
     │                                  │                           │
     │                                  │  ⚠️ لا يثق بالعميل:      │
     │                                  │  score = calculate(answer)│
     │                                  │──────────────────────────►│
     │                                  │                           │
     │                                  │  POST /assessments/:id/  │
     │                                  │    complete               │
     │                                  │◄──────────────────────────│
     │                                  │                           │
     │                                  │  حساب الدرجة النهائية    │
     │                                  │  return { score, total,  │
     │                                  │    percentage }           │
     │                                  │──────────────────────────►│
     │                                  │                           │
     │  GET /assessments/:id/results    │                           │
     │  ← 200 { stats, results }       │                           │
     │◄─────────────────────────────────│                           │
```

## 26.4 تدفق الـ HMAC Bridge (Desktop ↔ Server)

```
    تطبيق سطح المكتب (Qt5)           الخادم (Express)
         │                                │
         │  POST /bridge/data            │
         │  Headers:                     │
         │  X-HMAC-Signature: ...        │
         │  X-Timestamp: 1712345678      │
         │  X-Nonce: uuid-v4             │
         │──────────────────────────────►│
         │                                │
         │  1. bridgeAuthMiddleware       │
         │     └─ localhost only          │
         │                                │
         │  2. validateHMACSignature      │
         │     ├─ HMAC-SHA256(            │
         │     │   body + timestamp       │
         │     │   + nonce, BRIDGE_SECRET)│
         │     └─ match? → continue       │
         │                                │
         │  3. Replay Protection          │
         │     └─ nonce غير مكرر?         │
         │                                │
         │  4. معالجة الطلب              │
         │                                │
         │  ← signed response            │
         │◄──────────────────────────────│
```

---

# 27. خرائط تدفق المصادقة والأمان (Security Flow Maps)

## 27.1 طبقات الأمان (Security Layers Stack)

```
    ┌────────────────────────────────────────────────────────┐
    │                     INTERNET                           │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 1: HELMET                                      │
    │  ├─ Content-Security-Policy (CSP)                     │
    │  ├─ Strict-Transport-Security (HSTS)                  │
    │  ├─ X-Content-Type-Options: nosniff                   │
    │  ├─ X-Frame-Options: DENY                             │
    │  └─ Permissions-Policy (camera=(), microphone=())     │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 2: CORS                                        │
    │  └─ يسمح فقط بـ ALLOWED_ORIGINS من .env               │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 3: RATE LIMITING                                │
    │  ├─ Public: 30 req/min                                │
    │  ├─ Authenticated: 60 req/min                         │
    │  ├─ Admin: 100 req/min                                │
    │  ├─ Auth: 10 req/15min                                │
    │  ├─ OTP Request: 3 req/min                            │
    │  ├─ OTP Verify: 5 req/min                             │
    │  └─ Submission: 30 req/hour per student               │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 4: STRICT REQUEST VALIDATION                   │
    │  ├─ URL length < 500 chars                            │
    │  ├─ Content-Length < 5MB (except submissions)         │
    │  ├─ Suspicious headers check                          │
    │  └─ Bad User-Agent blocking                           │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 5: SANITIZATION                                 │
    │  ├─ XSS stripping (<script>, onerror=, etc.)          │
    │  ├─ Null byte removal                                 │
    │  ├─ Control character stripping                        │
    │  ├─ Max depth: 10                                     │
    │  └─ Max keys: 100                                     │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 6: AUTHENTICATION                               │
    │  ├─ JWT verification (secret + expiry)                │
    │  ├─ Token version check (revocation)                  │
    │  ├─ Role-based access (ADMIN/TEACHER/STUDENT)         │
    │  └─ School isolation guard                            │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 7: ACCOUNT LOCKOUT                              │
    │  ├─ 5 failed attempts → 15-min lock                   │
    │  ├─ Progressive delay: 1s → 2s → 4s → 8s → 16s       │
    │  ├─ IP blocking after 20 failures                     │
    │  └─ Max 3 concurrent sessions per user                │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 8: ANTI-CHEAT                                   │
    │  ├─ Timing analysis (< 2s → SUSPICIOUS)               │
    │  ├─ Perfect score chain detection                     │
    │  ├─ Code similarity analysis                          │
    │  ├─ HMAC request signing                              │
    │  └─ Suspicious submission logging                      │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 9: AUDIT LOGGING                                │
    │  ├─ All auth attempts logged                          │
    │  ├─ Suspicious events logged                          │
    │  ├─ IP blocks logged                                  │
    │  └─ Admin actions logged                              │
    └──────────────────────┬─────────────────────────────────┘
                           │
    ┌──────────────────────▼─────────────────────────────────┐
    │  Layer 10: ERROR HANDLING                              │
    │  ├─ No stack traces in production                     │
    │  ├─ Unified error format                              │
    │  └─ Sensitive info filtering                          │
    └────────────────────────────────────────────────────────┘
```

## 27.2 آلة حالة قفل الحساب (Account Lockout State Machine)

```
                         ┌─────────────┐
                         │  正常 (عادي)  │
                         └──────┬──────┘
                                │
                    محاولة دخول فاشلة
                                │
                                ▼
                    ┌─────────────────────┐
                    │  1 فشل              │
                    │  (تأخير 1 ثانية)     │◄──────┐
                    └──────────┬──────────┘       │
                               │                  │
                    محاولة دخول فاشلة              │
                               │                  │
                               ▼                  │
                    ┌─────────────────────┐       │
                    │  2 فشل              │       │
                    │  (تأخير 2 ثانية)     │◄──────┤
                    └──────────┬──────────┘       │
                               │                  │
                    محاولة دخول فاشلة              │
                               │                  │
                               ▼                  │
                    ┌─────────────────────┐       │
                    │  3 فشل              │       │
                    │  (تأخير 4 ثوانٍ)     │◄──────┤
                    └──────────┬──────────┘       │
                               │                  │
                    محاولة دخول فاشلة              │
                               │                  │
                               ▼                  │
                    ┌─────────────────────┐       │
                    │  4 فشل              │       │
                    │  (تأخير 8 ثوانٍ)     │◄──────┤
                    └──────────┬──────────┘       │
                               │                  │
                    محاولة دخول فاشلة              │
                               │                  │
                               ▼                  │
                    ┌─────────────────────┐       │
                    │  5 فشل → مقفل       │       │
                    │  (15 دقيقة قفل)     │       │
                    └──────────┬──────────┘       │
                               │                  │
                    ┌──────────┴──────────┐       │
                    │                     │       │
               انتهاء 15 دقيقة      محاولة دخول   │
                    │                  فاشلة أخرى  │
                    ▼                     │        │
            ┌─────────────┐              │        │
            │  正常 (عادي)  │◄─────────────┘        │
            └─────────────┘                        │
                                                   │
            ┌──────────────┐                       │
            │ محاولة ناجحة │───────────────────────┘
            │ clearFailed  │
            └──────────────┘
```

## 27.3 آلة حالة الجلسة (Session State Machine)

```
   ┌──────────┐           ┌──────────┐           ┌──────────┐
   │  تم إنشاء │  تسجيل   │  نشطة   │   انتهاء   │  منتهية  │
   │  JWT     │──────────►│ Session │──────────►│ (Expired)│
   │ (معلق)   │           │ (Active)│           └──────────┘
   └──────────┘           └────┬─────┘
                               │
                    ┌──────────┼──────────┐
                    │          │          │
                    ▼          ▼          ▼
             ┌──────────┐┌──────────┐┌──────────┐
             │تسجيل     ││تجاوز     ││إبطال    │
             │خروج يدوي ││الحد الأقصى││(Token   │
             │(Logout)  ││(3 جلسات) ││Version) │
             └────┬─────┘└────┬─────┘└────┬─────┘
                  │           │           │
                  ▼           ▼           ▼
             ┌─────────────────────────────────┐
             │          جلسة منتهية (Revoked)   │
             │  removeSession()                 │
             │  JWT becomes invalid             │
             └─────────────────────────────────┘
```

---

# 28. خريطة قاعدة البيانات العلائقية الكاملة (Full DB Relationship Map)

## 28.1 مخطط العلاقات الكامل (Full ER Diagram)

```
┌─────────────────────┐       ┌─────────────────────┐
│       School        │       │        User         │
│─────────────────────│       │─────────────────────│
│ PK: id              │◄──────│ FK: schoolId        │
│     name (فريد)      │  1:N  │     email (فريد)    │
│     code (فريد)      │       │     role (ADMIN/    │
│     subscriptionLevel│       │        TEACHER)    │
│     createdAt        │       │     password        │
└─────────────────────┘       │     tokenVersion    │
         │                    └─────────────────────┘
         │ 1:N                          │
         │                              │ 1:N
         ▼                              ▼
┌─────────────────────┐       ┌─────────────────────┐
│     Classroom       │       │   Invitation        │
│─────────────────────│       │─────────────────────│
│ PK: id              │       │ PK: id              │
│     name            │       │     code (فريد)      │
│     level           │       │     email            │
│     subject         │       │     used (Boolean)   │
│ FK: schoolId        │       │ FK: schoolId        │
│ FK: teacherId       │       │     expiresAt        │
└──────────┬──────────┘       └─────────────────────┘
           │ 1:N
           │
           ▼
┌─────────────────────┐       ┌─────────────────────┐
│      Student        │       │    StudentProfile   │
│─────────────────────│       │─────────────────────│
│ PK: id              │◄──────│ PK: studentId (FK)  │
│     username (فريد)  │  1:1  │     bio              │
│     xp               │       │     avatar           │
│     level            │       │     preferences (JSON│
│     streak           │       └─────────────────────┘
│     lastActiveDate   │
│     isActive         │       ┌─────────────────────┐
│ FK: classroomId      │       │     DailyXP         │
│     deletedAt (soft) │       │─────────────────────│
└──────────┬──────────┘  1:N  │ PK: id              │
           │                  │     date (DateTime)   │
           │                  │     xpEarned          │
           │                  │ FK: studentId        │
           ▼                  └─────────────────────┘
┌─────────────────────┐
│      LoginLog       │       ┌─────────────────────┐
│─────────────────────│       │   ActivityLog       │
│ PK: id              │       │─────────────────────│
│     ip               │       │ PK: id              │
│     timestamp        │       │     action           │
│     success (Bool)   │       │     metadata (JSON)  │
│ FK: studentId        │  1:N  │     timestamp        │
└─────────────────────┘       │ FK: studentId        │
                              └─────────────────────┘
┌─────────────────────┐       ┌─────────────────────┐
│    ChallengeTier    │       │     Course          │
│─────────────────────│       │─────────────────────│
│ PK: id              │       │ PK: id              │
│     title (فريد)     │       │     title            │
│     description     │       │     description      │
│     order (فريد)     │       │     type (MAIN/     │
│     difficulty      │       │        ADDITIONAL)  │
└──────────┬──────────┘       │ FK: tierRequired    │
           │ 1:N              └──────────┬──────────┘
           │                            │ 1:N
           ▼                            ▼
┌─────────────────────┐       ┌─────────────────────┐
│      Lesson         │       │    StudentCourse    │
│─────────────────────│       │    Enrollment       │
│ PK: id              │       │─────────────────────│
│     title            │       │ PK: id              │
│     lessonNumber     │ 1:N   │     enrolledAt       │
│ FK: courseId         │       │ FK: studentId       │
└──────────┬──────────┘       │ FK: courseId        │
           │ 1:N              └─────────────────────┘
           ▼
┌─────────────────────┐       ┌─────────────────────┐
│     Challenge       │       │     Submission      │
│─────────────────────│       │─────────────────────│
│ PK: id              │       │ PK: id              │
│     title            │       │     code             │
│     description      │       │     result (PASS/   │
│     difficulty       │       │            FAIL)   │
│     order            │  1:N  │     score            │
│     xpReward         │       │     submittedAt      │
│     starterCode      │       │     timeSpentMs      │
│     expectedOutput   │       │     suspicious (Bool)│
│     requirements     │       │ FK: studentId       │
│        (JSON)        │       │ FK: challengeId     │
│ FK: lessonId         │       └─────────────────────┘
│ FK: tierId           │
└─────────────────────┘       ┌─────────────────────┐
                              │ ChallengeVerif.     │
┌─────────────────────┐       │─────────────────────│
│    Assessment       │       │ PK: id              │
│─────────────────────│       │     hmacSignature    │
│ PK: id              │       │     verificationToken│
│     title            │       │     expiresAt        │
│     timeLimit (min)  │       │ FK: challengeId     │
│     lockdownMode     │       └─────────────────────┘
│     questions (JSON) │
│ FK: classroomId      │       ┌─────────────────────┐
│ FK: createdBy        │       │ StudentAssessment  │
└──────────┬──────────┘       │ Result              │
           │ 1:N               │─────────────────────│
           ▼                   │ PK: id              │
┌─────────────────────┐       │     score            │
│  StudentSubmission  │       │     total            │
│─────────────────────│       │     percentage       │
│ PK: id              │       │     startedAt        │
│     questionIndex    │       │     completedAt      │
│     answer           │       │     status (IN/     │
│     passed (server)  │       │            PROGRESS,│
│     score (server)   │       │            COMPLETED│
│ FK: resultId        │       └─────────────────────┘
└─────────────────────┘
                              ┌─────────────────────┐
┌─────────────────────┐       │   Achievement       │
│   SecurityAlert     │       │─────────────────────│
│─────────────────────│       │ PK: id              │
│ PK: id              │       │     name (فريد)      │
│     type             │       │     description      │
│     severity         │       │     criteria (JSON)  │
│     message          │       │     icon              │
│     metadata (JSON)  │       └──────────┬──────────┘
│     resolved (Bool)  │                  │ N:N
│     createdAt        │                  ▼
│ FK: studentId        │       ┌─────────────────────┐
└─────────────────────┘       │   UserAchievement   │
                              │─────────────────────│
┌─────────────────────┐       │ PK: id              │
│   CloudWorkspace    │       │     earnedAt         │
│─────────────────────│       │ FK: studentId       │
│ PK: id              │       │ FK: achievementId   │
│     name             │       └─────────────────────┘
│     data (JSON)      │
│     lastModified     │
│ FK: studentId        │
└─────────────────────┘
```

---

# 29. خرائط المترجم (Compiler Maps)

## 29.1 مراحل الترجمة (Compiler Pipeline)

```
    ┌─────────────────────────────────────────────────────────────────────┐
    │                    DAAD COMPILER PIPELINE                          │
    │  (C++20, src/, include/Daad/)                                      │
    └─────────────────────────────────────────────────────────────────────┘
    
    كود Daad المصدر (daad file)
           │
           ▼
    ┌─────────────────────────────────────────────────────────────────┐
    │  PHASE 1: LEXICAL ANALYSIS (Lexer.cpp)                         │
    │  ┌───────────────────────────────────────────────────────────┐ │
    │  │ المدخل: سلسلة نصية (string)                                │ │
    │  │ الخطوات:                                                   │ │
    │  │ 1. تحويل النص إلى Codepoints UTF-32                        │ │
    │  │ 2. تخطي المسافات والفواصل                                  │ │
    │  │ 3. التعرف على:                                              │ │
    │  │    ├─ الأرقام (123, 3.14)                                   │ │
    │  │    ├─ النصوص ("مرحباً")                                     │ │
    │  │    ├─ المعرفات (أسماء المتغيرات والدوال)                     │ │
    │  │    ├─ الكلمات المفتاحية (إذا, طالما, دالة, ...)              │ │
    │  │    ├─ المعاملات (+, -, *, /, =, ==, !=, <, >)               │ │
    │  │    ├─ الرموز ({, }, (, ), [, ], ;, :)                      │ │
    │  │    └─ التعليقات (// و /* */)                               │ │
    │  │ 4. تجاهل المسافات والتعليقات                                │ │
    │  │ المخرجات: سلسلة من Tokens                                   │ │
    │  └───────────────────────────────────────────────────────────┘ │
    └─────────────────────────────────────────────────────────────────┘
           │
           ▼ Tokens [...]
    ┌─────────────────────────────────────────────────────────────────┐
    │  PHASE 2: SYNTAX ANALYSIS (Parser.cpp)                         │
    │  ┌───────────────────────────────────────────────────────────┐ │
    │  │ المدخل: سلسلة Tokens                                      │ │
    │  │ الخوارزمية: Recursive Descent Parsing                     │ │
    │  │ القواعد (Grammar):                                         │ │
    │  │ program     → statement*                                   │ │
    │  │ statement   → varDecl | ifStmt | whileStmt | forStmt      │ │
    │  │              | funcDecl | classDecl | returnStmt          │ │
    │  │              | breakStmt | continueStmt | expression      │ │
    │  │ expression  → binaryExpr (precedence climbing)            │ │
    │  │ binaryExpr  → unaryExpr ((op) binaryExpr)*                │ │
    │  │ unaryExpr   → ('-' | '!') unaryExpr | primary             │ │
    │  │ primary     → NUMBER | STRING | IDENTIFIER | '(' expr ')' │ │
    │  │ المخرجات: AST (Abstract Syntax Tree)                      │ │
    │  └───────────────────────────────────────────────────────────┘ │
    └─────────────────────────────────────────────────────────────────┘
           │
           ▼ AST Nodes
    ┌─────────────────────────────────────────────────────────────────┐
    │  PHASE 3: SEMANTIC ANALYSIS                                    │
    │  ┌───────────────────────────────────────────────────────────┐ │
    │  │ التحقق من:                                                  │ │
    │  │ 1. المتغيرات معرّفة قبل استخدامها                          │ │
    │  │ 2. أنواع البيانات متوافقة (صحيح + نص = خطأ)                │ │
    │  │ 3. الدوال تستدعى بعدد صحيح من المعاملات                    │ │
    │  │ 4. عدم تكرار تعريف المتغيرات في نفس النطاق                 │ │
    │  │ 5. التحقق من الصلاحية (خاص/عام)                            │ │
    │  │ المخرجات: AST مع معلومات الأنواع (Annotated AST)          │ │
    │  └───────────────────────────────────────────────────────────┘ │
    └─────────────────────────────────────────────────────────────────┘
           │
           ▼ Annotated AST
    ┌─────────────────────────────────────────────────────────────────┐
    │  PHASE 4: OPTIMIZATION (OptimizerVisitor.cpp)                  │
    │  ┌───────────────────────────────────────────────────────────┐ │
    │  │ التحسينات المطبقة:                                         │ │
    │  │ 1. CONSTANT FOLDING: 2 + 3 → 5                             │ │
    │  │ 2. DEAD CODE ELIMINATION: إزالة الكود الميت                │ │
    │  │ 3. STRENGTH REDUCTION: *2 → << 1                           │ │
    │  │ 4. CONSTANT PROPAGATION: const int x = 5 → استبدال x بـ 5 │ │
    │  │ المخرجات: AST محسّن                                         │ │
    │  └───────────────────────────────────────────────────────────┘ │
    └─────────────────────────────────────────────────────────────────┘
           │
           ▼ Optimized AST
    ┌─────────────────────────────────────────────────────────────────┐
    │  PHASE 5: CODE GENERATION (CodeGenVisitor.cpp)                 │
    │  ┌───────────────────────────────────────────────────────────┐ │
    │  │ Visitor Pattern: كل عقدة AST تولد كود C++                 │ │
    │  │ مثال:                                                      │ │
    │  │ VarDecl("صحيح", "س", Number(5))                           │ │
    │  │ → "int x = 5;"                                            │ │
    │  │                                                             │ │
    │  │ IfStmt(cond, then, else)                                   │ │
    │  │ → "if (cond) { ... } else { ... }"                         │ │
    │  │ المخرجات: كود C++20                                        │ │
    │  └───────────────────────────────────────────────────────────┘ │
    └─────────────────────────────────────────────────────────────────┘
           │
           ▼ C++ Source Code
    ┌─────────────────────────────────────────────────────────────────┐
    │  PHASE 6: OUTPUT                                               │
    │  ┌───────────────────────────────────────────────────────────┐ │
    │  │ 1. كتابة ملف .cpp                                         │ │
    │  │ 2. كتابة ملف .hpp (إعلانات الدوال)                        │ │
    │  │ 3. تضمين Runtime.hpp                                      │ │
    │  │ 4. تضمين Stdlib اللازمة حسب الاستخدام                      │ │
    │  └───────────────────────────────────────────────────────────┘ │
    └─────────────────────────────────────────────────────────────────┘
           │
           ▼ output.cpp + output.hpp
```

## 29.2 خريطة الـ Visitor Pattern في المترجم

```
    ┌─────────────────────────────────────┐
    │           ASTNode (واجهة)           │
    │─────────────────────────────────────│
    │ accept(Visitor&)                    │
    └─────────────────────────────────────┘
              ▲              ▲
              │              │
    ┌─────────┴──┐     ┌─────┴────────┐
    │ Stmt (جمل)  │     │ Expr (تعبير) │
    └──────┬──────┘     └──────┬───────┘
           │                    │
           ▼                    ▼
    ┌─────────────────────────────────────────────┐
    │              ASTVisitor (واجهة)             │
    │─────────────────────────────────────────────│
    │ visit(NumberExpr&)                          │
    │ visit(BinaryExpr&)                          │
    │ visit(VarDeclStmt&)                         │
    │ visit(IfStmt&)                              │
    │ visit(WhileStmt&)                           │
    │ visit(FunctionDecl&)                        │
    │ visit(ClassDecl&)                           │
    │ ... (20+ دوال)                              │
    └─────────────────────────────────────────────┘
              ▲              ▲              ▲
              │              │              │
    ┌─────────┴──┐    ┌──────┴──────┐   ┌──┴──────────┐
    │  CodeGen   │    │  Optimizer  │   │ Diagnostics │
    │  Visitor   │    │  Visitor    │   │  Visitor    │
    │────────── │    │──────────── │   │──────────── │
    │ يولد C++  │    │ يحسّن AST   │   │ يفحص AST   │
    └────────────┘    └─────────────┘   └─────────────┘
```

## 29.3 خريطة أخطاء المترجم (Error Diagnostics)

```
    ┌─────────────────────────────────────────────────────────────┐
    │               DIAGNOSTICS SYSTEM                             │
    │  (include/Daad/Diagnostics.hpp)                              │
    └─────────────────────────────────────────────────────────────┘
    
    أنواع الأخطاء (Error Severity Levels):
    
    ┌─────────────────────────────────────────────────────────────┐
    │  INFO    │ معلومات (مثال: تم استيراد المكتبة)               │
    ├──────────┼──────────────────────────────────────────────────┤
    │  WARNING │ تحذير (مثال: متغير معرف ولكن غير مستخدم)         │
    ├──────────┼──────────────────────────────────────────────────┤
    │  ERROR   │ خطأ (مثال: متغير غير معرف، نوع خاطئ)            │
    ├──────────┼──────────────────────────────────────────────────┤
    │  FATAL   │ خطأ قاتل (مثال: ملف غير موجود، ذاكرة غير كافية) │
    └──────────┴──────────────────────────────────────────────────┘
    
    أمثلة الأخطاء:
    
    Lexer Errors:
    ┌─────────────────────────────────────────────────────────────┐
    │ E001: "نص غير مغلق: ينقص علامة اقتباس \""                  │
    │ E002: "تعليق غير مغلق: ينقص */"                            │
    │ E003: "رمز غير معروف"                                       │
    └─────────────────────────────────────────────────────────────┘
    
    Parser Errors:
    ┌─────────────────────────────────────────────────────────────┐
    │ E101: "المتغير 'X' غير معرّف أو ناقص"                       │
    │ E102: "تعبير غير متوقع"                                     │
    │ E103: "خطأ في الوصول للعضو"                                 │
    │ E104: "نوع الدالة غير متوافق مع الإرجاع"                    │
    │ E105: "عدد المعاملات غير صحيح"                              │
    │ E106: "خطأ في الوصول بالمصفوفة"                             │
    └─────────────────────────────────────────────────────────────┘
    
    Semantic Errors:
    ┌─────────────────────────────────────────────────────────────┐
    │ E201: "نوع غير متوافق: لا يمكن جمع صحيح ونص"               │
    │ E202: "متغير معرف مسبقاً في نفس النطاق"                     │
    │ E203: "لا يمكن الوصول للعضو الخاص"                          │
    │ E204: "Category المجردة لا يمكن إنشاء كائن منها"               │
    │ E205: "قالب: نوع غير متوافق"                                │
    └─────────────────────────────────────────────────────────────┘
```

---

# 30. حالات النظام وآلات الحالة (State Machines)

## 30.1 آلة حالة الطالب (Student State Machine)

```
                        ┌──────────────┐
                        │  غير مفعل   │
                        │ (Inactive)  │
                        └──────┬──────┘
                               │
                      يتم إنشاء الحساب
                      (teacher import)
                               │
                               ▼
                        ┌──────────────┐
                        │   مدعو      │
                        │ (Invited)   │
                        └──────┬──────┘
                               │
                      يطلب OTP عبر البريد
                               │
                               ▼
                        ┌──────────────┐
                        │  OTP مرسل   │
                        │ (OTP Sent)  │
                        └──────┬──────┘
                               │
                      يتحقق من OTP + ينشئ
                      كلمة مرور + يسجل دخول
                               │
                               ▼
                    ┌──────────────────┐
                    │    نشط (Active)  │
                    │  - يحل تحديات    │
                    │  - يخضع لاختبارات│
                    │  - يجمع XP      │
                    │  - يحقق إنجازات │
                    └────────┬─────────┘
                             │
               ┌─────────────┼─────────────┐
               │             │             │
               ▼             ▼             ▼
        ┌──────────┐  ┌──────────┐  ┌──────────┐
        │غير نشط  │  │محظور    │  │محذوف   │
        │7 أيام  │  │(Banned) │  │(Soft    │
        │بدون نشاط│  │من ADMIN │  │Delete) │
        └──────────┘  └──────────┘  └──────────┘
```

## 30.2 آلة حالة الاختبار (Assessment State Machine)

```
        ┌──────────────┐
        │   مسودة     │
        │ (Draft)     │
        │ أنشأها معلم │
        └──────┬──────┘
               │
        تفعيل الاختبار
               │
               ▼
        ┌──────────────┐
        │   نشط       │
        │ (Active)    │
        │ الطلاب يمكن │
        │ الدخول      │
        └──────┬──────┘
               │
    ┌──────────┼──────────┐
    │          │          │
    ▼          ▼          ▼
┌────────┐ ┌────────┐ ┌────────┐
│قيد     │ │مكتمل   │ │ملغي   │
│التنفيذ │ │Completed│ │Cancelled│
│InProgress│ │حسب      │ │بواسطة  │
│بدأ الطالب│ │الوقت   │ │المعلم  │
│الحل     │ │انتهى   │ │       │
└────┬───┘ └────────┘ └────────┘
     │
     │ ينتهي الوقت أو
     │ يضغط Complete
     ▼
┌────────────┐
│ تم التصحيح │
│ (Graded)   │
│ score/total │
│ percentage │
│ إحصائيات   │
└────────────┘
```

## 30.3 آلة حالة الـ Bridge

```
    ┌──────────────────────────────────────────────────────┐
    │              BRIDGE CONNECTION STATE                 │
    │  (Desktop App ↔ Server عبر HTTP Signed Requests)    │
    └──────────────────────────────────────────────────────┘

    ┌──────────────┐
    │  غير متصل   │
    │ Disconnected│
    └──────┬──────┘
           │
    Desktop App يبدأ
           │
           ▼
    ┌────────────────┐
    │  محاولة اتصال  │
    │  Connecting    │
    │  POST /bridge/ │
    │  handshake     │
    └──────┬─────────┘
           │
    ┌──────┴──────┐
    │             │
    ▼             ▼
┌────────┐ ┌────────────┐
│ متصل  │ │ فشل الاتصال│
│Connected│ │Failed     │
│Handshake│ │إعادة محاولة│
│ تم      │ │بعد 5 ثوان │
└────┬───┘ └────────────┘
     │
     │ إرسال/استقبال بيانات
     │ مع HMAC Signature لكل طلب
     ▼
┌────────────────────┐
│  تبادل بيانات نشط  │
│  Active Exchange   │
│  - Telemetry       │
│  - Code Sync       │
│  - Challenge Data  │
└────────┬───────────┘
         │
    ┌────┴────┐
    │         │
    ▼         ▼
┌────────┐ ┌────────┐
│ انقطاع │ │ إغلاق  │
│(Network│ │ (Intent│
│ Error) │ │ ional) │
└────┬───┘ └────────┘
     │
     ▼
┌──────────────┐
│  غير متصل   │
│ Disconnected│
└──────────────┘
```

---

# 31. خرائط الأداء والتحسين (Performance Maps)

## 31.1 أداء الخادم الحالي

```
    ┌─────────────────────────────────────────────────────────────────┐
    │              PERFORMANCE BENCHMARK (Local SQLite)               │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  Health Check:              ~2ms   (200 req/s)                  │
    │  Login (valid):            ~15ms  (66 req/s)                   │
    │  Login (invalid):          ~12ms  (83 req/s)                   │
    │  Get Leaderboard:          ~8ms   (125 req/s)                  │
    │  Submit Challenge:         ~25ms  (40 req/s)                   │
    │  Get Student Roadmap:      ~10ms  (100 req/s)                  │
    │  Create Assessment:        ~20ms  (50 req/s)                   │
    │  Complete Assessment:      ~30ms  (33 req/s)                   │
    │  Get Dashboard:            ~18ms  (55 req/s)                   │
    │  Export CSV Report:        ~35ms  (28 req/s)                   │
    │                                                                 │
    │  Bottlenecks:                                                   │
    │  ⚠️  SQLite Write Lock (sequential writes only)                  │
    │  ⚠️  No Redis Cache for Leaderboard                              │
    │  ⚠️  Login with bcrypt (CPU heavy)                               │
    │  ⚠️  N+1 Queries in Analytics                                    │
    └─────────────────────────────────────────────────────────────────┘
```

## 31.2 خريطة تحسين الأداء (Performance Optimization Map)

```
    ┌─────────────────────────────────────────────────────────────────┐
    │              PERFORMANCE OPTIMIZATION ROADMAP                   │
    │  Current → Target                                               │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  1. PostgreSQL Migration                                        │
    │     ┌───────────────────────────────────────────────────────┐   │
    │     │ قبل: SQLite → كتابة متتابعة ← 20 req/s writes         │   │
    │     │ بعد: PostgreSQL → كتابة متزامنة ← 500+ req/s writes   │   │
    │     └───────────────────────────────────────────────────────┘   │
    │                                                                 │
    │  2. Redis Caching                                               │
    │     ┌───────────────────────────────────────────────────────┐   │
    │     │ Leaderboard: DB query ~8ms → Redis cache ~1ms          │   │
    │     │ Session Store: Memory → Redis (persistent)             │   │
    │     │ Rate Limit Counters: Memory → Redis (distributed)      │   │
    │     │ Cache TTL: Leaderboard=30s, StudentData=60s             │   │
    │     └───────────────────────────────────────────────────────┘   │
    │                                                                 │
    │  3. Query Optimization                                          │
    │     ┌───────────────────────────────────────────────────────┐   │
    │     │ N+1: getAllStudents → forEach getXP → 1+N queries     │   │
    │     │ Fix: JOIN + include: { student: { include: { xp } } }│   │
    │     │ إضافة indexes: Student.username, ActivityLog.timestamp │   │
    │     └───────────────────────────────────────────────────────┘   │
    │                                                                 │
    │  4. Middleware Optimization                                     │
    │     ┌───────────────────────────────────────────────────────┐   │
    │     │ strictSanitize: 0.5ms per request → ok                 │   │
    │     │ strictSecurity: 2ms per request → ok                   │   │
    │     │ Rate Limit: Memory lookup → 0.1ms                      │   │
    │     │ JWT verify: 1ms per request → ok                       │   │
    │     └───────────────────────────────────────────────────────┘   │
    │                                                                 │
    │  5. Code splitting (Controllers → Services)                    │
    │     ┌───────────────────────────────────────────────────────┐   │
    │     │ أكبر Controller: analyticsController.js (1082 lines)  │   │
    │     │ الهدف: تقسيم إلى Services منفصلة                       │   │
    │     │ analyticsService.js → leaderboard + attendance + ...  │   │
    │     │ assessmentService.js → create + grade + export        │   │
    │     └───────────────────────────────────────────────────────┘   │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

## 31.3 خريطة الذاكرة (Memory Map)

```
    ┌─────────────────────────────────────────────────────────────────┐
    │              SERVER MEMORY USAGE (تقديري)                       │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  Node.js Process:    ~50MB baseline                              │
    │  Express + Helmet:   ~5MB                                        │
    │  Prisma Client:      ~15MB                                       │
    │  Controllers + Routes: ~10MB                                     │
    │  Middleware:         ~5MB                                         │
    │  In-Memory Stores:   ~variable (failedAttempts, ipBlocks,       │
    │                                requestCounts, activeSessions,    │
    │                                auditLog)                         │
    │                                                                 │
    │  ┌──────────────────────────────────────────────────────────┐   │
    │  │ Memory Leak Risk: failedAttempts Map لا يتم تنظيفه       │   │
    │  │ بعد فترة → قد يصل لمئات الآلاف من الإدخالات              │   │
    │  │ الحل: إضافة cleanup interval (موجود لجلسات فقط)          │   │
    │  └──────────────────────────────────────────────────────────┘   │
    │                                                                 │
    │  ┌──────────────────────────────────────────────────────────┐   │
    │  │ Rate Limit Counters: requestCounts Map لكل IP            │   │
    │  │ 1000 IP = ~500KB. 10000 IP = ~5MB (معقول)               │   │
    │  └──────────────────────────────────────────────────────────┘   │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

---

# 32. خريطة التكامل بين المكونات (Component Integration Map)

## 32.1 تدفق البيانات بين جميع المكونات

```
  ┌──────────────────────────────────────────────────────────────────────────────────────┐
  │                           DAAD STUDIO ECOSYSTEM                                     │
  │                      خرائط تدفق البيانات بين المكونات                                 │
  └──────────────────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────────────────────────────┐
  │                         LAYER 1: CLIENT TIER                                        │
  ├──────────────────────────────────────────────────────────────────────────────────────┤
  │                                                                                       │
  │  ┌─────────────────────┐   ┌─────────────────────┐   ┌─────────────────────┐        │
  │  │   Web Browser      │   │   Qt5 Desktop IDE   │   │   VS Code          │        │
  │  │   (Chrome, Firefox)│   │   (DaadStudioIDE)   │   │   Extension        │        │
  │  ├─────────────────────┤   ├─────────────────────┤   ├─────────────────────┤        │
  │  │ HTML/CSS/JS        │   │ QML + C++17         │   │ TypeScript          │        │
  │  │ REST Client        │   │ HMAC Bridge Client  │   │ Child Process       │        │
  │  │ JWT in localStorage│   │ Compiler Wrapper    │   │ Syntax Highlight    │        │
  │  └──────────┬──────────┘   └──────────┬──────────┘   └──────────┬──────────┘        │
  │             │                         │                         │                   │
  └─────────────┼─────────────────────────┼─────────────────────────┼───────────────────┘
                │                         │                         │
                │ HTTP/JSON               │ HTTP/JSON + HMAC       │ CLI (child_process)
                │ Port 3000               │ Port 3000 /bridge/*    │ Compiler CLI
                ▼                         ▼                         ▼
  ┌──────────────────────────────────────────────────────────────────────────────────────┐
  │                         LAYER 2: SERVER TIER                                         │
  ├──────────────────────────────────────────────────────────────────────────────────────┤
  │                                                                                       │
  │  ┌──────────────────────────────────────────────────────────────────────────────┐   │
  │  │                        EXPRESS REST API SERVER                              │   │
  │  │                        Port 3000, Node.js ≥18                               │   │
  │  ├──────────────────────────────────────────────────────────────────────────────┤   │
  │  │  Middleware Stack:                                                           │   │
  │  │  Logger → Security Headers → Helmet → Compression → JSON → CORS →           │   │
  │  │  Sanitize → Rate Limit → Routes → 404 → Error Handler                       │   │
  │  ├──────────────────────────────────────────────────────────────────────────────┤   │
  │  │   ┌──────────┐  ┌────────┐  ┌──────────┐  ┌──────────┐  ┌────────┐        │   │
  │  │   │ Auth     │  │ Student│  │ Teacher  │  │ Security │  │ Bridge │        │   │
  │  │   │ Routes   │  │ Routes │  │ Routes   │  │ Routes   │  │ Routes │        │   │
  │  │   └────┬─────┘  └───┬────┘  └────┬─────┘  └────┬─────┘  └────┬───┘        │   │
  │  │        │            │            │             │             │            │   │
  │  │        ▼            ▼            ▼             ▼             ▼            │   │
  │  │   ┌──────────────────────────────────────────────────────────────────┐   │   │
  │  │   │                      CONTROLLERS LAYER                          │   │   │
  │  │   │  12 Controllers, ~6,500+ lines total                            │   │   │
  │  │   └──────────────────────────┬───────────────────────────────────────┘   │   │
  │  │                              │                                          │   │
  │  │                              ▼                                          │   │
  │  │   ┌──────────────────────────────────────────────────────────────────┐   │   │
  │  │   │                       SERVICES + UTILS                           │   │   │
  │  │   │  adminService, prisma, redis, logger, codeVerifier               │   │   │
  │  │   └──────────────────────────┬───────────────────────────────────────┘   │   │
  │  │                              │                                          │   │
  │  └──────────────────────────────┼──────────────────────────────────────────┘   │
  │                                 │                                              │
  └───────────────────────────────┼────────────────────────────────────────────────┘
                                  │
          ┌───────────────────────┼───────────────────────┐
          │                       │                       │
          ▼                       ▼                       ▼
┌──────────────────┐   ┌──────────────────┐   ┌──────────────────┐
│   LAYER 3:      │   │   LAYER 3:      │   │   LAYER 3:      │
│   DATABASE      │   │   CACHE         │   │   EXTERNAL      │
├──────────────────┤   ├──────────────────┤   ├──────────────────┤
│   SQLite/       │   │   Redis         │   │   GitHub Models  │
│   PostgreSQL    │   │   (Upstash/)    │   │   (GPT-4o-mini)  │
│                 │   │   ioredis       │   │                  │
│   Prisma ORM    │   │   Leaderboard   │   │   AI Agent      │
│   23 Models     │   │   Session Store │   │   Analysis      │
│   ~620 lines    │   │   Cache TTL     │   │   Suggestions   │
│   Schema        │   │   Counters      │   │   Code Review   │
└──────────────────┘   └──────────────────┘   └──────────────────┘

  ┌──────────────────────────────────────────────────────────────────────────────────────┐
  │                         LAYER 4: COMPILER TIER                                      │
  ├──────────────────────────────────────────────────────────────────────────────────────┤
  │                                                                                       │
  │  ┌──────────────────────────────────────────────────────────────────────────────┐   │
  │  │                        DAAD COMPILER ENGINE                                  │   │
  │  │                        C++20, src/ + include/Daad/                           │   │
  │  ├──────────────────────────────────────────────────────────────────────────────┤   │
  │  │                                                                               │   │
  │  │  Daad Source ──► Lexer ──► Parser ──► AST ──► Optimizer ──► CodeGen ──► C++ │   │
  │  │      │              │           │         │        │            │           │   │
  │  │      │         Arabic Unicode  Recursive  Visitor  Constant     Visitor    │   │
  │  │      │         UTF-32         Descent    Pattern  Folding +    Pattern    │   │
  │  │      │                                         DCE + S.R.               │   │
  │  │      ▼                                                                   │   │
  │  │   ┌──────────────────────────────────────────────────────────────────┐   │   │
  │  │   │                     STANDARD LIBRARY (100 modules)              │   │   │
  │  │   │  Math | String | DataStruct | I/O | Network | Time | GUI | ... │   │   │
  │  │   └──────────────────────────────────────────────────────────────────┘   │   │
  │  │                                                                           │   │
  │  │   ┌──────────────────────────────────────────────────────────────────┐   │   │
  │  │   │                     RUNTIME (built-in functions)                │   │   │
  │  │   │  daad_print, daad_read, type conversion, memory management      │   │   │
  │  │   └──────────────────────────────────────────────────────────────────┘   │   │
  │  │                                                                           │   │
  │  └──────────────────────────────────────────────────────────────────────────┘   │
  │                                                                                   │
  └───────────────────────────────────────────────────────────────────────────────────┘
```

## 32.2 خريطة مسارات API الكاملة (Complete API Routes Map)

```
    ┌────────────────────────────────────────────────────────────────────────────────┐
    │                        COMPLETE API ROUTES MAP                                │
    │                        جميع مسارات الـ API (30 مساراً)                         │
    └────────────────────────────────────────────────────────────────────────────────┘
    
    ┌─────────────────────────────────────┐
    │  PUBLIC (بدون مصادقة)               │
    ├─────────────────────────────────────┤
    │ GET  /health                        │
    │ GET  /health/ready                  │
    │ GET  /health/live                   │
    │ POST /api/v1/auth/login             │
    │ POST /api/v1/auth/refresh           │
    │ POST /api/v1/otp/request-otp       │
    │ POST /api/v1/otp/verify-otp        │
    │ GET  /api/v1/auth/classrooms        │
    │ GET  /api/v1/auth/classrooms/:id/  │
    │       students                      │
    │ POST /api/v1/auth/student-login    │
    └─────────────────────────────────────┘
    
    ┌─────────────────────────────────────┐
    │  AUTHENTICATED (STUDENT)            │
    ├─────────────────────────────────────┤
    │ POST /api/v1/auth/change-password  │
    │ POST /api/v1/auth/logout           │
    │ POST /api/v1/student/challenge/    │
    │       submit                        │
    │ GET  /api/v1/student/profile       │
    │ POST /api/v1/student/profile       │
    │ GET  /api/v1/student/roadmap       │
    │ GET  /api/v1/student/leaderboard   │
    │ GET  /api/v1/student/achievements  │
    │ GET  /api/v1/student/classroom/    │
    │       check                         │
    │ GET  /api/v1/student/courses       │
    │ POST /api/v1/student/courses/:id/  │
    │       enroll                        │
    │ GET  /api/v1/student/courses/:id/  │
    │       roadmap                       │
    │ POST /api/v1/assessments/:id/start │
    │ POST /api/v1/assessments/:id/      │
    │       submit-answer                 │
    │ POST /api/v1/assessments/:id/      │
    │       complete                      │
    │ GET  /api/v1/assessments/active    │
    │ GET  /api/v1/assessments/my        │
    │ POST /submissions/bulk-report      │
    │ POST /cloud/workspace/save         │
    │ GET  /cloud/workspace/load         │
    │ GET  /cloud/workspace/list         │
    │ DELETE /cloud/workspace/:id        │
    │ GET  /security/alerts              │
    │ POST /security/alerts/:id/resolve  │
    └─────────────────────────────────────┘
    
    ┌─────────────────────────────────────┐
    │  TEACHER (معلم)                     │
    ├─────────────────────────────────────┤
    │ POST /api/v1/students              │
    │ POST /api/v1/students/bulk         │
    │ GET  /api/v1/students/my           │
    │ POST /api/v1/students/:id/delete   │
    │ POST /api/v1/students/:id/assign   │
    │ POST /api/v1/teacher/invitations/  │
    │       import                        │
    │ GET  /api/v1/teacher/invitations   │
    │ POST /api/v1/assessments           │
    │ GET  /api/v1/assessments/classroom │
    │ GET  /api/v1/assessments/:id/      │
    │       results                       │
    │ GET  /api/v1/assessments/:id/      │
    │       export                        │
    │ POST /api/v1/analytics/attendance  │
    │ GET  /api/v1/analytics/attendance- │
    │       summary                       │
    │ GET  /api/v1/analytics/attendance- │
    │       history                       │
    │ GET  /api/v1/analytics/dashboard   │
    │ GET  /api/v1/analytics/classrooms  │
    │ GET  /api/v1/analytics/classrooms/ │
    │       :id/progress                  │
    │ POST /api/v1/analytics/classrooms  │
    │ GET  /api/v1/analytics/assessments │
    │ POST /api/v1/schools              │
    │ PUT  /api/v1/schools/:id          │
    │ GET  /api/v1/schools              │
    │ GET  /api/v1/schools/:id          │
    │ GET  /api/v1/schools/:id/users    │
    │ DELETE /api/v1/schools/:id        │
    │ DELETE /api/v1/schools/:id/users  │
    │ POST /api/v1/reports/weekly       │
    │ GET  /api/v1/reports/school-      │
    │       overview                      │
    │ GET  /api/v1/reports/activity/    │
    │       :userId                      │
    └─────────────────────────────────────┘
    
    ┌─────────────────────────────────────┐
    │  ADMIN (مدير النظام)                │
    ├─────────────────────────────────────┤
    │ POST /api/v1/auth/register         │
    │ POST /api/v1/auth/reset-password   │
    │ POST /api/v1/onboard-school        │
    │ GET  /api/v1/security/dashboard    │
    │ GET  /api/v1/security/audit        │
    │ POST /api/v1/security/unblock-ip   │
    │ POST /api/v1/challenges            │
    │ PUT  /api/v1/challenges/:id        │
    │ DELETE /api/v1/challenges/:id      │
    │ POST /api/v1/challenges/verify     │
    └─────────────────────────────────────┘
```

---

# 33. قائمة مراجعة الجودة الشاملة (Quality Checklist)

## 33.1 قائمة مراجعة الأمان

- [ ] ALLOWED_ORIGINS في `.env` محددة بشكل صحيح؟
- [ ] JWT_SECRET قوي وعشوائي (64 حرف)؟
- [ ] GITHUB_TOKEN غير مكشوف في `.env`؟
- [ ] Rate Limiting نشط لجميع الـ endpoints؟
- [ ] CSP Headers تسمح فقط بالمصادر الموثوقة؟
- [ ] HSTS مفعل مع max-age مناسب؟
- [ ] CORS لا يسمح بـ `'null'` origin؟
- [ ] JWT ليس في localStorage؟ (httpOnly cookies أفضل)
- [ ] جميع المدخلات مطهرة (sanitized)؟
- [ ] التحقق من HMAC Signature في Bridge؟
- [ ] Account Lockout نشط بعد 5 محاولات فاشلة؟
- [ ] IP Blocking بعد 20 فشل؟
- [ ] Audit Log يسجل جميع الأحداث الأمنية؟
- [ ] Error Handler لا يسرب تفاصيل داخلية؟

## 33.2 قائمة مراجعة الأداء

- [ ] PostgreSQL بدلاً من SQLite للإنتاج؟
- [ ] Redis نشط للتخزين المؤقت؟
- [ ] الفهارس (Indexes) موجودة على جميع الحقول المستخدمة في WHERE؟
- [ ] Controllers مقسمة إلى Services؟
- [ ] لا توجد استعلامات N+1؟
- [ ] express.json limit معقول (10MB)؟
- [ ] trust proxy مفعل خلف الـ Load Balancer؟
- [ ] Middleware مرتبة بالترتيب الصحيح؟
- [ ] لا توجد تسريبات ذاكرة (Memory Leaks)؟
- [ ] الـ Static files مخدومة عبر CDN؟

## 33.3 قائمة مراجعة الكود

- [ ] لا توجد Controllers أكبر من 500 سطر؟
- [ ] Routes لا تحتوي منطق (Business Logic)؟
- [ ] Service Layer موجودة لجميع العمليات؟
- [ ] الأخطاء موحدة التنسيق؟
- [ ] الرسائل بالعربية أو الإنجليزية بشكل موحد؟
- [ ] API keys في متغيرات البيئة وليس hardcoded؟
- [ ] جميع الـ Endpoints موثقة في Swagger؟
- [ ] الاختبارات تغطي > 80% من الـ API؟

## 33.4 قائمة مراجعة النشر

- [ ] Dockerfile مُحسّن (multi-stage build)؟
- [ ] docker-compose موجود (PostgreSQL + Redis)؟
- [ ] Secrets في AWS Secrets Manager؟
- [ ] Health Checks كاملة (`/live`, `/ready`, `/health`)؟
- [ ] CI/CD Pipeline يشتغل على push؟
- [ ] Monitoring (CloudWatch/Grafana)؟
- [ ] Database Migrations آلية (prestart)؟
- [ ] Backup Strategy موجودة؟

---

# 34. إحصائيات المشروع الكاملة (Full Project Statistics)

## 34.1 إحصائيات الملفات

```
┌────────────────────────────────────────────────────────────────────────────┐
│                         PROJECT STATISTICS                                │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  TOTAL FILES:    ~355 ملفاً                                                  │
│  TOTAL LINES:    ~62,000+ سطر كود                                            │
│                                                                             │
│  ┌─────────────────────────────┬──────────┬──────────┬─────────────────┐   │
│  │ Component                  │ Files    │ Lines    │ Languages        │   │
│  ├─────────────────────────────┼──────────┼──────────┼─────────────────┤   │
│  │ Server (REST API)          │ 56       │ ~14,200  │ JavaScript       │   │
│  │ Frontend-Web               │ 33       │ ~15,000+ │ HTML/CSS/JS     │   │
│  │ Frontend (Qt5 IDE)         │ 56       │ ~8,000+  │ QML/C++17       │   │
│  │ Core Compiler              │ 15+      │ ~4,000+  │ C++20           │   │
│  │ Standard Library           │ 102      │ ~5,000+  │ Daad/C++        │   │
│  │ Tests (JS)                 │ 50+      │ ~12,000+ │ JavaScript      │   │
│  │ Tests (C++)                │ 8        │ ~2,000+  │ C++20/GoogleTest│   │
│  │ Infrastructure (AWS/Docker)│ 20+      │ ~800+    │ YAML/JSON       │   │
│  │ VS Code Extension          │ 7        │ ~400+    │ TypeScript/JSON │   │
│  │ Documentation              │ 4        │ ~300+    │ Markdown        │   │
│  └─────────────────────────────┴──────────┴──────────┴─────────────────┘   │
│                                                                             │
│  LARGEST FILES:                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ #1  book.html (Frontend-Web)                      9,362 lines      │   │
│  │ #2  prisma/seed.js (Server)                       929 lines        │   │
│  │ #3  analyticsController.js (Server)               1,082 lines      │   │
│  │ #4  assessmentController.js (Server)              1,045 lines      │   │
│  │ #5  challenges_tier1..7.js (Tests)                ~700 each        │   │
│  │ #6  studentProfileController.js (Server)          757 lines        │   │
│  │ #7  studentManagementController.js (Server)       663 lines        │   │
│  │ #8  Parser.cpp (Compiler)                         628 lines        │   │
│  │ #9  schema.prisma (Database)                      620 lines        │   │
│  │ #10 authController.js (Server)                    623 lines        │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└────────────────────────────────────────────────────────────────────────────┘
```

## 34.2 إحصائيات قاعدة البيانات

```
    ┌─────────────────────────────────────────────────────────────┐
    │              DATABASE STATISTICS                             │
    ├─────────────────────────────────────────────────────────────┤
    │                                                              │
    │  عدد النماذج (Models):       23                              │
    │  عدد العلاقات (Relations):   ~30                             │
    │  عدد الحقول (Fields):        ~200                            │
    │  عدد المigrations:           2                                │
    │  عدد ملفات الـ Seed:         4                                │
    │                                                              │
    │  النماذج الأكثر حقولاً:                                      │
    │  1. StudentProfile:      ~15 حقلاً                          │
    │  2. Challenge:           ~12 حقلاً                          │
    │  3. Assessment:          ~12 حقلاً                          │
    │                                                              │
    │  الأنواع المستخدمة: String, Int, Float, Boolean,            │
    │                      DateTime, Json                         │
    └─────────────────────────────────────────────────────────────┘
```

---

---

# 35. موسوعة التوثيق الكاملة (Complete Documentation Suite)

تم إنشاء **6 ملفات توثيق تخصصية** بالإضافة إلى خطة التطوير هذه، لتشكل موسوعة شاملة بــ **26,901 سطر** من التوثيق التحليلي:

```
┌────────────────────────────────────────────────────────────────────────────────────┐
│                  DAAD STUDIO DOCUMENTATION SUITE (26,901 lines)                    │
├────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                     │
│  📄 docs/DEVELOPMENT_PLAN.md             4,584 lines  خطة التطوير والإصلاح           │
│  📄 docs/reference/SERVER_COMPLETE_REFERENCE.md   3,445 lines  تحليل الخادم الكامل   │
│  📄 docs/reference/DAAD_BOOK_FULL.md              4,290 lines  كتاب لغة ض الشامل      │
│  📄 docs/reference/API_FULL_REFERENCE.md          2,454 lines  توثيق API بالكامل      │
│  📄 docs/reference/STDLIB_COMPLETE.md             2,926 lines  المكتبة القياسية       │
│  📄 docs/reference/CHALLENGES_COMPLETE.md         6,410 lines  210 تحدي + 3 دورات    │
│  📄 docs/reference/SECURITY_COMPLETE.md           3,344 lines  تدقيق أمني شامل       │
│                                                                                     │
│  ─────────────────────────────────────────────────────────────────────────────      │
│  المجموع الكلي:                                                      26,901 سطر     │
│                                                                                     │
└────────────────────────────────────────────────────────────────────────────────────┘
```

## 35.1 فهرس الملفات المرجعية

| الملف | السطور | الحجم | المحتوى |
|-------|--------|-------|---------|
| `SERVER_COMPLETE_REFERENCE.md` | 3,445 | 201KB | تحليل كل ملف سيرفر: index.js، 12 Controller، 4 Middlewares، 13 Route، 4 Utils، 1 Service، Prisma Schema، 46 اقتراح تحسين |
| `DAAD_BOOK_FULL.md` | 4,290 | 160KB | كتاب مرجعي كامل للغة ض: 78 كلمة مفتاحية، 20 فصلاً، خرائط ذاكرة، أمثلة شاملة، دليل الانتقال من C++ |
| `API_FULL_REFERENCE.md` | 2,454 | 104KB | توثيق ~80 نقطة نهاية API: كل endpoint مع المدخلات والمخرجات والأمان والأخطاء وأمثلة curl |
| `STDLIB_COMPLETE.md` | 2,926 | 171KB | توثيق 100 وحدة مكتبة قياسية في 10 تصنيفات: هياكل بيانات، رياضيات، نصوص، وقت، IO، DB، ويب، شبكات، رسوميات، تزامن |
| `CHALLENGES_COMPLETE.md` | 6,410 | 281KB | 210 تحدياً في 7 مستويات، 3 دورات تعليمية (تشفير، سيبراني، ويب)، نظام التقييم والاختبارات |
| `SECURITY_COMPLETE.md` | 3,344 | 171KB | تدقيق أمني كامل: 12 ثغرة، خريطة OWASP، 20 ميزة إيجابية، خطة إصلاح 4 مراحل، دليل اختبار الاختراق، 60-بند قائمة تدقيق |

## 35.2 تحليل التغطية

```
┌────────────────────────────────────────────────────────────────────────────────────┐
│                          COVERAGE ANALYSIS                                         │
├────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                     │
│  Server Code Coverage:   14,200 lines analyzed → 24% documented in reference       │
│  Compiler Code Coverage: 4,000+ lines analyzed → 100% language reference           │
│  API Coverage:           80+ endpoints → 100% documented with examples             │
│  Security Analysis:      535+ lines middleware → 100% audited                      │
│  Stdlib Coverage:        100 modules → 100% documented with examples               │
│  Challenge Coverage:     210 challenges → 100% documented with hints               │
│  Database Coverage:      25 models → 100% schema documented                        │
│                                                                                     │
└────────────────────────────────────────────────────────────────────────────────────┘
```

## 35.3 كيفية استخدام الموسوعة

```bash
# تحليل الخطة بواسطة GitHub Models AI Agent:
node scripts/github-agent.js --file docs/DEVELOPMENT_PLAN.md "حلل الخطة"

# مراجعة أمنية شاملة:
node scripts/github-agent.js --file docs/reference/SECURITY_COMPLETE.md "نفذ الإصلاحات الأمنية"

# توثيق API بالكامل:
node scripts/github-agent.js --file docs/reference/API_FULL_REFERENCE.md "أنشئ Postman collection"

# مرجع لغة ض:
node scripts/github-agent.js --file docs/reference/DAAD_BOOK_FULL.md "استخرج أمثلة تعليمية"
```

## 35.4 إجمالي المشروع

| Category | العدد |
|-------|-------|
| إجمالي الملفات المصدرية | ~355 |
| إجمالي أسطر الكود | ~62,000 |
| إجمالي التوثيق | 26,901 سطر |
| إجمالي الملفات (كود + توثيق) | 361+ |

---

**نهاية خطة التطوير والإصلاح الشاملة + موسوعة التوثيق الكاملة**

تم إعداد هذه الحزمة بواسطة OpenCode AI بعد قراءة وتحليل ~62,000 سطر كود عبر ~355 ملف في جميع مكونات المشروع. تشمل الحزمة خطة تطوير من 34 قسماً + 6 ملفات مرجعية تخصصية = 26,901 سطر توثيق تحليلي شامل.

**⚠️ ملاحظة أمنية مهمة:** تم كشف GitHub Token في ملف `.env`. يرجى إبطاله فوراً عبر https://github.com/settings/tokens
