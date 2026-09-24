ض ستوديو — نسخة Windows 7 (تجريبية للمقابلة)
============================================

المحتويات بعد التثبيت (Program Files\DhadStudio):
  bin\hello.exe         — برنامج الترحيب (مرحبا بالعالم!)
  bin\arithmetic.exe    — العمليات الحسابية (15)
  bin\factorial.exe     — مضروب العدد 5 (120)
  bin\loops.exe         — الحلقات (5050)
  bin\functions.exe     — الدوال (26)
  validate-windows7.bat — سكربت القبول: يشغّل البرامج الخمسة ويفحص
                          المخرجات ورموز الخروج (PASS/FAIL صريح)

التشغيل:
  1. ثبّت DhadStudio-Windows7.msi
  2. افتح مجلد التثبيت وشغّل validate-windows7.bat
  3. النتيجة المتوقعة: PASS: 5   FAIL: 0

المتطلبات: Windows 7 SP1 x64 أو أحدث. لا حاجة لأي مكتبات إضافية
(التنفيذيات مربوطة ستاتيكيًا: KERNEL32 + msvcrt فقط).

ملاحظة الأمانة: تم التحقق البنيوي للحزمة على Linux
(PE32+‎ وجداول MSI والاستخراج). التشغيل على Windows 7 الحقيقي
يُوثَّق عبر validate-windows7.bat على جهاز حقيقي.
