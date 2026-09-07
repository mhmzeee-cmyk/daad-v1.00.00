# 📚 المرجع الكامل لواجهة برمجة التطبيقات (API) — Dhad Studio

## 🗺️ خريطة المسارات (Route Map)

`
┌─────────────────────────────────────────────────────────────────────┐
│                          API v1                                      │
│                 Base: /api/v1                                        │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                     المصادقة (Authentication)                 │   │
│  │  /api/v1/auth/...                                            │   │
│  │  POST   /login                    ──► تسجيل الدخول             │   │
│  │  POST   /refresh                  ──► تحديث رمز الجلسة         │   │
│  │  POST   /register                 ──► إنشاء حساب جديد          │   │
│  │  PUT    /change-password          ──► تغيير كلمة المرور        │   │
│  │  POST   /reset-password           ──► إعادة تعيين كلمة المرور  │   │
│  │  POST   /logout                   ──► تسجيل الخروج             │   │
│  │  POST   /request-otp              ──► طلب رمز التحقق           │   │
│  │  POST   /verify-otp               ──► التحقق من رمز OTP        │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                  لوحة المعلومات (Analytics)                   │   │
│  │  /api/v1/analytics/...                                       │   │
│  │  GET    /analytics/leaderboard           ──► ترتيب الطلاب     │   │
│  │  GET    /analytics/attendance-summary    ──► ملخص الحضور      │   │
│  │  GET    /analytics/attendance-history    ──► سجل الحضور       │   │
│  │  POST   /analytics/attendance            ──► تسجيل الحضور     │   │
│  │  GET    /analytics/progress              ──► تقدم الطلاب      │   │
│  │  GET    /analytics/dashboard             ──► لوحة المعلم      │   │
│  │  GET    /analytics/classrooms            ──► فصول المعلم      │   │
│  │  POST   /analytics/classrooms            ──► إنشاء فصل        │   │
│  │  GET    /analytics/assessments           ──► تقييمات المعلم   │   │
│  │  GET    /analytics/classroom/:id/export  ──► تصدير تقرير الفصل│   │
│  │  GET    /analytics/security-alerts       ──► تنبيهات أمنية    │   │
│  │  POST   /analytics/security-alerts/:id/resolve ──► حل تنبيه   │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                التحديات (Challenges)                          │   │
│  │  /api/v1/challenges/...                                      │   │
│  │  GET    /challenges                    ──► قائمة التحديات     │   │
│  │  GET    /challenges/daily              ──► التحدي اليومي      │   │
│  │  POST   /challenges                    ──► إنشاء تحدي         │   │
│  │  PUT    /challenges/:id                ──► تحديث تحدي         │   │
│  │  DELETE /challenges/:id                ──► حذف تحدي           │   │
│  │  POST   /challenges/:id/publish        ──► نشر تحدي           │   │
│  │  POST   /challenges/:id/set-daily      ──► تعيين كتحدي يومي   │   │
│  │  POST   /submit-solution               ──► تقديم حل           │   │
│  │  GET    /submissions                   ──► سجل الحلول         │   │
│  │  POST   /challenge/verify              ──► التحقق من التحدي   │   │
│  │  GET    /lessons/:lessonId/tests       ──► اختبارات الدرس     │   │
│  │  GET    /challenges/tier/:tierId       ──► تحديات المستوى     │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          إدارة الطلاب (Student Management)                     │   │
│  │  /api/v1/...                                                 │   │
│  │  GET    /auth/classrooms              ──► قائمة الفصول        │   │
│  │  GET    /auth/classrooms/:id/students ──► طلاب الفصل          │   │
│  │  POST   /auth/student-login           ──► دخول الطالب         │   │
│  │  POST   /teacher/students             ──► إنشاء طالب          │   │
│  │  POST   /teacher/students/bulk        ──► إنشاء طلاب بالجملة  │   │
│  │  GET    /teacher/students             ──► قائمة الطلاب        │   │
│  │  DELETE /teacher/students/:id         ──► حذف طالب            │   │
│  │  POST   /teacher/students/assign      ──► تعيين طالب لفصل     │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          ملف الطالب (Student Profile)                         │   │
│  │  /api/v1/student/...                                         │   │
│  │  GET    /student/profile              ──► الملف الشخصي        │   │
│  │  GET    /student/leaderboard          ──► ترتيب الطلاب        │   │
│  │  GET    /student/roadmap              ──► خارطة التعلم        │   │
│  │  POST   /student/challenge/submit     ──► تقديم حل التحدي     │   │
│  │  GET    /student/achievements         ──► الإنجازات           │   │
│  │  GET    /student/classroom/check      ──► التحقق من الفصل     │   │
│  │  GET    /student/assessment/active    ──► التقييم النشط       │   │
│  │  GET    /student/assessments          ──► قائمة التقييمات     │   │
│  │  POST   /student/assessment/submit    ──► تقديم إجابة تقييم   │   │
│  │  POST   /student/assessment/start     ──► بدء تقييم           │   │
│  │  POST   /student/assessment/complete  ──► إنهاء تقييم         │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          التقييمات (Assessments)                               │   │
│  │  /api/v1/assessments/...                                      │   │
│  │  POST   /assessments/create             ──► إنشاء تقييم       │   │
│  │  GET    /assessments/:id/results        ──► نتائج التقييم     │   │
│  │  GET    /assessments/classroom/:id      ──► تقييمات الفصل     │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          المساحة السحابية (Cloud Workspace)                   │   │
│  │  /api/v1/workspace/...                                       │   │
│  │  POST   /workspace/save              ──► حفظ الكود            │   │
│  │  GET    /workspace/load/:id         ──► تحميل الكود           │   │
│  │  GET    /workspace/list              ──► قائمة المساحات       │   │
│  │  DELETE /workspace/:id             ──► حذف مساحة             │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          المقررات الدراسية (Courses)                          │   │
│  │  /api/v1/student/courses/...                                 │   │
│  │  GET    /student/courses              ──► قائمة المقررات     │   │
│  │  POST   /student/courses/:id/enroll   ──► التسجيل في مقرر    │   │
│  │  GET    /student/courses/:id/roadmap  ──► خارطة المقرر       │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          التقارير (Reports)                                    │   │
│  │  /api/v1/reports/...                                          │   │
│  │  GET    /reports/school-overview       ──► نظرة عامة للمدرسة │   │
│  │  GET    /reports/school-overview/:id   ──► نظرة عامة لمدرسة  │   │
│  │  POST   /reports/weekly                ──► تقرير أسبوعي      │   │
│  │  POST   /reports/weekly/:id            ──► تقرير أسبوعي لمدرسة│  │
│  │  GET    /reports/activity/:id          ──► نشاط مستخدم       │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          المدارس (Schools)                                    │   │
│  │  /api/v1/schools/...                                         │   │
│  │  GET    /schools                     ──► قائمة المدارس       │   │
│  │  GET    /schools/:id                 ──► تفاصيل المدرسة      │   │
│  │  GET    /schools/:id/users           ──► مستخدمي المدرسة     │   │
│  │  PUT    /schools/:id                 ──► تحديث المدرسة       │   │
│  │  DELETE /schools/:id                 ──► حذف المدرسة         │   │
│  │  DELETE /schools/:id/users/:userId   ──► حذف مستخدم          │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          الأمان (Security)                                    │   │
│  │  /api/v1/security/...                                        │   │
│  │  GET    /security/dashboard          ──► لوحة الأمان          │   │
│  │  GET    /security/audit              ──► سجل التدقيق          │   │
│  │  POST   /security/unblock-ip         ──► إلغاء حظر IP         │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          إعداد المدرسة (Onboarding)                            │   │
│  │  /api/v1/onboard-school            ──► تهيئة مدرسة            │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          الدعوات (Invitations)                                │   │
│  │  /api/v1/teacher/invitations/...                             │   │
│  │  POST   /teacher/invitations/import  ──► استيراد طلاب        │   │
│  │  GET    /teacher/invitations         ──► قائمة الدعوات       │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │          التقارير المجمعة (Bulk)                              │   │
│  │  /api/v1/submissions/bulk-report    ──► تقرير مجمع (MessagePack)│
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
`

**إجمالي نقاط النهاية: 80 نقطة نهاية**

---

# 📖 فهرس المحتويات

1. [مقدمة](#مقدمة)
2. [معلومات عامة](#معلومات-عامة)
3. [المصادقة (Authentication) — auth.js](#المصادقة-authentication--authjs)
4. [رمز التحقق لمرة واحدة (OTP) — otp.js](#رمز-التحقق-لمرة-واحدة-otp--otpjs)
5. [لوحة المعلومات (Analytics) — analytics.js](#لوحة-المعلومات-analytics--analyticsjs)
6. [التحديات (Challenges) — challenges.js](#التحديات-challenges--challengesjs)
7. [التحقق من التحديات (Challenge Verify) — challenge.js](#التحقق-من-التحديات-challenge-verify--challengejs)
8. [ملف الطالب (Student) — student.js](#ملف-الطالب-student--studentjs)
9. [التقييمات (Assessments) — student.js](#التقييمات-assessments--studentjs)
10. [المساحة السحابية (Workspace) — student.js](#المساحة-السحابية-workspace--studentjs)
11. [تنبيهات الأمان (Security Alerts) — student.js](#تنبيهات-الأمان-security-alerts--studentjs)
12. [إدارة الطلاب (Student Management) — studentManagement.js](#إدارة-الطلاب-student-management--studentmanagementjs)
13. [المقررات الدراسية (Courses) — courses.js](#المقررات-الدراسية-courses--coursesjs)
14. [الدعوات (Invitations) — invitations.js](#الدعوات-invitations--invitationsjs)
15. [تهيئة المدرسة (Onboarding) — onboard.js](#تهيئة-المدرسة-onboarding--onboardjs)
16. [التقارير (Reports) — reports.js](#التقارير-reports--reportsjs)
17. [المدارس (Schools) — schools.js](#المدارس-schools--schoolsjs)
18. [لوحة الأمان (Security Dashboard) — security.js](#لوحة-الأمان-security-dashboard--securityjs)
19. [الوسائط الأمنية المشتركة (Shared Middleware)](#الوسائط-الأمنية-المشتركة-shared-middleware)

---

# مقدمة

توثق هذه الوثيقة جميع نقاط نهاية واجهة برمجة التطبيقات (API) لخادم Dhad Studio. جميع المسارات مُحمّلة تحت المسار الأساسي /api/v1.

**إصدار API:** v1
**البروتوكول:** HTTPS (إلزامي في الإنتاج)
**تنسيق البيانات:** JSON (باستثناء /submissions/bulk-report الذي يدعم MessagePack)
**التشفير:** JWT (HS256) للمصادقة، HMAC-SHA256 للتحقق من سلامة البيانات

---

# معلومات عامة

## رؤوس HTTP العامة (General Headers)

| الرأس (Header) | إلزامي؟ | الوصف |
|---|---|---|
| Authorization: Bearer <token> | نعم (للمسارات المحمية) | رمز JWT |
| Content-Type: application/json | نعم (لـ POST/PUT) | تنسيق المحتوى |
| x-telemetry-signature | نعم (للتقارير المجمعة) | توقيع HMAC للبيانات |
| x-telemetry-timestamp | نعم (للتقارير المجمعة) | الطابع الزمني للتوقيع |
| User-Agent | لا | تعريف المتصفح/التطبيق |

## رموز الحالة العامة (Common HTTP Status Codes)

| الرمز | المعنى |
|---|---|
| 200 OK | نجاح العملية |
| 201 Created | تم إنشاء المورد بنجاح |
| 400 Bad Request | طلب غير صالح (خطأ في التحقق) |
| 401 Unauthorized | مصادقة مفقودة أو غير صالحة |
| 403 Forbidden | صلاحية غير كافية |
| 404 Not Found | المورد غير موجود |
| 409 Conflict | تعارض (مكرر) |
| 413 Payload Too Large | حجم الطلب كبير جداً |
| 414 URI Too Long | رابط URI طويل جداً |
| 423 Locked | الحساب مقفل |
| 429 Too Many Requests | تجاوز حد الطلبات |
| 500 Internal Server Error | خطأ داخلي في الخادم |
| 503 Service Unavailable | الخدمة غير متوفرة |

## الأدوار (Roles)

| الدور | الوصف |
|---|---|
| STUDENT | طالب — صلاحيات محدودة |
| TEACHER | معلم — يمكنه إنشاء وإدارة الطلاب والفصول والتحديات |
| ADMIN | مدير النظام — صلاحيات كاملة على جميع المدارس |

## دوال التحقق من الصلاحية (Auth Middleware)

| الدالة | الصلاحية المطلوبة |
|---|---|
| uthenticate | أي مستخدم مسجل الدخول (رمز JWT صالح) |
| equireStudent | دور STUDENT فقط |
| equireTeacher | دور TEACHER أو ADMIN |
| equireAdmin | دور ADMIN فقط |
| equireTeacherOrAdmin | دور TEACHER أو ADMIN |
| equireSchoolAccess | الوصول فقط لنفس المدرسة (إلا للمدير) |
| equireRole(...roles) | أدوار محددة |

## آليات الأمان العامة (Security Mechanisms)

1. **معدل الطلبات (Rate Limiting):** جميع المسارات العامة والمهمة محدودة بعدد الطلبات لكل نافذة زمنية
2. **قفل الحساب (Account Lockout):** بعد 5 محاولات فاشلة، يُقفل الحساب لمدة 15 دقيقة
3. **التأخير التصاعدي (Progressive Delay):** تأخير 1ث، 2ث، 4ث، 8ث، 16ث بعد كل محاولة فاشلة
4. **حظر IP:** بعد 20 فشل، يُحظر IP لمدة ساعة
5. **التعقيم (Sanitization):** تعقيم جميع المدخلات من أحرف التحكم و XSS
6. **التحقق من الرمز (Token Revocation):** التحقق من إصدار الرمز في كل طلب
7. **رؤوس الأمان (Security Headers):** HSTS، CSP، X-Frame-Options، إلخ
8. **التحقق من التوقيع (HMAC):** للبيانات الحساسة والتقارير المجمعة

---

# المصادقة (Authentication) — auth.js

## 1. POST /api/v1/auth/login

### المسار
POST /api/v1/auth/login

### الوصف
تسجيل الدخول للمستخدمين (معلمين، طلاب، مدراء). يدعم تسجيل الدخول بالبريد الإلكتروني (للمعلمين/المدراء) أو الرقم الوطني (للطلاب).

### المتطلبات
- **المصادقة:** عام (عام)
- **معدل الطلبات:** نعم — strictRateLimit('public') (افتراضي: 30 طلب/15 دقيقة)
- **الصلاحية:** لا شيء (عام)

### المدخلات

**رؤوس HTTP:**
| الرأس | القيمة |
|---|---|
| Content-Type | pplication/json |

**جسم الطلب (Body) - JSON:**
| الحقل | النوع | إلزامي | التحقق | الوصف |
|---|---|---|---|---|
| username | string | نعم | طول ≥ 254 حرف | البريد الإلكتروني أو الرقم الوطني |
| password | string | نعم | طول ≥ 128 حرف | كلمة المرور |

### التحقق من الأمان (قبل معالج الطلب)

1. loginSecurityCheck:
   - التحقق من قفل الحساب (isAccountLocked) — يُعيد 423 إذا كان الحساب مقفلاً
   - التحقق من التأخير التصاعدي (isProgressivelyDelayed) — يُعيد 429 مع etryAfter
2. strictRateLimit('public') — التحقق من معدل الطلبات

### المخرجات

**نجاح (200):**
\\\json
{
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "eyJhbGciOiJIUzI1NiIs...",
  "expiresIn": 3600,
  "userId": "uuid-string",
  "username": "أحمد محمد",
  "role": "TEACHER",
  "tokenVersion": 1,
  "profile": {
    "id": "uuid-string",
    "name": "أحمد محمد",
    "email": "ahmed@school.com",
    "nationalId": null,
    "role": "TEACHER",
    "school": {
      "id": "uuid-string",
      "name": "مدرسة القدس"
    }
  }
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | Username and password must be strings |
| 400 | Bad Request | Username and password are required |
| 400 | Bad Request | Invalid credentials (إذا تجاوز الطول) |
| 401 | Unauthorized | Invalid credentials (مستخدم غير موجود) |
| 401 | Unauthorized | Invalid credentials (كلمة مرور خاطئة) |
| 423 | Account Locked | Account is locked due to too many failed attempts |
| 429 | Too Many Attempts | Too many failed attempts. Please wait N seconds. |
| 429 | Too Many Requests | Rate limit exceeded |

### الأمان

- التحقق من قفل الحساب قبل أي معالجة
- تأخير تصاعدي (1ث، 2ث، 4ث، 8ث، 16ث)
- تسجيل كل محاولة فاشلة في سجل loginLog
- تسجيل أحداث الأمان عبر logAudit
- معدل طلبات صارم
- التحقق من طول المدخلات (منع هجمات تجاوز السعة)
- استخدام bcrypt (12 جولة) لمقارنة كلمة المرور

### مثال CURL

\\\ash
curl -X POST https://api.dhad.studio/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "ahmed@school.com",
    "password": "SecurePass123"
  }'
\\\

### التحسينات المقترحة

- إضافة دعم تسجيل الدخول عبر OAuth/SSO
- إضافة كAPTCHA للمسارات العامة
- استخدام Redis بدلاً من الذاكرة للتخزين المؤقت لمحاولات الفشل
- إضافة إشعارات عند تسجيل الدخول من جهاز جديد
- دعم المصادقة متعددة العوامل (MFA)

## 2. POST /api/v1/auth/refresh

### المسار
POST /api/v1/auth/refresh

### الوصف
تحديث رمز الوصول (access token) باستخدام رمز التحديث (refresh token). يُصدر رموزاً جديدة مع تدوير رمز التحديث.

### المتطلبات
- **المصادقة:** عام
- **معدل الطلبات:** نعم — strictRateLimit('public')
- **الصلاحية:** لا شيء

### المدخلات

**جسم الطلب (Body):**
| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| efreshToken | string | نعم | رمز التحديث المستلم من /login |

### المخرجات

**نجاح (200):**
\\\json
{
  "success": true,
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "eyJhbGciOiJIUzI1NiIs...",
  "expiresIn": 3600
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | Refresh token is required |
| 401 | Unauthorized | Invalid or expired refresh token |
| 401 | Unauthorized | Invalid token type |
| 401 | Unauthorized | User not found or inactive |
| 401 | Unauthorized | Token has been revoked |

### الأمان

- التحقق من نوع الرمز (	ype: "refresh")
- التحقق من إصدار الرمز (token version) ضد قاعدة البيانات
- تدوير رمز التحديث (إصدار رمز جديد مع كل طلب)
- انتهاء صلاحية الرمز (افتراضي: 7 أيام)
- استخدام خوارزمية HS256 حصراً (منع هجمات تبديل الخوارزمية)

### مثال CURL

\\\ash
curl -X POST https://api.dhad.studio/api/v1/auth/refresh \
  -H "Content-Type: application/json" \
  -d '{
    "refreshToken": "eyJhbGciOiJIUzI1NiIs..."
  }'
\\\

### التحسينات المقترحة

- إضافة قائمة سوداء للرموز الملغاة في Redis
- إشعار المستخدم عند تدوير الرمز
- تحديد عدد الرموز النشطة لكل مستخدم

## 3. POST /api/v1/auth/register

### المسار
POST /api/v1/auth/register

### الوصف
إنشاء حساب مستخدم جديد (طلاب، معلمين، مدراء). يمكن للمدير إنشاء في أي مدرسة، وللمعلم إنشاء في مدرسته فقط.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **معدل الطلبات:** نعم — strictRateLimit('public')
- **الصلاحية:** TEACHER أو ADMIN
- **القيود:**
  - لا يمكن للمعلم إنشاء حسابات ADMIN
  - لا يمكن للطالب إنشاء أي حسابات

### المدخلات

**جسم الطلب (Body):**
| الحقل | النوع | إلزامي | التحقق | الوصف |
|---|---|---|---|---|
| 
ame | string | نعم | ≥ 100 حرف | اسم المستخدم |
| email | string | للـ TEACHER/ADMIN | ≥ 254 حرف، صيغة صحيحة | البريد الإلكتروني |
| 
ationalId | string | للـ STUDENT | ≥ 20 حرف | الرقم الوطني |
| ole | string | نعم | أحد: STUDENT, TEACHER, ADMIN | الدور |
| schoolId | string | لا (افتراضي: مدرسة المستخدم) | — | معرف المدرسة |
| password | string | نعم | ≤ 8 أحرف، حرف كبير، حرف صغير، رقم | كلمة المرور |

### المخرجات

**نجاح (201):**
\\\json
{
  "message": "User \"أحمد محمد\" created successfully",
  "user": {
    "id": "uuid-string",
    "name": "أحمد محمد",
    "email": "ahmed@school.com",
    "nationalId": null,
    "role": "TEACHER",
    "school": { "id": "uuid-string", "name": "مدرسة القدس" },
    "createdAt": "2026-07-10T12:00:00.000Z"
  }
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | name and role are required |
| 400 | Bad Request | name must be 100 characters or less |
| 400 | Bad Request | role must be one of: STUDENT, TEACHER, ADMIN |
| 400 | Bad Request | email is required for teachers and admins |
| 400 | Bad Request | Invalid email format |
| 400 | Bad Request | nationalId is required for students |
| 400 | Bad Request | Password is required |
| 400 | Bad Request | Password must be at least 8 characters... |
| 403 | Forbidden | You can only create users in your own school |
| 403 | Forbidden | Teachers cannot create admin accounts |
| 403 | Forbidden | Students cannot create user accounts |
| 409 | Conflict | A user with this email already exists in this school |

### الأمان

- التحقق من صلاحية إنشاء الحسابات حسب الدور
- منع تصعيد الصلاحيات (Teacher → Admin)
- فصل المدارس (عزل البيانات)
- التحقق من قوة كلمة المرور
- تعقيم البريد الإلكتروني من أحرف التحكم (منع حقن الرؤوس)
- التحقق من التكرار (البريد الإلكتروني والرقم الوطني)
- تشفير كلمة المرور بـ bcrypt (12 جولة)

### مثال CURL

\\\ash
curl -X POST https://api.dhad.studio/api/v1/auth/register \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d '{
    "name": "أحمد محمد",
    "email": "ahmed@school.com",
    "role": "TEACHER",
    "schoolId": "school-uuid",
    "password": "SecurePass123"
  }'
\\\

### التحسينات المقترحة

- إضافة تسجيل الدخول الموحد (SSO) للتسجيل
- إضافة التحقق من البريد الإلكتروني قبل تفعيل الحساب
- إمكانية تخصيص صلاحيات أكثر تفصيلاً

## 4. PUT /api/v1/auth/change-password

### المسار
PUT /api/v1/auth/change-password

### الوصف
تغيير كلمة المرور للمستخدم الحالي (يتطلب كلمة المرور الحالية).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط (equireStudent)
- **معدل الطلبات:** لا

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| currentPassword | string | نعم | كلمة المرور الحالية |
| 
ewPassword | string | نعم | كلمة المرور الجديدة (≥ 8 أحرف، حرف كبير، حرف صغير، رقم) |

### المخرجات

**نجاح (200):**
\\\json
{
  "message": "Password changed successfully"
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | currentPassword and newPassword are required |
| 400 | Bad Request | Password must be at least 8 characters... |
| 401 | Unauthorized | Current password is incorrect |
| 404 | Not Found | User not found |
| 423 | Account Locked | Account is locked. Cannot change password. |

### الأمان

- التحقق من قفل الحساب قبل تغيير كلمة المرور
- التحقق من كلمة المرور الحالية
- تطبيق سياسة قوة كلمة المرور
- تسجيل حدث تغيير كلمة المرور في سجل التدقيق

### مثال CURL

\\\ash
curl -X PUT https://api.dhad.studio/api/v1/auth/change-password \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d '{
    "currentPassword": "OldPass123",
    "newPassword": "NewSecurePass456"
  }'
\\\

### التحسينات المقترحة

- إضافة إشعار عبر البريد الإلكتروني عند تغيير كلمة المرور
- منع إعادة استخدام آخر 5 كلمات مرور
- إضافة صلاحية جديدة لطلاب لتغيير كلمة المرور

## 5. POST /api/v1/auth/reset-password

### المسار
POST /api/v1/auth/reset-password

### الوصف
إعادة تعيين كلمة المرور لمستخدم محدد (للمدير فقط). يمسح أيضًا قفل الحساب للمستخدم المستهدف.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط (equireAdmin)
- **معدل الطلبات:** لا

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| userId | string | نعم | معرف المستخدم المستهدف |
| 
ewPassword | string | نعم | كلمة المرور الجديدة (قوية) |

### المخرجات

**نجاح (200):**
\\\json
{
  "message": "Password reset for \"أحمد محمد\""
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | userId is required |
| 400 | Bad Request | New password is required |
| 400 | Bad Request | Password must be at least 8 characters... |
| 403 | Forbidden | You can only reset passwords for users in your own school |
| 404 | Not Found | User not found |

### الأمان

- التحقق من الصلاحية (Admin فقط)
- التحقق من المدرسة (عزل البيانات)
- مسح محاولات الفشل للمستخدم المستهدف
- تطبيق سياسة قوة كلمة المرور
- تسجيل حدث إعادة التعيين في سجل التدقيق

### مثال CURL

\\\ash
curl -X POST https://api.dhad.studio/api/v1/auth/reset-password \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d '{
    "userId": "user-uuid",
    "newPassword": "NewSecurePass789"
  }'
\\\

### التحسينات المقترحة

- إضافة خيار إنشاء كلمة مرور عشوائية قوية
- إرسال كلمة المرور الجديدة عبر البريد الإلكتروني الآمن
- طلب تأكيد إضافي للحساسية العالية

## 6. POST /api/v1/auth/logout

### المسار
POST /api/v1/auth/logout

### الوصف
تسجيل الخروج — يزيد رقم إصدار الرمز (tokenVersion) لإبطال جميع رموز JWT النشطة للمستخدم.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** أي دور
- **معدل الطلبات:** لا

### المدخلات

لا توجد معاملات في جسم الطلب.

### المخرجات

**نجاح (200):**
\\\json
{
  "success": true,
  "message": "Logged out successfully"
}
\\\

### الأمان

- إبطال جميع الرموز بزيادة 	okenVersion
- تسجيل حدث تسجيل الخروج في سجل التدقيق
- لا يمكن إعادة استخدام الرموز القديمة بعد تسجيل الخروج

### مثال CURL

\\\ash
curl -X POST https://api.dhad.studio/api/v1/auth/logout \
  -H "Authorization: Bearer <token>"
\\\

### التحسينات المقترحة

- إضافة قائمة سوداء للرموز في Redis
- إمكانية تسجيل الخروج من أجهزة محددة فقط
- إشعارات عند تسجيل الخروج

---

# رمز التحقق لمرة واحدة (OTP) — otp.js

## 7. POST /api/v1/auth/request-otp

### المسار
POST /api/v1/auth/request-otp

### الوصف
طلب رمز تحقق لمرة واحدة (OTP) للطلاب لتفعيل حساباتهم. يُرسل الرمز إلى البريد الإلكتروني المسجل.

### المتطلبات
- **المصادقة:** عام
- **معدل الطلبات:** نعم — strictRateLimit('otp-request') (3 طلبات/دقيقة)
- **الصلاحية:** لا شيء (عام)

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| email | string | نعم | البريد الإلكتروني المسجل للطالب |

### المخرجات

**نجاح (200):**
\\\json
{
  "success": true,
  "message": "تم إرسال رمز التحقق إلى بريدك الإلكتروني"
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | البريد الإلكتروني مطلوب |
| 400 | Bad Request | صيغة البريد الإلكتروني غير صحيحة |
| 429 | Too Many Requests | Rate limit exceeded |

### الأمان

- معدل طلبات صارم (3/دقيقة)
- التحقق من صيغة البريد الإلكتروني
- تعقيم المدخلات

### مثال CURL

\\\ash
curl -X POST https://api.dhad.studio/api/v1/auth/request-otp \
  -H "Content-Type: application/json" \
  -d '{
    "email": "student@school.com"
  }'
\\\

### التحسينات المقترحة

- إضافة حماية من هجمات التعداد (enumeration) — عدم إفشاء وجود البريد
- استخدام Redis لتخزين OTP مع انتهاء صلاحية
- إضافة خيار إرسال OTP عبر SMS

## 8. POST /api/v1/auth/verify-otp

### المسار
POST /api/v1/auth/verify-otp

### الوصف
التحقق من رمز OTP وتفعيل حساب الطالب مع تعيين كلمة المرور.

### المتطلبات
- **المصادقة:** عام
- **معدل الطلبات:** نعم — strictRateLimit('otp-verify') (5 طلبات/دقيقة)
- **الصلاحية:** لا شيء (عام)

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| email | string | نعم | البريد الإلكتروني |
| otp | string | نعم | 6 أرقام |
| password | string | نعم | كلمة المرور الجديدة |

### المخرجات

**نجاح (200):**
\\\json
{
  "success": true,
  "message": "تم تفعيل الحساب بنجاح",
  "accessToken": "eyJ...",
  "refreshToken": "eyJ..."
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | البريد الإلكتروني ورمز التحقق مطلوبان |
| 400 | Bad Request | كلمة المرور مطلوبة |
| 400 | Bad Request | رمز التحقق يجب أن يكون 6 أرقام |
| 400 | Bad Request | رمز التحقق منتهي الصلاحية |
| 401 | Unauthorized | رمز التحقق غير صالح |
| 404 | Not Found | لا يوجد دعوة معلقة لهذا البريد الإلكتروني |
| 429 | Too Many Requests | Rate limit exceeded |

### الأمان

- معدل طلبات صارم (5/دقيقة) — حماية من هجمات القوة العمياء
- التحقق من أن OTP مكون من 6 أرقام
- انتهاء صلاحية OTP

### مثال CURL

\\\ash
curl -X POST https://api.dhad.studio/api/v1/auth/verify-otp \
  -H "Content-Type: application/json" \
  -d '{
    "email": "student@school.com",
    "otp": "123456",
    "password": "MySecurePass123"
  }'
\\\

### التحسينات المقترحة

- إضافة حد أقصى لمحاولات التحقق (3 محاولات ثم حظر)
- إعادة توجيه إلى صفحة تعيين كلمة المرور
- إرسال إشعار بنجاح التفعيل

---

# لوحة المعلومات (Analytics) — analytics.js

## 9. GET /api/v1/analytics/leaderboard

### المسار
GET /api/v1/analytics/leaderboard

### الوصف
لوحة شرف الطلاب — ترتيب الطلاب بناءً على نقاط الخبرة (XP) والإنجازات. مدعوم بتخزين Redis المؤقت.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN
- **معدل الطلبات:** لا (معدل API العام)

### المدخلات

**معاملات الاستعلام (Query Params):**
| المعامل | النوع | إلزامي | الافتراضي | الوصف |
|---|---|---|---|---|
| schoolId | string | لا | مدرسة المستخدم | معرف المدرسة (للمدير) |
| limit | number | لا | 50 | عدد الطلاب في الترتيب |
| efresh | boolean | لا | false | إجبار التحديث من قاعدة البيانات |

### المخرجات

**نجاح (200):**
\\\json
{
  "school": { "id": "uuid-string" },
  "leaderboard": [
    {
      "studentId": "uuid",
      "studentName": "أحمد",
      "rank": 1,
      "totalXP": 12500,
      "currentLevel": 7,
      "currentStreak": 5,
      "totalChallenges": 45,
      "passedChallenges": 40,
      "challengeCompletionRate": 89,
      "avgChallengeScore": 92,
      "totalProjects": 3,
      "avgProjectScore": 85,
      "honorScore": 88
    }
  ],
  "generatedAt": "2026-07-10T12:00:00.000Z",
  "totalStudents": 150,
  "source": "cache",
  "analyticsPeriod": {
    "type": "all_time",
    "from": "2026-01-01",
    "to": "2026-07-10"
  }
}
\\\

**خطأ (403):**
\\\json
{
  "error": "Forbidden",
  "message": "You can only access data from your own school"
}
\\\

### الأمان

- عزل المدارس — لا يمكن للمعلم رؤية مدارس أخرى
- استخدام Redis cache لتحسين الأداء
- التحقق من الصلاحية (Teacher/Admin)

### معاملات الترتيب (Honor Score)

honorScore = (completionRate × 0.3) + (avgChallengeScore × 0.25) + (avgProjectScore × 0.25) + (XP/100 × 0.2)

### مثال CURL

\\\ash
curl -X GET "https://api.dhad.studio/api/v1/analytics/leaderboard?limit=20&schoolId=school-uuid" \
  -H "Authorization: Bearer <token>"
\\\

### التحسينات المقترحة

- إضافة ترشيح حسب الفصل (classroomId)
- إضافة تصدير CSV للترتيب
- إضافة فترة زمنية مخصصة (شهر، أسبوع)
- دعم المقارنة بين المعلمين
- إضافة إحصائيات اتجاه (trend) للطلاب

## 10. GET /api/v1/analytics/attendance-summary

### المسار
GET /api/v1/analytics/attendance-summary

### الوصف
ملخص حضور الطلاب لليوم الحالي مع مقارنة باليوم السابق.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| المعامل | النوع | إلزامي | الوصف |
|---|---|---|---|
| schoolId | string | لا | معرف المدرسة (للمدير) |
| classroomId | string | لا | معرف الفصل (لتصفية فصل محدد) |

### المخرجات

**نجاح (200):**
\\\json
{
  "school": { "id": "uuid" },
  "period": {
    "today": "2026-07-10",
    "yesterday": "2026-07-09"
  },
  "attendance": {
    "today": {
      "totalPresent": 85,
      "totalExpected": 100,
      "rate": 85,
      "statusBreakdown": {
        "PRESENT": 80,
        "LATE": 5,
        "ABSENT": 15
      }
    },
    "yesterday": {
      "totalPresent": 90,
      "totalExpected": 100,
      "rate": 90
    },
    "trend": {
      "absoluteChange": -5,
      "percentageChange": -5,
      "trend": "DECLINING"
    }
  },
  "lastUpdated": "2026-07-10T12:00:00.000Z"
}
\\\

### مثال CURL

\\\ash
curl -X GET "https://api.dhad.studio/api/v1/analytics/attendance-summary?classroomId=class-uuid" \
  -H "Authorization: Bearer <token>"
\\\

## 11. GET /api/v1/analytics/attendance-history

### المسار
GET /api/v1/analytics/attendance-history

### الوصف
سجل الحضور التاريخي لفصل دراسي محدد.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| المعامل | النوع | إلزامي | الافتراضي | الوصف |
|---|---|---|---|---|
| classroomId | string | نعم | — | معرف الفصل |
| days | number | لا | 30 | عدد الأيام السابقة |

### المخرجات

\\\json
{
  "success": true,
  "attendance": [
    {
      "date": "2026-07-10",
      "present": 85,
      "absent": 15,
      "late": 5,
      "total": 100,
      "percentage": 85
    }
  ],
  "totalStudents": 100
}
\\\

## 12. POST /api/v1/analytics/attendance

### المسار
POST /api/v1/analytics/attendance

### الوصف
تسجيل حضور الطلاب في فصل دراسي. يدعم الإدراج أو التحديث (upsert).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| classroomId | string | نعم | معرف الفصل |
| date | string | لا | التاريخ (YYYY-MM-DD) |
| ecords | array | نعم | قائمة سجلات الحضور |

كل سجل في records:
| الحقل | النوع | إلزامي | الافتراضي | الوصف |
|---|---|---|---|---|
| studentId | string | نعم | — | معرف الطالب |
| status | string | لا | PRESENT | PRESENT, ABSENT, LATE |
| 
otes | string | لا | "" | ملاحظات |

### المخرجات

\\\json
{
  "success": true,
  "message": "Attendance recorded for 30 students",
  "count": 30,
  "date": "2026-07-10"
}
\\\

## 13. GET /api/v1/analytics/progress

### المسار
GET /api/v1/analytics/progress

### الوصف
مصفوفة تقدم الطلاب — تعرض نسبة إكمال كل طالب للتحديات في أول 5 دروس مع تحديد التمارين الأكثر فشلاً.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "progress": {
    "averageProgress": 65,
    "averageXP": 4500,
    "studentCount": 100,
    "totalChallenges": 25
  },
  "analytics": {
    "classProgress": {
      "overallCompletionRate": 65,
      "studentsByProgress": {
        "0-20%": 10, "21-40%": 15, "41-60%": 25,
        "61-80%": 30, "81-100%": 20
      }
    },
    "progressMatrix": [
      {
        "studentId": "uuid",
        "studentName": "أحمد",
        "overallProgress": 75,
        "exercisesCompleted": 18,
        "totalExercises": 25
      }
    ]
  },
  "mostFailedExercises": []
}
\\\

## 14. GET /api/v1/analytics/dashboard

### المسار
GET /api/v1/analytics/dashboard

### الوصف
لوحة معلومات المعلم — إحصائيات مجمعة: عدد الفصول، الطلاب، التقييمات، آخر التقديمات، وأفضل الطلاب.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "success": true,
  "dashboard": {
    "classrooms": {
      "total": 5,
      "active": 4,
      "list": [
        { "id": "uuid", "name": "الصف الخامس", "gradeLevel": "5", "subject": "البرمجة", "isActive": true }
      ]
    },
    "students": { "total": 120, "active": 95, "inactive": 25 },
    "assessments": { "total": 8 },
    "recentSubmissions": [
      {
        "id": "uuid",
        "studentName": "أحمد",
        "challengeTitle": "طباعة مرحبا",
        "passed": true,
        "score": 95,
        "createdAt": "2026-07-10T11:00:00.000Z"
      }
    ],
    "topStudents": [
      { "rank": 1, "name": "أحمد", "xp": 12500, "level": 7 }
    ]
  }
}
\\\

## 15. GET /api/v1/analytics/classrooms

### المسار
GET /api/v1/analytics/classrooms

### الوصف
قائمة فصول المعلم مع إحصائيات (عدد الطلاب، متوسط XP، آخر التقييمات).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "success": true,
  "classrooms": [
    {
      "id": "uuid", "name": "الصف الخامس", "description": "فصل البرمجة",
      "gradeLevel": "5", "subject": "البرمجة", "maxStudents": 40,
      "isActive": true, "studentCount": 30, "averageXP": 3200,
      "recentAssessments": 3, "createdAt": "2026-01-15T08:00:00.000Z"
    }
  ],
  "total": 5
}
\\\

## 16. POST /api/v1/analytics/classrooms

### المسار
POST /api/v1/analytics/classrooms

### الوصف
إنشاء فصل دراسي جديد. يتحقق من عدم وجود اسم مكرر في نفس المدرسة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| 
ame | string | نعم | اسم الفصل (فريد ضمن المدرسة) |
| subject | string | لا | المادة الدراسية |
| gradeLevel | string | لا | المستوى الدراسي |
| description | string | لا | وصف الفصل |
| maxStudents | number | لا | 40 (افتراضي) |

### المخرجات (201)

\\\json
{
  "success": true,
  "message": "Classroom created successfully",
  "classroom": {
    "id": "uuid", "name": "الصف الخامس",
    "subject": "البرمجة", "gradeLevel": "5",
    "maxStudents": 40, "isActive": true
  }
}
\\\

## 17. GET /api/v1/analytics/assessments

### المسار
GET /api/v1/analytics/assessments

### الوصف
قائمة جميع التقييمات الخاصة بفصول المعلم مع الإحصائيات (عدد النتائج، متوسط الدرجات).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "success": true,
  "assessments": [
    {
      "id": "uuid", "title": "اختبار منتصف الفصل",
      "classroomId": "uuid",
      "classroom": { "id": "uuid", "name": "الصف الخامس" },
      "assessmentType": "CODING", "allowedTime": 60,
      "startTime": "2026-07-11T08:00:00.000Z",
      "endTime": "2026-07-11T09:00:00.000Z",
      "totalPoints": 100, "passingScore": 60,
      "isPublished": true, "isActive": false,
      "questionCount": 5, "resultCount": 28,
      "averageScore": 72
    }
  ],
  "total": 8
}
\\\

## 18. GET /api/v1/analytics/classroom/:classId/export

### المسار
GET /api/v1/analytics/classroom/:classId/export

### الوصف
تصدير تقرير شامل للفصل بتنسيق CSV. يشمل معلومات الطلاب، XP، الحضور، التقييمات، والتنبيهات الأمنية.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN (معلم الفصل فقط)

### المخرجات

**نجاح (200):** محتوى CSV مع الرؤوس التالية:
Student ID,Student Name,Email,National ID,Total XP,Current Level,Current Streak,Total Submissions,Passed Submissions,Pass Rate (%),Average Score,Attendance Rate (%),Assessment Average,Security Alerts

**رؤوس HTTP:**
Content-Type: text/csv
Content-Disposition: attachment; filename="classroom_report_NAME_DATE.csv"

### الأمان

- التحقق من ملكية الفصل (Teacher يرى فصوله فقط)
- تعقيم حقول CSV لمنع حقن الصيغ (Formula Injection)

### مثال CURL

\\\ash
curl -X GET "https://api.dhad.studio/api/v1/analytics/classroom/class-uuid/export" \
  -H "Authorization: Bearer <token>" \
  -o classroom_report.csv
\\\

### التحسينات المقترحة

- إضافة خيار تصدير بتنسيق PDF أو Excel
- إضافة معاملات لاختيار الحقول المطلوب تصديرها
- إضافة توقيع رقمي للتقرير

## 19. GET /api/v1/challenges (from analytics.js)

### المسار
GET /api/v1/challenges

### الوصف
قائمة بجميع التحديات المنشورة. متاح للمستخدمين الموثَّقين.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** أي دور (مع authentication فقط)

### المدخلات

| المعامل | النوع | إلزامي | الوصف |
|---|---|---|---|
| difficulty | string | لا | BEGINNER, INTERMEDIATE, ADVANCED, EXPERT, all |

### المخرجات

\\\json
{
  "success": true,
  "challenges": [
    {
      "id": "uuid", "title": "طباعة مرحبا",
      "description": "...", "starterCode": "...",
      "expectedOutput": "مرحبا بالعالم",
      "difficulty": "BEGINNER", "challengeType": "CODING",
      "points": 100, "tier": 1, "estimatedTime": 15,
      "isSystem": true, "xpReward": 100, "category": "BEGINNER"
    }
  ],
  "total": 30
}
\\\

## 20. GET /api/v1/lessons/:lessonId/tests

### المسار
GET /api/v1/lessons/:lessonId/tests

### الوصف
استعراض حالات الاختبار (test cases) لدرس معين. للمعلمين والمدراء فقط.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "success": true,
  "lessonId": "uuid",
  "lessonTitle": "الدرس الأول",
  "message": "Lesson test cases endpoint active"
}
\\\

## 21. GET /api/v1/challenges/tier/:tierId

### المسار
GET /api/v1/challenges/tier/:tierId

### الوصف
قائمة تحديات مستوى معين مع روابط آمنة لحالات الاختبار.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### ملاحظات أمنية
- لا يتم كشف expectedOutput في قائمة التحديات
- عزل المدرسة — المعلم يرى فقط تحديات مدرسته
- روابط التحميل تحتاج صلاحية إضافية

---

# التحديات (Challenges) — challenges.js

## 22. GET /api/v1/challenges (from challenges.js)

### المسار
GET /api/v1/challenges

### الوصف
قائمة التحديات — يستخدم listChallenges. جميع المسارات في هذا الملف تتطلب المصادقة.

### المتطلبات
- **المصادقة:** نعم (JWT) — جميع المسارات
- **الصلاحية:** أي دور موثَّق

### المخرجات

مماثلة للنقطة 19.

## 23. GET /api/v1/challenges/daily

### المسار
GET /api/v1/challenges/daily

### الوصف
عرض التحدي اليومي المحدد من قبل المعلم.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** أي دور موثَّق

### المخرجات

\\\json
{
  "success": true,
  "challenge": { "...": "..." }
}
\\\

## 24. POST /api/v1/challenges

### المسار
POST /api/v1/challenges

### الوصف
إنشاء تحدي جديد. للمعلم أو المدير فقط.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN (equireRole("TEACHER", "ADMIN"))

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| 	itle | string | نعم | عنوان التحدي |
| description | string | لا | وصف التحدي |
| difficulty | string | نعم | BEGINNER, INTERMEDIATE, ADVANCED, EXPERT |
| points | number | نعم | عدد النقاط |
| 	ier | number | لا | المستوى (1-4) |
| order | number | لا | الترتيب داخل المستوى |
| starterCode | string | لا | الكود الابتدائي |
| expectedOutput | string | لا | المخرجات المتوقعة |
| equirements | object | لا | متطلبات الكود (للتحقق) |

### المخرجات (201)

\\\json
{
  "success": true,
  "challenge": { "...": "..." }
}
\\\

## 25. PUT /api/v1/challenges/:id

### المسار
PUT /api/v1/challenges/:id

### الوصف
تحديث تحدي موجود.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات
نفس حقول الإنشاء (اختيارية للتحديث).

### المخرجات

\\\json
{
  "success": true,
  "challenge": { "...": "..." }
}
\\\

## 26. DELETE /api/v1/challenges/:id

### المسار
DELETE /api/v1/challenges/:id

### الوصف
حذف تحدي.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "success": true,
  "message": "Challenge deleted"
}
\\\

## 27. POST /api/v1/challenges/:id/publish

### المسار
POST /api/v1/challenges/:id/publish

### الوصف
نشر تحدي ليكون متاحاً للطلاب.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "success": true,
  "message": "Challenge published"
}
\\\

## 28. POST /api/v1/challenges/:id/set-daily

### المسار
POST /api/v1/challenges/:id/set-daily

### الوصف
تعيين تحدي معين ليكون التحدي اليومي.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

\\\json
{
  "success": true,
  "message": "Daily challenge set"
}
\\\

## 29. POST /api/v1/submit-solution

### المسار
POST /api/v1/submit-solution

### الوصف
تقديم حل لتحدي (للطلاب فقط). محدود بـ 30 محاولة في الساعة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط
- **معدل الطلبات:** نعم — submissionRateLimit (30/ساعة)

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| challengeId | string | نعم | معرف التحدي |
| code | string | نعم | الكود المقدم |
| language | string | لا | لغة البرمجة (daad) |

### المخرجات

\\\json
{
  "success": true,
  "passed": true,
  "submissionId": "uuid",
  "score": 95,
  "message": "تم حل التحدي بنجاح"
}
\\\

## 30. GET /api/v1/submissions

### المسار
GET /api/v1/submissions

### الوصف
سجل الحلول المقدمة. متاح لجميع المستخدمين الموثَّقين.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** أي دور موثَّق

### المخرجات

\\\json
{
  "success": true,
  "submissions": [...],
  "total": 50
}
\\\

---

# التحقق من التحديات (Challenge Verify) — challenge.js

## 31. POST /api/v1/challenge/verify

### المسار
POST /api/v1/challenge/verify

### الوصف
التحقق من إكمال التحدي باستخدام التوقيع المشفر HMAC-SHA256. نظام مقاوم للعبث والغش — يتحقق من التوقيع والتوقيت والبيانات.

### المتطلبات
- **المصادقة:** لا (يعتمد على التوقيع المشفر)
- **معدل الطلبات:** نعم — strictRateLimit('auth') (10/دقيقة)
- **التحقق الإضافي:** checkAccountLocked, alidateChallengeSecurity

**سلسلة الوسائط:**
\\\
POST /challenge/verify
  → strictRateLimit('auth')
  → checkAccountLocked
  → validateChallengeSecurity
  → verifyChallengeCompletion
\\\

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| studentId | string | نعم | معرف الطالب |
| 	askId | string | نعم | معرف المهمة (التحدي) |
| 	imestamp | string | نعم | الطابع الزمني (milliseconds) — يجب أن يكون خلال 5 دقائق |
| signature | string | نعم | توقيع HMAC-SHA256 (64 حرف hex) |

### معادلة التوقيع

HMAC-SHA256(HMAC_SECRET, studentId + ":" + taskId + ":" + timestamp)

### المخرجات

**نجاح (200):**
\\\json
{
  "success": true,
  "confirmationToken": "abcdef123456...",
  "message": "تم التحقق من اكتمال التحدي بنجاح!"
}
\\\

**أخطاء محتملة:**

| رمز الحالة | كود الخطأ | الرسالة |
|---|---|---|
| 400 | Bad Request | يجب إرسال studentId، taskId، timestamp، و signature معًا |
| 400 | Bad Request | الطابع الزمني غير صالح |
| 400 | Bad Request | تنسيق التوقيع غير صالح |
| 403 | Token Expired | الطابع الزمني منتهي الصلاحية (أكثر من 5 دقائق) |
| 403 | Forbidden | التوقيع غير صالح. تم اكتشاف محاولة غش! |
| 423 | Account Locked | حساب الطالب مقفل |
| 429 | Too Many Requests | Rate limit exceeded |

### الأمان (متقدم)

1. **مقارنة زمنية ثابتة (Timing Safe):** استخدام crypto.timingSafeEqual لمنع هجمات التوقيت
2. **منع إعادة التشغيل (Replay Prevention):** التحقق من أن الطابع الزمني ضمن 5 دقائق
3. **قفل الحساب:** التحقق من عدم قفل حساب الطالب
4. **تنسيق التوقيع:** التحقق من صيغة HEX (64 حرف)
5. **سجل التدقيق:** تسجيل كل محاولة تحقق ناجحة وفاشلة
6. **تحديد معدل الطلبات:** 10 محاولات فقط لكل دقيقة
7. **رمز تأكيد إضافي:** إنشاء confirmationToken من HMAC إضافي

### مثال CURL

\\\ash
# إنشاء التوقيع
HMAC_SECRET="your-secret-key"
SIGNATURE=\

curl -X POST https://api.dhad.studio/api/v1/challenge/verify \
  -H "Content-Type: application/json" \
  -d '{
    "studentId": "student-uuid",
    "taskId": "challenge-uuid",
    "timestamp": "'\'",
    "signature": "'\'"
  }'
\\\

### التحسينات المقترحة

- إضافة تخزين دائم لسجلات التحقق في قاعدة البيانات
- إضافة إشعارات عند اكتشاف محاولات غش
- استخدام مفاتيح HMAC محددة لكل طالب
- إضافة آلية لإبطال الرموز التأكيدية
- إضافة فحص تكامل إضافي (checksum) للكود المقدم
- استخدام مفاتيح مؤقتة (ephemeral keys) مع صلاحية محدودة

## 46. POST /api/v1/workspace/save

### المسار
`POST /api/v1/workspace/save`

### الوصف
حفظ حالة محرر الكود الحالية (الكود واللغة) في المساحة السحابية.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `challengeId` | string | نعم | معرف التحدي |
| `code` | string | نعم | الكود المراد حفظه |
| `language` | string | لا | لغة البرمجة (daad) |

### المخرجات
```json
{
  "success": true,
  "message": "Code saved successfully",
  "workspace": {
    "id": "uuid", "challengeId": "challenge-uuid",
    "lastSavedAt": "2026-07-10T12:00:00.000Z"
  }
}
```
### 47. GET /api/v1/workspace/load/:challengeId

### المسار
`GET /api/v1/workspace/load/:challengeId`

### الوصف
تحميل حالة محرر الكود المحفوظة مسبقاً لتحدي معين.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط

### المخرجات

```json
{
  "success": true,
  "workspace": {
    "id": "uuid", "challengeId": "challenge-uuid",
    "savedCode": "print(\"مرحبا بالعالم\")",
    "language": "daad",
    "lastSavedAt": "2026-07-10T12:00:00.000Z"
  }
}
```

**إذا لم يوجد حفظ:**
```json
{
  "success": true,
  "workspace": null,
  "message": "No saved code found"
}
```

### 48. GET /api/v1/workspace/list

### المسار
`GET /api/v1/workspace/list`

### الوصف
قائمة جميع المساحات السحابية المحفوظة للطالب مع آخر وقت حفظ.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط

### المخرجات

```json
{
  "success": true,
  "workspaces": [
    {
      "id": "uuid", "challengeId": "challenge-uuid",
      "language": "daad",
      "lastSavedAt": "2026-07-10T12:00:00.000Z",
      "createdAt": "2026-07-01T08:00:00.000Z"
    }
  ],
  "total": 5
}
```

### 49. DELETE /api/v1/workspace/:challengeId

### المسار
`DELETE /api/v1/workspace/:challengeId`

### الوصف
حذف مساحة سحابية محفوظة لتحدي محدد.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط

### المخرجات

```json
{
  "success": true,
  "message": "Workspace deleted",
  "deleted": true
}
```

### التحسينات المقترحة للمساحة السحابية

- إضافة مزامنة تلقائية (Auto-save) كل 30 ثانية
- دعم حفظ إصدارات متعددة (Version history)
- إضافة مشاركة المساحات بين الطلاب
- تصدير الكود كملف للتحميل

---

# تنبيهات الأمان (Security Alerts) — student.js

### 50. GET /api/v1/analytics/security-alerts

### المسار
`GET /api/v1/analytics/security-alerts`

### الوصف
عرض التنبيهات الأمنية للمدرسة. للمعلمين والمدراء. تشمل التنبيهات: محاولات غش، أنشطة مشبوهة، اختراقات محتملة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| المعامل | النوع | إلزامي | الافتراضي | الوصف |
|---|---|---|---|---|
| `severity` | string | لا | — | LOW, MEDIUM, HIGH, CRITICAL |
| `resolved` | boolean | لا | — | true/false |
| `limit` | number | لا | 50 | عدد التنبيهات |

### المخرجات

```json
{
  "success": true,
  "alerts": [
    {
      "id": "uuid", "schoolId": "uuid",
      "severity": "HIGH", "resolved": false,
      "createdAt": "2026-07-10T12:00:00.000Z"
    }
  ],
  "summary": { "HIGH_unresolved": 3, "MEDIUM_resolved": 5 },
  "unresolvedCount": 10,
  "total": 15
}
```

### 51. POST /api/v1/analytics/security-alerts/:id/resolve

### المسار
`POST /api/v1/analytics/security-alerts/:id/resolve`

### الوصف
حل تنبيه أمني (تعيينه كمحلول). يتحقق من أن المستخدم لديه صلاحية الوصول للمدرسة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

```json
{
  "success": true,
  "message": "Alert resolved",
  "alert": {
    "id": "uuid", "resolved": true,
    "resolvedBy": "uuid",
    "resolvedAt": "2026-07-10T12:00:00.000Z"
  }
}
```

### 52. POST /api/v1/submissions/bulk-report

### المسار
`POST /api/v1/submissions/bulk-report`

### الوصف
تقديم تقرير مجمع (Bulk) للتليمتري (بيانات الاستخدام). يدعم تنسيق MessagePack الثنائي و JSON. **لا يُحتسب XP من هذه التقارير — لأغراض التسجيل فقط.**

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط

### المدخلات

**التنسيق:** مصفوفة JSON أو MessagePack buffer

```json
[
  {
    "challengeId": "uuid",
    "exerciseId": "uuid",
    "code": "print(\"مرحبا\")",
    "language": "daad"
  }
]
```

**رؤوس HTTP (لـ MessagePack):**
| الرأس | القيمة |
|---|---|
| Content-Type | application/msgpack |
| x-telemetry-signature | HMAC-SHA256 |
| x-telemetry-timestamp | timestamp (ms) |

### المخرجات

```json
{
  "success": true,
  "message": "Bulk sync completed: 10/10 submissions processed",
  "results": {
    "total": 10, "successful": 10, "failed": 0,
    "xpAwarded": 0, "errors": []
  },
  "timestamp": "2026-07-10T12:00:00.000Z"
}
```

### الأمان

- **الحد الأقصى:** 100 عنصر لكل طلب (منع DoS)
- **بدون XP:** لا يُمنح XP من تقارير العميل أبداً
- **توقيع HMAC:** مطلوب للتحقق من سلامة البيانات
- **جميع الدرجات تُسجل كصفر** حتى التحقق من الخادم
- **التخزين الأصلي:** حفظ البيانات الأصلية للتدقيق

---

# إدارة الطلاب (Student Management) — studentManagement.js

### 53. GET /api/v1/auth/classrooms

### المسار
`GET /api/v1/auth/classrooms`

### الوصف
قائمة جميع الفصول الدراسية (لشاشة دخول الطالب). متاح للعموم.

### المتطلبات
- **المصادقة:** لا (عام)
- **الصلاحية:** عام

### المخرجات

```json
{
  "success": true,
  "classrooms": [
    {
      "id": "uuid", "name": "الصف الخامس",
      "school": "مدرسة القدس", "studentCount": 30
    }
  ]
}
```

### 54. GET /api/v1/auth/classrooms/:classroom_id/students

### المسار
`GET /api/v1/auth/classrooms/:classroom_id/students`

### الوصف
قائمة الطلاب في فصل معين (لشاشة دخول الطالب). يُظهر فقط الاسم والمعرف.

### المتطلبات
- **المصادقة:** لا (عام)
- **الصلاحية:** عام

### المخرجات

```json
{
  "success": true,
  "students": [
    { "id": "uuid", "name": "أحمد محمد" }
  ]
}
```

### ملاحظات أمنية
- لا يُكشف الرقم الوطني (nationalId) في المسارات العامة

### 55. POST /api/v1/auth/student-login

### المسار
`POST /api/v1/auth/student-login`

### الوصف
دخول الطالب باستخدام معرف الفصل ومعرف الطالب وكلمة المرور. مع حماية من هجمات القوة العمياء (قفل الحساب + تأخير تصاعدي).

### المتطلبات
- **المصادقة:** لا (عام)
- **معدل الطلبات:** نعم — strictRateLimit('public')
- **الصلاحية:** عام

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `classroomId` | string | نعم | معرف الفصل |
| `studentId` | string | نعم | معرف الطالب |
| `password` | string | نعم | كلمة المرور |

### آليات الأمان (قبل المعالجة)

1. **التحقق من قفل الحساب:** إذا كان الحساب مقفلاً → 423
2. **التأخير التصاعدي:** انتظار تصاعدي (1ث–16ث) حسب عدد المحاولات

### المخرجات

```json
{
  "success": true,
  "accessToken": "eyJ...",
  "refreshToken": "eyJ...",
  "expiresIn": 3600,
  "userId": "uuid",
  "username": "أحمد محمد",
  "role": "STUDENT",
  "profile": {
    "id": "uuid", "name": "أحمد محمد",
    "nationalId": "12345", "role": "STUDENT",
    "classroom": { "id": "uuid", "name": "الصف الخامس", "schoolId": "uuid" },
    "school": { "id": "uuid", "name": "مدرسة القدس" }
  }
}
```

### مثال CURL

```bash
curl -X POST https://api.dhad.studio/api/v1/auth/student-login \
  -H "Content-Type: application/json" \
  -d '{
    "classroomId": "class-uuid",
    "studentId": "student-uuid",
    "password": "StudentPass123"
  }'
```

### 56. POST /api/v1/teacher/students

### المسار
`POST /api/v1/teacher/students`

### الوصف
إنشاء حساب طالب جديد. ينشئ تلقائياً اسم المستخدم (اسم الطالب + رمز المدرسة) وملف الطالب (StudentProfile).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `name` | string | نعم | اسم الطالب (≤ 100 حرف) |
| `classroomId` | string | لا | معرف الفصل |
| `nationalId` | string | لا | الرقم الوطني |
| `password` | string | نعم | كلمة المرور |

### المخرجات (201)

```json
{
  "success": true,
  "message": "تم إنشاء حساب الطالب \"أحمد\" بنجاح",
  "student": {
    "id": "uuid", "name": "أحمد",
    "username": "ahmedSCHL",
    "classroom": { "id": "uuid", "name": "الصف الخامس" },
    "createdAt": "2026-07-10T12:00:00.000Z"
  },
  "credentials": {
    "username": "ahmedSCHL",
    "note": "تم تعيين كلمة المرور من المعلم"
  }
}
```

### 57. POST /api/v1/teacher/students/bulk

### المسار
`POST /api/v1/teacher/students/bulk`

### الوصف
إنشاء حسابات طلاب متعددة دفعة واحدة (max 50). معالجة الأخطاء والازدواجات لكل طالب على حدة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `students` | array | نعم | قائمة بأسماء الطلاب (strings) أو objects |
| `classroomId` | string | لا | معرف الفصل |
| `password` | string | نعم | كلمة المرور لجميع الطلاب |

**الحد الأقصى:** 50 طالباً لكل طلب.

### المخرجات (201)

```json
{
  "success": true,
  "message": "تم إنشاء 48 من 50 طالب",
  "results": {
    "successful": [{ "id": "uuid", "name": "أحمد", "username": "ahmedSCHL" }],
    "failed": [{ "name": "محمد", "reason": "اسم مكرر" }],
    "total": 50
  }
}
```

### 58. GET /api/v1/teacher/students

### المسار
`GET /api/v1/teacher/students`

### الوصف
قائمة الطلاب في فصول المعلم مع إحصائيات المستوى و XP.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| المعامل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `classroomId` | string | لا | تصفية حسب فصل محدد |

### المخرجات

```json
{
  "success": true,
  "students": [
    {
      "id": "uuid", "name": "أحمد",
      "username": "ahmedSCHL",
      "classroom": { "id": "uuid", "name": "الصف الخامس" },
      "xp": 12500, "level": 7, "streak": 5,
      "isActive": true, "createdAt": "2026-01-15T08:00:00.000Z"
    }
  ],
  "total": 30
}
```

### 59. DELETE /api/v1/teacher/students/:id

### المسار
`DELETE /api/v1/teacher/students/:id`

### الوصف
حذف طالب (تعطيل ناعم — isActive = false). يتحقق من أن الطالب في فصل يخص المعلم.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

```json
{
  "success": true,
  "message": "تم حذف الطالب \"أحمد\""
}
```

### 60. POST /api/v1/teacher/students/assign

### المسار
`POST /api/v1/teacher/students/assign`

### الوصف
نقل طالب إلى فصل آخر. يتحقق من أن الطالب في نفس المدرسة والفصل يخص نفس المعلم.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `studentId` | string | نعم | معرف الطالب |
| `classroomId` | string | نعم | معرف الفصل الجديد |

### المخرجات

```json
{
  "success": true,
  "message": "تم نقل الطالب \"أحمد\" إلى فصل \"الصف السادس\""
}
```
---

# المقررات الدراسية (Courses) — courses.js

## 61. GET /api/v1/student/courses

### المسار
`GET /api/v1/student/courses`

### الوصف
قائمة المقررات الدراسية الإضافية المتاحة للطالب مع حالة التسجيل والفتح.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط

### المخرجات

```json
{
  "success": true,
  "courses": [
    {
      "id": "uuid", "title": "مقدمة في الذكاء الاصطناعي",
      "description": "...", "icon": "robot",
      "category": "AI", "order": 1, "unlockTier": 3,
      "lessonCount": 12,
      "enrolled": false, "unlocked": true,
      "status": "UNLOCKED"
    }
  ]
}
```

### منطق الفتح

- `ENROLLED` — الطالب مسجل في المقرر
- `UNLOCKED` — المقرر متاح (أكمل الطالب المستوى المطلوب)
- `LOCKED` — المقرر مقفل (لم يكمل المستوى المطلوب بعد)

## 62. POST /api/v1/student/courses/:courseId/enroll

### المسار
`POST /api/v1/student/courses/:courseId/enroll`

### الوصف
التسجيل في مقرر دراسي. يتحقق من أن متطلبات الفتح مستوفاة (المستوى المطلوب من التحديات الرئيسية).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط

### المخرجات

```json
{
  "success": true,
  "enrollment": {
    "studentId": "uuid",
    "courseId": "uuid",
    "enrolledAt": "2026-07-10T12:00:00.000Z"
  }
}
```

## 63. GET /api/v1/student/courses/:courseId/roadmap

### المسار
`GET /api/v1/student/courses/:courseId/roadmap`

### الوصف
خارطة المقرر الدراسي — الدروس والتحديات المتعلقة بمقرر محدد مع حالة التقدم والقفل التسلسلي.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** STUDENT فقط
- **متطلب إضافي:** يجب أن يكون الطالب مسجلاً في المقرر

### المخرجات

```json
{
  "success": true,
  "roadmap": {
    "course": { "id": "uuid", "title": "...", "description": "...", "icon": "...", "category": "..." },
    "lessons": [
      {
        "id": "uuid", "title": "الدرس الأول", "order": 1,
        "contentSlug": "ai-lesson-1", "status": "UNLOCKED",
        "totalExercises": 4, "completedExercises": 0,
        "completionRate": 0, "earnedPoints": 0, "totalPoints": 400,
        "challenges": [],
        "eligibleForNext": false
      }
    ],
    "overallProgress": {
      "totalLessons": 12, "completedLessons": 0,
      "totalChallenges": 48, "completedChallenges": 0,
      "totalPoints": 4800, "earnedPoints": 0,
      "avgCompletionRate": 0
    }
  }
}
```

---

# الدعوات (Invitations) — invitations.js

## 64. POST /api/v1/teacher/invitations/import

### المسار
`POST /api/v1/teacher/invitations/import`

### الوصف
استيراد طلاب جدد عن طريق إنشاء دعوات مع OTP. يُنشئ حسابات طلاب مع روابط تفعيل عبر البريد الإلكتروني.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `students` | array | نعم | قائمة الطلاب (name + email) |
| `classroomId` | string | لا | معرف الفصل المستهدف |

**كل عنصر في students:**
| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `name` | string | نعم | اسم الطالب |
| `email` | string | نعم | البريد الإلكتروني (لإرسال OTP) |

### المخرجات (201)

```json
{
  "success": true,
  "message": "تم إنشاء 30 من 30 دعوة",
  "results": { "successful": [...], "total": 30 }
}
```

### التحسينات المقترحة

- إضافة دعم رفع ملف CSV أو Excel
- إرسال رسائل ترحيبية تلقائية للطلاب الجدد
- إمكانية تخصيص رسالة الدعوة

## 65. GET /api/v1/teacher/invitations

### المسار
`GET /api/v1/teacher/invitations`

### الوصف
عرض جميع الدعوات المرسلة من المدرسة مع خيارات التصفية والترقيد.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| المعامل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `isActivated` | boolean | لا | تصفية حسب حالة التفعيل |
| `classroomId` | string | لا | تصفية حسب الفصل |
| `limit` | number | لا | عدد النتائج |
| `offset` | number | لا | بداية النتائج |

### المخرجات

```json
{
  "success": true,
  "invitations": [...],
  "total": 30
}
```

---

# تهيئة المدرسة (Onboarding) — onboard.js

## 66. POST /api/v1/onboard-school

### المسار
`POST /api/v1/onboard-school`

### الوصف
تهيئة مدرسة جديدة — إنشاء حسابات جماعية للمعلمين والطلاب دفعة واحدة. عملية ثقيلة جداً (تتضمن معاملة قاعدة بيانات كبيرة).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط
- **معدل الطلبات:** نعم — strictRateLimit('onboard') (5/دقيقة)

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `schoolName` | string | نعم | اسم المدرسة (1-200 حرف) |
| `teachers` | array | نعم | مصفوفة المعلمين (max 100) |
| `students` | array | نعم | مصفوفة الطلاب (max 1000) |
| `expectedStudents` | number | لا | التحقق من عدد الطلاب (منع التلاعب) |
| `defaultPassword` | string | نعم | كلمة المرور الافتراضية للجميع |

**كل معلم:**
| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `name` | string | نعم | الاسم (≤ 100 حرف) |
| `email` | string | نعم | البريد الإلكتروني (صيغة صحيحة) |

**كل طالب:**
| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `name` | string | نعم | الاسم (≤ 100 حرف) |
| `nationalId` | string | نعم | الرقم الوطني (≤ 20 حرف) |

### المخرجات (201)

```json
{
  "message": "School \"مدرسة القدس\" onboarded successfully",
  "school": { "id": "uuid", "name": "مدرسة القدس" },
  "stats": {
    "teachers": 10, "students": 250, "total": 260,
    "submitted": { "teachers": 10, "students": 250 }
  },
  "warnings": [
    "Student \"أحمد\" (nationalId: 12345) already exists — skipped"
  ]
}
```

### الأمان

- كشف الازدواجية: التحقق من الأرقام الوطنية (للطلاب) والبريد الإلكتروني (للمعلمين)
- التحقق من العدد: expectedStudents ضد العدد الفعلي (منع التلاعب)
- معاملة (Transaction): جميع العمليات في معاملة واحدة
- كلمة مرور قوية إلزامية
- تسجيل كامل في سجل النظام

### مثال CURL

```bash
curl -X POST https://api.dhad.studio/api/v1/onboard-school \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d '{
    "schoolName": "مدرسة القدس",
    "teachers": [{ "name": "أستاذ أحمد", "email": "ahmed@school.com" }],
    "students": [{ "name": "أحمد طالب", "nationalId": "1234567890" }],
    "expectedStudents": 1,
    "defaultPassword": "SecureSchoolPass2026!"
  }'
```

### التحسينات المقترحة

- إضافة دعم رفع ملف CSV مباشرة
- إرسال تقرير بالبريد الإلكتروني بعد الإنهاء
- إضافة التحقق المسبق من صلاحية جميع البيانات قبل البدء
- دعم تحميل صور الطلاب
- إضافة مرحلة تأكيد قبل الإنشاء الفعلي

---

# التقارير (Reports) — reports.js

## 67. GET /api/v1/reports/school-overview

### المسار
`GET /api/v1/reports/school-overview`

### الوصف
نظرة عامة على المدرسة — إحصائيات سريعة: عدد الطلاب/المعلمين، نشاط اليوم، الأسبوع، الشهر، وأكثر المستخدمين نشاطاً.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

```json
{
  "school": { "id": "uuid" },
  "totals": {
    "students": 250, "teachers": 12,
    "challenges": 45, "publishedChallenges": 30
  },
  "today": { "logins": 120 },
  "thisWeek": {
    "logins": 450, "submissions": 320,
    "passedSubmissions": 240, "passRate": 75
  },
  "thisMonth": { "logins": 1800, "submissions": 1200 },
  "topUsers": [
    { "userId": "uuid", "name": "أستاذ أحمد", "role": "TEACHER", "actions": 85 }
  ]
}
```

## 68. GET /api/v1/reports/school-overview/:schoolId

### المسار
`GET /api/v1/reports/school-overview/:schoolId`

### الوصف
نظرة عامة على مدرسة محددة. للمدير فقط.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط

## 69. POST /api/v1/reports/weekly

### المسار
`POST /api/v1/reports/weekly`

### الوصف
تقرير أسبوعي مفصل — تحليل النشاط لكل مستخدم، إحصائيات يومية، ومعدلات الإنجاز.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المخرجات

```json
{
  "school": { "id": "uuid" },
  "period": { "from": "2026-07-06", "to": "2026-07-10" },
  "summary": {
    "totalStudents": 250, "totalTeachers": 12,
    "activeStudents": 180, "activeTeachers": 10,
    "studentActivityRate": 72, "teacherActivityRate": 83,
    "totalSubmissions": 450, "passedSubmissions": 320,
    "passRate": 71,
    "totalChallenges": 12, "publishedChallenges": 8
  },
  "dailyBreakdown": [
    {
      "date": "2026-07-06", "dayName": "Saturday",
      "logins": 85, "activities": 120,
      "submissions": 65, "uniqueUsers": 80
    }
  ],
  "userStats": [
    {
      "userId": "uuid", "name": "أحمد",
      "role": "STUDENT", "loginDays": 5,
      "totalLogins": 10, "activities": 25,
      "submissions": 15, "passedSubmissions": 12,
      "interactionRate": 71
    }
  ]
}
```

## 70. POST /api/v1/reports/weekly/:schoolId

### المسار
`POST /api/v1/reports/weekly/:schoolId`

### الوصف
تقرير أسبوعي لمدرسة محددة. للمدير فقط.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط

## 71. GET /api/v1/reports/activity/:userId

### المسار
`GET /api/v1/reports/activity/:userId`

### الوصف
تقرير نشاط مفصل لمستخدم محدد — جلسات الدخول، النشاطات، الحلول المقدمة مع آخر 50 حدثاً.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** TEACHER أو ADMIN

### المدخلات

| المعامل | النوع | إلزامي | الافتراضي | الوصف |
|---|---|---|---|---|
| `days` | number | لا | 30 | عدد الأيام السابقة |

### المخرجات

```json
{
  "user": { "id": "uuid", "name": "أحمد", "role": "STUDENT" },
  "period": { "days": 30, "since": "2026-06-10" },
  "stats": {
    "activeDays": 20, "totalLogins": 35,
    "totalActivities": 120, "totalSubmissions": 45,
    "passedSubmissions": 35, "activityRate": 67
  },
  "actionBreakdown": {
    "xp_awarded": 45, "login": 35, "submission_created": 45
  },
  "recentLogins": [],
  "recentActivities": [],
  "recentSubmissions": []
}
```

---

# المدارس (Schools) — schools.js

## 72. GET /api/v1/schools

### المسار
`GET /api/v1/schools`

### الوصف
قائمة المدارس. ADMIN يرى جميع المدارس، TEACHER/STUDENT يرى مدرسته فقط.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** أي دور موثَّق

### المخرجات

```json
{
  "schools": [
    {
      "id": "uuid", "name": "مدرسة القدس",
      "provisionedAt": "2026-01-01T00:00:00.000Z",
      "userCount": 262, "courseCount": 3
    }
  ]
}
```

## 73. GET /api/v1/schools/:id

### المسار
`GET /api/v1/schools/:id`

### الوصف
تفاصيل المدرسة — قائمة المستخدمين (مرتبة: معلمين ثم طلاب) والإحصائيات.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** أي دور موثَّق مع requireSchoolAccess

### المخرجات

```json
{
  "id": "uuid", "name": "مدرسة القدس",
  "provisionedAt": "2026-01-01T00:00:00.000Z",
  "teachers": [
    { "id": "uuid", "name": "أستاذ أحمد", "role": "TEACHER", "email": "ahmed@school.com", "nationalId": null, "createdAt": "..." }
  ],
  "students": [],
  "stats": { "teachers": 12, "students": 250, "courses": 3 }
}
```

## 74. GET /api/v1/schools/:id/users

### المسار
`GET /api/v1/schools/:id/users`

### الوصف
قائمة مستخدمي المدرسة مع الإحصائيات (عدد المعلمين والطلاب).

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** أي دور موثَّق مع requireSchoolAccess

### المخرجات

```json
{
  "school": { "id": "uuid", "name": "مدرسة القدس" },
  "users": [...],
  "stats": { "teachers": 12, "students": 250, "total": 262 }
}
```

## 75. PUT /api/v1/schools/:id

### المسار
`PUT /api/v1/schools/:id`

### الوصف
تحديث اسم المدرسة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `name` | string | نعم | الاسم الجديد (1-200 حرف) |

### المخرجات

```json
{
  "message": "School updated",
  "school": { "id": "uuid", "name": "مدرسة القدس الجديدة", "provisionedAt": "..." }
}
```

## 76. DELETE /api/v1/schools/:id

### المسار
`DELETE /api/v1/schools/:id`

### الوصف
حذف المدرسة وجميع المستخدمين المرتبطين بها.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط

### الحماية

- يمنع حذف آخر مدرسة في النظام (لمنع تعطل النظام)
- حذف متتالي (CASCADE) للمستخدمين والمحتوى المرتبط

### المخرجات

```json
{
  "message": "School \"مدرسة القدس\" deleted",
  "deleted": { "users": 262, "courses": 3 }
}
```

## 77. DELETE /api/v1/schools/:id/users/:userId

### المسار
`DELETE /api/v1/schools/:id/users/:userId`

### الوصف
حذف مستخدم محدد من مدرسة. يتحقق من أن المستخدم في المدرسة المحددة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط

### الأمان

- يمنع حذف المستخدم لنفسه
- التحقق من أن المستخدم ينتمي للمدرسة المحددة

### المخرجات

```json
{
  "message": "User \"أحمد\" deleted"
}
```

---

# لوحة الأمان (Security Dashboard) — security.js

## 78. GET /api/v1/security/dashboard

### المسار
`GET /api/v1/security/dashboard`

### الوصف
لوحة معلومات الأمان — إحصائيات الأحداث الأمنية، محاولات الدخول الفاشلة، الحسابات المقفلة، عناوين IP المحظورة، والماسحات الضارة.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط
- **معدل الطلبات:** نعم — strictRateLimit('admin') (100/15 دقيقة)

### المخرجات

```json
{
  "success": true,
  "summary": {
    "totalEvents": 1500,
    "eventCounts": {
      "LOGIN_INVALID_PASSWORD": 120,
      "ACCOUNT_LOCKED": 15,
      "IP_BLOCKED": 5,
      "BLOCKED_SCANNER": 3
    },
    "security": {
      "failedLogins": 120,
      "lockedAccounts": 15,
      "blockedIPs": 5,
      "blockedScanners": 3
    }
  },
  "recentEvents": [
    {
      "timestamp": "2026-07-10T12:00:00.000Z",
      "action": "LOGIN_INVALID_PASSWORD",
      "ip": "192.168.1.100",
      "userId": null,
      "details": { "username": "admin", "ip": "192.168.1.100" }
    }
  ]
}
```

## 79. GET /api/v1/security/audit

### المسار
`GET /api/v1/security/audit`

### الوصف
سجل التدقيق الأمني — البحث والتصفية حسب الإجراء، المستخدم، أو عنوان IP.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط
- **معدل الطلبات:** نعم — strictRateLimit('admin')

### المدخلات

| المعامل | النوع | إلزامي | الافتراضي | الوصف |
|---|---|---|---|---|
| `limit` | number | لا | 100 | عدد النتائج |
| `action` | string | لا | — | تصفية حسب الإجراء |
| `userId` | string | لا | — | تصفية حسب المستخدم |
| `ip` | string | لا | — | تصفية حسب عنوان IP |

### المخرجات

```json
{
  "success": true,
  "count": 100,
  "entries": [
    {
      "timestamp": "2026-07-10T12:00:00.000Z",
      "action": "LOGIN_SUCCESS",
      "ip": "192.168.1.100",
      "userId": "uuid",
      "details": { "...": "..." }
    }
  ]
}
```

## 80. POST /api/v1/security/unblock-ip

### المسار
`POST /api/v1/security/unblock-ip`

### الوصف
إلغاء حظر عنوان IP محظور.

### المتطلبات
- **المصادقة:** نعم (JWT)
- **الصلاحية:** ADMIN فقط
- **معدل الطلبات:** نعم — strictRateLimit('admin')

### المدخلات

| الحقل | النوع | إلزامي | الوصف |
|---|---|---|---|
| `ip` | string | نعم | عنوان IP المراد إلغاء حظره |

### المخرجات

```json
{
  "success": true,
  "message": "IP 192.168.1.100 unblocked"
}
```

### مثال CURL

```bash
curl -X POST https://api.dhad.studio/api/v1/security/unblock-ip \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d '{"ip": "192.168.1.100"}'
```

---

# الوسائط الأمنية المشتركة (Shared Middleware)

## sanitizeMiddleware

يطبق على جميع المسارات عبر `router/index.js`. يقوم بتعقيم جميع المدخلات:

- إزالة أحرف \0 (null bytes)
- إزالة أحرف التحكم (\x00-\x08, \x0B, \x0C, \x0E-\x1F)
- إزالة \r (carriage return)
- قص المسافات الزائدة (trim)
- تطبيق متكرر على الكائنات والمصفوفات
- تعقيم req.body، req.query، و req.params

## strictRequestValidation

- التحقق من طول URL (max 2048)
- التحقق من حجم الجسم (max 1MB)
- التحقق من الرؤوس المشبوهة
- حظر وكلاء المستخدم المعروفين (sqlmap, nikto, nessus, إلخ)

## strictSecurityHeaders

- X-Frame-Options: DENY
- X-Content-Type-Options: nosniff
- X-XSS-Protection: 1; mode=block
- Strict-Transport-Security: max-age=31536000; includeSubDomains; preload
- Content-Security-Policy (صارم للمسارات الآمنة)
- Referrer-Policy: strict-origin-when-cross-origin
- Permissions-Policy: camera=(), microphone=(), geolocation=()
- إزالة X-Powered-By

## validatePasswordStrength

| المعيار | المتطلب |
|---|---|
| الحد الأدنى للطول | 8 أحرف |
| حرف كبير (A-Z) | مطلوب |
| حرف صغير (a-z) | مطلوب |
| رقم (0-9) | مطلوب |
| كلمات المرور الشائعة | ممنوعة (password, 123456, admin, إلخ) |

## strictRateLimit

| Category | الحد الأقصى | النافذة الزمنية |
|---|---|---|
| public | 30 | 15 دقيقة |
| authenticated | 60 | 15 دقيقة |
| admin | 100 | 15 دقيقة |
| onboard | 5 | 15 دقيقة |
| submission | 10 | 15 دقيقة |
| auth | 10 | 15 دقيقة |
| otp-request | 3 | 15 دقيقة |
| otp-verify | 5 | 15 دقيقة |

## Account Lockout & Progressive Delay

- **الحد الأقصى للمحاولات الفاشلة:** 5
- **مدة القفل:** 15 دقيقة
- **التأخير التصاعدي:** 1ث → 2ث → 4ث → 8ث → 16ث

## IP Blocking

- **الحد الأقصى لفشل IP:** 20
- **مدة حظر IP:** ساعة واحدة

## معاملات أمان الترميز (Coding Security Measures)

1. **مقارنة زمنية ثابتة (Timing Safe Comparison):** استخدام crypto.timingSafeEqual لجميع مقارنات التوقيع
2. **فشل مغلق (Fail-Closed):** في حالة فشل قاعدة البيانات، يتم رفض الطلب وليس السماح به
3. **التحقق من إصدار الرمز (Token Version):** التحقق من tokenVersion في كل طلب
4. **خوارزمية JWT محددة:** HS256 فقط (منع هجمات تبديل الخوارزمية)
5. **تقييد المدخلات:** حدود طول صارمة لجميع الحقول النصية
6. **تعقيم CSV:** منع حقن الصيغ في ملفات CSV (تسليط الضوء على =, +, -, @)

## قائمة أحداث التدقيق (Audit Events)

| الحدث | الوصف |
|---|---|
| LOGIN_SUCCESS | تسجيل دخول ناجح |
| LOGIN_INVALID_PASSWORD | كلمة مرور خاطئة |
| LOGIN_USER_NOT_FOUND | مستخدم غير موجود |
| LOGIN_BLOCKED_LOCKED_ACCOUNT | محاولة دخول لحساب مقفل |
| LOGIN_PROGRESSIVE_DELAY | تطبيق تأخير تصاعدي |
| ACCOUNT_LOCKED | قفل الحساب بعد 5 محاولات فاشلة |
| IP_BLOCKED | حظر IP بعد 20 فشل |
| BLOCKED_SCANNER | حظر ماسح ضار (sqlmap, nikto, إلخ) |
| IP_UNBLOCKED | إلغاء حظر IP من قبل المدير |
| PASSWORD_CHANGED | تغيير كلمة المرور |
| PASSWORD_RESET | إعادة تعيين كلمة المرور من قبل المدير |
| PASSWORD_CHANGE_BLOCKED_LOCKED | منع تغيير كلمة المرور لحساب مقفل |
| LOGOUT | تسجيل خروج |
| RATE_LIMIT_EXCEEDED | تجاوز معدل الطلبات |
| CHALLENGE_SUBMISSION_BLOCKED_TIER_LOCKED | منع تقديم حل لمستوى مقفل |
| CHALLENGE_SUBMISSION_BLOCKED_SEQUENTIAL | منع تقديم حل لتحدي تسلسلي مقفل |
| SUSPICIOUS_HEADER | كشف رأس مشبوه |
| BRIDGE_UNAUTHORIZED_IP | محاولة وصول غير مصرح بها للجسر |
| BRIDGE_SIGNATURE_INVALID | توقيع جسر غير صالح |
| SESSION_LIMIT_EXCEEDED | تجاوز حد الجلسات المتزامنة |

---

# ملخص جميع نقاط النهاية (Complete Endpoint Summary)

| # | الطريقة | المسار الكامل | الصلاحية | معدل الطلبات |
|---|---|---|---|---|
| 1 | POST | /api/v1/auth/login | عام | strictRateLimit('public') |
| 2 | POST | /api/v1/auth/refresh | عام | strictRateLimit('public') |
| 3 | POST | /api/v1/auth/register | TEACHER/ADMIN | strictRateLimit('public') |
| 4 | PUT | /api/v1/auth/change-password | STUDENT | لا |
| 5 | POST | /api/v1/auth/reset-password | ADMIN | لا |
| 6 | POST | /api/v1/auth/logout | مصادق | لا |
| 7 | POST | /api/v1/auth/request-otp | عام | strictRateLimit('otp-request') |
| 8 | POST | /api/v1/auth/verify-otp | عام | strictRateLimit('otp-verify') |
| 9 | GET | /api/v1/analytics/leaderboard | TEACHER/ADMIN | لا |
| 10 | GET | /api/v1/analytics/attendance-summary | TEACHER/ADMIN | لا |
| 11 | GET | /api/v1/analytics/attendance-history | TEACHER/ADMIN | لا |
| 12 | POST | /api/v1/analytics/attendance | TEACHER/ADMIN | لا |
| 13 | GET | /api/v1/analytics/progress | TEACHER/ADMIN | لا |
| 14 | GET | /api/v1/analytics/dashboard | TEACHER/ADMIN | لا |
| 15 | GET | /api/v1/analytics/classrooms | TEACHER/ADMIN | لا |
| 16 | POST | /api/v1/analytics/classrooms | TEACHER/ADMIN | لا |
| 17 | GET | /api/v1/analytics/assessments | TEACHER/ADMIN | لا |
| 18 | GET | /api/v1/analytics/classroom/:classId/export | TEACHER/ADMIN | لا |
| 19 | GET | /api/v1/challenges (analytics) | مصادق | لا |
| 20 | GET | /api/v1/lessons/:lessonId/tests | TEACHER/ADMIN | لا |
| 21 | GET | /api/v1/challenges/tier/:tierId | TEACHER/ADMIN | لا |
| 22 | GET | /api/v1/challenges (challenges) | مصادق | لا |
| 23 | GET | /api/v1/challenges/daily | مصادق | لا |
| 24 | POST | /api/v1/challenges | TEACHER/ADMIN | لا |
| 25 | PUT | /api/v1/challenges/:id | TEACHER/ADMIN | لا |
| 26 | DELETE | /api/v1/challenges/:id | TEACHER/ADMIN | لا |
| 27 | POST | /api/v1/challenges/:id/publish | TEACHER/ADMIN | لا |
| 28 | POST | /api/v1/challenges/:id/set-daily | TEACHER/ADMIN | لا |
| 29 | POST | /api/v1/submit-solution | STUDENT | 30/ساعة |
| 30 | GET | /api/v1/submissions | مصادق | لا |
| 31 | POST | /api/v1/challenge/verify | عام (توقيع) | strictRateLimit('auth') |
| 32 | GET | /api/v1/student/profile | STUDENT | لا |
| 33 | GET | /api/v1/student/leaderboard | STUDENT | لا |
| 34 | GET | /api/v1/student/roadmap | STUDENT | لا |
| 35 | POST | /api/v1/student/challenge/submit | STUDENT | لا |
| 36 | GET | /api/v1/student/achievements | STUDENT | لا |
| 37 | GET | /api/v1/student/classroom/check | STUDENT | لا |
| 38 | POST | /api/v1/assessments/create | TEACHER/ADMIN | 1/6ساعات |
| 39 | GET | /api/v1/student/assessment/active | STUDENT | لا |
| 40 | GET | /api/v1/student/assessments | STUDENT | لا |
| 41 | POST | /api/v1/student/assessment/submit | STUDENT | 20/تقييم |
| 42 | POST | /api/v1/student/assessment/start | STUDENT | لا |
| 43 | POST | /api/v1/student/assessment/complete | STUDENT | لا |
| 44 | GET | /api/v1/assessments/:assessmentId/results | TEACHER/ADMIN | لا |
| 45 | GET | /api/v1/assessments/classroom/:classId | TEACHER/ADMIN | لا |
| 46 | POST | /api/v1/workspace/save | STUDENT | لا |
| 47 | GET | /api/v1/workspace/load/:challengeId | STUDENT | لا |
| 48 | GET | /api/v1/workspace/list | STUDENT | لا |
| 49 | DELETE | /api/v1/workspace/:challengeId | STUDENT | لا |
| 50 | GET | /api/v1/analytics/security-alerts | TEACHER/ADMIN | لا |
| 51 | POST | /api/v1/analytics/security-alerts/:id/resolve | TEACHER/ADMIN | لا |
| 52 | POST | /api/v1/submissions/bulk-report | STUDENT | لا |
| 53 | GET | /api/v1/auth/classrooms | عام | لا |
| 54 | GET | /api/v1/auth/classrooms/:id/students | عام | لا |
| 55 | POST | /api/v1/auth/student-login | عام | strictRateLimit('public') |
| 56 | POST | /api/v1/teacher/students | TEACHER/ADMIN | لا |
| 57 | POST | /api/v1/teacher/students/bulk | TEACHER/ADMIN | لا |
| 58 | GET | /api/v1/teacher/students | TEACHER/ADMIN | لا |
| 59 | DELETE | /api/v1/teacher/students/:id | TEACHER/ADMIN | لا |
| 60 | POST | /api/v1/teacher/students/assign | TEACHER/ADMIN | لا |
| 61 | GET | /api/v1/student/courses | STUDENT | لا |
| 62 | POST | /api/v1/student/courses/:courseId/enroll | STUDENT | لا |
| 63 | GET | /api/v1/student/courses/:courseId/roadmap | STUDENT | لا |
| 64 | POST | /api/v1/teacher/invitations/import | TEACHER/ADMIN | لا |
| 65 | GET | /api/v1/teacher/invitations | TEACHER/ADMIN | لا |
| 66 | POST | /api/v1/onboard-school | ADMIN | 5/دقيقة |
| 67 | GET | /api/v1/reports/school-overview | TEACHER/ADMIN | لا |
| 68 | GET | /api/v1/reports/school-overview/:schoolId | ADMIN | لا |
| 69 | POST | /api/v1/reports/weekly | TEACHER/ADMIN | لا |
| 70 | POST | /api/v1/reports/weekly/:schoolId | ADMIN | لا |
| 71 | GET | /api/v1/reports/activity/:userId | TEACHER/ADMIN | لا |
| 72 | GET | /api/v1/schools | مصادق | لا |
| 73 | GET | /api/v1/schools/:id | مصادق | لا |
| 74 | GET | /api/v1/schools/:id/users | مصادق | لا |
| 75 | PUT | /api/v1/schools/:id | ADMIN | لا |
| 76 | DELETE | /api/v1/schools/:id | ADMIN | لا |
| 77 | DELETE | /api/v1/schools/:id/users/:userId | ADMIN | لا |
| 78 | GET | /api/v1/security/dashboard | ADMIN | strictRateLimit('admin') |
| 79 | GET | /api/v1/security/audit | ADMIN | strictRateLimit('admin') |
| 80 | POST | /api/v1/security/unblock-ip | ADMIN | strictRateLimit('admin') |