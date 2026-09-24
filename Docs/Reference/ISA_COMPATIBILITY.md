# ISA_COMPATIBILITY.md — مصفوفة توافق ISA (من الكود الفعلي، لا من الوثائق القديمة)
> المصادر: `CPU/src/main.c` (مجمّع CLI + محاكي CLI)، `CPU/src/dhad_asm.c` (المجمّع المشترك)،
> `CPU/src/dhad_cpu.c` (النواة الموحدة: GUI/live)، `DAAD/compiler/backend/dhad_backend.c`,
> `CPU/include/dhad_isa_constants.h` + `CPU/hardware/HARDWARE_SPEC.md` (المواصفة).

## Extended opcodes (`0xF0` prefix) — الجدول الكامل

| Opcode | Instruction | CLI assembler (main.c + dhad_asm.c) | CLI CPU (main.c) | GUI CPU (dhad_cpu.c) | DAAD backend | Tests | Meaning |
|---|---|---|---|---|---|---|---|
| 0x00 | JMP | ✅ addr16 | ✅ | ✅ | ✅ | T3a | قفز غير شرطي |
| 0x10 | JZ | ✅ | ✅ | ✅ | ✅ | T3b | قفز إذا صفر |
| 0x20 | JNZ | ✅ | ✅ | ✅ | ✅ | (T9 loop) | قفز إذا غير صفر |
| 0x30 | JC | ✅ | ✅ | ✅ | ✅ | T3d | قفز إذا حمل |
| 0x40 | JN | ✅ | ✅ | ✅ | ✅ | T3e | قفز إذا سالب |
| 0x50 | PRINT_CH | ✅ | ✅ | ✅ | ✅ | T10a | طباعة حرف |
| 0x60 | INPUT | ✅ | ✅ | ✅ | ✅ | T10b | إدخال |
| 0x70–0x7F | XOR r | ✅ (`E0\|r` شكلًا، base 0x70) | ✅ base+reg | ✅ base+reg | ✅ | T4c | أو حصري |
| 0x80–0x8F | OR r | ✅ | ✅ | ✅ | ✅ | T4b | أو منطقي |
| 0x90–0x9F | AND r | ✅ | ✅ | ✅ | ✅ | T4a | و منطقي |
| 0xA0 | NOT | ✅ | ✅ | ✅ | ✅ | T4d | ليس |
| 0xB0 | SHL | ✅ | ✅ | ✅ | ✅ | T4e | إزاحة يسار |
| 0xC0 | SHR | ✅ | ✅ | ✅ | ✅ | T4f | إزاحة يمين |
| 0xD0 | SWAP | ✅ 3B operands | ✅ | ✅ | ✅ | T5c | تبديل |
| 0xD1 | DEC | ✅ | ✅ | ✅ | ✅ | T5a | إنقاص |
| 0xD2 | INC | ✅ | ✅ | ✅ | ✅ | T5b | زيادة |
| 0xD3 | HALT | ✅ | ✅ | ✅ | ✅ | T5d | توقف |
| 0xE0 | CMP s0 | ✅ | ✅ | ✅ | ✅ | T4g | مقارنة (s0 فقط آمنة في CLI) |
| 0xE1 | **EI في CLI / CMP s1 في GUI** | يُصدِر EI فقط | **EI** | **CMP s1** | ⚠️ يتجنب (يستخدم s0/s4-s7) | T6 (كمقاطعات) | ⚠️ **DIVERGENCE** |
| 0xE2 | **DI في CLI / CMP s2 في GUI** | يُصدِر DI فقط | **DI** | **CMP s2** | ⚠️ يتجنب | T6 | ⚠️ **DIVERGENCE** |
| 0xE3 | **RETI في CLI / CMP s3 في GUI** | يُصدِر RETI فقط | **RETI** | **CMP s3** | ⚠️ يتجنب | T10c/T6 | ⚠️ **DIVERGENCE** |
| 0xE4–E7 | CMP s4–s7 | ✅ (`E0\|r`) | ✅ (مُصلح B2b) | ✅ | ✅ (s4 مُستخدم) | T4h (جديد) | مقارنة — متوافق |
| 0xF1 | LDRI (4B: dst,hi,lo) | ✅ (مُصلح B2a) | ✅ 16-bit كامل | ✅ 16-bit كامل (مُصلح B2a) | ✅ يولّد | T17 (مُوصولة الآن) | قراءة غير مباشرة |
| 0xF2 | STRI (4B: src,hi,lo) | ✅ (مُصلح B2a) | ✅ 16-bit كامل | ✅ 16-bit كامل (مُصلح B2a) | ✅ يولّد | T17 | كتابة غير مباشرة |
| 0xF3 | EI (مواصفة) | ❌ (يُصدِر E1) | ❌ (E1 هو EI) | ✅ | — (لا يولّد مقاطعات) | — | تفعيل مقاطعات |
| 0xF4 | DI (مواصفة) | ❌ (E2) | ❌ (E2) | ✅ | — | — | تعطيل مقاطعات |
| 0xF5 | RETI (مواصفة) | ❌ (E3) | ❌ (E3) | ✅ | — | — | عودة من مقاطعة |

## ملاحظات التوافق
1. لا ملف `.ضasm` في الحزمة يستخدم `cmp s1/s2/s3` (مُتحقق بـ grep) — التباين غير مُثار في الاختبارات.
2. لا ملفات `.bin` مخزنة تحتوي E1–E3 بمعنى CMP (المخزنة: demo/add/output قديمة ما قبل LDRI — تُعاد توليدها، غير معتمدة).
3. الـ backend يتجنب s1–s3 في CMP scratch (`dhad_backend.c:810`) — يعمل identically على الطرفين.
4. T6/T10c (مقاطعات) خضراء على CLI؛ سلوك GUI للمقاطعات غير مُتحقق (E1 هناك = CMP).

## القرار (J.1-B): OPTION B — تجميد + منع صريح، لا إعادة ترميز الآن
- **المختار: B** — الإبقاء على ISA الحالية + جعل القيد **صاخبًا**: المجمّع سيرفض `cmp s1/s2/s3`
  بتشخيص عربي واضح بدل السلوك المتباين الصامت. (التنفيذ: تعديل صغير قادم في نفس المرحلة.)
- **لماذا لا A الآن:** إعادة الترميز (EI/DI/RETI→F3/F4/F5 في main.c + المجمّعين) تُبطل أي `.bin`
  مخزنة وتتطلب ترحيل اختبارات المقاطعات + تحقق GUI — تكلفة حقيقية مقابل فائدة صفرية
  اليوم (لا مستخدم فعلي لـ `cmp s1`؛ الـ backend يتجنبها أصلًا).
- **لماذا لا C/dual-decode:** مرفوض — سلوك غير حتمي عبر الثنائيات.
- خطة الترحيل الكاملة (A) موثقة في `PHASE_J_ISA_MIGRATION_PLAN.md` — **DESIGN COMPLETE, IMPLEMENTATION DEFERRED**.
