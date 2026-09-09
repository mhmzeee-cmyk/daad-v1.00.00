# INTERVIEW_OPERATIONAL_BASELINE.md

> أنشئ قبل أي تعديل — مرحلة Baseline فقط.

- **وقت البداية (UTC):** 2026-09-08T10:46:52Z
- **Git branch:** `main`
- **Git commit:** `ff5999d chore: comprehensive .gitignore + remove tracked binaries`
- **Git status (قبل التشغيل):** نظيف باستثناء `?? PROJECT_UNDERSTANDING.md` + `?? Sources-Archive/binaries/` (غير متتبعة)
- **نظام التشغيل:** Linux mhmoz 7.0.0-31-generic #31-Ubuntu SMP — Ubuntu 26.04 LTS (Resolute Raccoon) x86_64
- **CMake:** 4.2.3
- **GCC/G++:** 15.2.0 (Ubuntu 15.2.0-16ubuntu1)
- **Node:** v22.22.1 / npm 9.2.0
- **Python:** 3.14.4
- **Qt:** QMake 3.1 / Qt 5.15.18 / moc 5.15.18
- **Xvfb:** متوفر (`xvfb-run` موجود) — يسمح بتشغيل GUI headless
- **VSCode:** 1.136.1 x64 متوفر
- **Wine:** **غير متوفر** (`wine: command not found`)
- **Java:** **غير متوفر** (يمنع Logisim `.circ` GUI — لكن `build_dhad_cpu.py` بايثون قد يعمل)
- **Electron:** غير مثبت مسبقًا — `npx electron` يبدأ تنزيل 44.2.0 (يحتاج شبكة/وقت)
- **الأدوات المتوفرة مباشرة:** cmake/gcc/g++/make/node/python3/Qt5/Xvfb/VSCode
- **ما يمكن تشغيله مباشرة:** Compiler (ثنائية `Compiler/build/bin/daad-compiler` موجودة) + DAAD (`DAAD/build/daad` + 11 test_* موجودة) + CPU (`CPU/dhad_cpu` + `dhad_asm` موجودتان) + Studio Web (static, `npx serve`) + Studio Server (`node_modules` مثبتة مسبقًا) + VSCode Extension (`tsc` + `out/` موجود)
- **ما يحتاج Windows:** `Releases/Interview-Release/DhadStudio-Setup-Win7.msi` (5.4M, MSI V2, msitools 0.106, 2026-09-07) + `Compiler/daad-compiler.exe` + `CPU/desktop/dhad_cpu.exe` + كل `Scripts/*.bat` و`Studio/*.bat`
- **ما يحتاج Emulator/VM:** MSI يتطلب Windows 7 حقيقيًا — لا Wine ولا VM متوفرة مسبقًا في هذه البيئة (سيتم محاولة `msitools`/`7z`/`lessmsi` للفحص الاستاتيكي كبديل موثق، وليس كتشغيل حقيقي)
- **ما يحتاج GUI/Browser:** CPU Qt GUI + Studio Web + Electron — ستُشغَّل عبر `xvfb-run` حيث أمكن
- **ما لا يمكن تشغيله في البيئة الحالية (متوقع مبدئيًا):** تثبيت MSI تفاعلي على Win7 حقيقي (BLOCKED بيئيًا) + Logisim GUI (لا Java) — سيتم توثيقهما بدقة لا الادعاء حولهما

**الثنائيات الموجودة مسبقًا (ليست دليل تشغيل — فقط Baseline):**
- `Compiler/build/bin/daad-compiler` ✅ موجود
- `DAAD/build/daad` + `libdaad_lib.a` + `test_*` (11) ✅ موجودة
- `CPU/dhad_cpu` + `CPU/dhad_asm` ✅ موجودان
- `Studio/desktop-app/server/node_modules/` ✅ مثبتة (`@apidevtools`, `@babel`...)
- `Releases/Interview-Release/DhadStudio-Setup-Win7.msi` ✅ (5.4M)

**القاعدة المطبقة من هنا:** لا `REAL-PASS` إلا بعد `BUILD → RUN → INPUT → OUTPUT → VERIFY` بدليل محفوظ في `Operational-Evidence/`.
