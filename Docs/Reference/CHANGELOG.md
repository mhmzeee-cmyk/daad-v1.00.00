# CHANGELOG

كل التغييرات المهمة في ض استديو

---

## [1.0.0] - 2026-06-21

### المحرك الأساسي (Core Engine)
- ✅ إنشاء هيكل المجلدات الأساسي (src, include, tests, docs, config)
- ✅ تنفيذ `CMakeLists.txt` الجذري مع دعم C++20
- ✅ دعم UTF-8 الأصلي للمترجم
- ✅ تنفيذ GoogleTest لاختبارات الوحدة

### محلل لفظي (Lexer)
- ✅ دعم كامل لترميز UTF-8 للحروف العربية
- ✅ تتبع أرقام الأعمدة والصفوف
- ✅ التعرف على جميع أنواع الرموز (tokens)
- ✅ دعم الأعداد الصحيحة وال عشرية
- ✅ دعم النصوص المقتبسة

### محلل نحوي (Parser)
- ✅ استخدام تقنية Recursive Descent
- ✅ دعم تعريف المتغيرات (صحيح، مزدوج، حرف)
- ✅ دعم جمل الطباعة (طباعة())
- ✅ دعم هياكل التحكم (اذا، بينما، كرر)
- ✅ دعم الدوال
- ✅ دعم القوالب (Templates)
- ✅ نظام استرداد الأخطاء المتقدم

### شجرة النحو (AST)
- ✅ تنفيذ جميع العقد الأساسية:
  - NumberExprAST
  - VariableExprAST
  - BinaryExprAST
  - VarDeclStmtAST
  - IfStmtAST
  - WhileStmtAST
  - FunctionDeclAST
  - FunctionCallAST
  - ClassDeclAST
  - StructDeclAST
  - TemplateDeclAST
- ✅ استخدام std::unique_ptr لإدارة الذاكرة

### مولد الكود (CodeGen)
- ✅ توليد ملفات مزدوجة (.hpp و .cpp)
- ✅ حقن ترويسات Runtime تلقائياً
- ✅ خريطة ترجمة الأنواع (صحيح → int)
- ✅ دعم جميع عقد AST

### المُحسّن (Optimizer)
- ✅ خوارزمية Constant Folding
- ✅ تبسيط التعبيرات الحسابية الثابتة

### محرك التشخيص (Diagnostics)
- ✅ رسائل خطأ مفصلة مع موقع المصدر
- ✅ مستويات خطورة (ملاحظة، تحذير، خطأ، خطأ فادح)
- ✅ استرداد الأخطاء (Error Recovery)

### بيئة التشغيل (Runtime)
- ✅ دعم Android (__android_log_print)
- ✅ دعم iOS/macOS (NSLog/printf)
- ✅ دعم Windows/Linux (std::cout)

### المكتبة القياسية (Stdlib)
- ✅ مكتبة الحساب (Math.hpp)
- ✅ مكتبة النصوص (String.hpp)

### واجهة المستخدم (IDE)
- ✅ واجهة Qt5/QML
- ✅ محرر نصوص مع تمييز لوني
- ✅ تشغيل في خيط منفصل (Thread-Safe)
- ✅ دعم جميع المنصات

### امتداد VS Code
- ✅ TextMate Grammar للتلوين
- ✅ قوالب جاهزة (Snippets)
- ✅ أمر التحويل المدمج
- ✅ ملف package.json مكتمل

### CI/CD
- ✅ GitHub Actions workflow
- ✅ بناء على 6 منصات
- ✅ اختبارات تلقائية
- ✅ توليد حزم التوزيع

### التوثيق
- ✅ دليل التسليم (Handover Guide)
- ✅ إرشادات الترخيص (License Hints)
- ✅ تدابير الأمان (Security)
- ✅ README احترافي
- ✅ CHANGELOG