# PHASE_J_ISA_MIGRATION_PLAN.md — خطة ترحيل E1–E3 (تصميم مكتمل، تنفيذ مؤجل)
> الحالة: **DESIGN COMPLETE — IMPLEMENTATION DEFERRED** (لا تغيير ترميز في هذه المرحلة).
> الوضع الحالي المجمد: `cmp s1/s2/s3` مرفوضة بتشخيص عربي صريح في كلا المجمّعين.

## الهدف المؤجل
توحيد الترميز على مواصفة Phase-12.5 (`isa_constants.h` + `HARDWARE_SPEC.md`):
`CMP = E0–E7` للكل، `EI/DI/RETI = F3/F4/F5` في كل الثنائيات.

## خطوات الترحيل (عند اعتمادها)
1. `CPU/src/main.c` المفكك: `E1–E3` ← CMP (8 حالات)، `+F3/F4/F5` ← EI/DI/RETI.
2. `CPU/src/main.c` + `CPU/src/dhad_asm.c` المجمّعان: `EI/DI/RETI` تُصدِر F3/F4/F5.
3. إزالة منع `cmp s1–s3` + إزالة تحفظ الـ backend (سطر 810) إن أمكن.
4. إبطال كل `.bin` مخزنة تحتوي E1–E3 + إعادة توليدها.
5. إعادة تشغيل: CPU suite + DHAD integration + GUI interrupt demo (E1 في GUI تعني CMP اليوم!).

## معايير القبول
- `cmp s0–s7` متطابقة CLI/GUI، مقاطعات T6/T10c خضراء على الطرفين، لا `.bin` قديمة،
  `ISA_COMPATIBILITY.md` محدثة (بدون صفوف DIVERGENCE).

## المخاطر إن نُفذت بلا خطة
كسر صامت لبرامج المقاطعات + تباين GUI/CLI معكوس. لذا: فرع + مراجعة + تنفيذ مستقبل مخصص.
