ض ستوديو — نسخة Windows 10/11 (مستقرة)
=======================================

المحتويات بعد التثبيت (Program Files\DhadStudio):
  bin\hello.exe         — برنامج الترحيب (مرحبا بالعالم!)
  bin\arithmetic.exe    — العمليات الحسابية (15)
  bin\factorial.exe     — مضروب العدد 5 (120)
  validate-windows10.bat — قبول Windows 10 (3 برامج + مخارج)
  validate-windows11.bat — قبول Windows 11 (3 برامج + مخارج)

التشغيل (على الجهاز الحقيقي):
  1. ثبّت DhadStudio-Windows-x64.msi
  2. على Windows 10: شغّل validate-windows10.bat  → PASS: 3   FAIL: 0
  3. على Windows 11: شغّل validate-windows11.bat  → PASS: 3   FAIL: 0

المتطلبات: Windows 10 x64 (1607+) أو Windows 11 x64. لا مكتبات إضافية
(التنفيذيات مربوطة ستاتيكيًا: KERNEL32 + msvcrt فقط، بدون API-sets).

ملاحظة الأمانة: تم التحقق البنيوي للحزمة على Linux
(PE32+‎ وجداول MSI والاستخراج). التشغيل على Windows 10/11 الحقيقي
يُوثَّق عبر سكربتي القبول — كل نتيجة مستقلة عن الأخرى.
