# أرشيف التقارير القديمة

**تاريخ الأرشفة:** 2026-09-12

## ماذا أُرشف؟

تم نقل المجلدين التاليين إلى هنا عبر `git mv` للحفاظ على سجل Git الكامل:

- **`Reports/`** — 62 ملف: تقارير المراحل (PHASE2/PHASE3/PHASE45/PHASE6) + تقارير التدقيق (AUDIT) + تقارير الإصلاح والتنظيف. جميعها تقارير مرحلية مكتملة لا تزال مرجعًا تاريخيًا.
- **`Root-Reports/`** — 13 ملف: تقارير الجذر الأصلية (CODE_REVIEW, ORCHESTRATOR_TEST, SECURITY_HARDENING, PROJECT_MASTER_REFERENCE) + مجلد مخطط البوابات المنطقية (9 صور PNG).

## لماذا الأرشفة؟

لم يعد`\Reports/` و `Root-Reports/` مرجعًا حيًا — كلها تقريرات مراحل مكتملة. النقل يُنظّم `Docs/` ويُبقي المجلدات النشطة (`Reference/` و `Instructions/`) في المكان المعتاد.

## المرجع الحي والمواصفات

- **المرجع الحي (DevOps/DevPlan):** [`../Reference/`](../Reference/) — خطوط البناء، خطة التطوير، بنية المجلدات، الأهداف، handover.
- **المواصفات التقنية النشطة:** [`../../docs/`](../../docs/) — LANGUAGE_SPEC, SEMANTIC_RULES, STDLIB_SPEC, KEYWORDS_REGISTRY, vscode/.
- **الأدلة والتثبيت:** [`../Instructions/`](../Instructions/) — ملفات README ودليل التثبيت وشفرات التحقق.

## الأصول في Git

جميع الملفات الأصلية لا تزال متاحة عبر سجل Git. للاسترجاع:
```bash
# مثال: استرجاع ملف من Reports/
git show HEAD~1:Docs/Reports/REPORT.md

# أو استرجاع مسار كامل
git log --oneline --follow -- Docs/Reports/
```
