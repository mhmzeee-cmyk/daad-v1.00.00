# ض استديو - Daad Studio

## بناء المثبّت (Installer)

### المتطلبات

1. **CMake** (3.21 أو أعلى)
   ```bash
   winget install Kitware.CMake
   ```

2. **مترجم C++** (أحد الخيارات):
   ```bash
   # MinGW-w64 عبر MSYS2
   winget install MSYS2.MSYS2
   # ثم في MSYS2:
   pacman -S mingw-w64-x86_64-gcc

   # أو Visual Studio Build Tools
   winget install Microsoft.VisualStudio.2022.BuildTools
   ```

3. **NSIS** (لبناء المثبّت):
   ```bash
   winget install NSIS.NSIS
   ```

### خطوات البناء

#### الطريقة الأولى ( PowerShell ):

```powershell
# بناء المشروع
.\build.ps1

# بناء المثبّت
.\build-installer.ps1
```

#### الطريقة الثانية ( يدوياً ):

```bash
# 1. إنشاء مجلد البناء
mkdir build
cd build

# 2. تشغيل CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DDAAD_BUILD_TESTS=ON

# 3. بناء المشروع
cmake --build . --config Release --parallel

# 4. تشغيل الاختبارات
ctest --output-on-failure

# 5. بناء المثبّت
cd ../installer
makensis installer.nsi
```

### الملفات الناتجة

| الملف | الوصف |
|-------|-------|
| `build/bin/daad-compiler.exe` | أداة سطر الأوامر |
| `build/bin/DaadTests.exe` | اختبارات الوحدة |
| `build/lib/libDaadCore.a` | المكتبة الأساسية |
| `installer/DaadStudio-Setup.exe` | المثبّت |

### استخدام أداة سطر الأوامر

```bash
# ترجمة ملف ض إلى C++
daad-compiler example.ض -o output.cpp

# عرض المساعدة
daad-compiler --help
```

### أمثلة

```daad
// مثال أساسي
صحيح س = 10 ;
طباعة( س ) ;

// دالة
صحيح مجموع( صحيح أ ، صحيح ب ) {
    ارجع أ + ب ;
}

// فئة
فئة نقاط {
    صحيح س ;
    صحيح ص ;
}
```