# 🚀 دليل النشر الكامل - Dhad Studio

## 📋 ملخص

| المكون | المنصة | الرابط المتوقع |
|--------|--------|---------------|
| Backend API | Render | `https://dhad-studio-api.onrender.com` |
| Frontend | Vercel | `https://dhad-studio.vercel.app` |
| Database | Neon | مجاني |
| Cache | Upstash | مجاني |
| AI | Gemini | مجاني |

---

## 📦 الجزء 1: نشر Backend API

### 1.1 ارفع الكود على GitHub

```powershell
cd C:\Projects\dhad-studio
git add .
git commit -m "Dhad Studio v2.0 - Complete"
git push origin main
```

### 1.2 أنشر على Render

1. اذهب إلى [render.com](https://render.com)
2. اضغط **New** → **Web Service**
3. اختر مستودع GitHub الخاص بك
4. اضبط الإعدادات:

| الإعداد | القيمة |
|---------|--------|
| Name | `dhad-studio-api` |
| Runtime | `Node` |
| Build Command | `cd server && npm install` |
| Start Command | `cd server && npm start` |
| Plan | `Free` |

5. أضف **Environment Variables**:

```
DATABASE_URL=postgresql://user:pass@ep-xxx.neon.tech/daad?sslmode=require
REDIS_URL=redis://default:xxx@xxx.upstash.io:6379
GEMINI_API_KEY=your-gemini-key
JWT_SECRET=any-random-secret-32-chars
CLIENT_URL=https://dhad-studio.vercel.app
NODE_ENV=production
```

6. اضغط **Create Web Service**

### 1.3 تحقق من Backend

```
GET https://dhad-studio-api.onrender.com/health
```

يجب أن ترى:
```json
{
  "status": "healthy",
  "timestamp": "...",
  "uptime": 12345
}
```

---

## 🎨 الجزء 2: نشر Frontend

### 2.1 نشر على Vercel (الأسهل)

1. اذهب إلى [vercel.com](https://vercel.com)
2. سجل الدخول بـ GitHub
3. اضغط **Add New** → **Project**
4. اختر مستودع `dhad-studio`
5. اضبط الإعدادات:

| الإعداد | القيمة |
|---------|--------|
| Framework | `Other` |
| Root Directory | `frontend-web` |
| Build Command | `echo "Static site"` |
| Output Directory | `.` |

6. اضغط **Deploy**

### 2.2 نشر على Netlify (بديل)

1. اذهب إلى [netlify.com](https://netlify.com)
2. اسحب مجلد `frontend-web` مباشرة

أو باستخدام CLI:
```powershell
cd C:\Projects\dhad-studio\frontend-web
npx netlify-cli deploy --prod
```

---

## 🔗 الجزء 3: ربط Backend بالFrontend

بعد نشر كلاهما:

1. افتح `frontend-web/js/api.js`
2. عدّل `API_BASE`:

```javascript
const API_BASE = 'https://dhad-studio-api.onrender.com';
```

3. ارفع التعديل:
```powershell
cd C:\Projects\dhad-studio
git add frontend-web/js/api.js
git commit -m "Update API base URL"
git push origin main
```

4. Vercel سيعيد النشر تلقائياً

---

## 🔑 مفاتيح API المطلوبة

### 1. Gemini API Key (مجاني)
1. اذهب إلى [aistudio.google.com](https://aistudio.google.com)
2. سجل الدخول بحساب Google
3. اضغط **Get API Key**
4. انسخ المفتاح

### 2. Neon Database (مجاني)
1. اذهب إلى [neon.tech](https://neon.tech)
2. أنشئ حساباً
3. أنشئ مشروع جديد
4. انسخ **Connection String**
5. أضف `?sslmode=require` في النهاية

### 3. Upstash Redis (مجاني)
1. اذهب إلى [upstash.com](https://upstash.com)
2. سجل الدخول بـ GitHub
3. أنشئ قاعدة بيانات
4. انسخ **Redis URL**

---

## ✅ قائمة التحقق النهائية

### Backend
- [ ] `GET /health` يرجع 200
- [ ] `POST /api/auth/register` يعمل
- [ ] `POST /api/auth/login` يعمل
- [ ] `GET /api/challenges` يرجع قائمة التحديات
- [ ] `GET /api-docs` يعرض Swagger

### Frontend
- [ ] الصفحة الرئيسية تظهر بشكل صحيح
- [ ] صفحة تسجيل الدخول تعمل
- [ ] صفحة التسجيل تعمل
- [ ] لوحة تحكم الطالب تعرض البيانات
- [ ] لوحة تحكم المعلم تعرض البيانات
- [ ] صفحة التحديات تعرض التحديات

### Database
- [ ] PostgreSQL يتصل بنجاح
- [ ] Redis يعمل
- [ ] migrations تم تشغيلها

---

## 🛠️ استكشاف الأخطاء

### Backend لا يعمل
```
1. تحقق من السجلات في Render Dashboard
2. تأكد من أن DATABASE_URL صحيح وينتهي بـ ?sslmode=require
3. تأكد من تثبيت Prisma: cd server && npx prisma generate
```

### Frontend لا يتصل بالBackend
```
1. تحقق من API_BASE في api.js
2. تأكد من أن CORS مسموح في Backend
3. افتح Console في المتصفح وتحقق من الأخطاء
```

### خطأ 403 Forbidden
```
1. تأكد من أن JWT_SECRET موجود
2. تأكد من أن التوكييد صحيح
3. مسح localStorage وأعد تسجيل الدخول
```

---

## 💰 التكلفة الشهرية

| الخدمة | التكلفة |
|--------|---------|
| Render (Backend) | $0 (Free tier) |
| Vercel (Frontend) | $0 (Free tier) |
| Neon (Database) | $0 (Free tier) |
| Upstash (Redis) | $0 (Free tier) |
| Gemini (AI) | $0 (Free tier) |
| **المجموع** | **$0/شهر** |

---

## 📞 الدعم

- البريد: moomdh77@gmail.com
- GitHub: github.com/moomdh77-maker/dhad-studio
