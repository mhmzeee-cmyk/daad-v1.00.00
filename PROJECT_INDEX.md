# فهرس المشروع — Dhad-Studio-Unified
التاريخ: 2026-09-13
يُحدث هذا الملف مع كل توحيد.

## 1. المجلدات العلوية

| المسار | الحجم | الغرض |
|---|---|---|
| `CPU/` | 11M | معالج عربي 16-بت: محاكي C ومجمّع ضasm وواجهة Qt |
| `Compiler/` | 71M | مترجم ض إلى C++20 مع stdlib وأمثلة |
| `DAAD/` | 27M | لغة daad: سلسلة ‎.daad←.s←.exe‎ مع تدقيق |
| `Demo/` | 2.4M | عرض مصغر قابل للتشغيل مع سكربتات بدء |
| `Docs/` | 24M | أرشيف عملياتي تاريخي للمراحل والتقارير |
| `docs/` | 304K | المواصفة المعيارية للغة وقواعدها ومكتبتها |
| `Examples/` | 271M | مدونة أمثلة مترجمة بخمس صيغ لكل مثال |
| `Operational-Evidence/` | 388K | أدلة تشغيل مصنفة حسب المكوّن |
| `Releases/` | 8.2M | إصدار مجمّد واحد للمقابلة مع MSI وتوثيق |
| `release/` | 29M | مسرح إصدار نشط لويندوز 7 و10-11 |
| `Program Files/` | 12M | صورة تثبيت مصغرة بثنائيات تجريبية |
| `Scripts/` | 144K | أتمتة بناء وتشغيل عربية |
| `Sources-Archive/` | 35M | أرشيف بارد لا يُبنى منه |
| `Studio/` | 567M | منصة الويب وسطح المكتب الكاملة |
| `Tests/` | 52K | بوابتا اختبار عامتان فقط |

ملفات الجذر: ~24 تقرير `*.md` للمراحل + `ض.json` (السياق الأساسي).

## 2. محتويات `Studio/`

| المسار | الحجم | الغرض |
|---|---|---|
| `Studio/Electron/` | 4.8M | غلاف Electron قديم مصغر |
| `Studio/Full/` | 267M | المونوريبو الكامل الجامع |
| `Studio/Full/bridge/` | 1.5M | أداة Qt للمدارس مع واجهة QML |
| `Studio/Full/compiler/` | 21M | نسخة المترجم C++ داخل المونوريبو |
| `Studio/Full/frontend/` | 3.3M | واجهة Qt/QML مع نسخة ويب مضمنة |
| `Studio/Full/server/` | 242M | سيرفر EdTech بـ Express وPrisma وJWT |
| `Studio/Web/` | 2.9M | واجهة ويب ستاتيكية خالصة |
| `Studio/VSCode-Extension/` | 29M | إضافة لغة daad لمحرر VSCode |
| `Studio/desktop-app/` | 262M | تطبيق Electron الإنتاجي الجاهز للتغليف |
| `Studio/Windows-Launcher/` | 64K | قائمة مشغلات `.bat` عربية |
| `Studio/tests/` | 956K | مدونة تحديات أمنية واختراق |

## 3. العلاقات الاشتقاقية

- `desktop-app` مشتق آلياً: `scripts/prepare-build.js` ينسخ `server/` و`frontend-web/` إلى داخله قبل التغليف، ويستثني `node_modules` و`.env`.
- `Full/compiler` نسخة من `Compiler/`: نفس `CMakeLists` وقائمة `src/`، مع افتراق في 5 ملفات فقط تحتاج دمجاً انتقائياً.
- مثلث الإصدارات: `Releases/` نسخة مجمدة، و`release/` مسرح نشط، و`Program Files/` صورة منصبة مصغرة منهما.
- `Docs/` مقابل `docs/`: تكميل لا تكرار — الأول أرشيف عملياتي والثاني مواصفة قياسية، والاسمان خطران على ويندوز لحساسية الحالة.
- الواجهة مثلثة: `Web ≡ desktop/frontend-web ≡ Full/frontend-web` (62 ملفاً لكل منها) مع انحراف 8 ملفات فقط.

## 4. ما هو متجاهل من git

تحقق بـ `git check-ignore` مقابل `.gitignore` الجذري وملفات `.gitignore` الفرعية.

- `node_modules/` متجاهلة (مؤكد: `Studio/desktop-app/node_modules/x`).
- `build/` و`dist/` و`out/` متجاهلة (مؤكد: `Compiler/build` و`Full/server/dist/x.js` و`desktop-app/server/dist/x.js`).
- `*.exe` و`*.o` و`*.a` و`*.so` متجاهلة (مؤكد: `DAAD/add.exe`).
- `*.dat` متجاهلة (مؤكد: `test.dat`) — تشمل `Examples/data-pos` الضخمة.
- `*.zip` و`*.db*` و`*.log` و`CMakeFiles/` و`Makefile` و`.env` متجاهلة (مؤكد: `release/test.zip` و`x.db`).
