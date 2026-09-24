# ض Core - هيكل المشروع (Project Structure)

**الإصدار:** 0.1.0
**تاريخ الإصدار:** 2026-08-03

---

## الهيكل المقترح

```
daad-lang/
│
├── docs/                          # الوثائق الرسمية
│   ├── specification.md           # المواصفة الرسمية
│   ├── language-overview.md       # نظرة عامة
│   ├── keywords.md                # الكلمات المفتاحية
│   ├── grammar.md                 # القواعد
│   ├── type-system.md             # نظام الأنواع
│   ├── registers.md               # السجلات
│   ├── memory.md                  # الذاكرة
│   ├── abi.md                     # واجهة التطبيق الثنائية
│   ├── binary-format.md           # صيغة الملفات
│   ├── compiler-architecture.md   # بنية المترجم
│   ├── style-guide.md             # دليل التنسيق
│   ├── decisions.md               # القرارات
│   ├── dictionary.md              # القاموس
│   ├── error-codes.md             # أكواد الأخطاء
│   ├── stability-test.md          # اختبار الاستقرار
│   └── roadmap.md                 # الخريطة الزمنية
│
├── compiler/                      # المترجم
│   ├── lexer/                     # المحلل البصري
│   │   ├── lexer.h
│   │   ├── lexer.c
│   │   └── tokens.h
│   │
│   ├── parser/                    # المحلل النحوي
│   │   ├── parser.h
│   │   ├── parser.c
│   │   └── ast.h
│   │
│   ├── semantic/                  # المحلل الدلالي
│   │   ├── semantic.h
│   │   ├── semantic.c
│   │   └── symbol_table.h
│   │
│   ├── ir/                        # التمثيل الداخلي
│   │   ├── ir.h
│   │   ├── ir.c
│   │   └── ir_optimizer.h
│   │
│   ├── optimizer/                 # المُحسّن
│   │   ├── optimizer.h
│   │   └── optimizer.c
│   │
│   └── backend/                   # الخلفية
│       ├── backend.h
│       ├── backend.c
│       └── codegen.h
│
├── backends/                      # المعماريات
│   ├── x86/                       # x86-64
│   │   ├── x86.h
│   │   ├── x86.c
│   │   └── x86_encoding.h
│   │
│   ├── arm64/                     # ARM64
│   │   ├── arm64.h
│   │   ├── arm64.c
│   │   └── arm64_encoding.h
│   │
│   └── riscv/                     # RISC-V
│       ├── riscv.h
│       ├── riscv.c
│       └── riscv_encoding.h
│
├── runtime/                       # مكتبة التشغيل
│   ├── runtime.h
│   ├── runtime.c
│   ├── memory.c                   # إدارة الذاكرة
│   └── io.c                       # الإدخال/الإخراج
│
├── stdlib/                        # المكتبة القياسية
│   ├── string.h
│   ├── string.c
│   ├── math.h
│   └── math.c
│
├── tests/                         # الاختبارات
│   ├── valid/                     # برامج صحيحة
│   │   ├── 001-variable.deff
│   │   ├── 002-constant.deff
│   │   └── ...
│   │
│   ├── invalid/                   # برامج خاطئة
│   │   ├── 001-reserved-word.deff
│   │   ├── 002-undefined-type.deff
│   │   └── ...
│   │
│   ├── runtime/                   # اختبارات التنفيذ
│   │   ├── 001-arithmetic.deff
│   │   ├── 002-comparison.deff
│   │   └── ...
│   │
│   ├── types/                     # اختبارات الأنواع
│   │   ├── 001-int.deff
│   │   ├── 002-float.deff
│   │   └── ...
│   │
│   ├── memory/                    # اختبارات الذاكرة
│   │   ├── 001-stack.deff
│   │   ├── 002-heap.deff
│   │   └── ...
│   │
│   ├── abi/                       # اختبارات ABI
│   │   ├── 001-register-passing.deff
│   │   └── ...
│   │
│   ├── lexer/                     # اختبارات Lexer
│   │   ├── 001-numbers.deff
│   │   └── ...
│   │
│   ├── parser/                    # اختبارات Parser
│   │   ├── 001-var-decl.deff
│   │   └── ...
│   │
│   └── conformance/               # اختبارات التوافق
│       └── 001-full-program.deff
│
└── tools/                         # أدوات مساعدة
    ├── daad.c                     # البرنامج الرئيسي
    └── Makefile
```

## مبادئ التنظيم

### 1. الفصل بين المكونات

- **compiler/**: المترجم الأساسي
- **backends/**: المعماريات المستقلة
- **runtime/**: مكتبة التشغيل
- **stdlib/**: المكتبة القياسية
- **tests/**: الاختبارات

### 2. سهولة التوسع

عند إضافة معمارية جديدة:
1. إنشاء مجلد جديد في `backends/`
2. تنفيذ الواجهة المطلوبة
3. ربطه بالمترجم الرئيسي

### 3. الاختبارات الشاملة

كل مكون له اختبارات خاصة:
- `tests/lexer/` - اختبارات المحلل البصري
- `tests/parser/` - اختبارات المحلل النحوي
- `tests/types/` - اختبارات نظام الأنواع
- `tests/memory/` - اختبارات الذاكرة
- `tests/abi/` - اختبارات واجهة التطبيق الثنائية
- `tests/conformance/` - اختبارات التوافق الشاملة

## الملفات الرئيسية

| الملف | الوصف |
|-------|-------|
| `tools/daad.c` | نقطة الدخول الرئيسية |
| `compiler/lexer/lexer.c` | تنفيذ المحلل البصري |
| `compiler/parser/parser.c` | تنفيذ المحلل النحوي |
| `compiler/semantic/semantic.c` | تنفيذ المحلل الدلالي |
| `compiler/ir/ir.c` | تنفيذ التمثيل الداخلي |
| `compiler/optimizer/optimizer.c` | تنفيذ المُحسّن |
| `compiler/backend/backend.c` | تنفيذ الخلفية |
| `backends/x86/x86.c` | دعم x86-64 |
| `backends/arm64/arm64.c` | دعم ARM64 |
| `backends/riscv/riscv.c` | دعم RISC-V |

---

**تاريخ آخر تحديث:** 2026-08-03
