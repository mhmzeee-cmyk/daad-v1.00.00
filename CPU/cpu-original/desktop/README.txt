# ═══════════════════════════════════════════════
#  معالج ض — المحاكي والمجمّع (8-bit)
#  Dhad Processor v2.0 — Assembler & Simulator
# ═══════════════════════════════════════════════

## Requirements / المتطلبات
- Windows 10/11
- لا يحتاج أي برامج إضافية (portable)

## Usage / الاستخدام

### الطريقة 1: السحب والإفلات
1. اسحب ملف `.ضasm` وأفلته على ملف `شغّل..bat`
2. ستظهر النتيجة فوراً

### الطريقة 2: التيرمنال
```
dhad_cpu.exe examples\add.ضasm --state
```

## Command Options / خيارات التشغيل
```
dhad_cpu.exe <ملف> [خيارات]

--state    عرض حالة المعالج الكاملة
--hex      عرض الكود المجمّع hexadecimal
--debug    تتبع تنفيذ تعليمة بتعليقمة
--bin      حفظ الكود المجمّع كملف .bin
--out <ف>  تحديد اسم ملف الإخراج
```

## Examples / الأمثلة الجاهزة
```
dhad_cpu.exe examples\add.ضasm --state          # جمع رقمين
dhad_cpu.exe examples\countdown.ضasm --state    # عد تنازلي
dhad_cpu.exe examples\factorial.ضasm --state    # مضروب
dhad_cpu.exe examples\function.ضasm --state     # نداء دالة
dhad_cpu.exe examples\logic.ضasm --state        # عمليات منطقية
dhad_cpu.exe examples\hello.ضasm                # مرحبا بالعالم
dhad_cpu.exe examples\test_full.ضasm --state    # اختبار شامل
```

## ISA Reference / مرجعة مجموعة التعليمات

### Instructions / التعليمات
| Mnemonic | Arabic | Description |
|----------|--------|-------------|
| `حمّل سج,imm8` | LOAD | تحميل قيمة فورية 8-bit |
| `نقل الوجه,المصدر` | MOV | نقل بين السجلات |
| `اقرأ سج,عنوان` | LDMEM | تحميل من الذاكرة |
| `خزن سج,عنوان` | STMEM | تخزين في الذاكرة |
| `ادفع` | PUSH | دفع المح للمكدس |
| `اسحب` | POP | سحب من المكدس للمح |
| `نداء عنوان` | CALL | نداء دالة |
| `إرجاع` | RET | العودة من الدالة |
| `جمع سج` | ADD | المح += سج |
| `طرح سج` | SUB | المح -= سج |
| `ضرب سج` | MUL | المح *= سج |
| `قسمة سج` | DIV | المح /= سج |
| `باقي سج` | MOD | المح %= سج |
| `اطبع` | PRINT | طباعة المح كرقم |
| `اطبع_حرف` | PRINT_CH | طباعة المح كحرف |
| `قفز عنوان` | JMP | قفز دائم |
| `قفز_إذا_صفر عنوان` | JZ | قفز إذا Z=1 |
| `قفز_إذا_غيرصفر عنوان` | JNZ | قفز إذا Z=0 |
| `قفز_إذا_حمل عنوان` | JC | قفز إذا C=1 |
| `قفز_إذا_سالب عنوان` | JN | قفز إذا N=1 |
| `توقف` | HALT | إيقاف المعالج |

### Registers / السجلات
| Name | Description |
|------|-------------|
| س0-س7 | سجلات عامة 8-bit |
| المح (acc) | مسجل التراكم |
| Z, N, C | أعلام الحالة |

## File Structure / هيكل المجلد
```
desktop/
├── dhad_cpu.exe          ← المترجم والمحاكي
├── شغّل..bat             ← ملف التشغيل
├── README.txt            ← هذا الملف
└── examples/
    ├── add.ضasm          # جمع رقمين
    ├── countdown.ضasm    # عد تنازلي
    ├── factorial.ضasm    # مضروب
    ├── function.ضasm     # نداء دالة
    ├── hello.ضasm        # مرحبا بالعالم
    ├── logic.ضasm        # عمليات منطقية
    └── test_full.ضasm    # اختبار شامل
```
