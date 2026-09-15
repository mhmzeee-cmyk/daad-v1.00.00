# نواقص لغة ض عالية المستوى — مقارنة C++ (الأصل) مقابل JS (السحابية)

![شعار لغة ض — حرف ض أخضر على خلفية بيضاء](docs/logo-ض.jpeg)

> **الشعار الرسمي للغة:** `docs/logo-ض.jpeg` — حرف **ض** أخضر داكن على خلفية بيضاء.
> المصدر: `/home/m_hmoz/Downloads/Gemini_Generated_Image_a3k73oa3k73oa3k7.jpeg` (نُسخ إلى `docs/logo-ض.jpeg` ليكون شعار اللغة المعتمد).
> الاستخدام المعتمد: أعلى كل صفحة توثيق + `Studio/Web/icons.svg` مستقبلًا + `README` + شاشة البداية `splash.html`.
> **النطاق:** هذه الوثيقة تقارن **فقط** مسار اللغة عالية المستوى:
> - **الأصل C++:** `Compiler/` (`.ض` → C++20) — `src/Keywords.cpp` + `include/Daad/Keywords.hpp` + `src/Lexer.cpp` + `src/Parser.cpp (1683)` + `include/Daad/AST.hpp (677)` + `src/CodeGen.cpp (840)` + `src/SandboxValidator.cpp` + `stdlib/*.hpp` + `stdlib/*/*.ض (100 ملف)` + `stdlib_arabic/`
> - **السحابية JS:** `Studio/Web/js/` — `dhad-lexer.js (703)` + `dhad-parser.js (1572)` + `dhad-ast.js (639)` + `dhad-codegen.js (1065)` + `dhad.js (264)` + `dhad-libraries.js (2765)` + `dhad-images.js (512)` + `dhad-highlight.js (280)` + نسخة السيرفر `Studio/desktop-app/server/src/utils/dhad/`
> - **خارج المعادلة تمامًا:** النسخة منخفضة المستوى (`.daad` → x86/DHAD في `DAAD/`) — لا تُقرأ ولا تُقارن هنا بطلب صريح.

> **قاعدة الدقة:** `FACT` = مثبت من ملف:سطر مقروء. `INFERENCE` = استنتاج. `GAP` = فرق مثبت يستدعي إضافة/إصلاحًا.
> **القاعدة الذهبية للاتجاه:** الأصل C++ هو **مرجع الحقيقة للغة** (يُنتج C++ يُترجم بـ g++). نسخة JS يجب أن **تطابقه سلوكيًا** (تقبل نفس البرامج وتُنتج نفس النتائج عبر JS)، لا العكس — إلا حيث يتفوق JS بميزة ويب مشروعة (تُوثق كإضافة مقصودة لا كنقص في C++).

---

## 0. الخلاصة التنفيذية — أين النواقص؟

| المجال | عدد النواقص المثبتة | الأخطر |
|---|---|---|
| الكلمات المفتاحية (Keywords) | 1 كلمة جوهرية (`ليس`) + 4 عيوب تلوين | `ليس` تعمل في JS كـ `!` ولا تعمل في C++ إطلاقًا |
| المعاملات (Operators) | 7 فروقات (`%=` `&` `->` `::` `@` `#` `0x`/`e`) | `%=` موجودة في C++ مفقودة في JS؛ `&`/`->`/`::` موجودة في JS مفقودة في C++ |
| الأرقام والنصوص | 8 فروقات (هندية/سداسية/علمية/اقتباس/ثماني) | الأرقام المشرقية `١٢٣` لا تعمل في **الطرفين**؛ `0x`/علمي يعمل في JS فقط |
| عقد AST | 9 فروقات هيكلية | `Raw` في C++ فقط؛ `Program` في JS فقط؛ `base` للمصفوفات متعددة الأبعاد في C++ فقط |
| المحلل Parser | 6 فروقات (GUI/إدخال/قوالب) | ودجات GUI تُحلل في JS ولا تُحلل في C++؛ `ادخل` متعدد في JS مفرد في C++ |
| مولد الكود CodeGen | 20+ فروقات (17 خطأ نقطة + TODO + struct + sizeof) | 17 سطرًا في JS تولد `__dhad loadImage` (بدون نقطة) وهو **كود مكسور**؛ `// TODO: type` يبتلع العقد المجهولة silently |
| المكتبة القياسية Stdlib | ~687 دالة C++ فريدة مقابل ~264 دالة JS فريدة (التقاطع ~30 فقط) | كل طرف يملك مئات الدوال التي لا يملكها الآخر — التفصيل في §7 |
| Sandbox الأمان | مجموعتان منفصلتان تمامًا (18 نمط C++ مقابل 26 نمط JS) | لا تغطية متقاطعة إطلاقًا — كل مولد محمي من هجمات لغته فقط |
| التلوين Highlight | 7 كلمات مفقودة/زائدة/مُصحفة | `ادخل/اذا/بينما/كرر/فئة/اطبع/س/ص` لا تُلوَّن؛ `قائمة` تلوَّن خطأً؛ `مجرد` مُصحفة بدون شدة |
| نسخة السيرفر | 4 ملفات مكررة (lexer/parser/ast/codegen) | انحراف صامت محتمل بين `Web/js` و`server/src/utils/dhad` |

**الإجمالي: ~70 ناقصًا مثبتًا بالملف:السطر.** كل واحد موثق أدناه مع **مكانه + لماذا + طريقة الإصلاح خطوة بخطوة + كود الإضافة الجاهز**.

---

## 1. سجل الكلمات المفتاحية الكامل — الطرفان جنبًا إلى جنب

### 1.1 سجل C++ (المصدر: `Compiler/src/Keywords.cpp:8-149` + `include/Daad/Keywords.hpp:11-54`)

```
[أنواع 12]: صحيح→int، عشري→double، منطقي→bool، حرف→char، نص→string، ثابت→const،
  فراغ→void، تلقائي→auto، صواب→true، خطأ→false، عدم→nullptr، عرّف→typedef
[تحكم 20 سطحيًا]: إذا/اذا→if، وإلا→else، طالما/بينما→while، افعل→do، لكل/كرر→for،
  في→for، اختر→switch، حالة→case، افتراضي→default، توقف/اكسر→break، استمر/تابع→continue،
  انتقل→goto (مسجلة لكن Parser يرفضها)، س→x، ص→y، بديل→else if، زد→++، انقص→--
[كائنات 14]: صنف/فئة→class، هيكل→struct، تعداد→enum، واجهة→class، نطاق→namespace،
  عام→public، خاص→private، محمي→protected، يرث→public، ذاتي/هذا→this، الأصل→(بلا مكافئ)، مجرّد→virtual
[دوال/ذاكرة 10]: دالة→(خاص)، ارجع→return، جديد→new، احذف→delete، مؤشر→*، مرجع→&،
  ساكن→static، مضمن→inline، خارجي→extern، قالب→template
[أخطاء 10]: حاول→try، امسك→catch، أخيراً→finally، ارمِ→throw، تأكد→assert،
  استثناء→std::exception، نوع_الـ→decltype، حجم_الـ→sizeof
[متقدم 10]: تزامن→mutex، انتظر/بانتظار→co_await، خيط→thread، احجز→lock_guard،
  مشترك→shared_ptr، فريد→unique_ptr، استورد→#include، صدّر→export، وحدة→module
[منطق عربي 2]: و→&&، أو→||
[IO ‏2]: طباعة/اطبع→daad_print، ادخل→cin
[GUI ‏14]: زر_أمر→QPushButton، حقل_نص→QLineEdit، قائمة_خيارات→QComboBox، صورة→QLabel،
  مربع_اختيار→QCheckBox، شريط_تمرير→QSlider، قائمة_منسدلة→QComboBox، لوحة→QWidget،
  تسمية→QLabel، عمود→QVBoxLayout، صف→QHBoxLayout، شبكة→QGridLayout، شريط_تلوين→QProgressBar، علامة_تبويب→QTabWidget
[صور 20]: حمّل_صورة→image::load، ارسم_صورة→draw، حجم_صورة→size، احفظ_صورة→save،
  قص_صورة→crop، غيّر_حجم→resize، لف_صورة→rotate، قلب_صورة→flip، شفافية→opacity،
  فلتر→filter، تراكب→overlay، خلفية→background، بكسل→pixel، ارسم→drawShape، ملء→fill،
  مستطيل→rectangle، دائرة→circle، خط→line، نص_على_لوحة→text، مسح→clear
الإجمالي: ~104 صيغ سطحية → ~95 نوعًا (KeywordType)
```

### 1.2 سجل JS (المصدر: `Studio/Web/js/dhad-lexer.js:179-315` — تعليقها `103 total: 83 base + 20 image`)

```
[أنواع 12]: صحيح، عشري، منطقي، حرف، نص، فراغ، تلقائي، ثابت، عرّف، صواب، خطأ، عدم (مطابق C++)
[تحكم]: إذا/اذا، وإلا، طالما/بينما، لكل/كرر، افعل، اختر، حالة، افتراضي، توقف/اكسر،
  استمر/تابع، انتقل، بديل، زد، انقص (مطابق C++ سطحيًا)
[كائنات 14]: صنف/فئة، هيكل، تعداد، واجهة، نطاق، عام، خاص، محمي، يرث، هذا/ذاتي، الأصل، مجرّد (مطابق)
[دوال 10]: دالة، ارجع، جديد، احذف، مؤشر، مرجع، ساكن، مضمن، خارجي، قالب (مطابق)
[أخطاء 8]: حاول، امسك، أخيراً، ارمِ، تأكد، استثناء، نوع_الـ، حجم_الـ (مطابق)
[متقدم 10]: تزامن، انتظر/بانتظار، خيط، احجز، مشترك، فريد، استورد، صدّر، وحدة، في (مطابق)
[خاص 6]: طباعة/اطبع، ادخل، ليس→KW_NOT، و→AND_ARABIC، أو→OR_ARABIC
[محاور 2]: س→KW_X، ص→KW_Y (مطابق)
[GUI ‏14]: (مطابق حرفيًا)
[صور 20]: (مطابق حرفيًا)
```

### 1.3 GAP-K1 — `ليس` (NOT العربي) — **الناقص الأخطر في الكلمات**

- **المكان:**
  - JS موجود: `dhad-lexer.js:84` (`KW_NOT: 'KW_NOT' // ليس`) + `:270` (`'ليس': TT.KW_NOT`) + `dhad-parser.js:1051` (`ليس→!`) + `dhad-highlight.js:28` (`ARABIC_LOGIC` فيها `ليس`).
  - C++ مفقود: لا يوجد `m_keywordToType["ليس"]` في `Keywords.cpp:8-149` (البحث يعيد صفرًا) ولا `KwNot` في `Keywords.hpp:11-54`. الإشارة الوحيدة `Parser.cpp:375` (`TOKEN_KEYWORD && ليس`) ميتة — لا يمكن الوصول إليها لأن `ليس` تُlex كـ `IDENTIFIER` أصلًا.
- **لماذا:** نسيان عند نقل القاموس — كل المنطق العربي (`و/أو`) نُقل (`Keywords.cpp:104-105`) ونُسي `ليس`. الدليل أن التعليق يقول `// 8c. منطق عربي — يتوافق مع Web` لكنه يحوي سطرين فقط بدل ثلاثة.
- **الأثر:** برنامج `إذا (ليس صواب) { ... }` يعمل في السحابة ويفشل في الأصل (`Variable(ليس)` ثم خطأ `لم يتم التعرف`).
- **طريقة الإصلاح — إضافة كلمة (تُطبق حرفيًا):**
  1. في `Compiler/include/Daad/Keywords.hpp:45` أضف النوع:
     ```cpp
     // 8c. Arabic logic
     KwAndArabic, KwOrArabic, KwNotArabic,
     ```
  2. في `Compiler/src/Keywords.cpp:103-105` سجّل الكلمة:
     ```cpp
     m_keywordToType["و"]        = KeywordType::KwAndArabic;
     m_keywordToType["أو"]       = KeywordType::KwOrArabic;
     m_keywordToType["ليس"]      = KeywordType::KwNotArabic;   // <-- إضافة
     ```
  3. في `Keywords.cpp:256-258` أضف المكافئ:
     ```cpp
     case KeywordType::KwAndArabic:   info.cppEquivalent = "&&"; break;
     case KeywordType::KwOrArabic:    info.cppEquivalent = "||"; break;
     case KeywordType::KwNotArabic:   info.cppEquivalent = "!"; break;   // <-- إضافة
     ```
  4. في `Compiler/src/Parser.cpp` عالجها أينما عولج `!`: في `parseUnary` أضف فرع `ليس` → `Unary(!)`، وفي `getCurrentPrecedence` لا حاجة (أحادي).
  5. في `Compiler/src/CodeGen.cpp:visit(Unary)` تحقق أن `!` يُطبع (موجود أصلًا).
  6. أضف اختبارًا: `صحيح س = 5 ؛ إذا (ليس خطأ) { طباعة(س) ؛ }` يجب أن يُنتج `if ((!false))`.
  7. حدّث `docs/KEYWORDS_REGISTRY.json`: أضف `"ليس"` لفئة `arabic_logic` وارفع `total_keywords` إلى 116.

### 1.4 GAP-K2 — عيوب التلوين السبعة (Highlight فقط — لا تكسر التنفيذ لكن تضلل المتعلم)

المصدر: `Studio/Web/js/dhad-highlight.js:10-42` مقابل السجلين أعلاه. `FACT` كل سطر:

| # | الكلمة | الوضع | المكان | الإصلاح (سطر واحد لكل كلمة) |
|---|---|---|---|---|
| 1 | `ادخل` | مفقودة من التلوين (موجودة في الطرفين) | `highlight.js:29` فيها `طباعة` فقط | أضف `'ادخل'` إلى `PRINT_KEYWORD` أو أنشئ `INPUT_KEYWORD` |
| 2 | `اذا` (بدون همزة) | مفقودة | `CONTROL` فيها `إذا` فقط | أضف `'اذا'` بجانب `'إذا'` |
| 3 | `بينما` | مفقودة | `CONTROL` فيها `طالما` فقط | أضف `'بينما'` |
| 4 | `كرر` | مفقودة | `CONTROL` فيها `لكل` فقط | أضف `'كرر'` |
| 5 | `فئة` | مفقودة | `TYPE` فيها `صنف` فقط | أضف `'فئة'` |
| 6 | `اطبع` | مفقودة | `PRINT` فيها `طباعة` فقط | أضف `'اطبع'` |
| 7 | `س` `ص` | مفقودة | لا قائمة محاور إطلاقًا | أضف `AXIS_KEYWORDS = ['س','ص']` وسجّلها |
| 8 | `قائمة` (عارية) | **زائدة خطأً** — لا توجد في أي lexer (الصحيح `قائمة_خيارات`/`قائمة_منسدلة`) | `TYPE:12` | احذف `'قائمة'` من `TYPE_KEYWORDS` |
| 9 | `مجرد` | **مُصحفة** (بدون شدة `651`) — الكلمة الحقيقية `مجرّد` بشدة | `CONTROL:21` | استبدل `'مجرد'` بـ `'مجرّد'` (انسخها من `dhad-lexer.js:229`) |

**طريقة الإصلاح العامة لأي كلمة تلوين:** افتح `dhad-highlight.js` وأضف/احذف النص في المصفوفة المناسبة فقط — لا حاجة لأي تغيير في `ALL_KEYWORDS` (تُبنى تلقائيًا `:44-52`).

---

## 2. سجل المعاملات الكامل + النواقص

### 2.1 جدول الحقيقة (FACT)

| المعامل | C++ (`Lexer.cpp:64-114` + `Token.hpp:8-57`) | JS (`dhad-lexer.js:600-676`) | الحكم |
|---|---|---|---|
| `+ - * / % = < > ! ( ) { } [ ] ; : . , ?` | نعم | نعم | متطابق |
| `== != <= >= && \|\| += -= *= /= ++ -- ^ ^=` | نعم | نعم | متطابق |
| `و` `أو` (عربي) | نعم (`347-348` → `&&` `\|\|`) | نعم (`157-158` + precedence) | متطابق |
| `%=` | **نعم** (`90` + `Token PERCENT_EQUALS` + `Parser 195`) | **لا** (لا `PERCENT_ASSIGN`؛ `parseIdentifierStmt:913` و`parseExprStatement:940` يسقطانه) | **GAP-O1: JS ناقصة** |
| `&` المفردة | **لا** (فقط `&&`؛ المفردة → `رمز غير معروف:117`) | **نعم** (`AMPERSAND:172` + `parseUnary &→passthrough:1072`) | **GAP-O2: C++ ناقص** |
| `->` | **لا** (تُlex `-` ثم `>` منفصلين؛ `Parser:555` يفحص توكنين) | **نعم** (`ARROW:152,638`) | **GAP-O3: C++ ناقص توكنًا** (يعمل بفحص توكنين لكن هش) |
| `::` | **لا** | **نعم** (`DOUBLE_COLON:173,644` + `parsePostfix :::1108`) | **GAP-O4: C++ ناقص** |
| `@` `#` | **لا** (`#` → unknown) | **نعم** (`AT:174,HASH:175` + `#→comment:576`) | **GAP-O5: C++ ناقص تعليق `#`** |
| `\|` المفردة | لا في الطرفين | لا في الطرفين | متطابق (غير مدعوم — يوثق كقيد) |
| `<<` `>>` `&=` `\|=` | لا في الطرفين | لا في الطرفين | متطابق (غير مدعوم) |

### 2.2 GAP-O1 — `%=` مفقودة في JS

- **المكان:** C++ موجود (`Lexer.cpp:90` + `Parser.cpp:195,250` + `CodeGen:290`). JS: لا `TT.PERCENT_ASSIGN` في `:130-154`، ولا فرع في `parseIdentifierStmt:913` (`+=|-=|*=|/=|^=` فقط) ولا `parseExprStatement`.
- **لماذا:** نسيان عند نسخ قائمة الإسناد المركب — كلها نُسخت إلا `%=`، والتلوين تبعها (`highlight.js:177-193` بلا `%=`).
- **الإصلاح (JS — 4 أسطر):**
  1. في `TT` أضف بعد `SLASH_ASSIGN:149`: `PERCENT_ASSIGN: 'PERCENT_ASSIGN', // %=`
  2. في `tokenize` بعد فرع `/= (:629)`: `if (cp===37 && c2===61) { push(PERCENT_ASSIGN,'%='); advance(2); continue; }`
  3. في `parseIdentifierStmt` و`parseExprStatement` أضف `'%='` لقائمة المركب.
  4. في `dhad-codegen.js:emitCompoundAssignment` لا حاجة (يمرر `op` كما هو — `%=` صالح في JS).
  5. في `highlight.js` أضف `'%='` لقائمة المعاملات.

### 2.3 GAP-O2/O3/O4/O5 — `&` `->` `::` `#` مفقودة في C++ (موجودة في JS)

- **لماذا مهمة:** `&` لازمة لمعامل `مرجع→&` (`Keywords.cpp:207` يسجلها لكن لا توكن لها!)؛ `->` لازمة لتعريف الدوال `دالة f(...) -> نوع` (تعمل اليوم بفحص توكنين `Parser:555` لكن أي مسافة/تعليق بينهما قد يكسرها)؛ `::` لازمة لاستدعاءات النطاق؛ `#` تعليق عربي يستخدمه المتعلمون في السحابة فيفشل في الأصل.
- **الإصلاح (C++ — بإضافة توكنات):**
  1. في `Token.hpp:8-57` أضف: `TOKEN_ARROW, TOKEN_DOUBLE_COLON, TOKEN_AMPERSAND, TOKEN_AT, TOKEN_HASH` (و`TOKEN_PERCENT_EQUALS` موجودة أصلًا).
  2. في `Lexer.cpp` قبل المفردات أضف الفحص الثنائي: `->` `::`، والمفرد: `& @ #` (`#` → تخطَّ حتى نهاية السطر كتعليق مثل JS).
  3. في `Parser.cpp:parseVariableDeclaration:555` استبدل فحص `MINUS+GREATER` بفحص `TOKEN_ARROW` الواحد.
  4. في `parseUnary` أضف تمرير `&` `*` (مثل JS:1072/1067) لدعم `مرجع`/`مؤشر`.
  5. في `CodeGen.cpp` لا تغيير (تُطبع كما هي).

---

## 3. الأرقام والنصوص والترميز — 8 فروقات

### 3.1 GAP-N1 — الأرقام المشرقية `١٢٣` لا تعمل في **الطرفين** (الأخطر تعليميًا)

- **المكان:** `Compiler/src/Lexer.cpp:120-132` (`while '0'-'9'` فقط) + `UnicodeUtils.cpp:64-73` (لا `0660/06F0`)؛ `dhad-lexer.js:399-401` (`48-57` فقط) + `readNumber:449`. البحث عن `0660|06F0|Indic` يعيد صفرًا في الطرفين. **FACT:** اختبار `Tests/run_all_tests.sh:222` يستخدم `١٠` مشرقية على Web Lexer — سيفشل أو يُlex كـ UNKNOWN (التقرير القديم سجله UNKNOWN ولم يُحسم).
- **لماذا:** كلتاهما تفترضان ASCII فقط رغم أن اللغة عربية.
- **الإصلاح (يُطبق في الطرفين — القرار التصميمي: **قبول وتطبيع** لا رفض):**
  - C++ في `scanNumber`: قبل الحلقة طبّع `٠-٩ (U+0660-0669)` و`۰-۹ (U+06F0-06F9)` إلى `0-9` (اطرح `0x0660`/`0x06F0` وأضف `'0'`). وفي `UnicodeUtils::isDigit` أضف النطاقين.
  - JS في `isDigit`: `return (cp>=48&&cp<=57)||(cp>=0x0660&&cp<=0x0669)||(cp>=0x06F0&&cp<=0x06F9)` وفي `readNumber` طبّع بنفس الطرح قبل `parseFloat/parseInt`.
  - اختبار القبول: `صحيح س = ١٢٣ ؛ طباعة(س) ؛` يجب أن يطبع `123` في الطرفين.

### 3.2 GAP-N2 — السداسي `0x` والعلمي `e` والبادئة `.5` تعمل في JS فقط

- **المكان:** JS: `readNumber:456` (`0x`) + `:480` (`e/E±`) + `:588` (`.5`) مع تطبيع `parseFloat/parseInt:495`. C++: `scanNumber:120` عشري فقط (`0x10` تُlex `0` ثم `x10`؛ `1e3` تُlex `1` ثم `e3`).
- **لماذا:** C++ كُتبت للمنهج الأساسي فقط.
- **الإصلاح (C++ — اختياري لكن موصى به للتوافق):** انسخ منطق JS الثلاثة (hex/sci/leading-dot) إلى `scanNumber`، أو **وثّق القيد** رسميًا في `docs/LANGUAGE_SPEC.md` بأن `0x` والعلمي `غير مدعوم في الأصل` (الحل الأدنى المقبول: توثيق + خطأ عربي واضح بدل lex صامت خاطئ).

### 3.3 GAP-S1 — الاقتباس المفرد `'` يعمل في JS فقط

- **المكان:** C++ `Lexer.cpp:60` (`"` فقط؛ `'` → `رمز غير معروف:117`). JS `readString:582` (`"` أو `'`).
- **الإصلاح (C++ سطر واحد):** في `scanString` اقبل `'\''` كفاصل مثل `'"'` (أو وثّق أن `'` مرفوض).

### 3.4 GAP-S2 — الثماني `\033` يعمل في C++ فقط؛ `\b` يُحفظ في C++ ويُسقط في JS

- **المكان:** C++ `Lexer.cpp:168-175` (ثماني حتى 3 أرقام) + `:183` (`\<c>` يُحفظ). JS `:518-524` (لا ثماني؛ `\<c>→c` تُسقط الشرطة — `\b→b` بدل `\b`).
- **الإصلاح (JS سطران):** أضف فرع الثماني (انسخ حلقة C++) وغيّر العام إلى حفظ `\<c>` (أو أضف `\b \f \v` صراحةً).

### 3.5 GAP-S3 — السلاسل متعددة الأسطر مسموحة في C++ ومقطوعة silently في JS؛ غير المغلقة خطأ في C++ وصامتة في JS

- **المكان:** C++ لا حارس سطر جديد (مسموح) + `نص غير مغلق:150` عند EOF. JS `:526` تُرجع عند `\n` (تقطع) و`:534` تُرجع عند EOF بلا خطأ.
- **الإصلاح (JS):** أضف خطأ `نص غير مغلق` عند EOF وقرر: إما السماح بتعدد الأسطر (احذف حارس `\n`) ليطابق C++، أو رفضه بخطأ صريح في الطرفين (وثّقه).

### 3.6 GAP-C1 — تعليق `#` يعمل في JS فقط

- **المكان:** JS `:576` (`#` → حتى `\n`). C++: `#` → unknown.
- **الإصلاح:** أضفه في C++ (سطر واحد في `getNextToken`) — تعليق عربي شائع.

---

## 4. عقد AST — 9 فروقات هيكلية (بالأسماء الدقيقة)

المصدر C++: `Compiler/include/Daad/AST.hpp:26-98` (17 تعبيرًا + 48 عبارة). المصدر JS: `Studio/Web/js/dhad-ast.js:25-556`.

| # | العقدة | C++ | JS | الحكم والإصلاح |
|---|---|---|---|---|
| 1 | `Raw` | **نعم** (`RawExprAST:176` لـ `[]` الفارغة — `Parser:471` + `CodeGen:218`) | **لا** (صفر `Raw`؛ `[]` → `FunctionCall(__array)`) | **GAP-A1:** أضف `RawExprAST` في `dhad-ast.js` + فرع `genExpr` يُرجع `raw`، أو وثّق أن `[]` تعني مصفوفة فارغة في JS مقابل خام في C++ |
| 2 | `Program` | **لا** (يستخدم `StmtList vector`) | **نعم** (`ProgramAST:366` + `parseProgram:97` + `parseBlock:865`) | **GAP-A2 (مقصود):** لا توحيد مطلوب — طبقة داخلية. وثّقها فقط |
| 3 | `ArraySubscript (+base)` | **نعم** (`199 name,index,base?` + تسلسل `416-421` + `CodeGen:582 base?base[idx]`) | **لا** (`92 name,index` فقط؛ `parsePostfix:1116` تُسقط غير-Variable؛ `genExpr:919` يتجاهل base) | **GAP-A3:** `a[i][j]` تعمل في C++ وتنكسر في JS. الإصلاح: أضف `base` في JS (انسخ حقول C++) |
| 4 | `ArraySubscriptAssign (+base)` | **نعم** (`439 +base` + `593`) | **لا** (`341 name,index,value`) | **GAP-A4:** نفس إصلاح A3 للكتابة |
| 5 | `Class` | `374 name,members:ParamList,methods:vector<Function>` + `CodeGen:485 header+qualified` | `267 name,body[]` مسطحة + `emitClasses:227` closures + `parent` ديناميكي | **GAP-A5:** shape مختلف جذريًا — التوافق بالاختبارات لا بالحقول. الإصلاح: اختبارات OOP متقاطعة (`فئة` بحقول وطرق ووراثة) تثبت التكافؤ السلوكي |
| 6 | `Struct` | `385 name فقط!` + `CodeGen:513 struct{name{}}` (**تُسقط الأعضاء!**) | `275 name,body` + `emitStructDecl:696` (تحفظ الحقول) | **GAP-A6 (C++ مكسور):** `هيكل نقطة { صحيح س ؛ }` تفقد `س` في C++ وتحفظها في JS. الإصلاح في C++: أضف `members` لـ `StructDeclAST` واطبعها في `visit(Struct)` (انسخ من `ClassDecl`) |
| 7 | `Template` | `419 kind,params,body:Stmt` + `CodeGen:563 template<>` | `323 name,params,body` + `emitStmt:401 erase` | **GAP-A7:** JS تمحو القوالب runtime (مقصود) مقابل C++ تولدها. وثّق: القوالب **compile-time في C++ وممحوة في JS** |
| 8 | `Input` | `486 varName:string مفرد` + `CodeGen:669 cin>>` | `358 varName:targets[] مصفوفة` + `CodeGen:775 forEach __input()` + parser متعدد `802-846` | **GAP-A8:** `ادخل أ، ب` تعمل في JS وتأخذ الأول فقط (أو خطأ) في C++ (`612-634` اسم واحد). الإصلاح: وسّع C++ ليقبل قائمة (انسخ حلقة JS) |
| 9 | `MemberAccess/FunctionCall` | `MemberAccess(object:string,member:string)` + `FunctionCall(name:string)` | `MemberAccess(object:expr,member)` + `FunctionCall(name:string\|MemberAccess)` للتسلسل + `::` + دمج `قص من` | **GAP-A9:** JS أغنى (تسلسل `a.b().c` + `::`). الإصلاح في C++: وسّع `object` ليكون `expr` لا `string` (كبير — بديله الأدنى: وثّق أن التسلسل العميق JS-only) |
| 10 | `ImageSize/Pixel` كتعبير | نعم (`219/226` expr + `CodeGen:730/780 m_lastExpr`) | مسجلة expr (`407/492`) لكن `genExpr:786-943` **بلا فرع لها!** (فقط `emitStmt:409/419`) | **GAP-A10 (JS مكسور):** `س = حجم_صورة(p) + 1` تعمل في C++ وتُنتج `/* unknown */` في JS. الإصلاح سطر واحد لكل فرع في `genExpr` (انسخ من `emitImageSize/emitPixel`) |

---

## 5. المحلل Parser — 6 فروقات سلوكية

### GAP-P1 — ودجات GUI تُحلل في JS ولا تُحلل في C++ (رغم تسجيل كلماتها!)

- **المكان:** JS: `parseStatement:175-189` (14 ودجت → `parseWidgetDecl:850` → `VarDecl(widgetType,name)`). C++: `grep زر_أمر|حقل_نص Compiler/src/Parser.cpp` → **صفر** (فقط الصور `119-124`)؛ `parseStatement:39-154` بلا أي فرع GUI.
- **لماذا:** الكلمات سُجلت (`Keywords.cpp:108-121` → `QPushButton...`) لكن لم يُكتب لها parser ولا codegen (لا `visit(Button)` في `CodeGen.cpp` — البحث يعيد صفرًا).
- **الإصلاح (خياران — اختر واحدًا ووثّقه):**
  - (أ) **تفعيل GUI في C++:** أضف في `parseStatement` فرعًا يحول `زر_أمر اسم = ...` إلى `VarDecl(GUI-type)` وفي `CodeGen` اطبع `QPushButton* name = new QPushButton(...)` (يحتاج Qt — ثقيل).
  - (ب) **الحل الأدنى المقبول:** اجعل C++ يرفض الودجات **بخطأ عربي صريح** (`GUI يتطلب Qt — غير مدعوم في transpiler الحالي`) بدل `لم يتم التعرف` المبهم، ووثّق أن GUI **JS-only** (السحابة ترسم بـ DOM/Canvas بينما الأصل يحتاج Qt).

### GAP-P2 — `ادخل` متعدد في JS مقابل مفرد في C++ (مفصّل في A8 أعلاه)

### GAP-P3 — `لكل` المزدوجة (`for` العداد مقابل `for-each`) محلولة بشكل مختلف

- **المكان:** C++: `parseForEachOrForStatement:660` (تفريق داخلي واحد). JS: `parseFor:376` + `parseForEach:437` منفصلان + `KW_IN:79` (`في`).
- **الحكم:** لا فجوة مثبتة بالقراءة وحدها — يحتاج اختبارًا متقاطعًا (`لكل (صحيح ا = 0 ؛ ...)` مقابل `لكل (عنصر في مجموعة)` في الطرفين). **سجّل كـ UNKNOWN يحتاج `real_regression` لغويًا.**

### GAP-P4 — `استورد/صدّر` no-op في الطرفين (متطابق — ليس فجوة لكنه قيد)

- **المكان:** C++ `CodeGen:648-654` (`// no-op`)؛ JS `emitStmt:402-403` (`break // no-op`).
- **الحكم:** متطابق — لكن يجب توثيقه للمستخدم: **الوحدات غير مدعومة تنفيذيًا في أي طرف** (أي `استورد` تُتجاهل silently — خطر تضليل). الإصلاح التوثيقي: حذّر في `LANGUAGE_SPEC.md` أو اجعلها خطأ صريحًا.

### GAP-P5 — `انتقل` (GOTO) مرفوضة في C++ بخطأ صريح، ومسجلة في JS بلا قرار واضح

- **المكان:** C++ `Parser.cpp:65-75` (رفض صريح `غير مدعومة`). JS: `KEYWORDS:210` (`انتقل→KW_GOTO`) + `TT:40` لكن `parseStatement:107-230` **بلا فرع GOTO** (ستسقط إلى `parseExprStatement` أو خطأ عام).
- **الإصلاح:** وحّد القرار: إما رفض صريح في الطرفين (انسخ رسالة C++ إلى JS) أو دعم `goto` (غير موصى به). **الموصى: رفض صريح موحد.**

### GAP-P6 — `قالب` يتطلب `<` في C++ وإلا خطأ/متغير، وفي JS أكثر تساهلًا

- **المكان:** C++ `parseTemplate:957-984` (يتطلب `<`). JS `parseTemplate:705`.
- **الحكم:** يحتاج اختبارًا متقاطعًا — سجّل UNKNOWN.

---

## 6. مولد الكود CodeGen — 20+ فروقات (الأخطر تنفيذيًا)

### GAP-C1 — 17 سطرًا تولد `__dhad loadImage` بدون نقطة (كود JS مكسور نحويًا!) — **الأخطر**

- **المكان (FACT — كل سطر):** `Studio/Web/js/dhad-codegen.js`:
  `959` (`__dhad loadImage`)، `964` (`__dhad drawImage`)، `969` (`__dhad imageSize`)، `974` (`__dhad saveImage`)، `979` (`__dhad cropImage`)، `984` (`__dhad resize`)، `989` (`__dhad rotateImage`)، `994` (`__dhad flipImage`)، `999` (`__dhad opacity`)، `1004` (`__dhad filter`)، `1009` (`__dhad overlay`)، `1014` (`__dhad background`)، `1019` (`__dhad pixel`)، `1034` (`__dhad rectangle`)، `1039` (`__dhad circle`)، `1044` (`__dhad line`)، `1054` (`__dhad clear`).
  الصحيح فقط: `1024` (`__dhad.draw`)، `1029` (`__dhad.fill`)، `1049` (`__dhad.text`).
  المقابل C++ صحيح دائمًا: `daad::image::load/draw/...` (`CodeGen.cpp:719-837` كلها `::`).
- **لماذا:** نسخ-لصق نسي النقطة. أي برنامج صور مولد من السحابة **يفشل بـ SyntaxError** (`__dhad loadImage` ليست JS صالحة).
- **الإصلاح (17 استبدالًا حرفيًا — الأهم في هذه الوثيقة):**
  في كل سطر أعلاه استبدل `'__dhad X'` بـ `'__dhad.X'`. مثال:
  ```js
  // قبل (مكسور):
  this.line(stmt.varName + ' = __dhad loadImage(' + ...);
  // بعد (صحيح):
  this.line(stmt.varName + ' = __dhad.loadImage(' + ...);
  ```
  وكرر للـ 17. ثم أضف اختبارًا: ولّد `حمّل_صورة("a.png")` وتحقق أن الناتج يحوي `__dhad.loadImage` وأن `node --check` ينجح.
- **الملفات المكررة:** نفس الخطأ في `desktop-app/frontend-web/js/dhad-codegen.js` + `Full/frontend/frontend-web/js/dhad-codegen.js` + `server/src/utils/dhad/codegen.js` (+ `.bak`) — أصلح الأصل `Web/js` ثم انسخ (أو أصلح الكل ببحث `__dhad ` بمسافة).

### GAP-C2 — `// TODO: type` يبتلع العقد المجهولة silently

- **المكان:** `dhad-codegen.js:427-428` (`default: this.line('// TODO: ' + stmt.type)`) + `genExpr:941-942` (`return '/* unknown: '+type+' */'`). C++ لا fallback — كل زائر موجود (exhaustive visitor).
- **لماذا خطر:** برنامج يستخدم عقدة غير مدعومة **ينجح ظاهريًا** لكنه يولد تعليقًا بدل كود — سلوك صامت مضلل.
- **الإصلاح:** غيّر `default` ليُسجل خطأً صريحًا: `throw new Error('عقدة غير مدعومة: ' + stmt.type)` أو أضفها لمصفوفة `errors` (مثل Parser) بدل `line(...)`. ونفس الشيء لـ `genExpr default`.

### GAP-C3 — `Struct` تُسقط الأعضاء في C++ (مفصّل A6) — إصلاح C++:

```cpp
// في AST.hpp: أضف حقل members (انسخ من ClassDecl):
struct StructDeclAST : StmtAST {
    std::string name;
    std::vector<std::pair<std::string,std::string>> members; // <-- إضافة (type,name)
};
// في Parser.cpp:parseClassDeclaration انسخ حلقة الأعضاء لـ parseStruct (حاليًا الاسم فقط)
// في CodeGen.cpp:visit(StructDeclAST&):
    header() << "struct " << name << " {\n";
    for (auto& [type,name] : node.members) header() << "  " << mapType(type) << " " << sanitizeIdent(name) << ";\n";
    header() << "};\n";
```

### GAP-C4 — `Switch` على النصوص: C++ يولد `if-else chain` وJS تولد `switch` خام

- **المكان:** C++ `CodeGen.cpp:381` (string cases → `if-else`)؛ JS `emitSwitch:562` (دائمًا `switch` + **بلا `break` تلقائي:570**).
- **الحكم:** سلوك مختلف لحالات النصوص ولسقوط `fallthrough`. الإصلاح: اختبار متقاطع (`اختر (نص) { حالة "أ": ... }`) ثم توحيد (انسخ نهج `if-else` للنصوص إلى JS).

### GAP-C5 — `Sizeof` تعني `sizeof` في C++ و`(x).length` في JS؛ `Typeof` تعني `decltype` مقابل `typeof`

- **المكان:** C++ `706/711`؛ JS `925/928`.
- **الحكم:** اختلاف دلالي مشروع (C++ compile-time مقابل JS runtime) — **وثّقه** في `LANGUAGE_SPEC.md` بدل إصلاحه.

### GAP-C6 — `^` تعني `std::pow` في C++ و`**` في JS (متكافئ) — لا إصلاح، وثّق فقط

### GAP-C7 — `Import/Export` no-op في الطرفين (مفصّل P4) + `Template` ممحوة في JS (مفصّل A7)

---

## 7. المكتبة القياسية — السجل الكامل والفروقات (الأضخم)

### 7.1 بنية الطرفين (FACT)

| الوجه | C++ | JS |
|---|---|---|
| الملفات | 5 hpp (`DaadStdlib.hpp 263` + `DaadStdlibExt.hpp 2141` + `DaadGraphics.hpp 398` + `Math.hpp 6` + `String.hpp 5` + `Runtime/DaadRuntime.hpp 53`) + 100 `.ض` (10 مجلدات × 10) + `stdlib_arabic/` (10 مجلدات عربية) | ملف واحد `dhad-libraries.js (2765)` + شيم runtime في `dhad-codegen.js:emitRuntime:86-223` (10 دوال فقط) |
| عدد الدوال الفريدة (بعد إزالة المولدة `الحد_*/الفارق_*`) | **~687** في `.ض` + ~40 حقيقية في hpp (الباقي `template return 0` stubs) | **~264** داخل `code:` blocks |
| التقاطع (نفس الاسم العربي) | ~30 دالة (القائمة في 7.2) | ~30 دالة |
| آلية الاستدعاء | `#include` + `using daad::stdlib::...` انتقائي (`CodeGen:110-159`) | حقن نص `code:` + `__dhad`/`__print` runtime |
| التوثيق | لا فهرس آلي (أداة `build_stdlib_index.py` تتوقع `stdlib/` الغائبة في DAAD — مكسورة) | تعليق رأسي كاذب (`10 categories, 90 files, 600+ funcs:4-6` — الفعلي 15 فئة و264 دالة) |

### 7.2 التقاطع (~30 دالة تعمل في الطرفين — حافظ عليها ولا تكسرها)

```
طول، استخرج، ابحث، استبدل، صغير، كبير، نظف، يبدأ_بـ، ينتهي_بـ، قسّم،
قوة، مطلق، أقصى، أدناه، عشوائي، مضروب، جذر، طول_مصفوفة، أضف، احذف_من،
عكس_مصفوفة، ابحث_في، افتح، سجّل، اقرأ_سطر، فصل، ليس_نهاية، أغلق، يحتوي، اقتطع،
جمع، طرح، ضرب، قسمة، باقي
```
**تحذير الانحراف:** نفس الاسم ≠ نفس التوقيع! مثال: C++ `طول(string|vector)` مُحمّلة (`DaadStdlib.hpp:40,144`) مقابل JS `طول(s)` نصية فقط + `codegen طول→.length:851`. اختبر كل واحدة قبل الادعاء بالتوافق.

### 7.3 دوال C++ فقط (اختر منها — القائمة الكاملة 687، عينة ممثلة بالفئة)

```
[مصفوفات متقدمة]: حجم_المصفوفة، أضف_لمصفوفة، احصل_من_مصفوفة، عّين_في_مصفوفة،
  احذف_من_مصفوفة، قلب_المصفوفة، فرّغ_المصفوفة، انسخ_مصفوفة، ادمج_مصفوفتين، أضف_وأعد
[ملفات حقيقية]: اكتب_stdout، اكتب_stderr، بحث_نمط، تحويل، فتح_ملف، سجل_سطر، افتح_لقراءة، افتح_للكتابة
[رياضيات]: جيب (Math.hpp)، جذر، قوة، مطلق ... + كل `math/*.ض` (250 دالة عدًّا)
[نصوص]: طول_نص، بحث_موضع، هل_يحتوي، استبدال_نمط ... (152 دالة)
[شبكات/قواعد/تزامن/رسوميات/وقت/ويب]: ~350 دالة (BFS/DFS/Threads/Mutex/SQL/HTTP/Canvas/ANSI...)
[رسوميات ANSI]: DaadGraphics (ألوان aswad/abyad... + Canvas + ansi move_to/clear/rectangle/circle/line/pixel)
```
**لماذا غائبة عن JS:** JS كُتبت للمنهج التفاعلي (متصفح بلا ملفات/شبكات حقيقية) فاختارت 264 دالة ويب-آمنة فقط.

### 7.4 دوال JS فقط (اختر منها — القائمة الكاملة 264، عينة ممثلة)

```
[إنجليزية مختلطة!]: sin، cos، tan، π، لog، GET، POST، PUT، DELETE، SQL Injection، XSS ...
[مصفوفات وظيفية]: فرز، تصفية، تحويل (map!)، مسطح، مجموع، يوجد ...
[مكدس/طابور]: إنشاء_مكدس، ادفع، أخرج، إنشاء_طابور ...
[شبكات متصفح]: GET/POST/PUT/DELETE، تحميل_JSON، تحميل_ملف ...
[تخزين متصفح]: إنشاء_تخزين، احفظ، اقرأ، احذف_تخزين ...
[وقت]: الوقت_الحالي، الطابع_الزمني، السنة/الشهر/اليوم ...
[ملفات (محاكاة)]: اقرأ_ملف، اكتب_لملف، نسخ_ملف، ضغط ...
[أحداث/واجهة]: استمع، زر، حقل_إدخال، صفحة، عنوان1 ...
[أمان تعليمي]: تشفير_base64/sha256/xor/قيصر، تحقق_بريد/رابط/رقم، كشف_XSS/SQLi ...
[اختبارات]: وصف، it، توقع، تقرير ...
[محرك رسومي]: محرك_جديد، مسح_الشاشة، ارسم_مستطيل/دائرة/خط/نص، مفتاح_مضغوط، ابدأ_التحريك ...
```
**لماذا غائبة عن C++:** تحتاج متصفحًا (DOM/fetch/Canvas) أو كُتبت بلغة هجينة (`s.length` و`s.split` داخل `code:` ليست C++ صالحًا — انظر `dhad-libraries.js:16-76`).

### 7.5 طريقة إضافة دالة جديدة (القالب المعتمد — انسخه لكل دالة)

**لإضافة `دالة X` إلى C++ (الأصل):**
1. اختر المجلد: `Compiler/stdlib/<الفئة>/X.ض` (أو أضف لملف قائم) — الفئات: `concurrency/database/data_structures/graphics/io/math/networking/text/time/web`.
2. اكتب الدالة بنحو `.ض` الصرف (مثال):
   ```arabic
   دالة مضروب(صحيح ن) -> صحيح {
       إذا (ن <= 1) { ارجع 1 ؛ }
       ارجع ن * مضروب(ن - 1) ؛
   }
   ```
3. إذا احتاجت C++ خامًا أضفها في `Compiler/stdlib/DaadStdlib.hpp` (للنصوص/مصفوفات) أو `DaadStdlibExt.hpp` (للملفات) أو `DaadGraphics.hpp` (للرسوم):
   ```cpp
   inline int مضروب(int n) { return n <= 1 ? 1 : n * مضروب(n - 1); }
   ```
4. إذا أردت `using` تلقائيًا أضف الاسم في `CodeGen.cpp:stdlibExportedNames:110-148`.
5. انسخها إلى `Compiler/stdlib_arabic/<الفئة_العربية>/` (المرآة العربية).
6. اختبر: `daad-compiler test.ض -o test.cpp && g++ -std=c++20 test.cpp -o test -I Compiler/include -I Compiler && ./test`.

**لإضافة `دالة X` إلى JS (السحابة):**
1. افتح `Studio/Web/js/dhad-libraries.js` واختر الفئة (أو أنشئ `'فئة_جديدة': { name, icon, desc, code: `...` }`).
2. أضف داخل `code:` بذات نحو `.ض` لكن بجسم JS-صالح (يُحقن نصًا!):
   ```
   دالة X(نص s) -> نص {
       ارجع s.toUpperCase()
   }
   ```
3. إذا احتاجت runtime حقيقيًا أضف شيمًا في `dhad-codegen.js:emitRuntime:86-127` (مثل `افتح/سجّل`) وفي `dhad-images.js` للصور.
4. انسخ الإضافة إلى النسخ الثلاث: `desktop-app/frontend-web/js/` + `Full/frontend/frontend-web/js/` + `server/src/utils/dhad/` (أو وحّدها بسكربت نسخ).
5. اختبر: `node -e "eval(fs.readFileSync('dhad-lexer.js')); ... full pipeline"` + `node --check`.

**قاعدة عدم الكسر:** أي دالة في التقاطع (§7.2) تُضاف/تُعدل في طرف **يجب** إضافتها/مطابقتها في الطرف الآخر بنفس الاسم والتوقيع، وإلا وُسمت `(C++-only)` أو `(JS-only)` في `stdlib_index.json`.

---

## 8. Sandbox الأمان — مجموعتان لا تتقاطعان (وثّق ولا توحد بسذاجة)

| | C++ (`SandboxValidator.cpp:13-78`) | JS (`dhad.js:46-92`) |
|---|---|---|
| يحمي مولد | C++ (`system/popen/exec/remove/socket/fork/exit/...` 18 regex + 8 dangerous headers + `func>20` + `new[]`) | JS (`process/require/eval/Function/fetch/XHR/WebSocket/globalThis/__proto__/prototype/constructor/document/localStorage/...` 26 regex + تطبيع Unicode) |
| يتجاهل تمامًا | كل هجمات JS (مثل `fetch(` تمر من C++ بسلام) | كل هجمات C++ (مثل `system(` تمر من JS بسلام) |
| عيب مثبت | `<cstdlib>` مسموح + خطير معًا (`:13` و`:68`) — ازدواج | `collapsed` بلا `\n` قد يُخفي `eval\n(`؟ — يحتاج تدقيقًا (UNKNOWN) |

**الإصلاح:** لا توحيد — أبقِ الاثنين (كل مولد بلغته). أضف فقط: (1) أزل ازدواج `<cstdlib>` (اختر قائمة واحدة)؛ (2) وثّق أن **أمان C++ لا يحمي السحابة والعكس**.

---

## 9. نسخة السيرفر المكررة — انحراف صامت محتمل

- **المكان:** `Studio/desktop-app/server/src/utils/dhad/{lexer,parser,ast,codegen}.js` + `Full/server/...` — نسخ من `Web/js` (يثبتها `diff -q` — أسماء `ast.js/codegen.js/lexer.js/parser.js` بدون بادئة `dhad-`).
- **المشكلة:** أي إصلاح في `Web/js` (مثل GAP-C1 النقاط الـ17) **لا يصل للسيرفر** تلقائيًا — والسيرفر هو الحَكَم (`serverEvaluator`).
- **الإصلاح:** سكربت نسخ واحد (`cp Web/js/dhad-*.js server/src/utils/dhad/`) يُشغَّل بعد كل إصلاح، أو استبدال النسخ بروابط رمزية، أو فحص CI يقارن التطابق (`diff -q` يفشل البناء عند الانحراف).

---

## 10. خطة الإصلاح مرتبة بالأولوية (نفّذ بهذا الترتيب، ملف واحد في كل خطوة)

| # | الإصلاح | الملفات | الأثر | الحجم |
|---|---|---|---|---|
| 1 | **النقاط الـ17** (`__dhad ` → `__dhad.`) في 4 نسخ | `Web/js/dhad-codegen.js:959-1054` + مراياها | يصلح كل برامج الصور المكسورة | 17 سطرًا |
| 2 | **`ليس`** في C++ (نوع + تسجيل + مكافئ + parser) | `Keywords.hpp:45` + `Keywords.cpp:103/256` + `Parser.cpp` | يوحد المنطق العربي | 5 أسطر |
| 3 | **`TODO/default` → خطأ صريح** في JS | `dhad-codegen.js:427/941` | يمنع الصمت المضلل | سطران |
| 4 | **`ImageSize/Pixel` في `genExpr`** | `dhad-codegen.js:786` (+ فرعان) | يصلح `س = حجم_صورة(p)` | 4 أسطر |
| 5 | **`Struct members` + `Array base` + `Input multi`** في C++/JS | `AST.hpp:385` + `CodeGen.cpp:513` + `dhad-ast.js:92/341` + `Parser.cpp:612` | يصلح 3 هياكل | ~20 سطرًا |
| 6 | **`%=` في JS + `& -> :: #` في C++** | `dhad-lexer.js` + `Lexer.cpp` + `Token.hpp` | يوحد المعاملات | ~15 سطرًا |
| 7 | **الأرقام المشرقية** في الطرفين | `Lexer.cpp:120` + `dhad-lexer.js:399` | يفتح اللغة للجميع | ~10 أسطر |
| 8 | **التلوين الـ9** | `dhad-highlight.js:10-42` | يصلح تجربة المتعلم | 9 أسطر |
| 9 | **توثيق القيود** (`GUI Qt-only` + `Template` + `Import no-op` + `Sizeof` + `0x`) | `docs/LANGUAGE_SPEC.md` + `KEYWORDS_REGISTRY.json` | يمنع التوقعات الكاذبة | صفحتان |
| 10 | **فهرس المكتبات** (`stdlib_index.json` حقيقي + وسم only) | سكربت توليد جديد | يحسم §7 نهائيًا | سكربت واحد |
| 11 | **سكربت مزامنة النسخ الأربع** | `Scripts/sync_dhad_js.sh` جديد | يمنع الانحراف §9 | 10 أسطر |
| 12 | **اختبارات متقاطعة** (نفس `.ض` في الطرفين) | `Tests/highlevel_parity.sh` جديد | يثبت التكافؤ سلوكيًا | ~30 حالة |

---

## 11. طريقة إضافة أي شيء جديد — القوالب الجاهزة (انسخ والصق)

### 11.1 إضافة كلمة مفتاحية جديدة (مثال: `بينما_جديدة`)

**C++ (4 مواقع إجبارية):**
```cpp
// 1. include/Daad/Keywords.hpp — أضف النوع:
KwWhileNew,
// 2. src/Keywords.cpp:initializeStandardKeywords — سجّل النص:
m_keywordToType["بينما_جديدة"] = KeywordType::KwWhileNew;
// 3. src/Keywords.cpp:switch — أضف المكافئ:
case KeywordType::KwWhileNew: info.cppEquivalent = "while"; break;
// 4. src/Parser.cpp:parseStatement — أضف الفرع:
else if (text == "بينما_جديدة") return parseWhileStatement();
// 5. (اختياري) src/CodeGen.cpp — فقط إذا احتاجت إصدارًا خاصًا
// 6. docs/KEYWORDS_REGISTRY.json — أضفها للفئة وارفع total_keywords
```

**JS (3 مواقع إجبارية):**
```js
// 1. dhad-lexer.js:TT — أضف النوع:
KW_WHILE_NEW: 'KW_WHILE_NEW',
// 2. dhad-lexer.js:KEYWORDS — سجّل النص:
'بينما_جديدة': TT.KW_WHILE_NEW,
// 3. dhad-parser.js:parseStatement — أضف الفرع:
case TT.KW_WHILE_NEW: return this.parseWhile();
// 4. dhad-highlight.js:CONTROL_KEYWORDS — أضف النص للتلوين
```

### 11.2 إضافة دالة مكتبة جديدة (مثال: `دالة عكس_النص`)

انظر §7.5 (القالب الكامل للطرفين + النسخ + الاختبار).

### 11.3 إضافة مكتبة (فئة) جديدة (مثال: `الذكاء_الاصطناعي`)

**C++:** `mkdir Compiler/stdlib/ai Compiler/stdlib_arabic/الذكاء_الاصطناعي` + ملفات `.ض` + (اختياري) `Ai.hpp` + سجّل في `stdlib_index.json`.
**JS:** أضف في `dhad-libraries.js`:
```js
'الذكاء_الاصطناعي': {
  name: 'الذكاء_الاصطناعي',
  icon: '🤖',
  desc: 'دوال الذكاء الاصطناعي',
  code: `
دالة اسأل(نص سؤال) -> نص {
    ارجع __ai.ask(سؤال)
}
  `
},
```
ثم انسخ للنسخ الثلاث (§9) وحدّث `slides_data.json` إن لزم.

---

## 12. الشعار الرسمي — الاعتماد والاستخدام

- **الملف:** `docs/logo-ض.jpeg` (72K، حرف ض أخضر داكن `#0B5C2E` تقريبًا على أبيض نقي) — منسوخ من `~/Downloads/Gemini_Generated_Image_a3k73oa3k73oa3k7.jpeg`.
- **الحالة:** **معتمد كشعار اللغة الوحيد** (لا تستخدم `icons.svg` القديمة بديلًا).
- **أين يُوضع لاحقًا (تسجيل فقط — لا تنفيذ الآن):**
  1. أعلى `docs/LANGUAGE_SPEC.md` و`README` (ماركداون: `![شعار لغة ض](logo-ض.jpeg)`)
  2. `Studio/Web/icons.svg` → استبدال/دمج (أيقونة المتصفح)
  3. `Studio/desktop-app/splash.html` + `Studio/Electron/splash.html` (شاشة البداية)
  4. `Studio/VSCode-Extension` (أيقونة الإضافة `package.json:icon`)
  5. `release/` أغلفة MSI (بانر `DhadStudio.wxs`)
- **قواعد الاستخدام:** خلفية بيضاء دائمًا؛ لا تمديد/تشويه؛ الحد الأدنى 32px؛ الأخضر للمناسبات الرسمية والأبيض-على-أخضر للأيقونات الصغيرة (تُشتق لاحقًا).

---

## FINAL VERDICT — حكم النواقص

- **يعمل في الطرفين (~30 دالة + ~95 كلمة + كل التحكم الأساسي):** المتغيرات/الشروط/الحلقات/الدوال/OOP البسيط/الطباعة/الصور الـ20 تسجيلًا — **يثبتها التقاطع §7.2 والقاموس §1.**
- **مكسور في JS (يُصلح أولًا):** النقاط الـ17 (§6-C1) + `TODO` الصامت (C2) + `ImageSize/Pixel` كتعبير (A10) — **3 إصلاحات تُعيد كل برامج الصور للحياة.**
- **مكسور/ناقص في C++ (يُصلح ثانيًا):** `ليس` (K1) + `Struct` تُسقط الأعضاء (A6) + GUI مسجلة بلا parser (P1) + `& -> :: #` (O2-O5) — **4 إصلاحات توحد اللغة.**
- **ناقص في الطرفين (يُصلح ثالثًا):** المشرقية `١٢٣` (N1) + ~650 دالة غير متقاطعة (§7) — **يحتاج قرار منتج (أي الدوال تُعمم؟).**
- **مقصود ومشروع (يوثق فقط):** `Template` ممحوة في JS + `Sizeof/Typeof` الدلالي + `^` + `Program` الداخلية + Sandbox المنفصل — **5 توثيقات لا إصلاحات.**
- **التالي:** نفّذ جدول §10 بالترتيب (1→12)، ملف واحد في كل خطوة، مع اختبار متقاطع (`نفس .ض → قارن C++ output مقابل JS output`) قبل إغلاق أي GAP.
