# DUPLICATE CLEANUP REPORT — Dhad Studio v1.0.0

**التاريخ:** September 6, 2026
**الإجراء:** Cleanup Phase — No Code Changes
**ال Backup:** `/tmp/Dhad-Studio-backup-before-cleanup/`

---

##ملخص الأرقام

| البند | العدد |
|-------|-------|
| إجمالي الملفات قبل التنظيف | 864 |
| مجموعات الملفات المتطابقة (Exact Duplicates) | 212 |
| إجمالي الملفات المكررة | 436 |

---

## التصنيف التفصيلي

### 1. Dhad-Studio-Full/compiler/src — EXACT DUPLICATE

| الملف | الحالة | ملاحظات |
|-------|--------|---------|
| `src/main.cpp` | IDENTICAL لـ Daad-Compiler | |
| `src/Compiler.cpp` | IDENTICAL | |
| `src/Lexer.cpp` | IDENTICAL | |
| `src/Parser.cpp` | IDENTICAL | |
| `src/CodeGen.cpp` | IDENTICAL | |
| `src/AST.cpp` | IDENTICAL | |
| `src/Keywords.cpp` | IDENTICAL | |
| `src/Token.cpp` | IDENTICAL | |
| `src/UnicodeUtils.cpp` | IDENTICAL | |
| `src/Diagnostics.cpp` | IDENTICAL | |
| `src/Optimizer.cpp` | IDENTICAL | |
| `src/SandboxValidator.cpp` | IDENTICAL | |
| `src/_backup_original/*` | IDENTICAL (6 ملفات) | |
| `CMakeLists.txt` | IDENTICAL | |
| `Makefile` | IDENTICAL | |

**الاستخدام:** لا يوجد أي Reference من أي مكون آخر إلى `Dhad-Studio-Full/compiler/`
**القرار:** ✅ DELETE — Daad-Compiler هو المصدر الأساسي (Canonical)

---

### 2. Dhad-Studio-Full/frontend/frontend-web ↔ Dhad-Studio-Web — EXACT DUPLICATE

| الملف | الحالة |
|-------|--------|
| جميع ملفات HTML (root + pages/) | IDENTICAL |
| جميع ملفات JS (js/) | IDENTICAL |
| جميع ملفات CSS (css/) | IDENTICAL |
| sw.js, _redirects, icons.svg | IDENTICAL |

**الاستخدام:**
- `Dhad-Studio-Full/server` يقدم هذا الـ Frontend عبر `frontend-web/`
- `Dhad-Studio-Web` يعمل كنسخة مستقلة للنشر على Netlify/Vercel

**القرار:** ⚠️ KEEP BOTH — كلاهما مستخدم بشكل فعلي (Full = Self-contained, Web = Standalone)

---

### 3. Dhad-Studio-Full/server/src/utils/dhad/ — EXACT DUPLICATE (within Full)

| الملف | المطابق له |
|-------|-----------|
| `lexer.js` | `frontend-web/js/dhad-lexer.js` |
| `parser.js` | `frontend-web/js/dhad-parser.js` |
| `ast.js` | `frontend-web/js/dhad-ast.js` |
| `codegen.js` | `frontend-web/js/dhad-codegen.js` |

**الاستخدام:** الخادم يستخدم هذه الملفات للتقييم من جانب الخادم (Server-side evaluation)
**القرار:** ⚠️ KEEP — الخادم يحتاج نسخة مستقلة للعمل offline/on-server

---

### 4. daad-studio-v1.0.0-windows-x64 — USED RELEASE

- 106 ملف متطابق مع Daad-Compiler/stdlib
- يحتوي على ملفات exe مُجمّعة مسبقاً
- **القرار:** ⚠️ KEEP — نسخة توزيع جاهزة

---

### 5. cpu/desktop — GENERATED + INTENTIONALLY DIFFERENT

- يحتوي على `dhad_gui`, `dhad_asm.o`, `dhad_cpu.exe` (Build Artifacts)
- يحتوي على أمثلة مطابقة لـ cpu/examples/ و cpu/tests/
- يحتوي على ملفات فريدة: `ARCHITECTURE.md`, `شغّل..bat`
- **القرار:** ⚠️ KEEP — نسخة توزيع + Build Artifacts مطلوبة

---

### 6. desktop/ — LEGACY ( Older Version)

| الملف | المطابق له في cpu/desktop/ |
|-------|---------------------------|
| `dhad_cpu.exe` | `cpu/desktop/dhad_cpu.exe` (أقدم) |
| `examples/*.ضasm` | `cpu/examples/*.ضasm` |
| `شغّل..bat` | `cpu/desktop/شغّل..bat` |
| `README.txt` | `cpu/desktop/README.txt` |

**الاستخدام:** لا يوجد أي Reference
**القرار:** ✅ DELETE — نسخة قديمة، cpu/desktop/ هي الأحدث والأكمل

---

### 7. Root-level Files

| الملف | الحجم | التصنيف | القرار |
|-------|-------|---------|--------|
| `Dhad-Studio-v1.0.0-All.zip` | 184MB | GENERATED (Project archive) | ✅ DELETE — المحتوى موجود كمجلدات |
| `Dhad-Studio-v1.0.0-Windows.zip` | 7MB | GENERATED (Windows archive) | ✅ DELETE — المحتوى موجود في daad-studio-v1.0.0-windows-x64/ |
| `daad-studio-v1.0.0-windows-x64.zip` | 420KB | GENERATED (Windows archive) | ✅ DELETE — المحتوى موجود في daad-studio-v1.0.0-windows-x64/ |
| `desktop_dhad_v2.0.zip` | 198KB | LEGACY (Old version) | ✅ DELETE — الإصدار الأحدث موجود |
| `__------__--_--___--_-__---__-_-___---_--_--_-___...` | 5.9MB | UNKNOWN (Windows DLL) | ⚠️ KEEP — غير متأكد من الأهمية |
| `desktop.ini` | 124B | GENERATED (Windows system) | ✅ DELETE — ملف نظام Windows |
| `IndexerVolumeGuid` | 537KB | GENERATED (Windows system) | ✅ DELETE — ملف نظام Windows |
| `بحث_مشروع_معالج_16بت...odt` | 31KB | DOCUMENTATION | ⚠️ KEEP — وثيقة بحثية |
| `README.txt` | 3.4KB | DOCUMENTATION | ⚠️ KEEP |
| `دليل-التثبيت-والتشغيل.txt` | 24KB | DOCUMENTATION | ⚠️ KEEP |

---

### 8. Dhad-Studio-Electron/node_modules — GENERATED

- 475 ملف (npm dependencies)
- **القرار:** ⚠️ KEEP — مطلوب لتشغيل Electron

---

## ملخص القرارات

### ✅ سيتم حذفها (CLEAR SAFE TO DELETE)

| # | الملف/المجلد | السبب | عدد الملفات |
|---|-------------|-------|------------|
| 1 | `Dhad-Studio-Full/compiler/` | EXACT DUPLICATE لـ Daad-Compiler (كل ملفات src متطابقة) | 21 ملف |
| 2 | `desktop/` | LEGACY — نسخة قديمة، cpu/desktop/ أحدث وأكمل | 10 ملفات |
| 3 | `Dhad-Studio-v1.0.0-All.zip` | GENERATED — محتوى المجلدات موجود بالفعل | 1 ملف (184MB) |
| 4 | `Dhad-Studio-v1.0.0-Windows.zip` | GENERATED — محتوى daad-studio-v1.0.0-windows-x64/ موجود | 1 ملف (7MB) |
| 5 | `daad-studio-v1.0.0-windows-x64.zip` | GENERATED — المحتوى موجود في المجلد | 1 ملف (420KB) |
| 6 | `desktop_dhad_v2.0.zip` | LEGACY — الإصدار الأحدث موجود | 1 ملف (198KB) |
| 7 | `desktop.ini` | GENERATED — ملف نظام Windows | 1 ملف |
| 8 | `IndexerVolumeGuid` | GENERATED — ملف نظام Windows | 1 ملف |

**إجمالي الملفات المحذوفة:** 37 ملف
**الحجم المحذوف:** ~192MB

### ⚠️ سيتم الاحتفاظ بها (KEEP)

| # | الملف/المجلد | السبب |
|---|-------------|-------|
| 1 | `Daad-Compiler/` | CANONICAL — المصدر الأساسي |
| 2 | `Dhad-Studio-Full/` (بدون compiler/) | USED — يحتوي Frontend + Server + Bridge |
| 3 | `Dhad-Studio-Web/` | USED — نسخة ويب مستقلة |
| 4 | `Dhad-Studio-Electron/` | USED — تطبيق سطح مكتب |
| 5 | `VSCode-Extension/` | USED — إضافة VS Code |
| 6 | `cpu/` | USED — محاكي المعالج |
| 7 | `daad-studio-v1.0.0-windows-x64/` | USED_RELEASE — نسخة توزيع |
| 8 | `__------__...dll` | UNRESOLVED — غير متأكد |
| 9 | `بحث_مشروع_معالج_16بت...odt` | DOCUMENTATION |
| 10 | `README.txt` | DOCUMENTATION |
| 11 | `دليل-التثبيت-والتشغيل.txt` | DOCUMENTATION |

---

## ملاحظات مهمة

1. **Dhad-Studio-Full/compiler/ REMOVAL** لن يؤثر على أي مكون آخر — لا يوجد أي Reference إليه
2. **desktop/ REMOVAL** لن يؤثر على أي شيء — لا يوجد أي Reference إليه
3. **ZIP files REMOVAL** لن يؤثر على المشروع — المحتوى موجود كمجلدات
4. **لم يتم تعديل أي كود** — هذه عملية حذف فقط

---

## إحصائيات ما بعد التنظيف المتوقعة

| البند | قبل | بعد |
|-------|------|------|
| إجمالي الملفات | 864 | ~827 |
| عدد المجلدات الرئيسية | 8 | 7 |
| الحجم التقريبي | ~250MB | ~58MB |
