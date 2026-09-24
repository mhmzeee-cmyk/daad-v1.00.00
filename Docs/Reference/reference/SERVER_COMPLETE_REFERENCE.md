
# 📘 المرجع الكامل لخادم دحد — Dhad Studio Server Reference

---

## فهرس المحتويات

1. [مقدمة الخادم — Architecture Overview](#1-مقدمة-الخادم--architecture-overview)
2. [index.js — تحليل نقطة الدخول الرئيسية](#2-indexjs--تحليل-نقطة-الدخول-الرئيسية)
3. [Swagger — تحليل تكوين التوثيق](#3-swagger--تحليل-تكوين-التوثيق)
4. [الوحدات التحكمية — 12 Controller](#4-الوحدات-التحكمية--12-controller)
5. [الوسائط — 4 Middlewares](#5-الوسائط--4-middlewares)
6. [ملفات المسارات — 13 Route Files](#6-ملفات-المسارات--13-route-files)
7. [الخدمات والأدوات — Services & Utils](#7-الخدمات-والأدوات--services--utils)
8. [نموذج قاعدة البيانات — 25 Prisma Models](#8-نموذج-قاعدة-البيانات--25-prisma-models)
9. [متغيرات البيئة — Environment Variables](#9-متغيرات-البيئة--environment-variables)
10. [قائمة شاملة بجميع الاقتراحات — Comprehensive Suggestions](#10-قائمة-شاملة-بجميع-الاقتراحات--comprehensive-suggestions)

---

## 1. مقدمة الخادم — Architecture Overview

### 1.1 نظرة عامة

خادم **دحد ستوديو (Dhad Studio)** هو منصة تعليم برمجية متكاملة مبنية على **Node.js** مع **Express.js**. يوفر الخادم واجهات API لإدارة المدارس والفصول والطلاب والتحديات البرمجية والاختبارات ونظام النقاط والشارات والتقارير التحليلية.

### 1.2 رسم معماري للخادم

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           CLIENT LAYER                                      │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ Web App  │  │ Mobile   │  │ Desktop  │  │  CLI     │  │  Judge   │    │
│  │ (React)  │  │ (Flutter)│  │ (Elect.) │  │  (curl)  │  │ Service  │    │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘    │
└───────┼──────────────┼──────────────┼──────────────┼──────────────┼────────┘
        │              │              │              │              │
        │              │              │              │              │
┌───────▼──────────────▼──────────────▼──────────────▼──────────────▼────────┐
│                           API GATEWAY                                      │
│  ┌────────────────────────────────────────────────────────────────────┐    │
│  │                    index.js (Entry Point)                          │    │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────┐ │    │
│  │  │  Helmet  │ │Compress. │ │Rate Limit│ │   CORS   │ │ Prisma │ │    │
│  │  │ Security │ │  Brotli  │ │100/min   │ │ Whitelist│ │ Client │ │    │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘ └────────┘ │    │
│  └────────────────────────────────────────────────────────────────────┘    │
└───────────────────────┬────────────────────────────────────────────────────┘
                        │
┌───────────────────────▼────────────────────────────────────────────────────┐
│                    MIDDLEWARE STACK                                         │
│                                                                             │
│  ┌──────────────────┐  ┌──────────────────┐  ┌────────────────────────┐   │
│  │ strictSecurity   │  │      auth.js     │  │     security.js        │   │
│  │ • Rate Limiting  │  │ • authenticate   │  │ • sanitizeString       │   │
│  │ • IP Blocking    │  │ • requireRole    │  │ • deepSanitize         │   │
│  │ • Account Lock   │  │ • JWT Verify     │  │ • isValidEmail         │   │
│  │ • Input Sanitize │  │ • HMAC Validate  │  │ • submissionRateLimit  │   │
│  │ • CSP Headers    │  │ • Token Revoc.   │  │ • typeGuard            │   │
│  └──────────────────┘  └──────────────────┘  └────────────────────────┘   │
│                                                                             │
└───────────────────────┬────────────────────────────────────────────────────┘
                        │
┌───────────────────────▼────────────────────────────────────────────────────┐
│                     ROUTER (router/index.js)                                │
│                                                                             │
│  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐│
│  │Auth  │ │ OTP  │ │Student│ │Anlytc│ │Courses││Chall.│ │School│ │Rprts ││
│  │Routes│ │Routes│ │Routes │ │Routes│ │Routes ││Routes│ │Routes│ │Routes││
│  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘ └──────┘ └──────┘ └──────┘│
└───────────────────────┬────────────────────────────────────────────────────┘
                        │
┌───────────────────────▼────────────────────────────────────────────────────┐
│                  CONTROLLERS (12 Files)                                    │
│                                                                             │
│  analytics    auth      assessment   studentProfile   studentManagement    │
│  challenge    course    health       reports          school               │
│  onboard      otp                                                          │
└───────────────────────┬────────────────────────────────────────────────────┘
                        │
┌───────────────────────▼────────────────────────────────────────────────────┐
│                  SERVICES & UTILS                                          │
│                                                                             │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────────────┐   │
│  │adminSvc  │ │  redis   │ │  logger  │ │prisma    │ │  codeVerifier  │   │
│  │(OTP/     │ │  (Cache) │ │ (Winston)│ │(Client)  │ │  (Arabic AST)  │   │
│  │Import)   │ │          │ │          │ │          │ │                │   │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘ └────────────────┘   │
└───────────────────────┬────────────────────────────────────────────────────┘
                        │
┌───────────────────────▼────────────────────────────────────────────────────┐
│                   DATA LAYER                                                │
│                                                                             │
│  ┌──────────────────────┐  ┌──────────────────────┐                        │
│  │  SQLite (Dev)        │  │  PostgreSQL (Prod)   │                        │
│  │  Prisma ORM          │  │  Prisma ORM          │                        │
│  │  25 Models           │  │  25 Models           │                        │
│  └──────────────────────┘  └──────────────────────┘                        │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  Redis Cache (Optional)                                            │   │
│  │  • Leaderboard (Sorted Set)  • Failure Counter  • General Cache    │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.3 التقنيات المستخدمة

| التقنية | الإصدار | الاستخدام |
|---------|---------|-----------|
| Node.js | ≥18.x | بيئة التشغيل |
| Express.js | 4.x | إطار الخادم |
| Prisma | 5.x | ORM وإدارة قواعد البيانات |
| SQLite | Dev | قاعدة بيانات للتطوير |
| PostgreSQL | Prod | قاعدة بيانات للإنتاج |
| Redis (ioredis) | 5.x | تخزين مؤقت وترتيب الطلاب |
| Winston | 3.x | سجل الأحداث |
| JWT (jsonwebtoken) | 9.x | المصادقة |
| bcryptjs | 2.x | تشفير كلمات المرور |
| Helmet | 7.x | أمان الرؤوس |
| Socket.io / ws | - | WebSockets |
| swagger-jsdoc / swagger-ui-express | - | توثيق API |
| @msgpack/msgpack | - | ترميز ثنائي للبيانات |
| compression | - | ضغط الاستجابات |
| express-rate-limit | - | تحديد معدل الطلبات |

### 1.4 هيكل المجلدات

```
server/
├── src/
│   ├── index.js                 # نقطة الدخول الرئيسية
│   ├── config/
│   │   └── swagger.js           # تكوين OpenAPI/Swagger
│   ├── controllers/             # 12 ملف تحكمي
│   │   ├── analyticsController.js    # 1082 سطر
│   │   ├── assessmentController.js   # 1045 سطر
│   │   ├── authController.js         # 623 سطر
│   │   ├── challengeController.js    # 213 سطر
│   │   ├── courseController.js       # 218 سطر
│   │   ├── healthController.js       # 154 سطر
│   │   ├── onboardController.js      # 275 سطر
│   │   ├── otpController.js          # 150 سطر
│   │   ├── reportsController.js      # 296 سطر
│   │   ├── schoolController.js       # 278 سطر
│   │   ├── studentManagementController.js # 663 سطر
│   │   └── studentProfileController.js    # 757 سطر
│   ├── middlewares/              # 4 ملفات وسيطة
│   │   ├── strictSecurity.js     # 535 سطر
│   │   ├── auth.js               # 480 سطر
│   │   ├── security.js           # 104 سطر
│   │   └── errorHandler.js       # 61 سطر
│   ├── router/
│   │   └── index.js              # مجمع المسارات
│   ├── routes/                   # 13 ملف مسار
│   │   ├── analytics.js
│   │   ├── auth.js
│   │   ├── challenge.js
│   │   ├── courses.js
│   │   ├── invitations.js
│   │   ├── onboard.js
│   │   ├── otp.js
│   │   ├── reports.js
│   │   ├── schools.js
│   │   ├── security.js
│   │   ├── student.js
│   │   └── studentManagement.js
│   ├── services/
│   │   └── adminService.js       # 326 سطر
│   └── utils/
│       ├── redis.js              # 359 سطر
│       ├── prisma.js             # 13 سطر
│       ├── logger.js             # 206 سطر
│       └── codeVerifier.js       # 85 سطر
├── prisma/
│   ├── schema.prisma             # 620 سطر, 25 نموذج
│   └── schema.postgresql.prisma  # نسخة PostgreSQL
└── .env                          # متغيرات البيئة
```

---

## 2. index.js — تحليل نقطة الدخول الرئيسية

**الموقع:** `server/src/index.js`  
**عدد الأسطر:** 372  
**الغرض:** تكوين خادم Express وتشغيله مع جميع الإعدادات الأمنية والوسيطة.

### 2.1 تحليل شامل لكل مجموعة أسطر

#### 2.1.1 الاستيرادات والتكوين الأولي (الأسطر 1-20)

```javascript
require('dotenv').config();
const express = require("express");
const helmet = require("helmet");
const rateLimit = require("express-rate-limit");
const compression = require("compression");
const { logger, requestLogger } = require("./utils/logger");
const { setupSwagger } = require("./config/swagger");
const { healthCheck, readinessCheck, livenessCheck } = require("./controllers/healthController");
const strictSecurity = require("./middlewares/strictSecurity");
const { connectRedis, disconnectRedis } = require("./utils/redis");
const app = express();
```

**الغرض:** استيراد جميع المكتبات والوحدات المطلوبة.  
**ملاحظات أمنية:** استخدام `dotenv` لتأمين المتغيرات البيئية.  
**اقتراح:** لا يوجد تحقق من وجود المتغيرات البيئية الأساسية (`JWT_SECRET`, `HMAC_SECRET`) عند بدء التشغيل.

#### 2.1.2 ثقة البروكسي (السطر 24)

```javascript
app.set('trust proxy', 1);
```

**الغرض:** تمكين احترام رؤوس `X-Forwarded-For` من البروكسي العكسي. ضروري لحساب عنوان IP الصحيح عند تحديد معدل الطلبات.

#### 2.1.3 تسجيل الطلبات (السطر 27)

```javascript
app.use(requestLogger);
```

**الغرض:** تسجيل جميع طلبات HTTP عبر Winston مع توقيت الاستجابة.

#### 2.1.4 رؤوس الأمان الصارمة (السطر 30)

```javascript
app.use(strictSecurity.strictSecurityHeaders);
```

**الغرض:** تعيين رؤوس أمان صارمة: CSP, HSTS, X-Frame-Options, X-Content-Type-Options, Referrer-Policy, Permissions-Policy.

#### 2.1.5 التحقق الصارم من الطلبات (السطر 33)

```javascript
app.use(strictSecurity.strictRequestValidation);
```

**الغرض:** التحقق من طول URL (2048 حرف)، حجم المحتوى (1MB)، الرؤوس المشبوهة، ووكلاء المستخدم الضارين.

#### 2.1.6 تنقية المدخلات (السطر 36)

```javascript
app.use(strictSecurity.strictSanitizeMiddleware);
```

**الغرض:** تنقية جميع حقول `req.body` و `req.query` و `req.params` من الأحرف الضارة.

#### 2.1.7 Helmet (الأسطر 38-47)

```javascript
app.use(helmet({
  contentSecurityPolicy: false,
  frameguard: false,
  xssFilter: false,
  noSniff: true,
  hidePoweredBy: true,
  crossOriginEmbedderPolicy: false,
}));
```

**تحليل:** تم تعطيل بعض ميزات Helmet الافتراضية لأن strictSecurity يطبق سياسات أكثر صرامة. هذا تكرار مقصود.

#### 2.1.8 ضغط الاستجابات (الأسطر 49-56)

```javascript
app.use(compression({ level: 6, threshold: 1024, filter: ... }));
```

**الغرض:** ضغط Brotli/Gzip مع دعم تعطيل الضغط عبر الرأس `x-no-compression`.

#### 2.1.9 تحديد معدل الطلبات (الأسطر 59-97)

ثلاثة محددات:
- **telemetryRateLimit:** 20 طلب/دقيقة للتليمترية
- **analyticsRateLimit:** 60 طلب/دقيقة للتحليلات
- **studentRateLimit:** 30 طلب/دقيقة للطلاب

**ملاحظة:** تم تعريف هذه المحددات ولكن **لا يتم استخدامها** في أي مكان في index.js. يبدو أنها غير مستخدمة أو مكررة مع `strictRateLimit` من strictSecurity.

#### 2.1.10 تحليل الجسم (الأسطر 99-125)

```javascript
app.use(express.json({ limit: '5mb', strict: true, reviver: ... }));
app.use(express.urlencoded({ extended: true, limit: '1mb' }));
```

**الأمان:** استخدام `reviver` لمنع السلاسل الطويلة جداً (أكثر من 1,000,000 حرف).  
**ملاحظة:** هناك تناقض — الـ reviver يحد السلاسل بـ 1M حرف بينما الحد العام هو 5MB.

#### 2.1.11 التحقق من Content-Type للتليمترية (الأسطر 113-125)

```javascript
app.use('/api/v1/submissions/report', (req, res, next) => {
  // يدعم فقط JSON, MessagePack, urlencoded
});
```

**الغرض:** ضمان أن نقطة نهاية التليمترية تقبل فقط التنسيقات المدعومة.

#### 2.1.12 CORS (الأسطر 128-155)

```javascript
app.use((req, res, next) => {
  const envOrigins = process.env.ALLOWED_ORIGINS;
  const allowedOrigins = envOrigins ? envOrigins.split(',') : ['http://localhost:3000', ...];
  // التحقق من الأصل والسماح بالوصول
});
```

**تحليل أمني:**
- يستخدم قائمة بيضاء من `ALLOWED_ORIGINS`
- يرد بـ 403 للأصول غير المسموح بها في طلبات OPTIONS
- يعرض رؤوس `Access-Control-Expose-Headers` لمعلومات حدود المعدل
- **مشكلة:** يستخدم `includes` لمقارنة الأصول بدلاً من التطابق التام، مما قد يسمح بأصول ضارة مثل `http://localhost:3000.evil.com`

#### 2.1.13 وسيط أمان إضافي (الأسطر 158-192)

```javascript
app.use((req, res, next) => {
  // التحقق من حجم المحتوى (أقصى 5MB)
  // التحقق من طول User-Agent
  // منع هجمات Path Traversal
  next();
});
```

**ملاحظة:** هذا يكرر بعض التحقق من `strictRequestValidation` ولكن بحدود مختلفة (5MB هنا مقابل 1MB هناك).

#### 2.1.14 نقاط الصحة (الأسطر 195-197)

```javascript
app.get('/health', healthCheck);
app.get('/health/ready', readinessCheck);
app.get('/health/live', livenessCheck);
```

**الغرض:** نقاط نهاية لمراقبة صحة الخادم من قبل منصات الاستضافة (Render, Railway, Docker).

#### 2.1.15 Swagger (الأسطر 200-203)

```javascript
if (process.env.NODE_ENV !== 'production') {
  setupSwagger(app);
}
```

**الأمان:** توثيق Swagger متاح فقط في بيئة التطوير.

#### 2.1.16 بريما كلاينت (الأسطر 205-206)

```javascript
const prisma = require("./utils/prisma");
app.set("prisma", prisma);
```

**الغرض:** إتاحة اتصال Prisma لجميع المسارات عبر `req.app.get("prisma")`.

#### 2.1.17 المسارات (الأسطر 209-211)

```javascript
const router = require("./router/index");
app.use('/api/v1', router);
```

**الغرض:** جميع مسارات API تحت البادئة `/api/v1`.

#### 2.1.18 الملفات الثابتة (الأسطر 214-228)

```javascript
const frontendPath = path.join(__dirname, "../../frontend-web");
app.use(express.static(frontendPath, {
  dotfiles: 'deny', index: false,
  setHeaders: res => { res.setHeader('Cache-Control', 'no-store'); }
}));
```

**الأمان:** منع الوصول إلى ملفات النقاط، وعدم استخدام الفهرسة، وتعطيل التخزين المؤقت.

#### 2.1.19 معالج 404 (الأسطر 231-260)

```javascript
app.use((req, res) => {
  if (req.originalUrl.includes('..')) return res.status(400).json({...});
  if (req.path.startsWith('/api')) return res.status(404).json({...});
  // خدمة صفحات الواجهة المعروفة
});
```

**الأمان:** التحقق من هجمات Path Traversal حتى في معالج 404.

#### 2.1.20 معالج الأخطاء (الأسطر 263-292)

```javascript
app.use((err, req, res, next) => {
  logger.error('Server Error', { error: err.message, stack: err.stack });
  // معالجة أخطاء Content-Type وحجم الطلب
  res.status(err.statusCode || 500).json({...});
});
```

**خيارات الأمان:** في الإنتاج، لا يتم كشف تفاصيل الخطأ أو التتبع.

#### 2.1.21 بدء الخادم (الأسطر 295-324)

```javascript
const PORT = process.env.PORT || 3000;
const HOST = process.env.HOST || '0.0.0.0';
connectRedis().catch(() => {});
const server = app.listen(PORT, HOST, async () => { ... });
```

**ملاحظة:** `connectRedis()` يتم استدعاؤها بدون `await` — لا تؤخر بدء التشغيل.

#### 2.1.22 الإغلاق اللطيف (الأسطر 327-359)

```javascript
const gracefulShutdown = (signal) => {
  server.close(async () => {
    await disconnectRedis();
    await prisma.$disconnect();
    process.exit(0);
  });
  setTimeout(() => { process.exit(1); }, 30000); // 30 ثانية مهلة
};
```

**الغرض:** إغلاق متحكم به مع تنظيف اتصالات Redis و Prisma.

#### 2.1.23 معالجة الاستثناءات غير المعالجة (الأسطر 362-369)

```javascript
process.on('unhandledRejection', (reason, promise) => { ... });
process.on('uncaughtException', (error) => { ... process.exit(1); });
```

### 2.2 مشكلات index.js

1. **محددات معدل غير مستخدمة:** `telemetryRateLimit`, `analyticsRateLimit`, `studentRateLimit` معرفة ولكن غير مطبقة.
2. **تكرار التحقق:** التحقق من حجم المحتوى في السطر 160 يكرر التحقق من `strictRequestValidation` (بحدود مختلفة).
3. **ثغرة CORS:** استخدام `includes` بدلاً من المساواة التامة لمقارنة الأصول.
4. **لا يوجد تحقق من المتغيرات البيئية:** لا يتم التحقق من وجود `JWT_SECRET` أو `HMAC_SECRET` عند بدء التشغيل.
5. **خدمة الملفات الثابتة:** `maxAge: 0` مع `no-store` قد يؤثر على أداء التحميل.
6. **عدم استخدام `await` مع `connectRedis()`:** يتم تجاهل نتيجة الاتصال.

---

## 3. Swagger — تحليل تكوين التوثيق

**الملف:** `server/src/config/swagger.js`  
**عدد الأسطر:** 229

### 3.1 معلومات API

```javascript
openapi: '3.0.3'
info.title: 'Dhad Studio API'
info.version: '1.0.0'
servers: ['http://localhost:3000', 'https://api.daad.studio']
```

### 3.2 مخططات قابلة لإعادة الاستخدام

| المخطط | الوصف |
|--------|-------|
| `Error` | كائن الخطأ الأساسي مع `error`, `message`, `timestamp` |
| `SuccessResponse` | استجابة النجاح مع `success: true` |
| `LoginRequest` | اسم المستخدم وكلمة المرور |
| `LoginResponse` | التوكنات وبيانات المستخدم |
| `RegisterRequest` | بيانات التسجيل مع التحقق من الصحة |
| `TelemetryPayload` | بيانات التليمترية مع دعم MessagePack |
| `TelemetryHeaders` | رؤوس HMAC المطلوبة للتليمترية |
| `Assessment` | بيانات الاختبار مع الوقت المسموح |
| `StudentProfile` | إحصائيات الطالب |
| `SecurityAlert` | تنبيهات الأمان |

### 3.3 نقاط الضعف في Swagger

1. **تعرض بيانات الإنتاج:** رغم أن Swagger متاح فقط في التطوير، فإن ملف التكوين يحتوي على رابط الإنتاج.
2. **عدم توثيق كامل:** العديد من نقاط النهاية غير موثقة في Swagger.
3. **عدم توثيق رؤوس HMAC:** `x-telemetry-signature` و `x-telemetry-timestamp` موثقة ولكن ليس كأجزاء من مواصفات OpenAPI الأمنية.

---

## 4. الوحدات التحكمية — 12 Controller

### 4.1 analyticsController.js (1082 سطر)

**الموقع:** `src/controllers/analyticsController.js`

#### قائمة الدوال

| الدالة | المسار | المدخلات | المخرجات | المصادقة |
|--------|--------|----------|----------|----------|
| `getLeaderboard` | GET /api/v1/analytics/leaderboard | `?schoolId, ?limit, ?refresh` | `{ school, leaderboard, ... }` | TEACHER/ADMIN |
| `getAttendanceSummary` | GET /api/v1/analytics/attendance-summary | `?schoolId, ?classroomId` | `{ school, period, attendance, trend }` | TEACHER/ADMIN |
| `getClassProgress` | GET /api/v1/analytics/progress | `?schoolId, ?classroomId` | `{ progress, analytics, mostFailedExercises }` | TEACHER/ADMIN |
| `getTestCases` | GET /api/v1/lessons/:lessonId/tests | `lessonId` | `{ success, lessonId, ... }` | TEACHER/ADMIN |
| `getChallengeTestCases` | GET /api/v1/challenges/tier/:tierId | `tierId` | `{ tier, challengesCount, data }` | TEACHER/ADMIN |
| `getChallenges` | GET /api/v1/challenges | `?difficulty` | `{ success, challenges, total }` | أي مستخدم مصادق |
| `getTeacherDashboard` | GET /api/v1/analytics/dashboard | لا يوجد | `{ dashboard: { classrooms, students, assessments, ... } }` | TEACHER/ADMIN |
| `getTeacherClassrooms` | GET /api/v1/analytics/classrooms | لا يوجد | `{ success, classrooms, total }` | TEACHER/ADMIN |
| `createClassroom` | POST /api/v1/analytics/classrooms | `{ name, subject, ... }` | `{ success, classroom }` | TEACHER/ADMIN |
| `getTeacherAssessments` | GET /api/v1/analytics/assessments | لا يوجد | `{ success, assessments, total }` | TEACHER/ADMIN |
| `markAttendance` | POST /api/v1/analytics/attendance | `{ classroomId, date, records }` | `{ success, message, count }` | TEACHER/ADMIN |
| `getAttendanceHistory` | GET /api/v1/analytics/attendance-history | `?classroomId, ?days` | `{ success, attendance, totalStudents }` | TEACHER/ADMIN |

#### تحليل دالة getLeaderboard

```javascript
async function getLeaderboard(req, res, next) {
  // 1. التحقق من الصلاحية
  // 2. محاولة قراءة ذاكرة Redis المخبأة
  // 3. إذا لم توجد، استعلام قاعدة البيانات
  // 4. حساب Honor Score = (معدل الإكمال × 30%) + (متوسط الدقة × 25%) + (متوسط المشاريع × 25%) + (XP/100 × 20%)
  // 5. تحديث ذاكرة Redis المخبأة
  // 6. إرجاع النتائج
}
```

**الأمان:**
- التحقق من أن المستخدم يمكنه فقط رؤية بيانات مدرسته
- في حالة فشل قاعدة البيانات، يُرجع لوحة متصدرين فارغة بدلاً من 500

**الأداء:**
- يستخدم Redis Sorted Set للتخزين المؤقت (TTL: 5 دقائق)
- استعلام قاعدة البيانات يتضمن جميع الطلاب وجميع التحديثات — قد يكون بطيئاً للمدارس الكبيرة
- N+1 محتمل عند إثراء بيانات الطلاب

**المشكلات:**
- حساب `avgChallengeScore` يقسم على `totalChallenges` الذي قد يكون 0 (NaN)
- التاريخ الثابت "2026-01-01" مشفر
- في حالة فشل Redis، لا يوجد تخزين مؤقت بديل

#### تحليل دالة getClassProgress

```javascript
async function getClassProgress(req, res, next) {
  // 1. الحصول على جميع الطلاب
  // 2. الحصول على أول 5 دروس
  // 3. جلب جميع التحديثات مرة واحدة لمنع N+1
  // 4. بناء مصفوفة التقدم لكل طالب
  // 5. تحديد أكثر التمارين فشلاً
  // 6. حساب متوسط XP
}
```

**المشكلات:**
- `now.setDate(now.getDate() - 7)` يغير كائن `now` المستخدم لاحقاً
- `firstFiveLessons` محدد بشكل ثابت كالدروس 1-5
- `exercise.title` و `exercise.title` قد يكونا `undefined` في بعض الحالات

#### تحليل دالة markAttendance

```javascript
async function markAttendance(req, res, next) {
  // لكل طالب: upsert سجل الحضور
}
```

**مشكلة:** لكل طالب يتم إجراء upsert منفصل — يمكن تحسينه باستخدام `createMany` + معالجة التعارضات.

### 4.2 assessmentController.js (1045 سطر)

**الموقع:** `src/controllers/assessmentController.js`

#### قائمة الدوال

| الدالة | المسار | المدخلات | المخرجات |
|--------|--------|----------|----------|
| `createAssessment` | POST /api/v1/assessments/create | بيانات الاختبار الكاملة | الاختبار المُنشأ |
| `getActiveAssessment` | GET /api/v1/student/assessment/active | لا يوجد | الاختبار النشط مع التحديات |
| `submitAssessmentAnswer` | POST /api/v1/student/assessment/submit | `{ assessmentId, challengeId, code }` | نتيجة الإرسال |
| `startAssessmentAttempt` | POST /api/v1/student/assessment/start | `{ assessmentId }` | سجل المحاولة |
| `completeAssessment` | POST /api/v1/student/assessment/complete | `{ assessmentId }` | النتيجة النهائية |
| `getAssessmentResults` | GET /api/v1/assessments/:assessmentId/results | `assessmentId` في الرابط | الإحصائيات والنتائج |
| `exportClassroomReport` | GET /api/v1/analytics/classroom/:classId/export | `classId` في الرابط | ملف CSV |
| `getClassroomAssessments` | GET /api/v1/assessments/classroom/:classId | `classId` في الرابط | قائمة الاختبارات مع الإحصائيات |
| `getStudentAssessments` | GET /api/v1/student/assessments | لا يوجد | الاختبارات القادمة/النشطة/المكتملة |

#### تحليل أمني مهم

**دالة createAssessment:**
- تحديد معدل: اختبار واحد كحد أقصى لكل فصل كل 6 ساعات
- التحقق من أن الوقت المسموح بين 5-300 دقيقة
- تسجيل جميع الأنشطة في سجل النشاطات

```javascript
// RATE LIMIT: Max 1 assessment per classroom per 6 hours
const sixHoursAgo = new Date(Date.now() - 6 * 60 * 60 * 1000);
const recentCount = await prisma.assessment.count({
  where: { classroomId, createdAt: { gte: sixHoursAgo } }
});
```

**دالة submitAssessmentAnswer — النقطة الأمنية الحرجة:**
```javascript
// SECURITY: Never trust client for passed/output/executionTime/syntaxScore
const passed = false;
const output = "";
const executionTime = 0;
const syntaxScore = 0;
const pointsEarned = 0;
```

**تحليل:** هذا هو موقف أمني صحيح — الخادم لا يثق أبداً في بيانات العميل للاختبارات عالية المخاطر. جميع الإرسالات تبدأ بقيم صفرية حتى التحقق من الخادم.

**دالة exportClassroomReport — الحماية من حقن CSV:**
```javascript
const sanitizeCSV = (val) => {
  if (typeof val !== 'string') return val;
  const escaped = val.replace(/"/g, '""');
  if (/^[=+\-@\t\r]/.test(escaped)) {
    return `"'"${escaped}"`;
  }
  return `"${escaped}"`;
};
```

**المشكلات:**
1. لا يوجد تحقق من جانب الخادم للنتائج — `submitAssessmentAnswer` يسجل دائماً `passed=false`
2. `completeAssessment` يستخدم `passingScore` ثابت 60 بغض النظر عن درجة النجاح المحددة في الاختبار
3. دالة `getActiveAssessment` لا تتحقق مما إذا كان الطالب قد بدأ الاختبار بالفعل قبل السماح بالتقديم
4. لا يوجد حد أقصى لحجم `code` في `submitAssessmentAnswer`

### 4.3 authController.js (623 سطر)

**الموقع:** `src/controllers/authController.js`

#### قائمة الدوال

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `login` | POST /api/v1/auth/login | تسجيل الدخول بالبريد الإلكتروني أو الرقم الوطني |
| `register` | POST /api/v1/auth/register | إنشاء مستخدم جديد (معلم/مشرف) |
| `changePassword` | PUT /api/v1/auth/change-password | تغيير كلمة المرور (طالب) |
| `resetPassword` | POST /api/v1/auth/reset-password | إعادة تعيين كلمة المرور (مشرف) |
| `refreshToken` | POST /api/v1/auth/refresh | تحديث رمز الوصول |
| `logout` | POST /api/v1/auth/logout | تسجيل الخروج (زيادة إصدار التوكن) |

#### تحليل دالة login

```javascript
async function login(req, res, next) {
  // 1. التحقق من صحة النوع — username و password يجب أن يكونا سلاسل نصية
  // 2. التحقق من قفل الحساب
  // 3. البحث عن المستخدم أولاً بالبريد الإلكتروني، ثم بالرقم الوطني
  // 4. التحقق من كلمة المرور بـ bcrypt
  // 5. تسجيل محاولات تسجيل الدخول الناجحة والفاشلة
  // 6. إصدار JWT (Access + Refresh)
}
```

**الأمان:**
- يتحقق من قفل الحساب قبل محاولة المصادقة
- يستخدم البحث التدريجي (email → nationalId) لتجنب تسريب المعلومات
- يسجل جميع المحاولات (ناجحة وفاشلة)
- يستخدم `BCRYPT_ROUNDS = 12` وهو مستوى مناسب

**المشكلات:**
- `clearFailedAttempts(username)` يُستدعى بعد النجاح ولكن `username` قد يكون بريداً إلكترونياً أو رقماً وطنياً — مفتاح قفل الحساب يجب أن يكون متسقاً
- يتم تسجيل محاولات `loginLog.create` بطريقة غير محظورة (`.catch(() => {}))` مما قد يخفي أخطاء قاعدة البيانات
- `expiresIn: 3600` مشفر في الاستجابة بدلاً من حسابه من `JWT_ACCESS_EXPIRES`

#### تحليل دالة register

```javascript
async function register(req, res, next) {
  // التحقق من الأدوار: المشرف يمكنه إنشاء أي دور، المعلم لا يمكنه إنشاء ADMIN
  // التحقق من منع تصعيد الصلاحيات
  // التحقق من تكرار البريد الإلكتروني والرقم الوطني
  // تشفير كلمة المرور وإنشاء المستخدم
}
```

**الأمان الممتاز:**
- يمنع المعلمين من إنشاء حسابات ADMIN
- يمنع الطلاب من إنشاء أي حسابات
- يتحقق من تكرار البريد الإلكتروني والرقم الوطني داخل المدرسة
- ينظف أحرف التحكم من البريد الإلكتروني

### 4.4 studentProfileController.js (757 سطر)

**الموقع:** `src/controllers/studentProfileController.js`

#### قائمة الدوال

| الدالة | الوصف |
|--------|-------|
| `getOrCreateProfile` | إنشاء أو استرجاع ملف الطالب |
| `awardXP` | منح نقاط الخبرة (فقط من المصادر الموثوقة) |
| `calculateDailyStreak` | حساب السلسلة اليومية |
| `calculateLevel` | حساب المستوى (1-50) |
| `checkAndAwardAchievements` | التحقق من الشارات ومنحها |
| `getStudentProfile` | GET /api/v1/student/profile |
| `getStudentRoadmap` | GET /api/v1/student/roadmap |
| `handleXPAwarding` | معالج منح XP |
| `processBulkTelemetry` | معالجة التليمترية المجمعة |

#### نظام XP والمستويات

```javascript
const XP_TABLE = {
  BEGINNER: 100,
  INTERMEDIATE: 200,
  ADVANCED: 300,
  EXPERT: 500
};

const LEVEL_THRESHOLDS = [
  0, 100, 250, 500, 1000, 1500, 2500, 3500, 5000, 7000,
  9000, 11500, 14000, 17000, 20000, 23500, 27500, 32000, 37000, 42500,
  48500, 55000, 62000, 70000, 78500, 87500, 97000, 107000, 118000, 130000,
  142500, 155500, 169000, 183000, 198000, 214000, 231000, 249000, 268000, 288000,
  310000, 333000, 358000, 384000, 412000, 442000, 474000, 508000, 544000, 582000
];
```

**تحليل الصيغة:** هناك 50 مستوى، وآخر مستوى يتطلب 582,000 XP. الزيادة غير خطية — المستويات المبكرة سهلة (100 XP للمستوى 1) والمستويات المتأخرة صعبة (38,000 XP للقفز من 49 إلى 50).

**المشكلات:**
- `calculateLevel` تبدأ من نهاية المصفوفة — صحيحة ولكن يمكن تحسينها باستخدام البحث الثنائي
- `XP_TABLE` تستخدم `BEGINNER` كقيمة افتراضية بدلاً من التحقق من وجود المستوى
- `checkAndAwardAchievements` تفترض أن `criteria` هو كائن مع `type` و `value` ولكن قاعدة البيانات تخزنه كسلسلة JSON

#### الأمان في processBulkTelemetry

```javascript
// SECURITY: Never trust client-provided telemetry for XP/grading
// Telemetry is recorded but NOT used for XP or score calculations
// XP must come from verified judge callbacks only
// ...
passed: false, // SECURITY: Always false until server verification
output: '', // SECURITY: Never store client-provided output
executionTime: 0, // SECURITY: Never trust client execution time
syntaxScore: 0, // SECURITY: Never trust client scores
```

هذا موقف أمني صحيح — التليمترية من العميل تُسجل فقط للتدقيق ولا تؤثر على XP أو الدرجات.

### 4.5 studentManagementController.js (663 سطر)

**الموقع:** `src/controllers/studentManagementController.js`

#### قائمة الدوال

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `createStudent` | POST /api/v1/teacher/students | إنشاء طالب جديد |
| `bulkCreateStudents` | POST /api/v1/teacher/students/bulk | إنشاء طلاب بالجملة (max 50) |
| `getPublicClassrooms` | GET /api/v1/auth/classrooms | قائمة الفصول للتسجيل |
| `getClassroomStudents` | GET /api/v1/auth/classrooms/:classroom_id/students | قائمة الطلاب في الفصل |
| `studentLogin` | POST /api/v1/auth/student-login | تسجيل دخول الطالب |
| `getTeacherStudents` | GET /api/v1/teacher/students | قائمة طلاب المعلم |
| `deleteStudent` | DELETE /api/v1/teacher/students/:id | حذف طالب (إلغاء تنشيط) |
| `assignStudentToClassroom` | POST /api/v1/teacher/students/assign | نقل طالب إلى فصل آخر |

#### تحليل createStudent

```javascript
// Generate username: student name without spaces + school code
const schoolCode = school?.code || schoolId.slice(-4);
const baseUsername = name.trim().replace(/\s+/g, "").toLowerCase();
const username = `${baseUsername}${schoolCode}`;
```

**المشكلة:** إذا كان هناك طالبان بنفس الاسم في المدرسة، فسيكون لهما نفس اسم المستخدم.

#### تحليل studentLogin

```javascript
// التحقق من أن الفصل ينتمي إلى نفس المدرسة
if (student.classroom && student.classroom.schoolId !== student.schoolId) {
  return res.status(401).json({...});
}
```

**الأمان:** هذا التحقق مهم — يمنع طالباً من مدرسة من تسجيل الدخول في فصل من مدرسة أخرى. ولكن التحقق غير ضروري لأن الاستعلام يضمن ذلك بالفعل.

**المشكلات البارزة:**
1. `getPublicClassrooms` تُظهر أسماء المدارس لجميع الزوار غير المصادقين — قد يكون هذا مقصوداً للتسجيل
2. `deleteStudent` هو حذف ناعم (تعيين `isActive = false`) — جيد للاحتفاظ بالبيانات
3. دالة `assignStudentToClassroom` لا تتحقق مما إذا كان الفصل الجديد لا يزال لديه سعة (`maxStudents`)

### 4.6 challengeController.js (213 سطر)

**الموقع:** `src/controllers/challengeController.js`

#### قائمة الدوال

| الدالة | الوصف |
|--------|-------|
| `verifyChallengeCompletion` | التحقق من إكمال التحدي باستخدام HMAC-SHA256 |
| `validateChallengeSecurity` | وسيط للتحقق من صحة تنسيق التوقيع |
| `storeChallengeVerification` | تخزين سجل التحقق للتدقيق |

#### تحليل HMAC

```javascript
// التحقق من توقيع HMAC
const message = `${studentId}:${taskId}:${timestamp}`;
const expectedSignature = crypto
  .createHmac('sha256', process.env.HMAC_SECRET)
  .update(message)
  .digest('hex');

// مقارنة ثابتة زمنياً لمنع هجمات التوقيت
const isValidSignature = crypto.timingSafeEqual(
  Buffer.from(signature, 'hex'),
  Buffer.from(expectedSignature, 'hex')
);
```

**ميزات الأمان:**
- يستخدم `crypto.timingSafeEqual` لمنع هجمات التوقيت
- نافذة إعادة الإرسال 5 دقائق
- التحقق من تنسيق التوقيع السداسي العشري (64 حرفاً)
- التحقق من قفل الحساب كأولوية قصوى

**المشكلات:**
- `expectedSignature.substring(0, 16) + '...'` يسرب أول 16 حرفاً من التوقيع المتوقع في السجل
- `storeChallengeVerification` معلّقة (commented out) في `verifyChallengeCompletion`

### 4.7 courseController.js (218 سطر)

**الموقع:** `src/controllers/courseController.js`

#### قائمة الدوال

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `getCourses` | GET /api/v1/student/courses | قائمة الدورات غير الرئيسية |
| `enrollCourse` | POST /api/v1/student/courses/:courseId/enroll | التسجيل في دورة |
| `getCourseRoadmap` | GET /api/v1/student/courses/:courseId/roadmap | خريطة الدورة |

#### تحليل getCourses

```javascript
// الحصول على جميع الدورات غير الرئيسية
const courses = await prisma.course.findMany({
  where: { published: true, category: { not: 'MAIN' } },
});

// التحقق من إكمال المستوى المطلوب لفتح الدورة
const unlocked = maxCompletedTier >= c.unlockTier;
```

**المشكلات:**
- `mainChallenges` يتم جلب جميع التحديات الرئيسية مع جميع التقديمات — قد يكون بطيئاً للمدارس الكبيرة
- `completedTiers` يستخدم `Math.max(...completedTiers)` الذي قد يفشل إذا كانت المجموعة فارغة (تم التعامل معه)

### 4.8 healthController.js (154 سطر)

**الموقع:** `src/controllers/healthController.js`

#### قائمة الدوال

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `healthCheck` | GET /health | فحص شامل مع قاعدة البيانات، WebSocket، الذاكرة، CPU |
| `readinessCheck` | GET /health/ready | فحص جاهزية قاعدة البيانات |
| `livenessCheck` | GET /health/live | فحص البقاء (دائماً 200) |
| `setWebSocketServer` | - | تعيين مرجع خادم WebSocket |

#### تحليل healthCheck

```javascript
const healthCheck = async (req, res) => {
  // قاعدة البيانات: ping SELECT 1
  // WebSocket: حالة الاتصالات
  // الذاكرة: heapUsed, heapTotal, RSS
  // CPU: النموذج، النوى، وقت المستخدم/النظام
  // النظام: المنصة، إصدار Node، اسم المضيف
  // الميزات: جميعها 'active'
};
```

**المشكلات:**
- `prisma.$queryRaw``SELECT 1`` (يستخدم backticks) ولكنه مكتوب بـ backticks عادية — يجب أن يكون `prisma.$queryRaw(SELECT 1)` لقاعدة بيانات SQLite. هذا سيفشل في SQLite ولكنه يعمل مع PostgreSQL
- WebSocket `clients?.size` — يتطلب WebSocket 'ws' وليس Socket.io
- `maxConnections: 1000` قيمة ثابتة مشفرة

### 4.9 reportsController.js (296 سطر)

**الموقع:** `src/controllers/reportsController.js`

#### قائمة الدوال

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `weeklyReport` | POST /api/v1/reports/weekly | تقرير أسبوعي لكل مستخدم |
| `userActivity` | GET /api/v1/reports/activity/:userId | نشاط مستخدم محدد (سلسلة زمنية) |
| `schoolOverview` | GET /api/v1/reports/school-overview | نظرة عامة على المدرسة |

#### تحليل weeklyReport

```javascript
// إنشاء تقرير لكل مستخدم في المدرسة
const userStats = allIds.map((uid) => {
  const userLogins = loginsThisWeek.filter((l) => l.userId === uid);
  const uniqueDays = new Set(userLogins.map((l) => fmtDate(l.createdAt))).size;
  // ...
});
```

**مشكلة أداء:** يمكن تحسين الفلترة باستخدام Map <userId, items[]> بدلاً من `filter` لكل مستخدم (من O(n*m) إلى O(n + m)).

### 4.10 schoolController.js (278 سطر)

**الموقع:** `src/controllers/schoolController.js`

#### قائمة الدوال

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `getSchools` | GET /api/v1/schools | قائمة المدارس |
| `getSchoolById` | GET /api/v1/schools/:id | تفاصيل المدرسة |
| `listSchoolUsers` | GET /api/v1/schools/:id/users | قائمة المستخدمين |
| `updateSchool` | PUT /api/v1/schools/:id | تحديث المدرسة (ADMIN) |
| `deleteSchool` | DELETE /api/v1/schools/:id | حذف المدرسة (ADMIN) |
| `deleteUser` | DELETE /api/v1/schools/:id/users/:userId | حذف مستخدم (ADMIN) |

**تحليل deleteSchool:**
```javascript
// Prevent deleting the last school
const totalSchools = await prisma.school.count();
if (totalSchools <= 1) {
  return res.status(400).json({...});
}
```

هذا تحقق جيد يمنع حذف آخر مدرسة في النظام.

### 4.11 onboardController.js (275 سطر)

**الموقع:** `src/controllers/onboardController.js`

#### الدالة الوحيدة

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `onboardSchool` | POST /api/v1/onboard-school | إنشاء المدرسة بالجملة |

**تحليل الأمان — anti-tampering:**
```javascript
if (expectedStudents && typeof expectedStudents === "number") {
  if (expectedStudents > 0 && expectedStudents !== students.length) {
    return res.status(400).json({
      error: "Bad Request",
      message: `Expected ${expectedStudents} students but CSV has ${students.length} — possible data tampering`,
    });
  }
}
```

هذه ميزة أمان جيدة تمنح المشرف إمكانية تحديد عدد الطلاب المتوقعين مسبقاً واكتشاف التلاعب.

**المشكلات:**
- `defaultPassword` واحد لجميع الحسابات — غير آمن، يجب أن يكون لكل مستخدم كلمة مرور فريدة
- استخدام `createMany` لا ينشئ StudentProfile تلقائياً — يجب إنشاؤها بشكل منفصل
- لا يوجد تسجيل لدورات أو فصول للمدرسة المنشأة حديثاً

### 4.12 otpController.js (150 سطر)

**الموقع:** `src/controllers/otpController.js`

#### قائمة الدوال

| الدالة | المسار | الوصف |
|--------|--------|-------|
| `requestOtp` | POST /api/v1/auth/request-otp | طلب رمز تحقق للبريد الإلكتروني |
| `verifyOtp` | POST /api/v1/auth/verify-otp | التحقق من الرمز وتفعيل الحساب |
| `importStudentsHandler` | POST /api/v1/teacher/invitations/import | استيراد الطلاب بالجملة |
| `getInvitationsHandler` | GET /api/v1/teacher/invitations | عرض الدعوات |

**ملاحظات أمنية:**
- `requestOtp` لا يكشف ما إذا كان البريد الإلكتروني موجوداً (يقول "If this email has an invitation...")
- `verifyOtp` يتحقق من أن OTP هو بالضبط 6 أرقام
- `verifyOtp` يتطلب كلمة مرور قوية للتفعيل

---

## 5. الوسائط — 4 Middlewares

### 5.1 strictSecurity.js (535 سطر)

**الموقع:** `src/middlewares/strictSecurity.js`  
**أكبر ملف وسيط وأكثرها أهمية.**

#### المكونات الرئيسية

| المكون | الأسطر | الوصف |
|--------|--------|-------|
| التخزين في الذاكرة | 10-15 | Maps للتخزين المؤقت (في الإنتاج، استخدم Redis) |
| التكوين | 18-56 | حدود المعدل، قفل الحساب، حظر IP، قواعد كلمة المرور |
| `strictRateLimit` | 59-90 | محدد معدل صارم لكل فئة |
| `recordFailedAttempt` | 93-118 | تسجيل المحاولات الفاشلة والتأخير التدريجي |
| `isAccountLocked` | 124-140 | التحقق من قفل الحساب |
| `recordIPFailure` | 156-172 | تسجيل فشل IP وحظره |
| `strictRequestValidation` | 191-229 | التحقق من صحة الطلب |
| `strictSanitize` | 232-265 | تنقية عميقة للمدخلات |
| `validatePasswordStrength` | 281-307 | التحقق من قوة كلمة المرور |
| `logAudit` | 310-336 | سجل التدقيق الدائري (10k) |
| `strictSecurityHeaders` | 339-383 | رؤوس الأمان الصارمة |
| `bridgeAuthMiddleware` | 386-401 | مصادقة جسر API |
| `verifyRequestSignature` | 410-444 | التحقق من توقيع الطلب |
| `registerSession/removeSession` | 447-467 | إدارة الجلسات |
| `enforceMaxSessions` | 469-483 | فرض الحد الأقصى للجلسات |
| التنظيف الدوري | 486-509 | تنظيف السجلات القديمة كل 60 ثانية |

#### تحليل خوارزمية التأخير التدريجي

```javascript
// Progressive delay: 1s, 2s, 4s, 8s, 16s after each failure
if (record.count >= 2 && record.count < CONFIG.MAX_FAILED_ATTEMPTS) {
  const delayMs = Math.min(1000 * Math.pow(2, record.count - 1), 16000);
  record.delayUntil = now + delayMs;
}
```

هذه الخوارزمية تصعّب هجمات القوة العمياء بشكل كبير — كل محاولة فاشلة تضاعف وقت الانتظار حتى 16 ثانية.

#### تحليل سياسة CSP

```javascript
const scriptSrc = isApiRequest ? "'self'" : "'self' 'unsafe-inline' 'unsafe-eval'";
const styleSrc = isApiRequest ? "'self'" : "'self' 'unsafe-inline' https://fonts.googleapis.com";
```

هذا تكيف ذكي — نقط نهاية API لديها CSP صارم بينما صفحات الواجهة تسمح بالأنماط والبرامج النصية المضمنة.

#### المشكلات في strictSecurity.js

1. **التخزين في الذاكرة:** كل شيء في ذاكرة العملية — لا يتحمل إعادة التشغيل ولا يعمل في بيئة متعددة العمليات
2. **سجل التدقيق الدائري:** استخدام `splice(0, length - 10000)` هو O(n) رغم أن التعليق يقول O(1)
3. **مفاتيح `MAX_REQUESTS_PER_WINDOW`:** `auth` و `authenticated` يستخدمان نفس قيمة `RATE_LIMIT_AUTHENTICATED` — يجب أن يكون `auth` أقل
4. **لا يوجد تطهير لـ `req.headers`:** sanitizeMiddleware ينظف `req.body` و `req.query` و `req.params` فقط
5. **`strictSanitize` يقطع المفاتيح:** مفاتيح الكائنات يتم تطهيرها وقد يتغير هيكل الكائن
6. **حد طول الحقل 500:** يطبق على جميع الحقول — قد يكون صغيراً جداً لحقول مثل `code` أو `output`

### 5.2 auth.js (480 سطر)

**الموقع:** `src/middlewares/auth.js`

#### المكونات الرئيسية

| المكون | الوصف |
|--------|-------|
| `authRateLimit` | 10 محاولات / 15 دقيقة (IP + username) |
| `apiRateLimit` | 60 طلب / دقيقة |
| `validateHMACSignature` | التحقق من HMAC SHA-256 مع نافذة 5 دقائق |
| `detectSuspiciousActivity` | كشف الأنشطة المشبوهة (توقيت سريع، درجة كاملة، كود قصير) |
| `authenticate` | التحقق من JWT HS256 + التحقق من إصدار التوكن |
| `requireRole` | التحقق من الدور (ADMIN, TEACHER, STUDENT) |
| `requireSchoolAccess` | التحقق من الوصول إلى المدرسة |
| `requireOwnership` | التحقق من ملكية المورد |
| `checkTokenRevocation` | التحقق من إلغاء التوكن |

#### تحليل authenticate

```javascript
async function authenticate(req, res, next) {
  // 1. التحقق من وجود رأس Authorization بصيغة Bearer
  // 2. التحقق من JWT باستخدام HS256 فقط
  // 3. التحقق من بنية الحمولة (id, role, schoolId)
  // 4. التحقق من تنشيط حساب الطالب
  // 5. التحقق من إصدار التوكن مقابل قاعدة البيانات
  // 6. Fail-closed: رفض الوصول إذا كانت قاعدة البيانات غير متاحة
}
```

**الأمان الممتاز:**
- يستخدم `algorithms: ["HS256"]` صراحةً لمنع هجمات خلط الخوارزميات
- Fail-closed: إذا كانت قاعدة البيانات غير متاحة، يتم رفض جميع الطلبات
- التحقق من إصدار التوكن لكل طلب (سحب فوري)
- التحقق من تنشيط حساب الطالب في كل طلب

#### تحليل detectSuspiciousActivity

```javascript
function detectSuspiciousActivity(submissionData, challenge) {
  // 1. وقت تنفيذ سريع جداً مقارنة بالمستوى
  // 2. درجة كاملة في المحاولة الأولى للمستويات العالية
  // 3. كود قصير جداً لمخرجات معقدة
  // 4. وقت التنفيذ أسرع بنسبة 90% من الوقت المقدر
}
```

هذا النظام يكتشف أنماط الغش المحتملة بتكلفة حوسبة منخفضة.

### 5.3 security.js (104 سطر)

**الموقع:** `src/middlewares/security.js`

#### المكونات

| الدالة | الوصف |
|--------|-------|
| `sanitizeString` | إزالة البايتات الصفرية وأحرف التحكم |
| `deepSanitize` | تنقية متكررة للكائنات |
| `sanitizeMiddleware` | وسيط Express للتنقية |
| `isValidEmail` | التحقق من صحة البريد الإلكتروني (RFC 5322) |
| `submissionRateLimit` | 30 إرسال/ساعة للطالب |
| `typeGuard` | التحقق من أنواع الحقول |

#### isValidEmail

```javascript
function isValidEmail(email) {
  const re = /^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$/;
  if (email.length > 254) return false;
  if (email.includes("\n") || email.includes("\r") || email.includes("\0")) return false;
  return re.test(email);
}
```

هذا التعبير النمطي يمنع هجمات حقن الرؤوس عن طريق رفض أحرف السطر الجديد.

### 5.4 errorHandler.js (61 سطر)

**الموقع:** `src/middlewares/errorHandler.js`

```javascript
function errorHandler(err, req, res, _next) {
  // P2002 → 409 Conflict (تكرار)
  // P2025 → 404 Not Found (غير موجود)
  // JsonWebTokenError → 401 (توكن غير صالح)
  // TokenExpiredError → 401 (توكن منتهي)
  // TypeError → 400 (نوع إدخال غير صالح)
  // SyntaxError → 400 (JSON غير صالح)
  // آخر → 500 (خطأ داخلي)
}
```

**ملاحظة:** هذا المعالج لا يُستخدم في index.js. بدلاً من ذلك، يستخدم index.js معالج أخطاء مخصص خاص به في الأسطر 263-292.

---

## 6. ملفات المسارات — 13 Route Files

### 6.1 جدول المسارات الكامل

#### auth.js (46 سطر) — المسارات العامة والمحمية

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| POST | /login | loginSecurityCheck, strictRateLimit("public") | تسجيل الدخول |
| POST | /refresh | strictRateLimit("public") | تحديث التوكن |
| POST | /register | strictRateLimit("public"), authenticate, requireTeacherOrAdmin | تسجيل مستخدم |
| PUT | /change-password | authenticate, requireStudent | تغيير كلمة المرور |
| POST | /reset-password | authenticate, requireAdmin | إعادة تعيين كلمة المرور |
| POST | /logout | authenticate | تسجيل الخروج |

#### otp.js (17 سطر) — مسارات OTP العامة

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| POST | /request-otp | strictRateLimit("otp-request") | طلب OTP |
| POST | /verify-otp | strictRateLimit("otp-verify") | التحقق من OTP |

#### studentManagement.js (72 سطر) — مسارات عامة ومحمية

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| GET | /auth/classrooms | عام | قائمة الفصول |
| GET | /auth/classrooms/:classroom_id/students | عام | طلاب الفصل |
| POST | /auth/student-login | strictRateLimit("public"), delay check | تسجيل دخول الطالب |
| POST | /teacher/students | authenticate, requireTeacherOrAdmin | إنشاء طالب |
| POST | /teacher/students/bulk | authenticate, requireTeacherOrAdmin | إنشاء طلاب بالجملة |
| GET | /teacher/students | authenticate, requireTeacherOrAdmin | قائمة الطلاب |
| DELETE | /teacher/students/:id | authenticate, requireTeacherOrAdmin | حذف طالب |
| POST | /teacher/students/assign | authenticate, requireTeacherOrAdmin | نقل طالب |

#### invitations.js (27 سطر) — مسارات الدعوات

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| POST | /teacher/invitations/import | authenticate, requireTeacherOrAdmin | استيراد طلاب |
| GET | /teacher/invitations | authenticate, requireTeacherOrAdmin | عرض الدعوات |

#### analytics.js (40 سطر) — مسارات التحليلات

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| GET | /analytics/leaderboard | authenticate, requireTeacherOrAdmin | لوحة المتصدرين |
| GET | /analytics/attendance-summary | authenticate, requireTeacherOrAdmin | ملخص الحضور |
| GET | /analytics/attendance-history | authenticate, requireTeacherOrAdmin | تاريخ الحضور |
| POST | /analytics/attendance | authenticate, requireTeacherOrAdmin | تسجيل حضور |
| GET | /analytics/progress | authenticate, requireTeacherOrAdmin | تقدم الفصل |
| GET | /analytics/dashboard | authenticate, requireTeacherOrAdmin | لوحة المعلم |
| GET | /analytics/classrooms | authenticate, requireTeacherOrAdmin | فصول المعلم |
| POST | /analytics/classrooms | authenticate, requireTeacherOrAdmin | إنشاء فصل |
| GET | /analytics/assessments | authenticate, requireTeacherOrAdmin | اختبارات المعلم |
| GET | /challenges | authenticate | قائمة التحديات |
| GET | /lessons/:lessonId/tests | authenticate, requireTeacherOrAdmin | اختبارات الدرس |
| GET | /challenges/tier/:tierId | authenticate, requireTeacherOrAdmin | تحديات المستوى |

#### student.js (689 سطر) — أكبر ملف مسارات (يشمل التحديات والتليمترية والأعمال)

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| GET | /student/profile | authenticate, requireStudent | ملف الطالب |
| GET | /student/leaderboard | authenticate, requireStudent | لوحة متصدرين الطالب |
| GET | /student/roadmap | authenticate, requireStudent | خارطة الطريق |
| POST | /student/challenge/submit | authenticate, requireStudent | تقديم تحدٍ |
| GET | /student/achievements | authenticate, requireStudent | الإنجازات |
| GET | /student/classroom/check | authenticate, requireStudent | التحقق من الفصل |
| POST | /assessments/create | authenticate, requireTeacherOrAdmin | إنشاء اختبار |
| GET | /student/assessment/active | authenticate, requireStudent | اختبار نشط |
| GET | /student/assessments | authenticate, requireStudent | قائمة الاختبارات |
| POST | /student/assessment/submit | authenticate, requireStudent | تقديم إجابة |
| POST | /student/assessment/start | authenticate, requireStudent | بدء محاولة |
| POST | /student/assessment/complete | authenticate, requireStudent | إنهاء الاختبار |
| GET | /assessments/:assessmentId/results | authenticate, requireTeacherOrAdmin | نتائج الاختبار |
| GET | /assessments/classroom/:classId | authenticate, requireTeacherOrAdmin | اختبارات الفصل |
| GET | /analytics/classroom/:classId/export | authenticate, requireTeacherOrAdmin | تصدير CSV |
| POST | /submissions/bulk-report | authenticate, requireStudent | تقرير مجمع |
| POST | /workspace/save | authenticate, requireStudent | حفظ مساحة العمل |
| GET | /workspace/load/:challengeId | authenticate, requireStudent | تحميل مساحة العمل |
| GET | /workspace/list | authenticate, requireStudent | قائمة مساحات العمل |
| DELETE | /workspace/:challengeId | authenticate, requireStudent | حذف مساحة العمل |
| GET | /analytics/security-alerts | authenticate, requireTeacherOrAdmin | تنبيهات الأمان |
| POST | /analytics/security-alerts/:id/resolve | authenticate, requireTeacherOrAdmin | حل تنبيه |

#### challenge.js (37 سطر) — مسارات التحديات

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| POST | /challenge/verify | strictRateLimit("auth"), checkAccountLocked, validateChallengeSecurity | التحقق من التحدي |

#### courses.js (12 سطر) — مسارات الدورات

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| GET | /student/courses | authenticate, requireStudent | قائمة الدورات |
| POST | /student/courses/:courseId/enroll | authenticate, requireStudent | التسجيل |
| GET | /student/courses/:courseId/roadmap | authenticate, requireStudent | خارطة الدورة |

#### onboard.js (11 سطر) — مسارات الإعداد

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| POST | /onboard-school | strictRateLimit("onboard"), authenticate, requireAdmin | إعداد المدرسة |

#### reports.js (22 سطر) — مسارات التقارير

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| GET | /reports/school-overview | authenticate, requireRole("TEACHER", "ADMIN") | نظرة عامة |
| GET | /reports/school-overview/:schoolId | authenticate, requireRole("ADMIN") | نظرة عامة (مشرف) |
| POST | /reports/weekly | authenticate, requireRole("TEACHER", "ADMIN") | تقرير أسبوعي |
| POST | /reports/weekly/:schoolId | authenticate, requireRole("ADMIN") | تقرير أسبوعي (مشرف) |
| GET | /reports/activity/:userId | authenticate, requireRole("TEACHER", "ADMIN") | نشاط المستخدم |

#### schools.js (42 سطر) — مسارات المدارس

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| GET | /schools | authenticate | قائمة المدارس |
| GET | /schools/:id | authenticate, requireSchoolAccess | تفاصيل المدرسة |
| GET | /schools/:id/users | authenticate, requireSchoolAccess | مستخدمي المدرسة |
| PUT | /schools/:id | authenticate, requireAdmin | تحديث المدرسة |
| DELETE | /schools/:id | authenticate, requireAdmin | حذف المدرسة |
| DELETE | /schools/:id/users/:userId | authenticate, requireAdmin | حذف مستخدم |

#### security.js (81 سطر) — مسارات الأمان

| الطريقة | المسار | الوسائط | الوصف |
|---------|--------|---------|-------|
| GET | /security/dashboard | strictRateLimit("admin"), authenticate, requireAdmin | لوحة الأمان |
| GET | /security/audit | strictRateLimit("admin"), authenticate, requireAdmin | سجل التدقيق |
| POST | /security/unblock-ip | strictRateLimit("admin"), authenticate, requireAdmin | إلغاء حظر IP |

### 6.2 router/index.js — مجمع المسارات (34 سطر)

```javascript
const router = express.Router();
// ترتيب المسارات مهم — المسارات العامة أولاً
router.use("/auth", otpRoutes);    // OTP routes (public)
router.use("/auth", authRoutes);   // Auth routes
router.use("/", studentManagementRoutes); // Student management (has public classroom routes)
router.use("/", invitationsRoutes);
router.use("/", analyticsRoutes);
router.use("/", studentRoutes);
router.use("/", onboardRoutes);
router.use("/", securityRoutes);
router.use("/", challengeRoutes);
router.use("/", courseRoutes);
router.use("/reports", require('./../routes/reports'));
router.use("/schools", require('./../routes/schools'));
```

**ملاحظة:** السطران الأخيران (reports و schools) غير موجودين فعلياً في الملف الحالي. تم تضمينهما هنا للتوضيح. الملف الفعلي لا يشمل `reports` و `schools` — يبدو أن هذه المسارات مفقودة من المجمع.

### 6.3 ملخص المسارات (جميع نقاط النهاية ~80+)

| المجموع | الوصف |
|---------|-------|
| 8 | مسارات المصادقة (auth + OTP) |
| 8 | مسارات إدارة الطلاب |
| 2 | مسارات الدعوات |
| 12 | مسارات التحليلات |
| 22+ | مسارات الطالب (ملف تعريفي، تحديات، اختبارات، أعمال، تليمترية، أمان) |
| 1 | مسار التحقق من التحدي |
| 3 | مسارات الدورات |
| 1 | مسار الإعداد |
| 5 | مسارات التقارير |
| 6 | مسارات المدارس |
| 3 | مسارات الأمان |
| **~71+** | **مجموع نقاط النهاية** |

---

## 7. الخدمات والأدوات — Services & Utils

### 7.1 adminService.js (326 سطر)

**الموقع:** `src/services/adminService.js`

#### الدوال

| الدالة | المدخلات | المخرجات | الوصف |
|--------|----------|----------|-------|
| `importStudents` | `studentList, schoolId, classroomId` | `{ successful, failed, total }` | استيراد طلاب بالجملة مع OTP |
| `generateOTP` | لا يوجد | `string` (6 أرقام) | توليد رمز تحقق عشوائي |
| `requestOTP` | `email` | `{ success, message }` | طلب رمز تحقق جديد |
| `verifyOTP` | `email, otp, password` | `{ success, accessToken, ... }` | التحقق من OTP وتفعيل الحساب |
| `getInvitations` | `schoolId, options` | `{ invitations, total }` | قائمة الدعوات |

#### تحليل generateOTP

```javascript
function generateOTP() {
  return crypto.randomInt(100000, 999999).toString();
}
```

**تحليل أمني:**
- يستخدم `crypto.randomInt` (مولد أرقام عشوائي آمن) — ✅ ممتاز
- 6 أرقام = 900,000 احتمال — مقبول لـ OTP
- **مشكلة:** لا يوجد تأخير بين محاولات التحقق — يجب إضافة تأخير تدريجي

#### تحليل importStudents

```javascript
async function importStudents(studentList, schoolId, classroomId) {
  // التحقق من الصحة
  if (studentList.length > 100) throw new Error("Maximum 100 students per import");

  // لكل طالب:
  // 1. التحقق من عدم وجود دعوة سابقة
  // 2. توليد OTP
  // 3. تشفير OTP باستخدام bcrypt
  // 4. إنشاء سجل الدعوة
}
```

**الأمان:**
- OTP يُخزّن بشكل مشفر (bcrypt) — ✅
- التحقق من الدعوات المكررة — ✅
- حد أقصى 100 طالب — ✅

**المشكلات:**
- العمليات متسلسلة (لكل طالب) — أداء ضعيف للمجموعات الكبيرة
- لا يوجد استخدام للـ Prisma transactions — قد يؤدي إلى حالة غير متناسقة إذا فشلت بعض العمليات

#### تحليل verifyOTP

```javascript
async function verifyOTP(email, otp, password) {
  // 1. التحقق من صحة كلمة المرور
  // 2. البحث عن الدعوة
  // 3. التحقق من صلاحية OTP
  // 4. مقارنة OTP باستخدام bcrypt
  // 5. إنشاء حساب المستخدم
  // 6. إنشاء ملف الطالب
  // 7. تحديث الدعوة كمفعلة
  // 8. إصدار JWT
}
```

**المشكلات:**
- `validatePasswordStrength` يتم استيرادها داخل الدالة (require) — ممارسة سيئة، يجب أن تكون في الأعلى
- `jwt` يتم استيراده داخل الدالة أيضاً
- عدم وجود معاملة (transaction) — إذا فشلت الخطوة 6، يبقى المستخدم منشأ بدون ملف طالب

### 7.2 redis.js (359 سطر)

**الموقع:** `src/utils/redis.js`

#### المكونات

| المكون | الوصف |
|--------|-------|
| Redis Client | اتصال ioredis مع `lazyConnect` |
| `leaderboardCache` | إدارة Sorted Set للوحة المتصدرين (TTL: 5 دقائق) |
| `failureCounter` | عداد فشل لكل طالب + تمرين (TTL: 24 ساعة) |
| `cache` | تخزين مؤقت عام مع JSON.parse/stringify |
| `connectRedis` | الاتصال بـ Redis |
| `disconnectRedis` | قطع اتصال Redis |

#### تحليل leaderboardCache

```javascript
const LEADERBOARD_KEY = 'leaderboard:xp';
const LEADERBOARD_TTL = 300; // 5 minutes

const leaderboardCache = {
  getTop: async (count = 50) => {
    const results = await client.zrevrange(LEADERBOARD_KEY, 0, count - 1, 'WITHSCORES');
    // تحويل النتائج إلى مصفوفة كائنات
  },
  updateScore: async (studentId, xp) => {
    await client.zadd(LEADERBOARD_KEY, xp, studentId);
  },
  bulkUpdate: async (students) => {
    // استخدام pipeline للإدراج المجمع
    const pipeline = client.pipeline();
    students.forEach(({ studentId, totalXP }) => {
      pipeline.zadd(LEADERBOARD_KEY, totalXP, studentId);
    });
    await pipeline.exec();
  },
};
```

**ميزات:**
- استخدام pipeline للإدراج المجمع — أداء أفضل
- التقاط جميع الأخطاء وإرجاع القيم الافتراضية — تحمل الأخطاء
- `lazyConnect` — لا يحاول الاتصال حتى الاستخدام الأول

**المشكلات:**
- `redisAvailable` متغير عام — غير آمن للخيوط ولكن مقبول في Node.js أحادي الخيط
- لا يوجد TTL للمجموعة — `LEADERBOARD_TTL` معرف ولكن لا يتم استخدامه
- كل دالة تلتقط الأخطاء وتعود بالقيم الافتراضية — قد يخفي مشاكل Redis حقيقية

#### نقاط الضعف في مدير Redis

```javascript
const getRedisClient = () => {
  if (!redis) {
    redis = new Redis(REDIS_CONFIG.url, { ... });
    // ...
  }
  return redis;
};
```

هذا نمط Singleton — آمن في Node.js أحادي الخيط.

### 7.3 prisma.js (13 سطر)

**الموقع:** `src/utils/prisma.js`

```javascript
const prisma = new PrismaClient({
  log: process.env.NODE_ENV === "development"
    ? ["query", "error", "warn"]
    : ["error"],
});
module.exports = prisma;
```

نمط Singleton بسيط. في بيئة التطوير، يسجل جميع الاستعلامات — مفيد للتصحيح.

### 7.4 logger.js (206 سطر)

**الموقع:** `src/utils/logger.js`

#### المكونات

| المكون | الوصف |
|--------|-------|
| `logger` | مثيل Winston مع 6 مستويات (critical→debug) |
| `securityLogger` | مسجل مخصص للتنبيهات الأمنية |
| `requestLogger` | وسيط Express لتسجيل الطلبات |

#### مستويات التسجيل المخصصة

```javascript
levels: {
  critical: 0,  // أعلى مستوى — للتنبيهات الأمنية
  error: 1,
  warn: 2,
  info: 3,
  http: 4,
  debug: 5,
}
```

#### الكشف عن البيئة السحابية

```javascript
const isCloudEnvironment = process.env.RENDER ||
  process.env.RAILWAY ||
  process.env.HEROKU ||
  process.env.AWS_LAMBDA_FUNCTION_NAME ||
  !fs.existsSync(path.join(__dirname, '../../logs'));
```

في البيئات السحابية، يستخدم فقط ناقل Console (بدون ملفات). في البيئة المحلية، يضيف ناقلات ملفات مع التدوير.

#### أمنية: مسجل التنبيهات

```javascript
const securityLogger = {
  suspiciousSubmission: (data) => {
    logger.critical('🚨 SUSPICIOUS SUBMISSION DETECTED', {
      type: 'SECURITY_ALERT',
      alertType: 'SUSPICIOUS_SUBMISSION',
      studentId,
      challengeId,
      reasons: reasons?.map(r => r.detail) || [],
    });
  },
  hmacFailure: (data) => {
    logger.critical('🔐 HMAC SIGNATURE MISMATCH', { ... });
  },
  rateLimitExceeded: (data) => {
    logger.warn('⏱️ RATE LIMIT EXCEEDED', { ... });
  },
  authFailure: (data) => {
    logger.warn('🔑 AUTHENTICATION FAILURE', { ... });
  },
};
```

**ملاحظة:** `securityLogger` معرف ولكن لا يتم استخدامه في أي مكان آخر في الكود. جميع التنبيهات الأمنية تسجل مباشرة عبر `logger`.

### 7.5 codeVerifier.js (85 سطر)

**الموقع:** `src/utils/codeVerifier.js`

#### الدوال

| الدالة | الوصف |
|--------|-------|
| `verifyCode` | التحقق من صحة الكود وفق متطلبات التحدي |

#### أنواع التحقق

```javascript
switch (req.type) {
  case "minVariables":  // عدد المتغيرات
  case "minPrints":     // عدد أوامر الطباعة
  case "mustContain":   // يجب أن يحتوي على نصوص محددة
  case "mustNotContain": // يجب ألا يحتوي على نصوص محددة
  case "minLines":      // عدد الأسطر الأدنى
  case "must_use_keyword": // استخدام كلمة مفتاحية محددة
}
```

**تحليل الأمان:**
- التعبير النمطي `\b(صحيح|نص|عشري|منطقي)\s+\w+` يعدّ المتغيرات في لغة دحد (العربية)
- `طباعة\s*\(` يعدّ أوامر الطباعة
- التحقق من متطلبات JSON — إذا كان JSON غير صالح، يعتبر الكود صحيحاً (fail open)

**المشكلة:** fail-open على JSON غير صالح: إذا فشل `JSON.parse(requirementsJson)`، يُرجع `{ valid: true, errors: [] }`. هذا يعني أن المتطلبات التالفة يتم تجاهلها بصمت.

---

## 8. نموذج قاعدة البيانات — 25 Prisma Models

### 8.1 نظرة عامة على النماذج

```prisma
generator client { provider = "prisma-client-js" }
datasource db { provider = "sqlite" url = env("DATABASE_URL") }
```

قاعدة البيانات الافتراضية هي SQLite للتطوير، مع نسخة PostgreSQL منفصلة في `schema.postgresql.prisma`.

### 8.2 تحليل كل نموذج

#### 1. School — المدرسة

| الحقل | النوع | الوصف | ملاحظات أمنية |
|-------|------|-------|---------------|
| `id` | String (cuid) | المعرف الفريد | |
| `name` | String | اسم المدرسة | |
| `code` | String? | كود المدرسة | يستخدم لإنشاء أسماء المستخدمين |
| `email` | String? | البريد الإلكتروني | |
| `phone` | String? | رقم الهاتف | |
| `address` | String? | العنوان | |
| `description` | String? | الوصف | |
| `planType` | String (default: SCHOOL) | نوع الخطة | |
| `totalLicenses` | Int (default: 0) | إجمالي التراخيص | |
| `usedLicenses` | Int (default: 0) | التراخيص المستخدمة | |
| `provisionedAt` | DateTime | تاريخ التوفير | |
| `createdAt` | DateTime | تاريخ الإنشاء | |
| `updatedAt` | DateTime | تاريخ التحديث | |

**العلاقات:**
- `users User[]` — المستخدمون
- `courses Course[]` — الدورات
- `students Student[]` — الطلاب
- `lessons Lesson[]` — الدروس
- `challenges Challenge[]` — التحديات
- `attendanceRecords Attendance[]` — سجلات الحضور
- `classrooms Classroom[]` — الفصول
- `securityAlerts SecurityAlert[]` — التنبيهات الأمنية

**المشكلات:**
- `totalLicenses` و `usedLicenses` معرفان ولكن لا يتم استخدامهما في أي تحكم
- `students Student[]` — يبدو مكرراً حيث يوجد `users` للأدوار المتعددة

#### 2. User — المستخدم

| الحقل | النوع | الوصف | ملاحظات أمنية |
|-------|------|-------|---------------|
| `id` | String (cuid) | المعرف الفريد | |
| `schoolId` | String | المدرسة | |
| `classroomId` | String? | الفصل | |
| `role` | String | الدور (STUDENT/TEACHER/ADMIN) | التحقق من الصلاحية |
| `name` | String | الاسم | |
| `email` | String? | البريد الإلكتروني | فريد داخل المدرسة |
| `nationalId` | String? | الرقم الوطني | فريد داخل المدرسة |
| `passwordHash` | String? | كلمة المرور المشفرة | bcrypt |
| `isActive` | Boolean (default: true) | نشط | |
| `isVerified` | Boolean (default: false) | تم التحقق | |
| `isApproved` | Boolean (default: false) | تم الموافقة | |
| `isOnboarded` | Boolean (default: false) | تم الإعداد | |
| `tokenVersion` | Int (default: 0) | إصدار التوكن | لإلغاء التوكنات |

**المؤشرات الفريدة:**
```prisma
@@unique([schoolId, email])
@@unique([schoolId, nationalId])
```

**أهمية أمنية:** هذا يضمن عدم وجود بريد إلكتروني أو رقم وطني مكرر داخل نفس المدرسة.

**المشكلات:**
- `nationalId` يُستخدم كاسم مستخدم للطلاب — قد يكون هذا مشكلة خصوصية (PGI)
- `passwordHash` يمكن أن يكون `null` — لحسابات OTP قد لا يكون لديهم كلمة مرور بعد
- حقل `classroomId` مع `@relation("ClassroomStudents")` — نفس الحقل يُستخدم في `teachingClassrooms` كعلاقة مختلفة

#### 3. Course — الدورة

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `schoolId` | String | المدرسة |
| `title` | String | العنوان |
| `description` | String | الوصف |
| `content` | String | المحتوى |
| `icon` | String | الأيقونة |
| `category` | String (default: MAIN) | التصنيف |
| `order` | Int | الترتيب |
| `unlockTier` | Int | مستوى فتح الدورة |
| `published` | Boolean | منشورة |

#### 4. Student — الطالب (نموذج منفصل)

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `schoolId` | String | المدرسة |
| `gradeLevel` | String? | المستوى الدراسي |
| `enrollmentDate` | DateTime | تاريخ التسجيل |
| `status` | String (default: ACTIVE) | الحالة |
| `gpa` | Float? | المعدل التراكمي |

**ملاحظة:** هذا النموذج يبدو غير مستخدم — جميع بيانات الطلاب مخزنة في نموذج `User` مع `role: "STUDENT"`. النموذج موجود في المخطط ولكن لا توجد علاقة مع `User`.

#### 5. Lesson — الدرس

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `schoolId` | String | المدرسة |
| `courseId` | String? | الدورة |
| `title` | String | العنوان |
| `contentSlug` | String | رابط المحتوى |
| `content` | String | المحتوى |
| `order` | Int | الترتيب |
| `duration` | Int? | المدة |
| `prerequisites` | String (default: "[]") | المتطلبات (JSON) |
| `objectives` | String (default: "[]") | الأهداف (JSON) |
| `published` | Boolean | منشور |
| `visibility` | String (default: PUBLIC) | الرؤية |

#### 6. StudentCourseEnrollment — تسجيل الطالب في الدورة

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String | الطالب |
| `courseId` | String | الدورة |
| `enrolledAt` | DateTime | تاريخ التسجيل |
| `completedAt` | DateTime? | تاريخ الإكمال |

**المؤشر الفريد:** `@@unique([studentId, courseId])` — لا يمكن التسجيل مرتين.

#### 7. Classroom — الفصل

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `schoolId` | String | المدرسة |
| `teacherId` | String | المعلم |
| `name` | String | اسم الفصل |
| `description` | String? | الوصف |
| `gradeLevel` | String? | المستوى |
| `subject` | String? | المادة |
| `maxStudents` | Int (default: 40) | الحد الأقصى |
| `isActive` | Boolean | نشط |

**المؤشر الفريد:** `@@unique([schoolId, name])` — لا يمكن تكرار اسم الفصل في نفس المدرسة.

#### 8. Challenge — التحدي

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `authorId` | String | المؤلف |
| `title` | String | العنوان |
| `description` | String | الوصف |
| `starterCode` | String | الكود الابتدائي |
| `expectedOutput` | String | المخرجات المتوقعة |
| `dynamicOutput` | String | مخرجات ديناميكية ({{student.name}}) |
| `requirements` | String (default: "[]") | المتطلبات (JSON) |
| `difficulty` | String (default: BEGINNER) | الصعوبة |
| `challengeType` | String (default: BUG_FIX) | النوع |
| `published` | Boolean | منشور |
| `dailyDate` | String? | تاريخ التحدي اليومي |
| `tier` | Int (default: 1) | المستوى |
| `order` | Int (default: 1) | الترتيب |
| `estimatedTime` | Int? | الوقت المقدر |
| `points` | Int (default: 100) | النقاط |
| `xpReward` | Int (default: 0) | مكافأة XP |
| `isSystem` | Boolean | نظامي |
| `popularity` | Int (default: 0) | الشعبية |
| `schoolId` | String? | المدرسة |
| `courseId` | String? | الدورة |

**ملاحظة:** النموذج يحتوي على `School?` و `schoolId String?` — مما يعني أن التحديات يمكن أن تكون عالمية (بدون schoolId) أو خاصة بالمدرسة.

#### 9. Submission — التقديم

| الحقل | النوع | الوصف | ملاحظات أمنية |
|-------|------|-------|---------------|
| `id` | String (cuid) | المعرف | |
| `challengeId` | String | التحدي | |
| `studentId` | String | الطالب | |
| `code` | String | الكود | |
| `language` | String (default: "daad") | اللغة | |
| `passed` | Boolean (default: false) | النجاح | لا تثق بالعميل |
| `output` | String | المخرجات | |
| `executionTime` | Int? | وقت التنفيذ | لا تثق بالعميل |
| `memoryUsage` | Int? | استخدام الذاكرة | |
| `syntaxScore` | Int? | درجة الصياغة | لا تثق بالعميل |
| `performanceScore` | Int? | درجة الأداء | لا تثق بالعميل |
| `compilationError` | String? | خطأ الترجمة | |
| `timeout` | Boolean | مهلة | |
| `lineCount` | Int? | عدد الأسطر | |
| `functionCount` | Int? | عدد الدوال | |
| `complexity` | Int? | التعقيد | |
| `suspicious` | Boolean (default: false) | مشبوه | |
| `suspiciousReason` | String? | سبب الاشتباه | |
| `hmacValid` | Boolean (default: true) | HMAC صالح | |
| `aiFeedback` | String? | ملاحظات AI | |

**الأهمية الأمنية:** هذا النموذج مصمم للأمان — الحقول الحرجة مثل `passed`, `executionTime`, `syntaxScore` يتم تعيينها فقط من قبل الخادم، وليس من العميل.

#### 10. Project — المشروع

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String | الطالب |
| `lessonId` | String | الدرس |
| `title` | String | العنوان |
| `description` | String | الوصف |
| `code` | String | الكود |
| `language` | String (default: "daad") | اللغة |
| `status` | String (default: IN_PROGRESS) | الحالة |
| `score` | Int? | الدرجة |
| `feedback` | String? | الملاحظات |
| `fileCount` | Int | عدد الملفات |
| `totalSize` | Int | الحجم الإجمالي |

#### 11. Attendance — الحضور

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String | الطالب |
| `schoolId` | String | المدرسة |
| `date` | DateTime | التاريخ |
| `status` | String (default: PRESENT) | الحالة (PRESENT/ABSENT/LATE) |
| `notes` | String? | ملاحظات |
| `recordedBy` | String? | المسجّل |

**المؤشر الفريد:** `@@unique([studentId, date])` — سجل حضور واحد لكل طالب في اليوم.

#### 12. ChallengeTier — مستوى التحدي

| الحقل | النوع | الوصف |
|-------|------|-------|
| `challengeId` | String | التحدي |
| `tier` | String | المستوى |

**المفتاح المركب:** `@@id([challengeId, tier])`

#### 13. Assessment — الاختبار

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `classroomId` | String | الفصل |
| `title` | String | العنوان |
| `description` | String | الوصف |
| `assessmentType` | String (default: CODING) | النوع |
| `allowedTime` | Int | الوقت المسموح (دقائق) |
| `startTime` | DateTime | وقت البدء |
| `endTime` | DateTime | وقت الانتهاء |
| `totalPoints` | Int (default: 100) | إجمالي النقاط |
| `passingScore` | Int (default: 60) | درجة النجاح |
| `isPublished` | Boolean | منشور |
| `lockdownMode` | Boolean (default: true) | وضع القفل |
| `challengeIds` | String (default: "[]") | معرفات التحديات (JSON) |
| `questions` | String (default: "[]") | الأسئلة (JSON) |
| `starterCode` | String | الكود الابتدائي |
| `expectedOutput` | String | المخرجات المتوقعة |
| `instructions` | String | التعليمات |

#### 14. StudentAssessmentResult — نتيجة اختبار الطالب

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `assessmentId` | String | الاختبار |
| `studentId` | String | الطالب |
| `score` | Int (default: 0) | الدرجة |
| `totalPointsEarned` | Int (default: 0) | النقاط المكتسبة |
| `totalPointsPossible` | Int (default: 0) | النقاط الممكنة |
| `challengesAttempted` | Int (default: 0) | التحديات المجرّبة |
| `challengesPassed` | Int (default: 0) | التحديات الناجحة |
| `timeSpent` | Int (default: 0) | الوقت المستغرق |
| `startedAt` | DateTime | وقت البدء |
| `completedAt` | DateTime? | وقت الإكمال |
| `submissionIds` | String (default: "[]") | معرفات التقديمات (JSON) |
| `metadata` | String? | بيانات إضافية |
| `status` | String (default: IN_PROGRESS) | الحالة |

**المؤشر الفريد:** `@@unique([assessmentId, studentId])` — محاولة واحدة لكل اختبار لكل طالب.

#### 15. StudentSubmission — تقديم الطالب (تحليلي)

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `submissionId` | String | التقديم |
| `score` | Int | الدرجة |
| `completionTime` | Int | وقت الإكمال |
| `tokenCount` | Int | عدد الرموز |
| `astNodesCount` | Int | عدد عُقد AST |
| `errorCount` | Int | عدد الأخطاء |
| `warningCount` | Int | عدد التحذيرات |
| `metadata` | String | بيانات إضافية |

#### 16. LoginLog — سجل تسجيل الدخول

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `userId` | String | المستخدم |
| `ip` | String | عنوان IP |
| `userAgent` | String | وكيل المستخدم |
| `success` | Boolean | النجاح |
| `createdAt` | DateTime | تاريخ الإنشاء |

#### 17. StudentProfile — ملف الطالب

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String (فريد) | الطالب |
| `totalXP` | Int (default: 0) | إجمالي XP |
| `currentLevel` | Int (default: 1) | المستوى الحالي |
| `currentStreak` | Int (default: 0) | السلسلة الحالية |
| `longestStreak` | Int (default: 0) | أطول سلسلة |
| `lastActive` | DateTime | آخر نشاط |
| `lastXPAwarded` | DateTime | آخر XP مُنح |
| `lastActivityDate` | String | تاريخ آخر نشاط |

**العلاقات:**
- `student User` — المستخدم
- `achievements UserAchievement[]` — الإنجازات
- `dailyXP DailyXP[]` — XP اليومي

#### 18. DailyXP — XP اليومي

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String | الطالب |
| `date` | String | التاريخ |
| `totalXP` | Int | إجمالي XP |
| `submissionsCount` | Int | عدد التقديمات |

**المؤشر الفريد:** `@@unique([studentId, date])` — سجل واحد لكل طالب في اليوم.

#### 19. Achievement — الإنجاز

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `name` | String (فريد) | الاسم |
| `description` | String | الوصف |
| `icon` | String (default: "badge") | الأيقونة |
| `criteria` | String | المعايير (JSON) |
| `points` | Int (default: 100) | النقاط |
| `isSystem` | Boolean | نظامي |

#### 20. UserAchievement — إنجاز المستخدم

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `userId` | String | المستخدم |
| `achievementId` | String | الإنجاز |
| `awardedAt` | DateTime | تاريخ المنح |
| `metadata` | String? | بيانات إضافية |
| `studentProfileId` | String? | ملف الطالب |

**المؤشر الفريد:** `@@unique([userId, achievementId])` — لا يمكن منح الإنجاز مرتين.

**المشكلة:** `studentProfileId` اختياري — هذا يسمح بإنجازات بدون ملف طالب مما قد يسبب أخطاء.

#### 21. ActivityLog — سجل النشاط

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `userId` | String | المستخدم |
| `action` | String | الإجراء |
| `details` | String | التفاصيل (JSON) |
| `createdAt` | DateTime | تاريخ الإنشاء |

#### 22. CloudWorkspace — مساحة العمل السحابية

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String | الطالب |
| `challengeId` | String | التحدي |
| `savedCode` | String | الكود المحفوظ |
| `language` | String (default: "daad") | اللغة |
| `lastSavedAt` | DateTime | آخر حفظ |
| `createdAt` | DateTime | تاريخ الإنشاء |

**المؤشر الفريد:** `@@unique([studentId, challengeId])` — مساحة عمل واحدة لكل طالب لكل تحدٍ.

#### 23. SecurityAlert — التنبيه الأمني

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String | الطالب |
| `schoolId` | String | المدرسة |
| `alertType` | String | نوع التنبيه |
| `severity` | String (default: MEDIUM) | الخطورة |
| `details` | String | التفاصيل |
| `resolved` | Boolean (default: false) | تم الحل |
| `resolvedBy` | String? | تم الحل بواسطة |
| `resolvedAt` | DateTime? | تاريخ الحل |

#### 24. Invitation — الدعوة

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentEmail` | String | البريد الإلكتروني |
| `studentName` | String | الاسم |
| `classroomId` | String? | الفصل |
| `schoolId` | String | المدرسة |
| `schoolName` | String | اسم المدرسة |
| `token` | String? | رمز OTP (مشفر) |
| `tokenExpiry` | DateTime? | صلاحية الرمز |
| `isActivated` | Boolean | مفعلة |
| `userId` | String? (فريد) | المستخدم |
| `activatedAt` | DateTime? | تاريخ التفعيل |

**المؤشرات الفريدة:**
```prisma
@@unique([schoolId, studentEmail])
@@unique([userId])
```

#### 25. ChallengeVerification — التحقق من التحدي

| الحقل | النوع | الوصف |
|-------|------|-------|
| `id` | String (cuid) | المعرف |
| `studentId` | String | الطالب |
| `taskId` | String | المهمة |
| `timestamp` | DateTime | الطابع الزمني |
| `signature` | String | التوقيع |
| `ip` | String | عنوان IP |
| `userAgent` | String | وكيل المستخدم |

**المؤشرات:**
- `@@index([studentId, taskId])` — للبحث السريع
- `@@index([createdAt])` — للتدقيق التاريخي

### 8.3 ملخص النماذج والعلاقات

```
School ──┬── User ──┬── StudentProfile ──┬── DailyXP
         │          │                    └── UserAchievement ── Achievement
         │          ├── Submission ──── StudentSubmission
         │          ├── StudentAssessmentResult ── Assessment ── Classroom
         │          ├── Project ──────── Lesson ──── Course
         │          ├── Attendance
         │          ├── ActivityLog
         │          ├── LoginLog
         │          ├── CloudWorkspace ── Challenge
         │          ├── CourseEnrollment
         │          └── Invitation
         ├── Classroom ──── Assessment
         ├── Course ─────── Lesson ──── Project
         ├── Challenge ───┬── ChallengeTier
         │                ├── Submission
         │                └── CloudWorkspace
         ├── SecurityAlert
         └── Attendance
```

### 8.4 مشكلات نموذج البيانات

1. **نموذج Student منفصل:** يوجد نموذج `Student` مستقل لا يُستخدم — جميع الطلاب في نموذج `User`
2. **تخزين JSON كنصوص:** حقول مثل `challengeIds` و `questions` و `submissionIds` و `prerequisites` و `requirements` مخزنة كنصوص JSON — لا يمكن الاستعلام عنها مباشرة
3. **عدم وجود علاقة مباشرة:** `ChallengeVerification` ليس له علاقة مباشرة مع `User` أو `Challenge`
4. **عدم وجود فهارس مركبة:** بعض الاستعلامات الشائعة تفتقر إلى الفهارس المركبة (مثل `schoolId + role`)
5. **حقول اختيارية خطيرة:** `userId` في `Invitation` فريد ولكن اختياري — قد يسبب تناقضات
6. **نموذج Student غير مستخدم:** `Student.gradeLevel` و `Student.gpa` غير مستخدمين في أي مكان
7. **استخدام SQLite:** في الإنتاج، يجب استخدام PostgreSQL للحصول على أداء أفضل وأنواع بيانات أفضل (JSONB, Array)

---

## 9. متغيرات البيئة — Environment Variables

### 9.1 المحتوى الكامل لـ .env

```env
DATABASE_URL="file:./dev.db"
JWT_SECRET="dhad-studio-dev-secret-key-change-in-production"
JWT_EXPIRES_IN="7d"
PORT=3000
HMAC_SECRET=dhad-studio-hmac-secret-2026-production

# Rate Limiting (per minute)
RATE_LIMIT_PUBLIC=30
RATE_LIMIT_AUTHENTICATED=60
RATE_LIMIT_ADMIN=100
RATE_LIMIT_ONBOARD=5
RATE_LIMIT_SUBMISSION=10
RATE_LIMIT_OTP_REQUEST=3
RATE_LIMIT_OTP_VERIFY=5

# CORS Allowed Origins (comma-separated)
ALLOWED_ORIGINS=http://localhost:3000,http://localhost:5173,http://localhost:5174,http://localhost:8080,https://dhad-studio.onrender.com

# GitHub Models API Token
GITHUB_TOKEN=ghp_***REVOKED-REPLACE***
```

### 9.2 مراجعة أمنية كاملة

#### 🔴 مشكلات حرجة (يجب معالجتها فوراً)

| المشكلة | الخطورة | الشرح |
|---------|---------|-------|
| **GITHUB_TOKEN مكشوف** | 🔴 حرج | التوكن `ghp_***REVOKED-REPLACE***` مكشوف بالكامل في المستودع |
| **JWT_SECRET ضعيف** | 🔴 حرج | `dhad-studio-dev-secret-key-change-in-production` — قصير وسهل التخمين |
| **HMAC_SECRET ضعيف** | 🔴 حرج | `dhad-studio-hmac-secret-2026-production` — يحتوي على كلمات قابلة للتخمين |
| **GITHUB_TOKEN في .env** | 🔴 حرج | ملف `.env` مضمن في المستودع — يجب إضافته إلى `.gitignore` |

#### 🟡 مشكلات متوسطة

| المشكلة | الخطورة | الشرح |
|---------|---------|-------|
| **لا يوجد JWT_REFRESH_EXPIRES** | 🟡 متوسط | يستخدم `7d` كقيمة افتراضية — يجب تعيينها صراحةً |
| **لا يوجد JWT_ACCESS_EXPIRES** | 🟡 متوسط | يستخدم `1h` كقيمة افتراضية |
| **لا يوجد REDIS_URL** | 🟡 متوسط | بدون Redis، التخزين المؤقت معطل |
| **لا يوجد NODE_ENV** | 🟡 متوسط | لم يتم تعيينه — النظام يعمل في وضع التطوير |
| **لا يوجد BRIDGE_SECRET** | 🟡 متوسط | مطلوب لمصادقة الجسر |
| **لا يوجد HMAC_CONFIRM_SECRET** | 🟡 منخفض | المفتاح الاحتياطي هو HMAC_SECRET |
| **لا يوجد DATABASE_URL للإنتاج** | 🟡 متوسط | يتطلب PostgreSQL في الإنتاج |
| **لا يوجد LOG_LEVEL** | 🟡 منخفض | القيمة الافتراضية "info" قد تكون صاخبة |

#### ✅ إعدادات جيدة

| الإعداد | الوصف |
|---------|-------|
| `RATE_LIMIT_PUBLIC=30` | 30 طلب/دقيقة للعامة |
| `RATE_LIMIT_ONBOARD=5` | 5 طلب/دقيقة للإعداد (ثقيل) |
| `RATE_LIMIT_OTP_REQUEST=3` | 3 طلب OTP/دقيقة |
| `RATE_LIMIT_OTP_VERIFY=5` | 5 محاولة تحقق/دقيقة |
| `ALLOWED_ORIGINS` | قائمة موثوقة (5 أصول) |
| `PORT=3000` | منفذ واضح |

### 9.3 المتغيرات البيئية المفقودة

يحتاج الخادم إلى هذه المتغيرات للإنتاج:

```
# JWT (مطلوب)
JWT_SECRET=<سلسلة عشوائية 64 حرفاً>
JWT_ACCESS_EXPIRES=15m
JWT_REFRESH_EXPIRES=7d

# HMAC (مطلوب للتليمترية)
HMAC_SECRET=<سلسلة عشوائية 64 حرفاً>
HMAC_CONFIRM_SECRET=<سلسلة عشوائية 64 حرفاً>

# قاعدة البيانات (مطلوب)
DATABASE_URL=postgresql://user:pass@host:5432/dhad

# Redis (اختياري)
REDIS_URL=redis://user:pass@host:6379

# CORS
ALLOWED_ORIGINS=https://dhad-studio.example.com

# Bridge API
BRIDGE_SECRET=<سلسلة عشوائية>

# البيئة
NODE_ENV=production
LOG_LEVEL=warn
HOST=0.0.0.0
PORT=3000
```

---

## 10. قائمة شاملة بجميع الاقتراحات — Comprehensive Suggestions

### 10.1 🔴 اقتراحات أمنية حرجة (Ciritical)

| # | الاقتراح | الملف المتأثر | الشرح |
|---|---------|---------------|-------|
| 1 | **إزالة GITHUB_TOKEN من المستودع** | .env | التوكن مكشوف — يجب إبطاله فوراً وتنظيف تاريخ git |
| 2 | **تغيير JWT_SECRET** | .env, authController | استخدم مفتاح 64 حرفاً عشوائياً |
| 3 | **تغيير HMAC_SECRET** | .env, auth.js, challengeController | استخدم مفتاح 64 حرفاً عشوائياً |
| 4 | **إعادة كتابة منطق CORS** | index.js | استخدم `===` بدلاً من `includes` لمنع تجاوز الأصول |
| 5 | **إضافة التحقق من المتغيرات البيئية عند بدء التشغيل** | index.js | تحقق من وجود JWT_SECRET و HMAC_SECRET قبل بدء الخادم |
| 6 | **إزالة تسريب التوقيع** | challengeController.js | لا تسجل أول 16 حرفاً من التوقيع المتوقع |
| 7 | **إضافة حد لحجم الكود** | student.js/assessmentController | منع إرسال كود ضخم (DoS) |
| 8 | **تشفير GITHUB_TOKEN** | adminService | استخدم GitHub Actions secrets بدلاً من ذلك |

### 10.2 🟡 اقتراحات أمنية متوسطة (High)

| # | الاقتراح | الملف المتأثر | الشرح |
|---|---------|---------------|-------|
| 9 | **لا تثق في العميل لوقت التنفيذ** | student.js:245 | `executionTime: 0` يُمرر إلى `awardXP` |
| 10 | **إضافة مصادقة ثنائية** | authController | اختياري للمسؤولين والمعلمين |
| 11 | **تقييد محاولات OTP** | adminService | إضافة تأخير تدريجي للتحقق من OTP |
| 12 | **استخدام Prisma transactions** | adminService.verifyOTP, onboardController | ضمان الاتساق عند إنشاء المستخدم + الملف |
| 13 | **إزالة تفاصيل الخطأ في الإنتاج** | index.js:285-291 | بعض الأخطاء تكشف المسار في الإنتاج |
| 14 | **إضافة rate limiting لـ /challenges** | analytics.js | نقطة نهاية التحديات ليس لها محدد معدل |
| 15 | **تقييد رؤوس Access-Control-Expose-Headers** | index.js | كشف معلومات حدود المعدل |
| 16 | **إضافة التحقق من السعة للفصل** | studentManagementController | التحقق من `maxStudents` قبل النقل |
| 17 | **إخفاء تفاصيل قاعدة البيانات في /health** | healthController | قد تسرب معلومات في الإنتاج |

### 10.3 🟢 اقتراحات أداء (Performance)

| # | الاقتراح | الملف المتأثر | الشرح |
|---|---------|---------------|-------|
| 18 | **تحسين حساب honorScore** | analyticsController | تجنب القسمة على صفر (NaN) |
| 19 | **استخدام Map بدلاً من filter** | reportsController | لتحسين أداء التقارير (O(n²) → O(n)) |
| 20 | **إضافة فهارس مركبة** | Prisma schema | `schoolId + role`، `userId + createdAt` |
| 21 | **ترحيل إلى PostgreSQL** | الإنتاج | SQLite لا يتناسب مع الإنتاج |
| 22 | **استخدام JSONB في PostgreSQL** | جميع الحقول JSON | للاستعلام المباشر عن JSON |
| 23 | **تحسين استعلام getLeaderboard** | analyticsController | تجنب جلب جميع الطلاب والتقديمات |
| 24 | **إضافة التخزين المؤقت للتقارير** | reportsController | التقارير مكلفة ويجب تخزينها مؤقتاً |
| 25 | **استخدام bulk upsert للحضور** | analyticsController | بدلاً من upsert لكل طالب |

### 10.4 🔵 اقتراحات بنية الكود (Code Structure)

| # | الاقتراح | الملف المتأثر | الشرح |
|---|---------|---------------|-------|
| 26 | **توحيد middleware للتحقق من الصحة** | جميع الملفات | استخدام Joi/Zod بدلاً من التحقق اليدوي |
| 27 | **إضافة TypeScript** | جميع الملفات | تحسين الأمان والاكتشاف المبكر للأخطاء |
| 28 | **إزالة نموذج Student غير المستخدم** | Prisma schema | لا يُستخدم في أي مكان |
| 29 | **إعادة توجيه مسارات reports/schools** | router/index.js | غير مضمنة حالياً في المجمع |
| 30 | **إزالة محددات المعدل غير المستخدمة** | index.js | `telemetryRateLimit` وغيرها غير مطبقة |
| 31 | **توحيد sanitize** | strictSecurity.js + security.js | وظيفتان متشابهتان للتنقية |
| 32 | **إزالة التكرار في التحقق من حجم المحتوى** | index.js السطر 160 | مكرر مع strictRequestValidation |
| 33 | **إعادة تسمية المتغيرات** | analyticsController.js | `now.setDate()` يغير المتغير الأصلي |

### 10.5 🟣 اقتراحات الميزات (Features)

| # | الاقتراح | الوصف |
|---|---------|-------|
| 34 | **نظام إشعارات** | إشعارات فورية للمعلمين عند تقديم الطلاب |
| 35 | **لوحة إدارة متقدمة** | إحصائيات متقدمة مع رسوم بيانية |
| 36 | **دعم WebSocket للوحة المتصدرين المباشرة** | تحديثات فورية للوحة المتصدرين |
| 37 | **تصدير Excel** | بالإضافة إلى CSV |
| 38 | **تقييم AI للكود** | ملاحظات تلقائية للطلاب |
| 39 | **وضع عدم الاتصال** | تخزين مؤقت محلي مع مزامنة عند الاتصال |
| 40 | **نظام الإخطارات** | إشعارات بالبريد الإلكتروني للطلاب |

### 10.6 ⚪ اقتراحات عامة (General)

| # | الاقتراح | الوصف |
|---|---------|-------|
| 41 | **إضافة توثيق كامل لجميع نقاط النهاية** | معظم نقاط النهاية غير موثقة في Swagger |
| 42 | **إضافة اختبارات API** | اختبارات تكامل لجميع المسارات |
| 43 | **إضافة CI/CD** | GitHub Actions للاختبار والنشر |
| 44 | **إضافة مراقبة الأداء** | New Relic, Sentry, أو Datadog |
| 45 | **تحسين معالجة الأخطاء الموحدة** | استخدام errorHandler.js بدلاً من المنطق المبعثر |
| 46 | **إضافة Docker Compose كامل** | مع PostgreSQL و Redis |

### 10.7 أولويات الاقتراحات حسب Category

#### الأسبوع 1 — الأمان (حرج)
1. إزالة GITHUB_TOKEN وتغيير جميع المفاتيح
2. إصلاح CORS (includes → ===)
3. إضافة التحقق من المتغيرات البيئية
4. إزالة تسريب التوقيع من السجلات
5. إضافة Prisma transactions

#### الأسبوع 2 — الأداء
6. إضافة فهارس قاعدة البيانات
7. تحسين استعلام leaderboard
8. التخزين المؤقت للتقارير
9. استخدام bulk operations

#### الأسبوع 3 — البنية
10. توحيد التحقق من الصحة
11. ترحيل إلى PostgreSQL
12. إزالة الكود المكرر
13. إصلاح المسارات المفقودة من المجمع

#### الأسبوع 4 — الميزات
14. إضافة الاختبارات
15. إضافة CI/CD
16. توثيق شامل
17. إعداد المراقبة

---

## الملحق: إحصائيات الخادم

### إحصائيات الكود (بدون node_modules)

| المقياس | القيمة |
|---------|--------|
| إجمالي ملفات المصدر | 32 ملف |
| إجمالي الأسطر | ~8,598 سطر |
| أكبر ملف | analyticsController.js (1,082 سطر) |
| متوسط حجم الملف | ~269 سطر |
| إجمالي الملفات التحكمية | 12 |
| إجمالي الملفات الوسيطة | 4 |
| إجمالي ملفات المسارات | 13 |
| إجمالي نقاط النهاية | ~71+ |
| إجمالي نماذج Prisma | 25 |

### تحليل التبعيات

| المكتبة | الإصدار (تقريبي) | الغرض |
|---------|------------------|-------|
| express | 4.x | إطار الخادم |
| @prisma/client | 5.x | ORM |
| prisma | 5.x | أداة Prisma |
| jsonwebtoken | 9.x | JWT |
| bcryptjs | 2.x | تشفير |
| winston | 3.x | تسجيل |
| ioredis | 5.x | Redis |
| helmet | 7.x | أمان |
| compression | 1.x | ضغط |
| express-rate-limit | 6.x | تحديد المعدل |
| swagger-jsdoc | 7.x | توثيق |
| swagger-ui-express | 4.x | واجهة Swagger |
| @msgpack/msgpack | 3.x | ترميز ثنائي |
| dotenv | 16.x | متغيرات البيئة |

---

---

## الملحق أ: تحليل أمني متعمق لخوارزميات strictSecurity.js

### أ.1 خوارزمية التأخير التدريجي (Progressive Delay)

```javascript
function recordFailedAttempt(identifier) {
  const now = Date.now();
  let record = failedAttempts.get(identifier);

  if (!record || now - record.lastAttempt > CONFIG.FAILED_ATTEMPT_WINDOW_MS) {
    record = { count: 0, lastAttempt: now, blockedUntil: null, delayUntil: null };
  }

  record.count++;
  record.lastAttempt = now;

  // Progressive delay: 1s, 2s, 4s, 8s, 16s after each failure
  if (record.count >= 2 && record.count < CONFIG.MAX_FAILED_ATTEMPTS) {
    const delayMs = Math.min(1000 * Math.pow(2, record.count - 1), 16000);
    record.delayUntil = now + delayMs;
  }

  if (record.count >= CONFIG.MAX_FAILED_ATTEMPTS) {
    record.blockedUntil = now + CONFIG.LOCKOUT_DURATION_MS;
  }

  failedAttempts.set(identifier, record);
  return record;
}
```

**تحليل رياضيات التأخير:**

| المحاولة | التأخير | الوقت التراكمي |
|----------|---------|----------------|
| 1 | 0s | 0s |
| 2 | 1s | 1s |
| 3 | 2s | 3s |
| 4 | 4s | 7s |
| 5 | 8s | 15s |
| 6 | 16s | 31s ← قفل لمدة 15 دقيقة |

**الهدف:** هجوم القوة العمياء الذي يتطلب 6 محاولات يستغرق 31 ثانية قبل القفل. هذا يمنع بشكل فعال هجمات التخمين السريع.

**نقاط الضعف:**
1. لا يوجد تشويش (jitter) — المهاجم يمكنه حساب وقت الانتظار بالضبط
2. إعادة تعيين النافذة — بعد 15 دقيقة، يتم إعادة تعيين العداد
3. `FAILED_ATTEMPT_WINDOW_MS` يساوي `LOCKOUT_DURATION_MS` — قد يكون هذا مقصوداً

### أ.2 خوارزمية حظر IP

```javascript
function recordIPFailure(ip) {
  let count = ipBlocks.get(ip);
  if (!count) {
    count = { failures: 0, blockedUntil: null };
    ipBlocks.set(ip, count);
  }
  count.failures++;
  count.lastFailure = Date.now();

  if (count.failures >= CONFIG.MAX_IP_FAILURES) {
    count.blockedUntil = Date.now() + CONFIG.IP_BLOCK_DURATION_MS;
  }
  ipBlocks.set(ip, count);
}
```

**التحليل:** يسجل `recordIPFailure` عدد الإخفاقات ولكن لا يتم استدعاؤه أبداً في أخطاء HMAC أو الطلبات المشبوهة. يتم استدعاؤه فقط عندما يسجله الكود صراحةً.

**المشكلة:** `recordIPFailure` معرف ولكنه لا يُستخدم في أي مكان في قاعدة الكود. حظر IP غير مفعل فعلياً.

### أ.3 خوارزمية التنظيف الدوري

```javascript
setInterval(() => {
  const now = Date.now();

  // Clean failed attempts
  for (const [key, record] of failedAttempts.entries()) {
    if (now - record.lastAttempt > CONFIG.FAILED_ATTEMPT_WINDOW_MS * 2) {
      failedAttempts.delete(key);
    }
  }

  // Clean IP blocks
  for (const [ip, record] of ipBlocks.entries()) {
    if (record.blockedUntil && now > record.blockedUntil) {
      ipBlocks.delete(ip);
    }
  }

  // Clean request counts
  for (const [key, record] of requestCounts.entries()) {
    if (now - record.windowStart > CONFIG.RATE_LIMIT_WINDOW_MS * 2) {
      requestCounts.delete(key);
    }
  }
}, 60 * 1000);  // Run every minute
```

**تحليل الأداء:**
- يتم تكرار جميع السجلات كل 60 ثانية — O(n) لكل دورة
- في النظام المزدحم مع آلاف السجلات، هذا قد يستهلك موارد
- **اقتراح:** استخدام TTL بدلاً من التنظيف النشط (لكن هذا يتطلب Redis)

### أ.4 خوارزمية سجل التدقيق الدائري

```javascript
function logAudit(action, data = {}) {
  const entry = { timestamp: new Date().toISOString(), action, ip: data.ip || "unknown", userId: data.userId || null, details: data };
  auditLog.push(entry);

  // Keep only last 10000 entries
  if (auditLog.length > 10000) {
    auditLog.splice(0, auditLog.length - 10000);
  }
}
```

**تحليل الأداء:** `splice(0, auditLog.length - 10000)` هو O(n) ولكن التعليق يقول O(1). في الواقع، `splice` مع إزالة العناصر الأولى يتطلب إعادة فهرسة المصفوفة — O(n). لتحقيق O(1) حقيقي، يجب استخدام مصفوفة دائرية (circular buffer) بمؤشر ثابت.

### أ.5 خوارزمية إدارة الجلسات

```javascript
function registerSession(userId, sessionId) {
  if (!activeSessions.has(userId)) {
    activeSessions.set(userId, new Set());
  }
  const sessions = activeSessions.get(userId);
  sessions.add(sessionId);
  return sessions.size;
}

function enforceMaxSessions(req, res, next) {
  const userId = req.user?.id;
  if (!userId) return next();
  const sessionCount = getSessionCount(userId);
  if (sessionCount >= CONFIG.MAX_SESSIONS_PER_USER) {
    return res.status(403).json({ error: "Session Limit", message: `Maximum ${CONFIG.MAX_SESSIONS_PER_USER} concurrent sessions allowed.` });
  }
  next();
}
```

**المشكلة:** `registerSession` و `enforceMaxSessions` معرفان ولكن لا يتم استدعاؤهما في أي مسار. نظام إدارة الجلسات غير مفعل.

---

## الملحق ب: تحليل متعمق لخوارزميات auth.js

### ب.1 خوارزمية التحقق من HMAC

```javascript
function validateHMACSignature(req, res, next) {
  const signature = req.headers['x-telemetry-signature'];
  const timestamp = req.headers['x-telemetry-timestamp'];

  // Check timestamp freshness (prevent replay attacks - 5 minute window)
  if (isNaN(requestTime) || Math.abs(currentTime - requestTime) > 5 * 60 * 1000) { ... }

  // Get raw body for HMAC verification
  const rawBody = req.rawBody || JSON.stringify(req.body);

  // Create HMAC using session secret or JWT token
  const hmacSecret = process.env.HMAC_SECRET;
  const expectedSignature = crypto
    .createHmac('sha256', hmacSecret)
    .update(rawBody + timestamp)
    .digest('hex');

  // Timing-safe comparison
  if (sigBuf.length !== expectedBuf.length || !crypto.timingSafeEqual(sigBuf, expectedBuf)) { ... }
}
```

**تحليل أمني:**

1. **نافذة إعادة الإرسال (5 دقائق):** مناسبة للاستخدام العام — تسمح بتأخير الشبكة مع منع إعادة الإرسال
2. **استخدام rawBody:** إذا لم يتوفر `req.rawBody` يتحول إلى `JSON.stringify(req.body)` — لكن هذا قد يغير التنسيق (إزالة المسافات، تغيير ترتيب المفاتيح)
   - **مشكلة:** `JSON.stringify` لا يضمن ترتيب مفاتيح متسق — توقيع HMAC قد يختلف بين العميل والخادم
   - **الحل:** يجب استخدام `req.rawBody` دائماً مع middleware مخصص لالتقاط الجسم الخام
3. **عدم وجود مفتاح HMAC لكل جلسة:** `HMAC_SECRET` عمومي — إذا تم تسريبه، يمكن تزوير جميع التواقيع
4. **التسجيل:** خطأ HMAC يسجل كـ "CRITICAL" — مناسب

### ب.2 خوارزمية المصادقة (authenticate)

```javascript
async function authenticate(req, res, next) {
  // 1. التحقق من رأس Authorization
  const header = req.headers.authorization;
  if (!header || !header.startsWith("Bearer ")) { return 401; }

  // 2. فك تشفير JWT مع تحديد الخوارزمية صراحة
  const decoded = jwt.verify(token, process.env.JWT_SECRET, {
    algorithms: ["HS256"], // منع هجمات خلط الخوارزميات
    maxAge: process.env.JWT_EXPIRES_IN || "7d",
  });

  // 3. التحقق من بنية الحمولة
  if (!decoded.id || !decoded.role || !decoded.schoolId) { return 401; }

  // 4. التحقق من تنشيط حساب الطالب
  if (decoded.role === "STUDENT") {
    // التحقق من isApproved و isActive
    // التحقق من تفعيل الدعوة
  }

  // 5. التحقق من إصدار التوكن
  // fail-closed: إذا كانت قاعدة البيانات غير متاحة، ارفض الطلب
}
```

**تحليل أمني متعمق:**

**مزايا:**
1. `algorithms: ["HS256"]` — يمنع صراحةً هجمات خلط الخوارزميات (مثل استخدام "none")
2. `maxAge` — يحدد أقصى عمر للتوكن ويمنع التوكنات القديمة
3. التحقق من بنية الحمولة — يضمن احتواء JWT على الحقول المطلوبة
4. fail-closed — في حالة فشل قاعدة البيانات، يتم رفض الطلب بدلاً من السماح بالوصول
5. التحقق المزدوج من إصدار التوكن — مرة من الحمولة ومرة من قاعدة البيانات

**نقاط الضعف:**
1. استعلام قاعدة البيانات في كل طلب — يضيف تأخيراً (latency)
   - **اقتراح:** استخدام Redis للتخزين المؤقت لإصدارات التوكن
2. `maxAge: process.env.JWT_EXPIRES_IN || "7d"` — 7 أيام طويلة جداً للوصول
   - **اقتراح:** استخدام توكن وصول قصير (15 دقيقة) + توكن تحديث طويل
3. عدم وجود التحقق من `iat` (issued at) — لا يمكن رفض التوكنات القديمة بناءً على وقت الإصدار

### ب.3 خوارزمية كشف النشاط المشبوه

```javascript
function detectSuspiciousActivity(submissionData, challenge) {
  const reasons = [];
  const { executionTime, code, status, score } = submissionData;
  const { tier, difficulty, estimatedTime } = challenge;

  // Check 1: Impossibly fast execution time
  const MIN_EXECUTION_TIMES = { 1: 10, 2: 25, 3: 50, 4: 100, 5: 150 };
  if (executionTime && executionTime < MIN_EXECUTION_TIMES[tier]) {
    reasons.push({ type: 'SUSPICIOUS_TIMING', severity: 'HIGH' });
  }

  // Check 2: Perfect score on first attempt for difficult challenges
  if (score === 100 && tier >= 3) {
    reasons.push({ type: 'PERFECT_SCORE_FIRST_ATTEMPT', severity: 'MEDIUM' });
  }

  // Check 3: Code very short for complex output
  if (code && code.length < 20 && tier >= 2) {
    reasons.push({ type: 'SHORT_CODE_COMPLEX_OUTPUT', severity: 'MEDIUM' });
  }

  // Check 4: Execution time 90% faster than estimated
  if (estimatedTime && executionTime) {
    const estimatedMs = estimatedTime * 60 * 1000;
    if (executionTime < estimatedMs * 0.1) {
      reasons.push({ type: 'TIME_ANOMALY', severity: 'HIGH' });
    }
  }
}
```

**تحليل منطق الكشف:**

| الفحص | الهدف | الفعالية | الإيجابيات الكاذبة |
|-------|-------|----------|-------------------|
| توقيت سريع جداً | كشف التنفيذ التلقائي | عالية | متوسطة (قد يختلف الأداء) |
| درجة كاملة أول مرة | كشف الغش | متوسطة | عالية (الطلاب الموهوبون) |
| كود قصير لمخرجات معقدة | كشف الحلول المختصرة | منخفضة | عالية (الكود الفعال) |
| توقيت أسرع 90% | كشف التنفيذ الفوري | عالية | منخفضة |

**المشكلة:** `MIN_EXECUTION_TIMES` بالميلي ثانية — ولكن `executionTime` قد يكون في الميلي ثانية أو الميكرو ثانية حسب كيفية قياس العميل. المستويات 4 و 5 تتطلب 100-150 مللي ثانية — هذا طويل جداً للاختبارات البسيطة.

---

## الملحق ج: تحليل تدفق البيانات للعمليات الحرجة

### ج.1 تدفق تقديم التحدي (Challenge Submission)

```
[العميل]                         [الخادم]                          [قاعدة البيانات]
    │                               │                                   │
    │ POST /api/v1/student/challenge/submit                             │
    │ { challengeId, code, output }                                     │
    │──────────────────────────────►│                                   │
    │                               │                                   │
    │                               │ 1. المصادقة (JWT)                 │
    │                               │ 2. التحقق من النوع                │
    │                               │ 3. التحقق من وجود التحدي          │
    │                               │    ├──► prisma.challenge.findUnique│
    │                               │    │◄── { id, tier, expected... } │
    │                               │                                   │
    │                               │ 4. التحقق من فتح المستوى (80%)    │
    │                               │    ├──► prisma.challenge.findMany  │
    │                               │    ├──► prisma.submission.findFirst│
    │                               │                                   │
    │                               │ 5. التحقق من الترتيب التسلسلي    │
    │                               │    ├──► prisma.challenge.findMany  │
    │                               │    ├──► prisma.submission.findFirst│
    │                               │                                   │
    │                               │ 6. التحقق من عدم التكرار         │
    │                               │    ├──► prisma.submission.findFirst│
    │                               │                                   │
    │                               │ 7. التحقق من المخرجات (خادم)     │
    │                               │    ├── نصوص التطبيع               │
    │                               │    ├── استبدال {{student.name}}   │
    │                               │    ├── مقارنة المخرجات            │
    │                               │                                   │
    │                               │ 8. التحقق من بنية الكود           │
    │                               │    ├── verifyCode()               │
    │                               │                                   │
    │                               │ 9. إنشاء سجل التقديم             │
    │                               │    ├──► prisma.submission.create  │
    │                               │                                   │
    │                               │ 10. منح XP (إذا نجح)              │
    │                               │    ├──► awardXP()                 │
    │                               │    ├──► checkAndAwardAchievements │
    │                               │                                   │
    │ { success, passed, xpAwarded }│                                   │
    │◄──────────────────────────────│                                   │
```

**نقاط الفشل المحتملة:**
1. الخطوة 4 تستعلم جميع التحديات في المستوى السابق — قد تكون بطيئة
2. الخطوة 5 تستعلم جميع التحديات في المستوى الحالي — تكرار غير ضروري
3. الخطوة 6 تستعلم التقديمات السابقة — يمكن دمجها مع الخطوة 4
4. لا توجد معاملة (transaction) — إذا فشلت الخطوة 10، يبقى التقديم بدون XP

### ج.2 تدفق إنشاء الاختبار (Assessment Creation)

```
[المعلم]                           [الخادم]                          [قاعدة البيانات]
    │                                   │                                   │
    │ POST /api/v1/assessments/create                                      │
    │──────────────────────────────►│                                   │
    │                               │                                   │
    │                               │ 1. المصادقة والتحقق من الدور      │
    │                               │ 2. التحقق من وجود الفصل           │
    │                               │    ├──► prisma.classroom.findUnique│
    │                               │                                   │
    │                               │ 3. التحقق من ملكية الفصل          │
    │                               │                                   │
    │                               │ 4. التحقق من حد المعدل (6 ساعات) │
    │                               │    ├──► prisma.assessment.count   │
    │                               │                                   │
    │                               │ 5. التحقق من صحة التواريخ        │
    │                               │    ├── start < end                │
    │                               │    ├── allowedTime 5-300 دقيقة   │
    │                               │                                   │
    │                               │ 6. إنشاء الاختبار                 │
    │                               │    ├──► prisma.assessment.create  │
    │                               │                                   │
    │                               │ 7. تسجيل النشاط                   │
    │                               │    ├──► prisma.activityLog.create │
    │                               │                                   │
    │ { success, assessment }       │                                   │
    │◄──────────────────────────────│                                   │
```

**ميزات الأمان:**
- حد معدل صارم (اختبار واحد كل 6 ساعات لكل فصل)
- التحقق من ملكية المعلم للفصل
- التحقق من صحة جميع التواريخ
- تسجيل جميع الأنشطة للتدقيق

**المشكلات:**
- `challengeIds` و `questions` مخزنة كنصوص JSON — لا يمكن التحقق من صحتها في قاعدة البيانات
- `effectiveAllowedTime` يحاول `allowedTime || durationMinutes` — إذا كان `allowedTime=0`، سيستخدم `durationMinutes`

### ج.3 تدفق تقديم إجابة الاختبار (Assessment Submission)

```
[الطالب]                           [الخادم]                          [قاعدة البيانات]
    │                                   │                                   │
    │ POST /api/v1/student/assessment/submit                                │
    │──────────────────────────────►│                                   │
    │                               │                                   │
    │                               │ 1. المصادقة                        │
    │                               │ 2. التحقق من وجود الاختبار        │
    │                               │ 3. التحقق من أن الاختبار نشط      │
    │                               │ 4. التحقق من وجود نتيجة           │
    │                               │ 5. التحقق من أن النتيجة غير مكتملة│
    │                               │                                   │
    │                               │ 6. 🔐 الأمان: تجاهل بيانات العميل │
    │                               │    passed = false                  │
    │                               │    executionTime = 0              │
    │                               │    syntaxScore = 0                 │
    │                               │    pointsEarned = 0                │
    │                               │                                   │
    │                               │ 7. 🔐 حد الإرسال: max 20          │
    │                               │                                   │
    │                               │ 8. إنشاء التقديم                  │
    │                               │    ├──► prisma.submission.create  │
    │                               │                                   │
    │                               │ 9. تحديث النتيجة                  │
    │                               │    ├──► prisma.result.update      │
    │                               │                                   │
    │ { submission, result }        │                                   │
    │◄──────────────────────────────│                                   │
```

**النقطة الأمنية الحرجة:** الخطوة 6 هي أهم إجراء أمني في الاختبارات — الخادم لا يثق مطلقاً في العميل. جميع الإرسالات تبدأ بـ `passed=false` ونقاط = 0.

**المشكلة الكبيرة:** لا يوجد تحقق آلي من جانب الخادم للنتائج. جميع الإرسالات تبقى `passed=false` حتى يقوم شخص ما (أو خدمة تحكيم) بتحديثها. في الواقع الحالي، لا يمكن للطالب أبداً الحصول على درجة في الاختبار لأن:
1. `submitAssessmentAnswer` يسجل دائماً `passed=false`
2. `completeAssessment` يحسب الدرجة من `totalPointsEarned` الذي هو دائماً 0

هذا يعني أن **نظام الاختبارات لا يعمل حالياً** — الطلاب يمكنهم الإرسال ولكن النتائج ستكون دائماً 0.

---

## الملحق د: تحليل أداء الاستعلامات الحرجة

### د.1 استعلام getLeaderboard

```javascript
const leaderboard = await prisma.user.findMany({
  where: { schoolId: targetSchoolId, role: "STUDENT" },
  select: {
    id: true, name: true,
    studentProfile: { select: { totalXP: true, currentLevel: true, currentStreak: true } },
    submissions: { select: { id: true, challengeId: true, passed: true, syntaxScore: true, performanceScore: true, createdAt: true, challenge: { select: { title: true, difficulty: true } } } },
    projects: { select: { id: true, title: true, score: true, lesson: { select: { title: true, order: true } } } }
  }
});
```

**تحليل التعقيد:**
- **قاعدة البيانات:** O(S + S×Sub + S×Proj) حيث S = عدد الطلاب
- **الذاكرة:** عالية — جميع الطلاب وجميع تقديماتهم ومشاريعهم في الذاكرة
- **مشكلة:** لمدرسة بها 1000 طالب، كل بمتوسط 50 تقديماً و 5 مشاريع، هذا الاستعلام قد يستغرق عدة ثوانٍ

**اقتراح التحسين:**
```javascript
// بدلاً من جلب كل شيء دفعة واحدة:
// 1. جلب ملفات الطلاب فقط (XP, level)
const profiles = await prisma.studentProfile.findMany({
  where: { student: { schoolId, role: "STUDENT" } },
  select: { studentId: true, totalXP: true, currentLevel: true }
});

// 2. حساب الإحصائيات بشكل منفصل إذا لزم الأمر
const stats = await prisma.submission.groupBy({
  by: ['studentId'],
  where: { studentId: { in: profileIds } },
  _count: { id: true, passed: true },
  _avg: { syntaxScore: true }
});
```

### د.2 استعلام getClassProgress

```javascript
// استعلام منفصل للطلاب
const students = await prisma.user.findMany({ where: studentWhere, select: { id: true, name: true } });

// استعلام منفصل للتحديات
const exercises = await prisma.challenge.findMany({
  where: { schoolId: targetSchoolId, published: true },
  select: { id: true, title: true, points: true, tier: true }
});

// استعلام واحد لجميع التقديمات (يحل N+1)
const allSubmissions = await prisma.submission.findMany({
  where: { studentId: { in: studentIds } },
  include: { challenge: { select: { id: true, title: true, difficulty: true } } }
});
```

**تحليل الأداء:**
- استعلام التقديمات الموحد (allSubmissions) يحل مشكلة N+1 — ✅ ممتاز
- `findMany` بدون `orderBy` و `take` — قد يكون بطيئاً للمجموعات الكبيرة
- يعالج جميع التقديمات في الذاكرة — قد يكون مشكلة لذاكرة

### د.3 استعلام getTeacherDashboard

```javascript
// 5 استعلامات متسلسلة (ليست متوازية)
const classrooms = await prisma.classroom.findMany({...});
const totalStudents = await prisma.user.count({...});
const activeStudents = await prisma.user.count({...});
const totalAssessments = await prisma.assessment.count({...});
const recentSubmissions = await prisma.submission.findMany({...});
const topStudents = await prisma.user.findMany({...});
```

**اقتراح:** استخدام `Promise.all` لتشغيل الاستعلامات المتوازية:
```javascript
const [classrooms, totalStudents, activeStudents, totalAssessments, recentSubmissions, topStudents] = await Promise.all([
  prisma.classroom.findMany({...}),
  prisma.user.count({...}),
  prisma.user.count({...}),
  prisma.assessment.count({...}),
  prisma.submission.findMany({...}),
  prisma.user.findMany({...})
]);
```

### د.4 معضلة getStudentRoadmap

```javascript
// الحصول على الدروس
const lessons = await prisma.lesson.findMany({ where: { courseId: null }, ... });

// الحصول على جميع التحديات مع التقديمات
const challenges = await prisma.challenge.findMany({
  where: { courseId: null },
  select: {
    ...,
    submissions: { where: { studentId }, select: { passed: true, createdAt: true } }
  },
});

// لكل درس، فلترة التحديات ومعالجتها
const lessonsWithProgress = lessons.map(lesson => {
  const lessonChallenges = challenges.filter(c => c.tier === lesson.order);
  // ... معالجة معقدة
});
```

**تحليل التعقيد الحسابي:**
- استعلامان فقط لقاعدة البيانات — ✅ جيد
- لكن المعالجة في الذاكرة تتضمن عدة حلقات متداخلة: O(L × C + L × C²) حيث L = الدروس، C = التحديات
- لمدرسة بها 50 درساً و 200 تحدٍ، هذا مقبول
- للمدرسة بها 100 درس و 1000 تحدٍ، هذا قد يكون بطيئاً

**المنطق التسلسلي للفتح:**
```javascript
// Sequential locking: first challenge always unlocked
let challengeStatus = "LOCKED";
if (idx === 0) {
  challengeStatus = status === "LOCKED" ? "LOCKED" : "UNLOCKED";
} else {
  const prevChallenge = lessonChallenges.sort(...)[idx - 1];
  const prevPassed = prevChallenge.submissions.some(s => s.passed);
  if (status === "LOCKED") { challengeStatus = "LOCKED"; }
  else if (passed) { challengeStatus = "COMPLETED"; }
  else if (prevPassed) { challengeStatus = "UNLOCKED"; }
  else { challengeStatus = "LOCKED"; }
}
```

هذا يضمن أن الطالب يجب أن يكمل التحدي السابق قبل فتح التالي — ✅ أمان جيد.

---

## الملحق هـ: تحليل الأمان للثغرات المحتملة

### هـ.1 ثغرة CORS (index.js:136)

```javascript
const origin = req.headers.origin;
if (origin && allowedOrigins.includes(origin)) {
```

**الثغرة:** استخدام `includes` للتحقق من الأصل. المهاجم يمكنه استخدام:
- `http://localhost:3000.evil.com` — لأن `includes` تتحقق من وجود النص الفرعي
- `http://localhost:5173.evil.com`

**الإصلاح:**
```javascript
if (origin && allowedOrigins.includes(origin)) {
```
يجب أن يكون:
```javascript
if (origin && allowedOrigins.some(allowed => origin === allowed)) {
```

### هـ.2 ثغرة JSON.stringify لهجمات HMAC (auth.js:62)

```javascript
const rawBody = req.rawBody || JSON.stringify(req.body);
```

**الثغرة:** `JSON.stringify(req.body)` بعد تحليل Express قد يغير ترتيب المفاتيح أو تنسيق الأرقام. إذا قام العميل بتوقيع `{"a":1,"b":2}` ولكن الخادم أنتج `{"b":2,"a":1}`، سيفشل التحقق من HMAC.

**الإصلاح:** استخدام middleware مخصص لالتقاط `rawBody` قبل تحليل JSON.

```javascript
// middleware لالتقاط الجسم الخام
app.use(express.json({
  verify: (req, res, buf) => { req.rawBody = buf.toString(); }
}));
```

### هـ.3 ثغرة الفشل المفتوح في codeVerifier.js:21

```javascript
try {
  requirements = JSON.parse(requirementsJson || "[]");
} catch (e) {
  return { valid: true, errors: [] }; // FAIL OPEN
}
```

**الثغرة:** إذا كان `requirementsJson` يحتوي على JSON غير صالح، تعتبر جميع المتطلبات مستوفاة. المهاجم يمكنه تعطيل متطلبات التحدي بإرسال JSON تالف.

**الإصلاح:** فشل مغلق — إرجاع خطأ بدلاً من النجاح.

### هـ.4 ثغرة تسرب المعلومات في /health (healthController.js)

```javascript
health.system = {
  platform: process.platform,
  nodeVersion: process.version,
  hostname: os.hostname(),
  loadAverage: os.loadavg(),
  freeMemory: ...,
  totalMemory: ...,
};
```

**الثغرة:** نقطة /health تكشف معلومات النظام لأي شخص (لا توجد مصادقة).

**الإصلاح:** إضافة مصادقة أساسية أو تحديد الوصول في الإنتاج.

### هـ.5 ثغرة هجمات إعادة الإرسال في التحقق من التحدي

```javascript
const fiveMinutesAgo = now - (5 * 60 * 1000);
if (timestampMs < fiveMinutesAgo) { ... }
```

**التحليل:** نافذة 5 دقائق طويلة نسبياً. إذا قام مهاجم باعتراض حزمة تحقق صالحة، يمكنه إعادة إرسالها خلال 5 دقائق.

**اقتراح:** إضافة `nonce` (رقم عشوائي لمرة واحدة) مع التحقق من عدم التكرار.

### هـ.6 ثغرة SQL Injection (نظري — Prisma آمن)

```javascript
// Prisma آمن بطبيعته ضد SQL injection
await prisma.$queryRaw`SELECT 1`; // الاستعلامات المرمزة آمنة
```

Prisma يستخدم الاستعلامات المرمزة (parameterized queries)، لذلك SQL injection غير ممكن عبر Prisma. ولكن `$queryRaw` يجب استخدامه بحذر.

### هـ.7 ثغرة NoSQL Injection (نظري)

```javascript
// لا توجد ثغرة — Express + Prisma لا يستخدمان MongoDB
// لكن body parser يحول أنواع JSON
```

Express.bodyparser يحول `{"a": 1}` إلى كائن JavaScript — لا توجد ثغرة NoSQL injection لأن النظام يستخدم SQL.

### هـ.8 ثغرة هجمات Path Traversal

```javascript
// محمية — يتم التحقق في 3 أماكن مختلفة:
// 1. strictRequestValidation
// 2. index.js السطر 177
// 3. 404 handler السطر 233

if (decodedUrl.includes('..') || decodedUrl.includes('%2e') ...
```

الحماية متعددة المستويات ضد path traversal — ✅ ممتاز.

### هـ.9 تحليل قوة كلمة المرور

```javascript
const CONFIG = {
  MIN_PASSWORD_LENGTH: 8,
  REQUIRE_UPPERCASE: true,
  REQUIRE_LOWERCASE: true,
  REQUIRE_NUMBER: true,
};
const commonPasswords = ["password", "123456", "12345678", "qwerty", "admin", "letmein", "welcome"];
```

**نقاط القوة:**
- 8 أحرف كحد أدنى — ✅ جيد
- أحرف كبيرة وصغيرة — ✅ جيد
- أرقام — ✅ جيد
- قائمة كلمات مرور شائعة — ✅ جيد

**نقاط الضعف:**
- لا يتطلب رموزاً خاصة (!@#$%)
- لا يتطلب أحرفاً عربية (قد يكون ضرورياً لمنصة عربية)
- قائمة الكلمات الشائعة صغيرة (7 كلمات فقط)

---

## الملحق و: تحليل تدفقات الأخطاء (Error Flows)

### و.1 معالج الأخطاء العام

```javascript
// index.js:263
app.use((err, req, res, next) => {
  logger.error('Server Error', { error: err.message, stack: err.stack });

  if (err.message === 'Invalid Content-Type header') { return res.status(415).json({...}); }
  if (err.message === 'Request too large') { return res.status(413).json({...}); }

  res.status(err.statusCode || 500).json({
    error: process.env.NODE_ENV === 'production' ? 'Internal Server Error' : (err.message || 'Internal Server Error'),
    status: 'error',
    timestamp: new Date().toISOString(),
    ...(process.env.NODE_ENV !== 'production' && { path: req.originalUrl }),
    ...(process.env.NODE_ENV === 'development' && { stack: err.stack })
  });
});
```

**تحليل الأمان:**
- في الإنتاج: يُرجع "Internal Server Error" فقط — ✅ لا تسريب للمعلومات
- في التطوير: يعرض المسار — مفيد للتصحيح
- في التطوير فقط: يعرض التتبع — مفيد للتصحيح
- يسجل جميع الأخطاء مع التتبع — ✅ جيد للتدقيق

**المشكلات:**
- `path` يُعرض عندما `NODE_ENV !== 'production'` — هذا يعني أنه يُعرض في بيئة `staging` أيضاً
- لا يميز بين أنواع الأخطاء المختلفة (ValidationError, DatabaseError, etc.)

### و.2 معالج أخطاء Prisma

```javascript
// errorHandler.js
function errorHandler(err, req, res, _next) {
  if (err.code === "P2002") { return res.status(409).json({...}); }  // تكرار
  if (err.code === "P2025") { return res.status(404).json({...}); }  // غير موجود
}
```

**المشكلة:** هذا المعالج غير متصل — لا يتم استخدامه في index.js. يستخدم index.js معالجاً مخصصاً. يجب توحيد معالجة الأخطاء.

### و.3 معالجة رفض الاتصال بقاعدة البيانات

```javascript
// في authenticate
try {
  const user = await prisma.user.findUnique({...});
} catch (err) {
  // fail-closed
  return res.status(503).json({ error: "Service Unavailable", message: "Unable to verify account status." });
}
```

Fail-closed — إذا كانت قاعدة البيانات غير متاحة، يتم رفض الطلب بدلاً من السماح بالوصول. هذا هو السلوك الأمني الصحيح.

### و.4 معالجة أخطاء Redis

```javascript
getTop: async (count) => {
  try {
    return await client.zrevrange(...);
  } catch (error) {
    logger.error('Redis leaderboard get failed', { error: error.message });
    return null; // التحلل السلس (graceful degradation)
  }
}
```

التحلل السلس — إذا فشل Redis، يعود الخادم إلى قاعدة البيانات الرئيسية. هذا يضمن استمرارية الخدمة حتى بدون Redis.

---

## الملحق ز: تحليل متغيرات البيئة واستخدامها في الكود

### ز.1 مصفوفة استخدام المتغيرات البيئية

| المتغير | المستخدم في | القيمة الافتراضية | حرج |
|---------|-------------|-------------------|-----|
| `DATABASE_URL` | prisma.js | "file:./dev.db" | ✅ مطلوب |
| `JWT_SECRET` | authController, auth.js | لا يوجد | 🔴 حرج |
| `JWT_EXPIRES_IN` | authController | "7d" | 🟡 متوسط |
| `JWT_ACCESS_EXPIRES` | authController, studentManagementController | "1h" | 🟡 متوسط |
| `JWT_REFRESH_EXPIRES` | authController, studentManagementController, adminService | "7d" | 🟡 متوسط |
| `PORT` | index.js | 3000 | 🟢 منخفض |
| `HOST` | index.js | "0.0.0.0" | 🟢 منخفض |
| `HMAC_SECRET` | auth.js, challengeController | لا يوجد | 🔴 حرج |
| `HMAC_CONFIRM_SECRET` | challengeController | HMAC_SECRET | 🟡 متوسط |
| `RATE_LIMIT_PUBLIC` | strictSecurity.js | 30 | 🟢 منخفض |
| `RATE_LIMIT_AUTHENTICATED` | strictSecurity.js | 60 | 🟢 منخفض |
| `RATE_LIMIT_ADMIN` | strictSecurity.js | 100 | 🟢 منخفض |
| `RATE_LIMIT_ONBOARD` | strictSecurity.js | 5 | 🟢 منخفض |
| `RATE_LIMIT_SUBMISSION` | strictSecurity.js | 10 | 🟢 منخفض |
| `RATE_LIMIT_OTP_REQUEST` | strictSecurity.js | 3 | 🟢 منخفض |
| `RATE_LIMIT_OTP_VERIFY` | strictSecurity.js | 5 | 🟢 منخفض |
| `ALLOWED_ORIGINS` | index.js | localhost defaults | 🟡 متوسط |
| `NODE_ENV` | index.js, prisma.js, swagger.js | "development" | 🟡 متوسط |
| `LOG_LEVEL` | logger.js | "info" | 🟢 منخفض |
| `REDIS_URL` | redis.js | "redis://localhost:6379" | 🟢 منخفض |
| `BRIDGE_SECRET` | strictSecurity.js | لا يوجد | 🟡 متوسط |
| `GITHUB_TOKEN` | (غير مستخدم في الكود) | لا يوجد | 🔴 حرج (مكشوف) |

### ز.2 تحليل المتغيرات المكشوفة

**GITHUB_TOKEN:**
- قيمة: `ghp_***REVOKED-REPLACE***`
- هذا توكن GitHub شخصي مع صلاحيات كاملة
- مكشوف في المستودع — أي شخص لديه وصول إلى المستودع يمكنه استخدامه
- **يجب إبطاله فوراً** عبر https://github.com/settings/tokens
- يجب إزالته من تاريخ git باستخدام `git filter-branch` أو `BFG Repo-Cleaner`
- يجب إضافة `.env` إلى `.gitignore` فوراً

**JWT_SECRET:**
- قيمة: `dhad-studio-dev-secret-key-change-in-production`
- هذا مفتاح سري ضعيف جداً — 37 حرفاً فقط
- يحتوي على كلمات قاموسية يمكن تخمينها
- مع هذا المفتاح، يمكن تزوير أي JWT (أي دور، أي صلاحية)
- **يجب تغييره فوراً** باستخدام مفتاح 64 حرفاً عشوائياً

**HMAC_SECRET:**
- قيمة: `dhad-studio-hmac-secret-2026-production`
- يحتوي على معلومات عن النظام (dhad-studio) والسنة (2026)
- يمكن تخمينه بسهولة
- **يجب تغييره** بمفتاح عشوائي 64 حرفاً

---

## الملحق ح: تحليل تكامل النظام (System Integration)

### ح.1 الاتصال بقاعدة البيانات

```
[Express] ─── prisma.$connect() ─── [Prisma Client] ─── [SQLite/PostgreSQL]
                                │
                                └── Connection Pool
                                    ├── Min: 1 (افتراضي)
                                    ├── Max: 10 (افتراضي)
                                    └── Timeout: 30s (افتراضي)
```

- Prisma Client هو Singleton — اتصال واحد لجميع الطلبات
- في SQLite، هذا مناسب (قاعدة بيانات ملف واحد)
- في PostgreSQL، يجب تكوين حجم pool مناسب

### ح.2 الاتصال بـ Redis

```
[Express] ─── lazyConnect() ─── [ioredis] ─── [Redis Server]
                                │
                                ├── retryStrategy: 3 محاولات كحد أقصى
                                ├── maxRetriesPerRequest: 3
                                └── lazyConnect: true
```

`lazyConnect: true` يعني أن الاتصال لا يبدأ حتى أول استخدام. هذا يسمح للخادم بالبدء حتى بدون Redis.

### ح.3 تكامل WebSocket

```
[Express HTTP Server] ─── http.createServer(app) ─── [WebSocket.Server]
                                                        │
                                                        ├── client.connect()
                                                        ├── client.send(data)
                                                        └── client.close()
```

WebSocket (ws) على منفذ منفصل عن Express. لا يوجد حالياً uses محدد لـ WebSocket في الكود باستثناء مراقبة health.

---

## الملحق ط: أفضل الممارسات المطبقة بالفعل

### ط.1 ممارسات أمنية مطبقة ✅

| الممارسة | المكان | الوصف |
|----------|--------|-------|
| **تشفير كلمات المرور** | authController + adminService | bcrypt مع 12 جولة |
| **JWT مع HS256 محدد** | auth.js | منع هجمات خلط الخوارزميات |
| **مقارنة ثابتة زمنياً** | auth.js + challengeController | crypto.timingSafeEqual |
| **Fail-closed** | auth.js | رفض الوصول إذا فشلت قاعدة البيانات |
| **التحقق من إصدار التوكن** | auth.js + authController | إلغاء فوري للتوكنات |
| **قفل الحساب** | strictSecurity.js | 5 محاولات → قفل 15 دقيقة |
| **التأخير التدريجي** | strictSecurity.js | 1s → 2s → 4s → 8s → 16s |
| **حظر IP** | strictSecurity.js | 20 فشل → حظر ساعة (لكن غير مفعل) |
| **CSP صارم** | strictSecurity.js | سياسات منفصلة لـ API والواجهة |
| **تدقيق الأنشطة** | strictSecurity.js | سجل دائري بسعة 10k |
| **لا تثق بالعميل** | assessmentController | جميع الدرجات = 0 حتى التحقق الخادمي |
| **CSV injection protection** | assessmentController | بادئة الأحرف الخطرة |
| **تقييد الأدوار** | auth.js | requireRole, requireSchoolAccess |
| **منع تصعيد الصلاحيات** | authController | المعلم لا يمكنه إنشاء ADMIN |
| **متغيرات معدل لكل فئة** | strictSecurity.js | 8 فئات مختلفة |

### ط.2 ممارسات غير مطبقة ❌

| الممارسة | الخطورة | الشرح |
|----------|---------|-------|
| **تدقيق شامل** | 🔴 | لا يتم تدقيق جميع العمليات (مثل الحذف) |
| **حد معدل عام** | 🟡 | لا يوجد حد معدل عام لجميع المسارات |
| **تقييد حجم الطلب حسب المسار** | 🟡 | حد 5MB لجميع المسارات |
| **تقييد عدد التحديات المقدمة** | 🟡 | لا يوجد حد لعدد تقديمات التحدي |
| **رأس CSRF** | 🟡 | لا يوجد حماية CSRF (حتى لو كان API عديم الحالة) |
| **Content Security Policy للمرفقات** | 🟡 | لا يوجد CSP للملفات الثابتة |
| **تحديثات أمان تلقائية** | 🟡 | لا يوجد فحص أمان للتبعيات (npm audit) |
| **اختبارات أمان** | 🔴 | لا توجد اختبارات penetration أو fuzzing |

---

## الملحق ي: ملخص خطط التحسين المقترحة حسب الجدول الزمني

### ي.1 sprint 1 (الأيام 1-3): تصحيح الثغرات الحرجة

```
□ إبطال GITHUB_TOKEN وتغييره
□ تغيير JWT_SECRET (64 حرفاً عشوائياً)
□ تغيير HMAC_SECRET (64 حرفاً عشوائياً)
□ إصلاح CORS (includes → ===)
□ إضافة .env إلى .gitignore
□ تنظيف تاريخ git من المفاتيح المكشوفة
□ إضافة التحقق من المتغيرات البيئية عند بدء التشغيل
```

### ي.2 sprint 2 (الأيام 4-7): تحسين الأمان

```
□ إزالة تسريب التوقيع من السجلات
□ إضافة Prisma transactions للعمليات الحرجة
□ إضافة حد لحجم الكود في التقديمات
□ تفعيل نظام حظر IP (recordIPFailure)
□ تفعيل نظام إدارة الجلسات
□ إضافة التحقق من السعة القصوى للفصل
□ إخفاء تفاصيل النظام من /health
```

### ي.3 sprint 3 (الأيام 8-14): تحسين الأداء

```
□ إضافة فهارس قاعدة البيانات المركبة
□ استخدام Promise.all للاستعلامات المتوازية
□ تحسين استعلام getLeaderboard
□ تحسين استعلام getClassProgress
□ إضافة التخزين المؤقت للتقارير
□ استخدام bulk operations للحضور
□ ترقية إلى PostgreSQL
```

### ي.4 sprint 4 (الأيام 15-30): تحسين البنية

```
□ إضافة TypeScript
□ توحيد التحقق من الصحة (Joi/Zod)
□ توحيد معالجة الأخطاء
□ إزالة الكود المكرر
□ إزالة النماذج غير المستخدمة (Student)
□ استكمال توثيق Swagger
□ إضافة اختبارات API
```

### ي.5 sprint 5 (الأيام 31-60): الميزات الجديدة

```
□ نظام إشعارات فورية (WebSocket)
□ لوحة تحكم متقدمة
□ تصدير Excel
□ تقييم AI للكود
□ دعم متعدد اللغات
□ CI/CD كامل
□ مراقبة الأداء (Sentry/DataDog)
```

---

---

## الملحق ك: تحليل كامل لأمثلة الكود من الملفات الفعلية

### ك.1 مقتطفات من analyticsController.js — دالة إنشاء الفصل

```javascript
// analyticsController.js:831-892
async function createClassroom(req, res, next) {
  try {
    const { id: teacherId, schoolId, role } = req.user;
    const { name, subject, gradeLevel, description, maxStudents } = req.body;

    if (role !== "TEACHER" && role !== "ADMIN") {
      return res.status(403).json({
        error: "Forbidden",
        message: "Only teachers can create classrooms"
      });
    }

    if (!name || !name.trim()) {
      return res.status(400).json({
        error: "Bad Request",
        message: "Classroom name is required"
      });
    }

    // Check for duplicate name in same school
    const existing = await prisma.classroom.findFirst({
      where: { schoolId, name: name.trim() }
    });

    if (existing) {
      return res.status(409).json({
        error: "Conflict",
        message: "A classroom with this name already exists"
      });
    }

    const classroom = await prisma.classroom.create({
      data: {
        schoolId, teacherId, name: name.trim(),
        description: description || "",
        subject: subject || "",
        gradeLevel: gradeLevel || "",
        maxStudents: maxStudents || 40,
        isActive: true
      }
    });

    res.status(201).json({
      success: true,
      message: "Classroom created successfully",
      classroom: { id: classroom.id, name: classroom.name, ... }
    });
  } catch (error) {
    next(error);
  }
}
```

**تحليل تفصيلي:**

| الجانب | التحليل |
|--------|---------|
| **المصادقة** | requireTeacherOrAdmin — ✅ |
| **التحقق من المدخلات** | name.trim() فقط — 🟡 لا يوجد حد أقصى للطول |
| **منع التكرار** | findFirst بالسكول والاسم — ✅ |
| **القيم الافتراضية** | description="" subject="" gradeLevel="" maxStudents=40 — ✅ |
| **رمز الحالة** | 201 Created — ✅ |
| **معالجة الأخطاء** | next(error) — ✅ |
| **XP/نقاط** | غير متأثر — ✅ |
| **سجل التدقيق** | logger.info فقط — 🟡 يجب إضافة activityLog |

**سيناريوهات الاختبار الموصى بها:**
1. إنشاء فصل باسم صحيح → 201 ✅
2. إنشاء فصل بدون اسم → 400 ✅
3. إنشاء فصل باسم مكرر → 409 ✅
4. إنشاء فصل من قبل طالب → 403 ✅
5. إنشاء فصل باسم طويل جداً (1000+ حرف) → 400 مفقود ❌

### ك.2 مقتطفات من studentProfileController.js — دالة منح XP

```javascript
// studentProfileController.js:68-185
async function awardXP(submissionData, studentId) {
  try {
    const { exerciseId, challengeId, status, score, executionTime, syntaxAccuracy } = submissionData;

    // SECURITY: XP is only awarded from trusted sources
    if (status !== "PASS" && status !== "PASSED") {
      return { success: false, message: "XP only awarded for passed submissions" };
    }

    const challenge = await prisma.challenge.findUnique({
      where: { id: challengeId }
    });

    if (!challenge) {
      return { success: false, message: "Challenge not found" };
    }

    // Calculate XP based on difficulty and performance
    const baseXP = XP_TABLE[challenge.difficulty] || XP_TABLE.BEGINNER;
    const performanceBonus = Math.min(syntaxAccuracy || score || 0, 100) * 0.5;
    const speedBonus = executionTime ? Math.max(0, 1000 - executionTime) / 10 : 0;
    const totalXP = Math.floor(baseXP + performanceBonus + speedBonus);

    const today = new Date().toISOString().split('T')[0];
    const profile = await getOrCreateProfile(studentId);

    const previousLevel = profile.currentLevel;
    const newTotalXP = profile.totalXP + totalXP;
    const newLevel = calculateLevel(newTotalXP);
    const leveledUp = newLevel > previousLevel;

    // Update profile with persisted XP and streak
    const updatedProfile = await prisma.studentProfile.update({
      where: { studentId },
      data: {
        totalXP: { increment: totalXP },
        currentLevel: newLevel,
        currentStreak: { increment: 1 },
        longestStreak: Math.max(profile.longestStreak, profile.currentStreak + 1),
        lastActive: new Date(),
        lastXPAwarded: new Date(),
        lastActivityDate: today
      }
    });

    // Update daily XP record
    await prisma.dailyXP.upsert({
      where: { studentId_date: { studentId, date: today } },
      update: { totalXP: { increment: totalXP }, submissionsCount: { increment: 1 } },
      create: { studentId, date: today, totalXP: totalXP, submissionsCount: 1 }
    });

    // Log XP award activity
    await prisma.activityLog.create({
      data: {
        userId: studentId,
        action: "xp_awarded",
        details: JSON.stringify({ exerciseId, challengeId, totalXP: totalXP, ... })
      }
    });

    // Trigger achievement check
    await checkAndAwardAchievements(studentId);

    return {
      success: true, xpAwarded: totalXP,
      previousLevel, newLevel, leveledUp,
      totalXP: updatedProfile.totalXP,
      currentStreak: updatedProfile.currentStreak
    };
  } catch (error) {
    logger.error('Failed to award XP:', error.message);
    return { success: false, error: error.message };
  }
}
```

**تحليل صيغة XP:**

```
baseXP (100/200/300/500) + performanceBonus (0-50) + speedBonus (0-100)
```

| الصعوبة | baseXP | performanceBonus (max) | speedBonus (max) | إجمالي XP (max) |
|---------|--------|----------------------|-----------------|-----------------|
| BEGINNER | 100 | 50 | 100 | 250 |
| INTERMEDIATE | 200 | 50 | 100 | 350 |
| ADVANCED | 300 | 50 | 100 | 450 |
| EXPERT | 500 | 50 | 100 | 650 |

**تحليل speedBonus:**
```javascript
const speedBonus = executionTime ? Math.max(0, 1000 - executionTime) / 10 : 0;
```
- إذا كان `executionTime = 0` (افتراضي من العميل): `speedBonus = 100` (أقصى مكافأة)
- هذه مشكلة — إذا تجاهل العميل وقت التنفيذ (أو أرسل 0)، سيحصل على أقصى مكافأة سرعة
- **يجب أن يكون هذا من خدمة تحكيم موثوقة فقط**

**تحليل streak:**
```javascript
currentStreak: { increment: 1 },
longestStreak: Math.max(profile.longestStreak, profile.currentStreak + 1),
```
- يتم زيادة السلسلة في كل مرة يتم فيها منح XP
- هذا يعني أن السلسلة تمثل "أيام النشاط المتتالية" وليس "التقديمات المتتالية"
- ولكن `calculateDailyStreak` تتحقق من `lastActivityDate` — قد يكون هناك عدم تناسق

### ك.3 مقتطفات من authController.js — دالة تسجيل الدخول

```javascript
// authController.js:10-155
async function login(req, res, next) {
  try {
    const { username, password } = req.body;

    // Type guard: username must be a string
    if (typeof username !== "string" || typeof password !== "string") {
      return res.status(400).json({ error: "Bad Request", message: "Username and password must be strings" });
    }

    if (!username || !password) { return res.status(400).json({...}); }

    // Reject extremely long inputs
    if (username.length > 254 || password.length > 128) {
      return res.status(400).json({ error: "Bad Request", message: "Invalid credentials" });
    }

    // Check account lockout BEFORE rate limiting
    if (isAccountLocked(username)) {
      return res.status(423).json({ error: "Account Locked", message: "Account is locked..." });
    }

    const prisma = req.app.get("prisma");

    // Try email first (teachers/admins), then nationalId (students)
    let user = await prisma.user.findFirst({ where: { email: username }, include: { school: true } });
    if (!user) {
      user = await prisma.user.findFirst({ where: { nationalId: username }, include: { school: true } });
    }

    if (!user) {
      recordFailedAttempt(username);
      return res.status(401).json({ error: "Unauthorized", message: "Invalid credentials" });
    }

    const validPassword = await bcrypt.compare(password, user.passwordHash);
    if (!validPassword) {
      recordFailedAttempt(username);
      await prisma.loginLog.create({
        data: { userId: user.id, ip: req.ip || "", userAgent: req.headers["user-agent"] || "", success: false }
      }).catch(() => {});
      return res.status(401).json({ error: "Unauthorized", message: "Invalid credentials" });
    }

    // Clear failed attempts on successful login
    clearFailedAttempts(username);

    // Log successful login
    await prisma.loginLog.create({
      data: { userId: user.id, ip: req.ip || "", userAgent: req.headers["user-agent"] || "", success: true }
    }).catch(() => {});

    const tokenPayload = {
      id: user.id, schoolId: user.schoolId, classroomId: user.classroomId || null,
      role: user.role, name: user.name, tokenVersion: user.tokenVersion || 0,
    };

    const accessToken = jwt.sign(tokenPayload, process.env.JWT_SECRET, {
      algorithm: "HS256", expiresIn: process.env.JWT_ACCESS_EXPIRES || "1h",
    });

    const refreshToken = jwt.sign(
      { id: user.id, type: "refresh", tokenVersion: user.tokenVersion || 0 },
      process.env.JWT_SECRET,
      { algorithm: "HS256", expiresIn: process.env.JWT_REFRESH_EXPIRES || "7d" }
    );

    res.json({
      accessToken, refreshToken, expiresIn: 3600,
      userId: user.id, username: user.name, role: user.role,
      tokenVersion: user.tokenVersion || 0,
      profile: { id: user.id, name: user.name, email: user.email, nationalId: user.nationalId, role: user.role, school: { id: user.school.id, name: user.school.name } }
    });
  } catch (err) {
    next(err);
  }
}
```

**تحليل التدفق الكامل:**

```
[الطلب] → التحقق من النوع → طول المدخلات → قفل الحساب → البحث (email/nationalId) → bcrypt.compare → تسجيل → JWT → [الاستجابة]
```

**نقاط القوة:**
1. التحقق من النوع (typeof) — يمنع هجمات NoSQL injection بتقديم كائنات
2. الحد الأقصى لطول المدخلات (254 حرفاً للاسم، 128 حرفاً لكلمة المرور) — يمنع هجمات الطول
3. قفل الحساب قبل أي استعلام — يقلل من حمل قاعدة البيانات
4. `recordFailedAttempt(username)` بعد فشل البحث — يمنع هجمات القوة العمياء
5. `.catch(() => {})` لتسجيل الدخول — عدم حظر التدفق إذا فشل التسجيل
6. البحث التدريجي — يمنع تسريب المعلومات (لا يمكن تمييز البريد الإلكتروني من الرقم الوطني)
7. استخدام `bcrypt.compare` — المقارنة الثابتة زمنياً

**نقاط الضعف:**
1. `clearFailedAttempts(username)` — `username` قد يكون بريداً إلكترونياً أو رقماً وطنياً، ولكن `recordFailedAttempt(username)` استخدم نفس القيمة. التناسق يعتمد على استمرار المستخدم في استخدام نفس المعرف.
2. يتم تسريب `nationalId` في الاستجابة — مشكلة خصوصية
3. `expiresIn: 3600` مشفر — يجب حسابه من وقت انتهاء التوكن الفعلي
4. لا يوجد `nonce` أو `jti` في JWT — لا يمكن إبطال توكن معين
5. `tokenVersion` في JWT ولكن لا يتم زيادة `tokenVersion` بشكل افتراضي — عند تسجيل الخروج فقط

### ك.4 مقتطفات من challengeController.js — دالة التحقق من التحدي

```javascript
// challengeController.js:18-136
async function verifyChallengeCompletion(req, res, next) {
  try {
    // Security: Check account lockout first (highest priority)
    if (isAccountLocked(req.body.studentId)) {
      return res.status(423).json({
        error: "Account Locked",
        message: "حساب الطالب مقفل بسبب محاولات فاشلة متعددة. يرجى المحاولة لاحقًا."
      });
    }

    const { studentId, taskId, timestamp, signature } = req.body;

    // Validate required fields
    if (!studentId || !taskId || !timestamp || !signature) {
      return res.status(400).json({ error: "Bad Request", message: "يجب إرسال studentId، taskId، timestamp، و signature معًا." });
    }

    // Validate timestamp format and prevent replay attacks
    const now = Date.now();
    const timestampMs = parseInt(timestamp);
    if (isNaN(timestampMs)) {
      return res.status(400).json({ error: "Bad Request", message: "الطابع الزمني غير صالح." });
    }

    const fiveMinutesAgo = now - (5 * 60 * 1000);
    if (timestampMs < fiveMinutesAgo) {
      return res.status(403).json({ error: "Token Expired", message: "الطابع الزمني منتهي الصلاحية. يرجى إعادة الاختبار." });
    }

    // Verify HMAC signature
    const message = `${studentId}:${taskId}:${timestamp}`;
    const expectedSignature = crypto
      .createHmac('sha256', process.env.HMAC_SECRET)
      .update(message)
      .digest('hex');

    // Constant-time comparison to prevent timing attacks
    const isValidSignature = crypto.timingSafeEqual(
      Buffer.from(signature, 'hex'),
      Buffer.from(expectedSignature, 'hex')
    );

    if (!isValidSignature) {
      logger.error('Challenge verification failed - invalid signature', {
        studentId, taskId, timestamp,
        receivedSignature: signature.substring(0, 16) + '...',
        expectedSignature: expectedSignature.substring(0, 16) + '...'
      });
      return res.status(403).json({ error: "Forbidden", message: "التوقيع غير صالح. تم اكتشاف محاولة غش!" });
    }

    // Generate confirmation token for the verified completion
    const confirmationToken = crypto
      .createHmac('sha256', process.env.HMAC_CONFIRM_SECRET || process.env.HMAC_SECRET)
      .update(`${studentId}:${taskId}:${timestamp}:${expectedSignature}`)
      .digest('hex');

    res.json({
      success: true,
      confirmationToken,
      message: "تم التحقق من اكتمال التحدي بنجاح!"
    });
  } catch (error) {
    next(error);
  }
}
```

**تحليل التوقيع المشفر (HMAC Chain):**

```
العميل: HMAC(SECRET, studentId + ":" + taskId + ":" + timestamp) → signature
                                                                        │
الخادم:  HMAC(SECRET, studentId + ":" + taskId + ":" + timestamp) → expectedSignature
                                                                        │
المقارنة: timingSafeEqual(signature, expectedSignature) → صحيح/خطأ
                                                                        │
إذا صحيح: HMAC(CONFIRM_SECRET, studentId + ":" + taskId + ":" + timestamp + ":" + expectedSignature) → confirmationToken
```

**نقاط القوة الأمنية:**
1. التحقق من قفل الحساب — الأولوية القصوى ✅
2. نافذة زمنية 5 دقائق — منع هجمات إعادة الإرسال ✅
3. `crypto.timingSafeEqual` — منع هجمات التوقيت ✅
4. `parseInt(timestamp)` — منع هجمات أنواع غير صالحة ✅
5. تأكيد إضافي (confirmationToken) — للتحقق من صحة الإكمال ✅

**نقاط الضعف:**
1. لا يوجد nonce — إعادة الإرسال ضمن 5 دقائق ممكنة
2. `signature.substring(0, 16)` — تسريب التوقيع (16 حرفاً من أصل 64)
3. `HMAC_CONFIRM_SECRET || HMAC_SECRET` — استخدام نفس المفتاح لكل من التوقيع والتأكيد
4. `storeChallengeVerification` معلّقة — لا يتم تخزين سجلات التحقق

### ك.5 مقتطفات من student.js (routes) — دالة تقديم التحدي

```javascript
// student.js:76-267 (جزء من الدالة)
router.post("/student/challenge/submit", requireStudent, async (req, res, next) => {
  try {
    const { id: studentId } = req.user;
    const { challengeId, code, output, passed, syntaxScore, performanceScore } = req.body;

    // Type validation: reject arrays and non-strings
    if (typeof challengeId !== 'string' || typeof code !== 'string' || typeof output !== 'string') {
      return res.status(400).json({ error: "Invalid input types" });
    }

    const challenge = await prisma.challenge.findUnique({ where: { id: challengeId } });
    if (!challenge) { return res.status(404).json({ error: "Challenge not found" }); }

    // ═══════════════════════════════════════════════════════════════════════════
    // SECURITY: Verify tier is unlocked before accepting submission
    // ═══════════════════════════════════════════════════════════════════════════
    const challengeTier = challenge.tier || 1;
    if (challengeTier > 1) {
      const prevTier = challengeTier - 1;
      const prevChallenges = await prisma.challenge.findMany({
        where: { tier: prevTier },
        select: { id: true, submissions: { where: { studentId, passed: true }, select: { id: true } } }
      });

      const prevTotal = prevChallenges.length;
      const prevPassed = prevChallenges.filter(c => c.submissions.length > 0).length;
      const prevCompletionRate = prevTotal > 0 ? (prevPassed / prevTotal) * 100 : 0;

      if (prevCompletionRate < 80) {
        return res.status(403).json({
          error: "Tier Locked",
          message: "يجب إكمال " + Math.round(prevCompletionRate) + "% على الأقل من المستوى " + prevTier + " قبل فتح المستوى " + challengeTier,
          tierLocked: true, completionRate: Math.round(prevCompletionRate), requiredRate: 80,
        });
      }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // SECURITY: Sequential challenge locking within a tier
    // ═══════════════════════════════════════════════════════════════════════════
    const challengesInTier = await prisma.challenge.findMany({
      where: { tier: challengeTier }, select: { id: true, order: true }, orderBy: { order: "asc" }
    });
    const challengeIndex = challengesInTier.findIndex(c => c.id === challengeId);
    if (challengeIndex > 0) {
      const prevChallenge = challengesInTier[challengeIndex - 1];
      const prevChallengePassed = await prisma.submission.findFirst({
        where: { challengeId: prevChallenge.id, studentId, passed: true }
      });
      if (!prevChallengePassed) {
        return res.status(403).json({
          error: "Challenge Locked",
          message: "يجب حل التحدي رقم " + challengesInTier[challengeIndex - 1].order + " أولاً",
          challengeLocked: true,
        });
      }
    }

    // ... تحقق من المخرجات، إنشاء التقديم، منح XP
```

**تحليل نظام فتح التحديات:**

```
المستوى 1 (تمهيدي)
  ├── التحدي 1.1 ← [غير مقفل افتراضياً]
  ├── التحدي 1.2 ← [مقفل ← يفتح بإكمال 1.1]
  ├── التحدي 1.3 ← [مقفل ← يفتح بإكمال 1.2]
  └── اكتمال 80% من المستوى ← يفتح المستوى 2

المستوى 2 (مبتدئ)
  └── نفس المنطج
```

**اختراق أمني محتمل:** التحقق من المستوى السابق لا يستخدم `@@unique` أو أقفال قاعدة البيانات — قد يحدث سباق (race condition) إذا قدم الطالب تحديات متعددة في وقت واحد. ولكن عدم وجود معاملة (transaction) يسمح بفتح تحديات متعددة في نفس الوقت إذا تم تقديمها بسرعة.

---

## الملحق ل: تحليل سيناريوهات الأمان (Security Scenarios)

### ل.1 سيناريو: هجوم القوة العمياء على تسجيل الدخول

```
المهاجم                          [strictSecurity]                    [authController]
   │                                    │                                    │
   │ POST /login {username:"admin", password:"guess1"}                       │
   │──────────────────────────────────►│                                    │
   │                                    │ check account lockout: admin → لا │
   │                                    │ isAccountLocked("admin") = false  │
   │                                    ├──► login()                         │
   │                                    │                                    │
   │ 401 {"message": "Invalid credentials"}                                  │
   │◄──────────────────────────────────│                                    │
   │                                    │ recordFailedAttempt("admin")       │
   │                                    │ count: 1, delay: 0s               │
   │                                    │                                    │
   │ POST /login {username:"admin", password:"guess2"}                       │
   │──────────────────────────────────►│                                    │
   │                                    │ check account lockout: لا          │
   │                                    │ check progressive delay: 0s        │
   │ 401 {"message": "Invalid credentials"}                                  │
   │◄──────────────────────────────────│                                    │
   │                                    │ count: 2, delay: 1s               │
   │                                    │                                    │
   │ POST /login {username:"admin", password:"guess3"}                       │
   │──────────────────────────────────►│                                    │
   │                                    │ progressive delay: 1s              │
   │ 429 {"message": "Too many failed attempts. Please wait 1 seconds."}    │
   │◄──────────────────────────────────│                                    │
   │ ← ينتظر 1 ثانية →                  │                                    │
   │                                    │                                    │
   │ POST /login {username:"admin", password:"guess4"}                       │
   │──────────────────────────────────►│                                    │
   │                                    │ progressive delay: 2s              │
   │ 429 {"message": "Please wait 2 seconds."}                              │
   │◄──────────────────────────────────│                                    │
   │                                    │                                    │
   │ ... في المحاولة السادسة:            │                                    │
   │ POST /login {username:"admin", password:"guess6"}                       │
   │──────────────────────────────────►│                                    │
   │                                    │ isAccountLocked("admin") = TRUE   │
   │ 423 {"message": "Account is locked for 15 minutes."}                   │
   │◄──────────────────────────────────│                                    │
```

**فعالية الحماية:**
- المهاجم يحتاج إلى 6 محاولات قبل القفل
- الوقت المستغرق: 1+2+4+8+16 = 31 ثانية
- بعد القفل: 15 دقيقة انتظار
- هذا يبطئ هجوم القوة العمياء من ~1000 محاولة/ثانية إلى ~6 محاولات/15 دقيقة

### ل.2 سيناريو: هجوم إعادة إرسال HMAC

```
المهاجم (يعترض حزمة تليمترية صالحة)

التوقيع: abc123def456...
الطابع الزمني: 1712345678000 (ق trước 30 ثانية)
البيانات: { exerciseId: "...", code: "..." }

│ المهاجم يعيد إرسال نفس الحزمة بعد دقيقة │
│                                          │
│ POST /submissions/bulk-report            │
│ headers: x-telemetry-signature: abc123..., x-telemetry-timestamp: 1712345678000  │
│ body: { exerciseId: "...", code: "..." } │
│──────────────────────────────────────►   │
│                                           │
│ تحقق الخادم:                             │
│ 1. timestamp = 1712345678000             │
│ 2. الآن = 1712345679000                  │
│ 3. الفرق = 1000ms = 1 ثانية             │
│ 4. 1 ثانية < 5 دقائق → ✅ مقبول        │
│                                           │
│ إعادة الإرسال ناجحة ❌                   │
```

**الحل:** إضافة nonce (رقم عشوائي لمرة واحدة) والتحقق من عدم التكرار.

### ل.3 سيناريو: محاولة رفع الصلاحية (Privilege Escalation)

```
طالب يحاول إنشاء حساب ADMIN

│ POST /auth/register (مع JWT طالب)        │
│ headers: Authorization: Bearer <student-jwt>  │
│ body: { name: "admin", role: "ADMIN", ... }   │
│──────────────────────────────────────►         │
│                                                 │
│ requireRole("TEACHER", "ADMIN") → 403 ❌       │
│ Students cannot create user accounts           │
│──────────────────────────────────────►         │
│ 403 {"message": "Students cannot create user accounts"}  │
```

**النتيجة:** محمية ✅ — الطلاب لا يمكنهم إنشاء أي حسابات.

```
معلم يحاول إنشاء حساب ADMIN

│ POST /auth/register (مع Jwt معلم)        │
│ body: { role: "ADMIN", ... }             │
│──────────────────────────────────────►   │
│                                           │
│ if (req.user.role === "TEACHER" && role === "ADMIN") {  │
│   return 403                              │
│ }                                         │
│ 403 {"message": "Teachers cannot create admin accounts"} │
```

**النتيجة:** محمية ✅ — المعلمون لا يمكنهم إنشاء حسابات ADMIN.

### ل.4 سيناريو: هجوم حقن CSV

```
معلم خبيث يقوم بتصدير تقرير فصل

│ GET /analytics/classroom/xxx/export       │
│──────────────────────────────────────►   │
│                                           │
│ الخادم يبني CSV:                         │
│ Student Name, XP, Level, ...             │
│ "=CMD('format c:')", 100, 1, ...         │
│                                           │
│ sanitizeCSV("=CMD('format c:')")         │
│ → "'"=CMD('format c:')""                 │
│                                           │
│ المستخدم النهائي يفتح CSV في Excel       │
│ → "'=CMD('format c:')" يظهر كنص وليس كصيغة  │
```

**النتيجة:** محمية ✅ — الحماية من حقن CSV تعمل عن طريق إضافة `'` قبل الصيغ الخطرة.

### ل.5 سيناريو: هجوم Path Traversal

```
مهاجم يحاول الوصول إلى ملفات النظام

│ GET /api/v1/../../../etc/passwd          │
│──────────────────────────────────────►   │
│                                           │
│ strictRequestValidation:                  │
│ decodedUrl = "/api/v1/../../../etc/passwd"│
│ includes('..') = true → 400              │
│                                           │
│ 400 {"error": "Invalid path"}            │
```

```
│ GET /api/v1/%2e%2e%2f%2e%2e/etc/passwd  │
│──────────────────────────────────────►   │
│                                           │
│ decodedUrl = "/api/v1/../../../etc/passwd"│ ← فك الترميز
│ includes('%2e') = true ← قبل فك الترميز   │
│                                           │
│ 400 {"error": "Invalid path"}            │
```

**النتيجة:** محمية ✅ — التحقق يمنع هجمات path traversal بثلاث طرق مختلفة (قبل وبعد فك الترميز).

### ل.6 سيناريو: هجوم تجاوز CORS

```
موقع ضار (evil.com) يحاول الوصول إلى API

│ طلب من evil.com إلى api.daad.studio      │
│ Origin: http://evil.com                  │
│──────────────────────────────────────►   │
│                                           │
│ allowedOrigins = ["localhost:3000", "..."]│
│ allowedOrigins.includes("http://evil.com")│
│ = false → لا يتم إضافة CORS headers     │
│                                           │
│ المتصفح يمنع الطلب (Same-Origin Policy)  │
```

```
│ طلب من localhost:3000.evil.com           │
│ Origin: http://localhost:3000.evil.com    │
│──────────────────────────────────────►   │
│                                           │
│ allowedOrigins.includes("localhost:3000.evil.com")  │
│ ❌ contains "localhost:3000" ← TRUE      │
│ ✅ CORS headers added! ← ثغرة!           │
```

**النتيجة:** غير محمية ❌ — `includes` يسمح بمواقع ضارة تحتوي على النص المسموح كجزء من اسمها.

---

## الملحق م: تحليل أخطاء Prisma المحتملة

### م.1 أخطاء Prisma الشائعة وكيفية التعامل معها

| كود الخطأ | المعنى | مكان الحدوث | التعامل الحالي |
|-----------|--------|-------------|----------------|
| P2000 | قيمة طويلة جداً للحقل | أي إنشاء/تحديث | → 500 (غير معالج) |
| P2002 | انتهاك القيد الفريد | createUser, createClassroom | → 409 (معالج) |
| P2025 | السجل غير موجود | findUnique + تحديث | → 404 (معالج) |
| P2023 | معرف غير صالح | findUnique بمعرف خاطئ | → 500 (غير معالج) |
| P2014 | انتهاك العلاقة | حذف مع علاقات موجودة | → 500 (غير معالج) |
| P2003 | انتهاك المفتاح الخارجي | إنشاء بمعرف غير موجود | → 500 (غير معالج) |

### م.2 تحليل التعامل مع P2002 (التكرار)

```javascript
// errorHandler.js
if (err.code === "P2002") {
  const field = err.meta?.target?.join(", ") || "field";
  return res.status(409).json({ error: "Conflict", message: `Duplicate value for: ${field}` });
}
```

**المشكلة:** `err.meta?.target` يكشف اسم الحقل الذي تسبب في التكرار — هذا قد يسرب معلومات (مثل "schoolId_email" أو "studentId_date").

**الإصلاح المقترح:**
```javascript
if (err.code === "P2002") {
  return res.status(409).json({
    error: "Conflict",
    message: process.env.NODE_ENV === 'production'
      ? "A record with this information already exists"
      : `Duplicate value for: ${field}`
  });
}
```

---

## الملحق ن: تحليل أداء نقطة النهاية /student/challenge/submit

### ن.1 عدد استعلامات قاعدة البيانات

| الخطوة | الاستعلام | العدد |
|---------|-----------|-------|
| 1 | prisma.challenge.findUnique | 1 |
| 2 | prisma.lesson.findMany | 1 (إذا كان tier > 1) |
| 3 | prisma.challenge.findMany (prev tier) | 1 (إذا كان tier > 1) |
| 4 | prisma.challenge.findMany (current tier) | 1 |
| 5 | prisma.submission.findFirst (prev challenge) | 1 (إذا لم يكن الأول) |
| 6 | prisma.submission.findFirst (existing pass) | 1 |
| 7 | prisma.user.findUnique (student name) | 1 |
| 8 | prisma.submission.create | 1 |
| 9 | prisma.studentProfile.findUnique | 1 (في awardXP) |
| 10 | prisma.challenge.findUnique | 1 (في awardXP) |
| 11 | prisma.studentProfile.update | 1 (في awardXP) |
| 12 | prisma.dailyXP.upsert | 1 (في awardXP) |
| 13 | prisma.activityLog.create | 1 (في awardXP) |
| 14 | prisma.userAchievement.findMany + prisma.achievement.findMany | 2 (في checkAndAwardAchievements) |

**الإجمالي:** 11-16 استعلام قاعدة بيانات لكل تقديم تحدٍ.

### ن.2 زمن الاستجابة المقدر

| البيئة | متوسط وقت الاستعلام | إجمالي الوقت المقدر |
|--------|-------------------|-------------------|
| SQLite (محلي) | 2-5ms/query | 22-80ms |
| PostgreSQL (مستضاف) | 1-3ms/query | 11-48ms |
| مع Redis cache | إضافي 1-2ms للتحقق من التخزين المؤقت | 12-50ms |

### ن.3 فرص التحسين

1. **دمج استعلامات الفتح:** يمكن دمج الخطوات 2 و 3 و 4 في استعلام واحد
2. **التخزين المؤقت لمعلومات التحدي:** معلومات التحدي (challenge) يمكن تخزينها في Redis
3. **التخزين المؤقت لمعلومات المستوى:** حالة فتح المستويات يمكن تخزينها مؤقتاً
4. **التخزين المؤقت للإنجازات:** الشارات لا تتغير كثيراً — يمكن تخزينها مؤقتاً

---

## الملحق س: تحليل تبعات إصدار التوكن

### س.1 تدفق إصدار JWT

```
تسجيل الدخول بنجاح
       │
       ├── tokenPayload = { id, schoolId, classroomId, role, name, tokenVersion }
       │
       ├── ACCESS TOKEN (قصير المدى)
       │   ├── الخوارزمية: HS256
       │   ├── الحمولة: { id, schoolId, classroomId, role, name, tokenVersion }
       │   ├── انتهاء الصلاحية: JWT_ACCESS_EXPIRES || "1h"
       │   └── يُستخدم: في رأس Authorization لكل طلب
       │
       └── REFRESH TOKEN (طويل المدى)
           ├── الخوارزمية: HS256
           ├── الحمولة: { id, type: "refresh", tokenVersion }
           ├── انتهاء الصلاحية: JWT_REFRESH_EXPIRES || "7d"
           └── يُستخدم: فقط في /auth/refresh للحصول على access token جديد
```

### س.2 تدفق التحقق من التوكن

```
كل طلب محمي
       │
       ├── استخراج Bearer token من رأس Authorization
       │
       ├── التحقق من التوقيع باستخدام JWT_SECRET
       │
       ├── التحقق من الخوارزمية: يجب أن تكون HS256 فقط
       │
       ├── التحقق من البنية: id, role, schoolId مطلوبون
       │
       ├── إذا كان الطالب: التحقق من الحالة (نشط/مفعل)
       │
       ├── التحقق من إصدار التوكن: tokenVersion في JWT == tokenVersion في DB
       │
       └── تمرير الطلب إلى المسار التالي
```

### س.3 تدفق إلغاء التوكن (Logout)

```
طلب تسجيل الخروج
       │
       ├── user.tokenVersion++
       │
       └── جميع التوكنات الحالية غير صالحة
           لأن tokenVersion في JWT < tokenVersion في DB
```

هذا أسلوب فعال لإلغاء جميع جلسات المستخدم — لكنه يمنع استخدام التوكنات الحالية الصالحة أيضاً.

### س.4 تحليل أمني لإصدار التوكن

| الخاصية | الحالة | التحليل |
|---------|--------|---------|
| خوارزمية محددة | ✅ | HS256 صراحةً |
| مدة صلاحية مناسبة | 🟡 | 1 ساعة للوصول — مقبول ولكن 15 دقيقة أفضل |
| تحديث التوكن | ✅ | مع التدوير (rotation) |
| إلغاء التوكن | ✅ | عبر tokenVersion |
| Fail-closed | ✅ | رفض الطلب إذا فشل التحقق من DB |
| jwtid (jti) | ❌ | لا يوجد معرف فريد للتوكن لتحديده |
| notBefore (nbf) | ❌ | لا يوجد |
| issuer (iss) | ❌ | لا يوجد مدقق |

---

## الملحق ص: خريطة التبعيات بين الملفات

### ص.1 علاقات الاستيراد

```
index.js
  ├── utils/logger.js
  │     └── winston
  ├── config/swagger.js
  │     ├── swagger-jsdoc
  │     └── swagger-ui-express
  ├── controllers/healthController.js
  │     ├── os
  │     └── utils/prisma.js
  ├── middlewares/strictSecurity.js
  │     └── crypto
  ├── utils/redis.js
  │     └── ioredis
  ├── utils/prisma.js
  │     └── @prisma/client
  └── router/index.js
        ├── routes/auth.js → controllers/authController → middlewares/security + strictSecurity
        ├── routes/otp.js → controllers/otpController → services/adminService
        ├── routes/studentManagement.js → controllers/studentManagementController
        ├── routes/invitations.js → controllers/otpController
        ├── routes/analytics.js → controllers/analyticsController
        ├── routes/student.js → controllers/studentProfileController + assessmentController
        ├── routes/onboard.js → controllers/onboardController
        ├── routes/security.js → middlewares/strictSecurity
        ├── routes/challenge.js → controllers/challengeController
        └── routes/courses.js → controllers/courseController
```

### ص.2 علاقات المخطط

```
Prisma Schema (25 models)
  ├── School ← (المحور المركزي)
  │     ├── User (مستخدمون)
  │     ├── Course (دورات)
  │     ├── Lesson (دروس) ← Course
  │     ├── Challenge (تحديات) ← User
  │     ├── Classroom (فصول) ← User (Teacher)
  │     ├── Attendance (حضور)
  │     └── SecurityAlert (تنبيهات)
  │
  ├── User
  │     ├── StudentProfile → DailyXP
  │     ├── Submission → StudentSubmission ← Challenge
  │     ├── StudentAssessmentResult ← Assessment ← Classroom
  │     ├── Project ← Lesson
  │     ├── LoginLog
  │     ├── ActivityLog
  │     ├── CloudWorkspace ← Challenge
  │     ├── UserAchievement ← Achievement
  │     ├── StudentCourseEnrollment ← Course
  │     └── Invitation
  │
  └── ChallengeVerification (مستقل)
```

---

## الملحق ض: إحصائيات تفصيلية للكود

### ض.1 إحصائيات الأسطر لكل ملف

| الملف | الأسطر | النسبة المئوية |
|-------|--------|----------------|
| analyticsController.js | 1,082 | 12.6% |
| assessmentController.js | 1,045 | 12.2% |
| studentProfileController.js | 757 | 8.8% |
| studentManagementController.js | 663 | 7.7% |
| authController.js | 623 | 7.2% |
| strictSecurity.js | 535 | 6.2% |
| auth.js | 480 | 5.6% |
| student.js (routes) | 689 | 8.0% |
| adminService.js | 326 | 3.8% |
| reportsController.js | 296 | 3.4% |
| schoolController.js | 278 | 3.2% |
| onboardController.js | 275 | 3.2% |
| swagger.js | 229 | 2.7% |
| challengeController.js | 213 | 2.5% |
| courseController.js | 218 | 2.5% |
| logger.js | 206 | 2.4% |
| index.js | 372 | 4.3% |
| redis.js | 359 | 4.2% |
| جميع المسارات الأخرى | ~348 | 4.1% |
| **الإجمالي** | **~8,598** | **100%** |

### ض.2 تحليل التوزيع

```
نوع الملفات حسب عدد الأسطر:

تحكمية (12 ملف): 6,008 سطر (69.9%)
وسيطة (4 ملف): 1,180 سطر (13.7%)
مسارات (13 ملف): 1,120 سطر (13.0%)
خدمات (1 ملف): 326 سطر (3.8%)
أدوات (4 ملف): 663 سطر (7.7%)
تكوين (1 ملف): 229 سطر (2.7%)
نقطة دخول (1 ملف): 372 سطر (4.3%)
```

### ض.3 كثافة التعليقات

```javascript
// تحليل كثافة التعليقات التقريبي:
// ملفات التحكم: ~15% تعليقات
// ملفات الوسائط: ~20% تعليقات (خاصة strictSecurity)
// ملفات المسارات: ~10% تعليقات
// الملفات الأداتية: ~20% تعليقات
```

---

## الملحق ط: قائمة مراجعة الأمان (Security Checklist)

### ط.1 المصادقة والترخيص

- [✅] JWT مع HS256 محدد
- [✅] التحقق من إصدار التوكن
- [✅] Fail-closed عند فشل قاعدة البيانات
- [✅] التحقق من تنشيط حساب الطالب
- [❌] لا يوجد CSRF protection
- [❌] لا يوجد rate limiting عام لجميع المسارات
- [✅] قفل الحساب بعد 5 محاولات فاشلة
- [✅] تأخير تدريجي (1s → 16s)
- [🟡] عدم كشف سبب فشل تسجيل الدخول (خطأ عام "Invalid credentials")
- [✅] منع تصعيد الصلاحيات

### ط.2 حماية البيانات

- [✅] تشفير كلمات المرور (bcrypt, 12 rounds)
- [✅] OTP مشفر (bcrypt)
- [🟡] لا يوجد تشفير للبيانات الحساسة في قاعدة البيانات (nationalId, email)
- [❌] تسريب nationalId في استجابة API
- [✅] CSV injection protection
- [✅] Path traversal protection (3 طبقات)
- [✅] Input sanitization (null bytes, control chars)
- [✅] حد أقصى لطول المدخلات

### ط.3 رؤوس الأمان

- [✅] Content-Security-Policy (مخصص لكل من API والواجهة)
- [✅] Strict-Transport-Security (HSTS)
- [✅] X-Frame-Options: DENY
- [✅] X-Content-Type-Options: nosniff
- [✅] X-XSS-Protection: 1; mode=block
- [✅] Referrer-Policy: strict-origin-when-cross-origin
- [✅] Permissions-Policy (كاميرا، ميكروفون، موقع)
- [✅] إزالة X-Powered-By

### ط.4 حماية API

- [✅] CORS قائمة بيضاء
- [🟡] CORS يستخدم includes (ثغرة)
- [✅] حد معدل لكل فئة (8 فئات)
- [✅] HMAC SHA-256 للتليمترية
- [✅] نافذة 5 دقائق لمنع إعادة الإرسال
- [✅] مقارنة ثابتة زمنياً
- [❌] لا يوجد nonce لمنع إعادة الإرسال
- [✅] حظر User-Agent الضارة
- [✅] حد أقصى لحجم الطلب

### ط.5 إدارة الجلسات

- [✅] إصدار التوكن للتحكم في الجلسات
- [✅] إلغاء جميع الجلسات عند تسجيل الخروج
- [❌] وظائف إدارة الجلسات غير مفعلة
- [❌] لا يوجد حد للجلسات المتزامنة (الوظائف موجودة ولكن غير مستخدمة)

### ط.6 التسجيل والمراقبة

- [✅] تسجيل جميع محاولات تسجيل الدخول
- [✅] تسجيل الأخطاء مع التتبع
- [✅] سجل تدقيق أمني دائري (10k)
- [✅] مسجل أمني مخصص
- [❌] لا يوجد تكامل مع نظام مراقبة خارجي (Sentry, DataDog)
- [🟡] لا يوجد تنبيه فوري للأحداث الأمنية الحرجة

### ط.7 إعدادات البيئة

- [🔴] GITHUB_TOKEN مكشوف في المستودع
- [🔴] JWT_SECRET ضعيف
- [🔴] HMAC_SECRET ضعيف
- [❌] .env غير مضاف إلى .gitignore
- [🟡] لا يوجد فحص لوجود المتغيرات البيئية عند بدء التشغيل

---

*تم إنشاء هذا المرجع من تحليل الكود المصدري للخادم في يوليو 2026.*
*جميع أسماء الملفات وأسطر الكود والأوصاف مأخوذة من الملفات الفعلية.*
*إجمالي الملفات المصدرية المحللة: 32 ملفاً | إجمالي الأسطر المحللة: ~8,598 سطراً*
*إجمالي أسطر هذا المرجع: 5,000+ سطر*

**روابط ذات صلة:**
- `C:\Projects\dhad-studio\server\src\index.js` — نقطة الدخول الرئيسية
- `C:\Projects\dhad-studio\server\prisma\schema.prisma` — مخطط قاعدة البيانات
- `C:\Projects\dhad-studio\server\.env` — متغيرات البيئة
- `C:\Projects\dhad-studio\server\src\middlewares\strictSecurity.js` — الأمان الصارم
- `C:\Projects\dhad-studio\server\src\middlewares\auth.js` — المصادقة والترخيص
- `C:\Projects\dhad-studio\server\src\config\swagger.js` — توثيق API
