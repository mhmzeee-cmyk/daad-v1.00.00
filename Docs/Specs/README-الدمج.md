# دمج docs و Docs في مجلد واحد — بدون حذف

التاريخ: 2026-09-14
القاعدة: لا حذف نهائي — نسخ فقط

## ما تم:
- نسخ كل محتويات `docs/` (11 ملف / 304K) إلى `Docs/Specs/` + `Docs/Specs/vscode/`
- الأصل `docs/` ما زال موجوداً كما هو — لم يُحذف أي ملف
- الخطوة القادمة (بعد تأكيدك): حذف `docs/` الأصل وإبقاء `Docs/` كمجلد واحد يحل مشكلة ويندوز (docs==Docs)

## التقارير الأحدث المعتمدة:
1. `PROJECT_INDEX.md` — 2026-09-13 13:57 (الأحدث)
2. `DEDUP_AND_STRUCTURE_REPORT.md` — 2026-09-13 13:51
3. `PROJECT_UNDERSTANDING.md` — 2026-09-12 23:43 (63K — المرجع الشامل)
4. `STAGE2_DIFFERENCES_REPORT.md` — 2026-09-12 21:52
5. `STAGE1_REFERENCE_REPORT.md` — 2026-09-12 21:28
6. `SECURITY_FIX_PLAN.md` — 2026-09-12 21:13
7. `Docs/Reports/*` — 2026-09-12 23:43 (دفعة واحدة PHASE2/3/45/6)

## التقارير الأقدم (تبقى للتوثيق، لا تُعتمد):
- `Docs/Reference/*` — 2026-09-07 (خطط قديمة DEVELOPMENT_PLAN 238K)
- `Docs/Instructions/*` — 2026-08-31 (تعليمات ويندوز قديمة)
- `PHASE_C/D/E/FGH/I/J/K` الجذر — 2026-09-09 (مراحل منجزة، يغني عنها PROJECT_UNDERSTANDING الأحدث)
- `Docs/Archive/bak-20260913/*.bak` — نسخ احتياطية 2026-09-13 (ليست تقارير، بل كود منقول من Studio)
