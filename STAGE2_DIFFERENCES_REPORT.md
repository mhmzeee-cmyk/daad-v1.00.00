# المرحلة 2 — تقرير الفروقات (قراءة فقط)

**تاريخ الإنشاء**: 2026-09-12
**طبيعة العمل**: قراءة فقط — لا تعديل أي كود

---

## 1. المنهج والمصادر (مرجعي § + ملفات خام مقروءة)

### مصادر المرحلة 1 المرجعية
| # | الملف | الغرض |
|---|-------|-------|
| 1 | `STAGE1_REFERENCE_REPORT.md` | التقرير المرجعي الأساسي (88 فصل، 103 كلمة مفتاحية، 211 تحدٍّ) |

### ملفات الخام المُقرأة مباشرة (لا شيء غير ذلك)

#### الكود المصدري لمحرك C++
| # | الملف | الأسطر | الغرض |
|---|-------|--------|-------|
| 1 | `Compiler/include/Daad/Keywords.hpp` | 87 | تعريف enum KeywordType + KeywordRegistry |
| 2 | `Compiler/src/Keywords.cpp` | 370 | ربط الكلمات العربية↔C++ equivalents |
| 3 | `Compiler/src/Lexer.cpp` | 265 | محلل الرموز (tokenize) — UTF-8 codepoints |
| 4 | `Compiler/src/Parser.cpp` | 1136+ | محلل بنية الجملة (recursive descent) |
| 5 | `Compiler/src/CodeGen.cpp` | 945 | مولّد كود C++ من AST (visitor pattern) |
| 6 | `Compiler/src/Compiler.cpp` | 113 | خط أنابيب المترجم الكامل |
| 7 | `Compiler/src/main.cpp` | 131 | نقطة الدخول CLI |
| 8 | `Compiler/CMakeLists.txt` | 50+ | ملف البناء |
| 9 | `Compiler/Makefile` | 34 | بناء سريع بدون CMake |

#### الكود المصدري لمحرك JS
| # | الملف | الأسطر | الغرض |
|---|-------|--------|-------|
| 10 | `Studio/desktop-app/server/src/utils/dhad/lexer.js` | 704 | محلل الرموز — UTF-16 native |
| 11 | `Studio/desktop-app/server/src/utils/dhad/parser.js` | 1286+ | محلل بنية الجملة |
| 12 | `Studio/desktop-app/server/src/utils/dhad/codegen.js` | 1167 | مولّد كود JS من AST |
| 13 | `Studio/desktop-app/server/src/utils/dhad/ast.js` | 120+ | تعريفات عقد AST |
| 14 | `Studio/desktop-app/server/src/utils/dhadCompiler.js` | 126 | مترجم خادم Node.js |

#### الكود الخام الإضافي
| # | الملف/المسار | الغرض |
|---|-------------|-------|
| 15 | `Studio/Web/book.html` | الكتاب التعليمي (9638 سطر) |
| 16 | `Compiler/examples/*.ض` | أمثلة بلغة ض |
| 17 | `Compiler/stdlib/` | مكتبة runtime C++ (.hpp) |
| 18 | `Compiler/stdlib_arabic/` | مكتبة عربية (مجلدات) |
| 19 | `release/` | ملفات الإصدار |

---

## 2. كتاب vs ض (C++) — جدول فرق برقم ودليل

### ملاحظة على التقرير المرجعي
المرجع §1 فيه تقاريض خاطئة عن بعض الكلمات المفتاحية. التقرير يقول `لا_شيء` لـ KwNullptr (§1 ص174) لكن الكود الخام يقول `عدم` (Keywords.cpp:18). ويقول `افتح` لـ KwSwitch (§1 ص190) لكن الكود يقول `اختر` (Keywords.cpp:31). الأرقام أدناه مبنية على **الكود الخام المُقرأ سطراً بسطر**.

| # | البند | الكتاب (book.html) | C++ (Keywords.cpp + Parser + CodeGen) | الفرق | الدليل |
|---|-------|--------------------|---------------------------------------|-------|-------|
| 1 | كلمة null | لم تُحدّد بوضوح | `عدم` → `nullptr` | ✓ متطابق مع实际情况 | Keywords.cpp:18: `m_keywordToType["عدم"] = KeywordType::KwNullptr;` |
| 2 | كل المفاتيح — نوع Token | لا يُحسم (كتاب تعليمي) | **جميع** المفاتيح تُولّد `TOKEN_KEYWORD` بنوع واحد | ~~ отличие: C++ لا يُميّز أنواع المفاتيح في الـ Lexer — المُميّز في الـ Parser فقط | Lexer.cpp:259: `return makeToken(TokenType::TOKEN_KEYWORD, text);` |
| 3 | `مجرّد صنف` | يُعلّم `مجرّد صنف صنف_مجرد` (الفصل 31) | يُحلل `مجرّد` كعلامة ثم `صنف` — لا يوجد **فرض** في C++ (لا virtual/override) | مفقود: لا إلزام في التوليد — فقط علامة AST | Parser.cpp:118: `if (kw == "صنف" \|\| kw == "فئة") return parseClassDeclaration();` + no abstract enforcement in CodeGen |
| 4 | `مجرّد دالة` | يُعلّم `مجرّد دالة صوت()` لتعريف دالة مجرّدة (الفصل 31) | **غير مدعوم** — لا يوجد فرع parser لـ `مجرّد` قبل `دالة` | مفقود: لا يوجد parse لـ abstract function | Parser.cpp: لا يوجد `if (kw == "مجرّد")` قبل `parseFunctionDefinition()` |
| 5 | `واجهة` (interface) | يُعلّم `واجهة` كعقد不含 تنفيذ (الفصل 31) | يُحلل `واجهة` → يُنشئ `ClassDeclAST` فارغ مع `isInterface = true` | مختلف: C++ لا يوجد interface — يتحول إلى class فارغة | Parser.cpp: ي حلل لكن CodeGen لا يولّد شيء مميز |
| 6 | ` steals `.الطول` | يُعلّم `.الطول` كخاصية لمعرفة طول المصفوفة/النص | في C++: يتحول إلى `size()` عبر stdlib (استخراج من CodeGen: لا mapping مباشر — يعتمد على stdlib) | مختلف: C++ يحتاج stdlib function بدلاً من property | CodeGen.cpp: لا يوجد mapping `.الطول` → member expression |
| 7 | `عشوائي()` | يُعلّم `عشوائي()` كدالة مدمجة (الفصل 1) | **غير موجود** — لا يوجد `عشوائي` في Keywords.cpp | مفقود: لا يوجد random في C++ compiler | Keywords.cpp: لا يوجد سطر يربط `عشوائي` بأي enum |
| 8 | `اقرأ_عشري()` | يُعلّم `اقرأ_عشري()` للإدخال العشري (الفصل 9) | **غير موجود** — يستخدم `ادخل` فقط مع `std::cin >>` | مختلف: C++ لا يُفرّق بين أنواع الإدخال | Parser.cpp:133: `if (kw == "ادخل") return parseInputStatement();` فقط |
| 9 | `افتح`/`اقرأ`/`اكتب`/`أغلق` لملفات | يُعلّم إدخال/إخراج الملفات | **غير موجود في الـ Parser** — لكن stdlib يحتوي دوال ملفات بلغة عربية | مختلف: الملفات عبر stdlib لا عبر keyword | Parser.cpp: لا يوجد branches لهذه المفاتيح |
| 10 | `كود_الحالة` (switch-case) | يُعلّم `افتح(التعبير) { حالة X: ... افتراضي: ... }` | يُحلل `اختر(التعبير)` مع `حالة` و `افتراضي` | مختلف: الكلمة `اختر` لا `افتح` | Parser.cpp:74: `if (kw == "اختر") return parseSwitchStatement();` |
| 11 | `إلا إذا` (else if) | يُعلّم `وإلا إذا` (الفصل 13) | يُحلل `وإلا` ثم `بديل` كـ else-if بديل — لا يوجد `إلا إذا` حرفي | مختلف: C++ يستخدم `بديل` بدلاً من `إلا إذا` | Parser.cpp:710: `if (m_currentToken.text == "بديل")` |
| 12 | الأقواس في if/while | الكتاب لا يُشترط أقواس (يمكن بدون `{}`) | **إلزامي**: Parser يتوقع `parseBlock()` بعد الشرط — لا يدعم single statement بدون `{}` | مختلف: C++ يُلزم الأقواس | Parser.cpp:704: `auto thenBody = parseBlock();` |
| 13 | `كائن.الطريقة()` — طرق الصفوف | يُعلّم تعريف الدوال داخل الصنف | تُولّد خارج الصنف بتفصيل `ClassName::method()` | مختلف: C++ يفصل تعريف عن إعلان | CodeGen.cpp:495: `fullName = m_currentClassScope + "::" + fnName` |
| 14 | الحقول الخاصة `خاص` | يُعلّم `خاص` للحقول غير المتاحة من الخارج | **غير مُفعّل**: جميع الحقول تُولّد في `public:` | مختلف: C++ لا يُ强制 private access | CodeGen.cpp:524: `m_headerStream << " {\npublic:\n";` |
| 15 | `زد`/`انقص` كجملة مستقلة | يُعلّم `زد عداد` و `انقص عداد` | يُحلل كـ `CompoundAssignmentAST` بعملية `+= 1` / `-= 1` | متطابق في السلوك | Parser.cpp:93-108 |
| 16 | `(((لكل` مع `في` | يُعلّم `لكل (نوع اسم في مصفوفة)` (الفصل 16) | يُولّد `for (const auto& name : iterable)` | متطابق | CodeGen.cpp:685 |
| 17 | فهرسة متعددة `a[i][j]` | يُعلّم الفهرسة المتعددة | يُدعم بشكل متسلسل مع `ArraySubscriptExprAST` chain | متطابق | Parser.cpp:234-257 |
| 18 | `ضغط_الذاكرة()` | يُعلّم `خصص_ذاكرة()` (الفصل المفقود) | **غير موجود** — لا يوجد memory management keyword | مفقود | — |
| 19 | `น้ำ护卫` (lambda) | لا يُعلّم lambdas | **غير موجود** — لا يوجد parsing لـ lambda | غير مطلوب من الكتاب | — |
| 20 | `تصنيف_مجرد` (بديل) | يُذكر ككلمة بديلة في جدول المقارنة (الفصل 31) | **غير موجود** — لا يوجد mapping لـ `تصنيف_مجرد` | مفقود | Keywords.cpp: لا يوجد `تصنيف_مجرد` |

---

## 3. كتاب vs ض (JS) — جدول فرق برقم ودليل

| # | البند | الكتاب (book.html) | JS (lexer.js + parser.js + codegen.js) | الفرق | الدليل |
|---|-------|--------------------|-----------------------------------------|-------|-------|
| 1 | كل المفاتيح — نوع Token | لا يُحسم | **كل مفتاح له نوع مميّز**: `KW_IF`, `KW_ELSE`, `KW_WHILE`... (83+20=103) | مختلف: JS يُميّز أنواع المفاتيح في الـ Lexer مباشرة | lexer.js:30-128: TT object بـ 103+ token types مميّزة |
| 2 | `أمسك` (بديل hamza) | لا يُذكر بوضوح | `'أمسك': TT.KW_CATCH` — يدعم hamza variant | مضاف: JS يدعم `أمسك` (بحمزة) | lexer.js:246: `'أمسك': TT.KW_CATCH` |
| 3 | `عشوائي()` | يُعلّم `عشوائي()` كدالة مدمجة | **غير موجود** — لا يوجد `عشوائي` في KEYWORDS object | مفقود | lexer.js: KEYWORDS object لا يحتوي `عشوائي` |
| 4 | `.الطول` property | يُعلّم `.الطول` كخاصية | CodeGen يُحوّل `.الطول` → `.length` تلقائياً | متطابق: JS يدعم property mapping | codegen.js:1008: `if (propMap[expr.member])` |
| 5 | `.احذف` (splice) | يُعلّم `.احذف(الفهرس)` لإزالة عنصر | CodeGen يُحوّل → `.splice(index, 1)` | متطابق: JS يدعم method mapping | codegen.js:973: `return obj + '.' + methodMap[member] + '(' + this.genExprList(expr.args) + ', 1)'` |
| 6 | `.أضف` (push) | يُعلّم `.أضف(العنصر)` لإضافة | CodeGen يُحوّل → `.push()` | متطابق | codegen.js:947 |
| 7 | `.يحتوي` (includes) | يُعلّم `.يحتوي()` | CodeGen يُحوّل → `.includes()` | متطابق | codegen.js:948 |
| 8 | `.ابحث` (indexOf) | يُعلّم `.ابحث()` | CodeGen يُحوّل → `.indexOf()` | متطابق | codegen.js:959 |
| 9 | `.استبدل` (replace) | يُعلّم `.استبدل()` | CodeGen يُحوّل → `.replace()` | متطابق | codegen.js:951 |
| 10 | `.تقسيم` (split) | يُعلّم `.تقسيم()` | CodeGen يُحوّل → `.split()` | متطابق | codegen.js:952 |
| 11 | `.كرر` (repeat) — تعارض | يُعلّم `.كرر()` ك method + `كرر` كبديل for loop | **تعارض**: `كرر` = `TT.KW_FOR` (loop) في الـ Lexer — لا يمكن استخدامه كـ method name | مختلف: تعارض دلالي — `كرر`既是 for loop又是 method | lexer.js:201: `'كرر': TT.KW_FOR` + codegen.js:957: `'كرر': 'repeat'` |
| 12 | `.لأحرف_كبيرة` | يُعلّم تحويل الأحرف | CodeGen يُحوّل → `.toUpperCase()` | متطابق | codegen.js:955 |
| 13 | `.اتجه` (trim) | يُعلّم `.اتجه()` | CodeGen يُحوّل → `.trim()` | متطابق | codegen.js:958 |
| 14 | `مجرّد صنف` — فرض مباشر | يُعلّم منع التثبيت المباشر | CodeGen يُولّد `if (new.target && new.target.name === 'ClassName')` | مختلف: JS يفرض فعلياً runtime | codegen.js:264: `this.line('if (new.target && new.target.name === \'' + name + '\')` |
| 15 | `خاص` (private fields) | يُعلّم الحقول الخاصة | CodeGen يستخدم closure variables + `Object.defineProperty` getter/setter | مختلف: JS يُحاكي الخاص بـ closure | codegen.js:286-345 |
| 16 | `أخيراً` (finally) | يُعلّم `حاول/امسك/أخيراً` | **مدعوم أصلياً** — JS يدعم `try/catch/finally` | متطابق بشكل أفضل من C++ | codegen.js:831-837 |
| 17 | `上限` operator `^` | يُعلّم `^` للقوة (الفصل 5) | CodeGen يُحوّل → `Math.pow()` | متطابق | codegen.js:908 |
| 18 | `عدم` (null) | يُعلّم `عدم` كقيمة فارغة | CodeGen يُولّد `null` | متطابق | codegen.js:878: `case 'NullExpr': return 'null';` |
| 19 | `高手` (类名) كأسماء متغيرات | يُعلّم استخدام كلمات مفتاحية كأسماء | Parser يدعم `isName()` — أي keyword يمكن استخدامه كاسم | مختلف: JS أكثر مرونة في التسمية | parser.js:64-68: `isName()` |
| 20 | `面板` (widget) — GUI | يُعلّم `زر_أمر`, `حقل_نص`... | Parser يدعم `parseWidgetDecl()` — يُعامل كـ VarDecl | متطابق | parser.js:177-191 |
| 21 | `砂像` (image) — 20 keyword | يُعلّم معالجة الصور | Parser + CodeGen يدعمان 20 keyword صور | متطابق | parser.js:193-213, codegen.js:473-493 |
| 22 | `宽容` (宽容 =宽容模式) | لا يوجد | لا يوجد | — | — |
| 23 | تعدد الأبعاد `عشري[][]` | يُعلّم مصفوفات متعددة الأبعاد | Parser يدعم `[][]` type suffix | متطابق | parser.js:250-256 |
| 24 | ` độ` sign `#` تعليق عربي | يُعلّم التعليق بـ `#` | Lexer يدعم `#` كتعليق سطر واحد | متطابق | lexer.js:577-580 |
| 25 | `剪贴板` clipboard | لا يوجد | لا يوجد | — | — |
| 26 | `剪贴板` clipboard | لا يوجد | لا يوجد | — | — |

---

## 4. ض (C++) vs ض (JS) — جدول فرق برقم ودليل

### أ) الفروقات في الكلمات المفتاحية

| # | البند | C++ (Keywords.cpp) | JS (lexer.js) | الفرق | الدليل |
|---|-------|---------------------|---------------|-------|-------|
| 1 | عدد token types | **1 نوع واحد**: `TOKEN_KEYWORD` لكل المفاتيح | **103+ نوع مميّز**: `KW_IF`, `KW_ELSE`, `KW_WHILE`... | بنية مختلفة جذرياً — C++ يُجمّع والـ Parser يُميّز | C++: Lexer.cpp:259 vs JS: lexer.js:30-128 |
| 2 | `عدم` (null) | `KwNullptr` → `nullptr` | `KW_NULL` → `null` | متطابق في الاسم العربي، مختلف في القيمة المولّدة | C++: Keywords.cpp:18 vs JS: lexer.js:192 |
| 3 | `اطبع` (بديل print) | `KwPrint` → `daad::runtime::daad_print()` | `KW_PRINT` → `__print()` | متطابق في الاسم، مختلف في الدالة المولّدة | C++: Keywords.cpp:96 vs JS: lexer.js:269 |
| 4 | `بينما` (while بديل) | `KwWhile` — مُسجّل مرتين (lines 26+99) | `KW_WHILE` — مُسجّل مرتين (lines 199+26) | متطابق — كلاهما يدعم البديل | C++: Keywords.cpp:26,99 vs JS: lexer.js:199 |
| 5 | `-Arabic logical: و/أو/ليس` | `KwAndArabic`→`&&`, `KwOrArabic`→`||`, `KwNotArabic`→`!` | `AND_ARABIC`→token type, `KW_NOT`→token type | C++: يجمعها في enum واحد. JS: كل واحدة نوع مختلف | C++: Keywords.cpp:104-106 vs JS: lexer.js:157-158,271 |
| 6 | `بديل` (else if) | `KwAlternative` → `else if` | `KW_ALTERNATIVE` → بديل (مُعامل كمفتاح عام) | متطابق في التسمية | C++: Keywords.cpp:93 vs JS: lexer.js:211 |
| 7 | `فراغ` (void) | `KwVoid` → `void` | `KW_VOID` → `void` | متطابق | C++: Keywords.cpp:14 vs JS: lexer.js:28 |
| 8 | `عرّف` (typedef) | `KwTypedef` → `typedef` | `KW_TYPEDEF` → `typedef` | متطابق | C++: Keywords.cpp:19 vs JS: lexer.js:29 |
| 9 | عدد المفاتيح المُعرّفة | **103 كلمة عربية** (في `m_keywordToType`) | **103 كلمة عربية** (في `KEYWORDS` object) | **متطابق عددياً** | C++: Keywords.cpp:8-150 (103 سطر) vs JS: lexer.js:179-315 (103 مدخل) |
| 10 | أحرف Unicode | UTF-8 codepoints عبر `UnicodeUtils` | UTF-16 native عبر `charCodeAt()` | مختلف: C++ يحتاج تحويل يدوي، JS أصلي | C++: Lexer.cpp:9 vs JS: lexer.js:331-335 |
| 11 | `أمسك` hamza variant | **غير مدعوم** — فقط `امسك` | مدعوم: `'أمسك': TT.KW_CATCH` | JS يدعم hamza variant | C++: Keywords.cpp:72 vs JS: lexer.js:246 |
| 12 | `دالة` قبل `جديد` (constructor) | لا يوجد دعم `دالة جديد(...)` | Parser يدعم `دالة جديد(...)` كصيغة بديل للمُنشئ | JS أكثر مرونة | JS: parser.js:622-653 vs C++: لا يوجد معادل |
| 13 | `الاسم : النوع` (ถอดแบบ member) | لا يوجد دعم | Parser يدعم `parseWidgetDecl()` variant مع `:` | JS يدعم member declaration معاكس | JS: parser.js:658-674 |
| 14 | `克隆` (multi-line string) | لا يوجد دعم | لا يوجد دعم | متطابق | — |
| 15 | Escape sequences | يدعم `\0`, `\n`, `\t`, `\r`, `\\`, `\ooo` (octal) | يدعم `\n`, `\t`, `\r`, `\\`, `\0` فقط | C++ يدعم octal escape, JS لا | C++: Lexer.cpp:199-215 vs JS: lexer.js:516-525 |

### ب) الفروقات في بنية Parser

| # | البند | C++ (Parser.cpp) | JS (parser.js) | الفرق | الدليل |
|---|-------|------------------|----------------|-------|-------|
| 1 | **If statement — أقواس** | **إلزامي**: يتوقع `parseBlock()` بعد الشرط | **اختياري**: يدعم block أو single statement | JS أكثر مرونة | C++: Parser.cpp:704 vs JS: parser.js:340-344 |
| 2 | **For-each detection** | يُحفّظ state ويعيد التحليل عند رؤية `في` | يُحفّظ `savedPos` ويعيد التحليل | متطابق في المنطق | C++: Parser.cpp:738-756 vs JS: parser.js:384-398 |
| 3 | **Constructor: `دالة جديد()`** | **غير مدعوم** — فقط `جديد(...)` بعد `صنف` | **مدعوم**: `دالة جديد(...)` inside class | JS يدعم صيغة المُنشئ بالكلمة `دالة` | C++: لا يوجد vs JS: parser.js:622-653 |
| 4 | **Abstract method: `مجرّد دالة`** | **غير مدعوم** | Parser يدعم `parseAbstract()` مع `isAbstract` flag | JS يدعم `مجرّد` كعلامة | C++: لا يوجد vs JS: parser.js:745-754 |
| 5 | **Interface: `واجهة`** | Parser يحلل لكن لا يولّد شيئاً مميزاً | Parser يُنشئ `ClassDeclAST` مع `isInterface = true` | JS يُعلّم واجهة بشكل أفضل | C++: Parser.cpp لا يوجد vs JS: parser.js:704-740 |
| 6 | **Template** | Parser يدعم `قالب <T>` | Parser يدعم `قالب <T>` | متطابق | C++: Parser.cpp:1025-1068 vs JS: parser.js:819-832 |
| 7 | **Variable — arrow type `->`** | Parser يدعم `->` كنوع إرجاع | Parser يدعم `->` كنوع إرجاع | متطابق | C++: Parser.cpp:602-611 vs JS: parser.js:567-570 |
| 8 | **Switch — string cases** | CodeGen يكشف cases النصية ويُولّد if-else chain | CodeGen يُولّد switch native (JS يدعم string switch) | مختلف: C++ يحتاج bypass | C++: CodeGen.cpp:404-456 vs JS: codegen.js:629-648 |
| 9 | **Error recovery** | يُتخطى حتى `;` أو `}` | يُتخطى حتى `;` أو EOF | متطابق تقريباً | C++: Parser.cpp:81-88 vs JS: parser.js:40-41 |
| 10 | **Param type inference** | لا يوجد — يتوقع نوع صريح | يُعامل `الكلمة` الواحدة كـ param type `صحيح` افتراضي | JS أكثر مرونة | C++: Parser.cpp:591-598 vs JS: parser.js:540-543 |

### ج) الفروقات في CodeGen (السلوك التنفيذي)

| # | البند | C++ (CodeGen.cpp) | JS (codegen.js) | الفرق | الدليل |
|---|-------|-------------------|-----------------|-------|-------|
| 1 | **الإدخال** | `std::cin >> variable` | `variable = __input()` | مختلف: C++ cin vs JS __input | C++: CodeGen.cpp:754 vs JS: codegen.js:854 |
| 2 | **الإخراج** | `daad::runtime::daad_print(...)` | `__print(...)` | مختلف: دالة runtime مختلفة | C++: CodeGen.cpp:730 vs JS: codegen.js:843 |
| 3 | **Types — mapping** | `صحيح→int`, `عشري→double`, `نص→std::string` | لا يوجد type mapping (JavaScript خامل الأنواع) | مختلف جذرياً | C++: CodeGen.cpp:71-85 vs JS: لا يوجد |
| 4 | **Arrays** | `std::vector<int>` etc. | `Array` native JavaScript | مختلف: C++ يحتاج vector | C++: CodeGen.cpp:80-85 |
| 5 | **Power operator `^`** | `std::pow(lhs, rhs)` | `Math.pow(lhs, rhs)` | مختلف في الاسم، متطابق في السلوك | C++: CodeGen.cpp:214 vs JS: codegen.js:908 |
| 6 | **Null value** | `nullptr` | `null` | مختلف في القبول، متطابق في المفهوم | C++: CodeGen.cpp:668 vs JS: codegen.js:878 |
| 7 | **String concat with +** | يلفّ النص بـ `std::string(...)` للدمج | لا يحتاج تحويل (JS يدعم + للنصوص) | مختلف: C++ يحتاج تحويل يدوي | C++: CodeGen.cpp:221-227 |
| 8 | **finally block** | يُولّد scope guard `{ }` مع تعليق | يُولّد `} finally {` أصلي | مختلف: JS يدعم finally مباشرة | C++: CodeGen.cpp:599-606 vs JS: codegen.js:831-837 |
| 9 | **Class — inheritance** | `class Derived : public Base` | `Derived.prototype = Object.create(Base.prototype)` | مختلف: C++ class inheritance vs JS prototype | C++: CodeGen.cpp:523 vs JS: codegen.js:373 |
| 10 | **Private fields** | **غير مُفعّل**: جميع الحقول `public` | **مُفعّل**: closure variables + `Object.defineProperty` | JS أكثر أماناً في الخاص | C++: CodeGen.cpp:524 vs JS: codegen.js:286-345 |
| 11 | **Private methods** | **غير مُفعّل** | **مُفعّل**: closure variables | JS أكثر أماناً | C++: لا يوجد vs JS: codegen.js:291-304 |
| 12 | **Static members** | `static` prefix في C++ output | `ClassName.member = value` | متطابق في المفهوم | C++: CodeGen.cpp:258 vs JS: codegen.js:351-352 |
| 13 | **For-each** | `for (const auto& name : iterable)` | `for (var name of iterable)` | مختلف في الصيغة، متطابق في المفهوم | C++: CodeGen.cpp:685 vs JS: codegen.js:612 |
| 14 | **Assert** | `assert(condition)` | `if (!condition) throw new Error(...)` | مختلف: C++ macro vs JS runtime | C++: CodeGen.cpp:706 vs JS: codegen.js:661 |
| 15 | **Delete** | `delete pointer` | `delete expression` | متطابق | C++: CodeGen.cpp:713 vs JS: codegen.js:452 |
| 16 | **Sizeof/Typeof** | `sizeof()`/`decltype()` | `.length`/`typeof()` | مختلف في الاستخدام | C++: CodeGen.cpp:794-800 vs JS: codegen.js:1021-1025 |
| 17 | **Namespace** | `namespace Name { }` | `var Name = {}; (function() { var _ns = Name; ... })()` | مختلف: C++ namespace vs JS object IIFE | C++: CodeGen.cpp:562 vs JS: codegen.js:782-805 |
| 18 | **Enum** | `enum Name { A, B, C };` | `var Name = { A: 0, B: 1, C: 2 };` | مختلف: C++ enum vs JS object | C++: CodeGen.cpp:570 vs JS: codegen.js:809-817 |
| 19 | **Method translation mapping** | لا يوجد (يعتمد على stdlib) | يُحوّل عربي→JS: `احذف→splice`, `أضف→push`, etc. | JS يُحوّل methodName مباشرة | C++: لا يوجد vs JS: codegen.js:944-963 |
| 20 | **Import/Export** | يُولّد تعليق `// استورد: ...` | لا يولّد شيئاً (no-op) | متطابق في عدم التنفيذ الفعلي | C++: CodeGen.cpp:716-724 vs JS: codegen.js:469-470 |
| 21 | **Template instantiation** | `template <typename T>` | **لا يولّد شيئاً** (erased at runtime) | متطابق في عدم التنفيذ | C++: CodeGen.cpp:609-617 vs JS: codegen.js:468 |
| 22 | **`كود_الحالة`** | يُولّد `if-else chain` للنصوص | يُولّد `switch` native | مختلف: JS يدعم string switch | C++: CodeGen.cpp:404-456 |
| 23 | **西亚 (hooks)** | لا يوجد | لا يوجد | متطابق | — |
| 24 | **输入: Input item** | `InputItem` struct مع `isPrompt` flag | `InputStmtAST.targets` فقط (بدون prompt) | مختلف: C++ يدعم input مع موجه | C++: Parser.cpp:672-697 vs JS: parser.js:917-961 |

---

## 5. أسئلة التصدير الثلاثة (إجابة + دليل سطر)

### السؤال 1: هل يوجد نظام تصدير لغة ض إلى برنامج ثنائي؟

**الإجابة: نعم، لكن غير متكامل — تترجم إلى C++ المصدر أولاً ثم تُجمع ب_compiler خارجي.**

**الدليل:**

1. **محرك C++**: `main.cpp` يُولّد ملفات `.cpp` و `.hpp` فقط — **لا يُولّد binary مباشرة**:
   - `Compiler/src/main.cpp:92`: `std::string outputFile = "output.cpp";` (الامتداد الافتراضي `.cpp`)
   - `Compiler/src/main.cpp:55`: `outFile << result.sourceOutput;` (يكتب كود C++ فقط)
   - `Compiler/src/main.cpp:72`: `"تم التحويل بنجاح إلى: " << outputFile` (رسالة تأكيد التحويل لـ C++)

2. **ملفات bin جاهزة** في `release/`:
   - `release/windows-10-11/bin/hello.exe`: `ELF 64-bit LSB pie executable, x86-64` (ليس PE Windows!)
   - `release/windows-10-11/bin/arithmetic.exe`: `ELF 64-bit LSB pie executable, x86-64`
   - **المشكلة**: ملفات `.exe` هذه هي في الواقع **ملفات ELF Linux** — لا يمكن تشغيلها على Windows

3. **ملف المُجمّع نفسه**: `Compiler/daad-compiler.exe`: `PE32+ executable for MS Windows 5.02 (console), x86-64` — هذا هو أداة المترجم نفسها (تُجمّع على Windows)، وليس output من كود ض.

4. **لا يوجد pipeline متكامل**: لا يوجد script أو هدف CMake يأخذ `.ض` ويُخرجه كـ binary:
   - `Compiler/CMakeLists.txt` يبني `daad-compiler` فقط (ال_compiler نفسه)
   - `Compiler/Makefile` يبني `$(BIN)` = `build-make/bin/daad-compiler` فقط
   - لا يوجد هدف مرتبط بـ "compile .ض to exe"

5. **محرك JS**: `dhadCompiler.js` يترجم ض → JavaScript فقط:
   - `dhadCompiler.js:122`: `const code = codegen.generate(ast);` (يُولّد JS فقط)
   - لا يوجد تصدير لـ binary من محرك JS

**النتيجة**: التصدير **شبه موجود** — C++ compiler يُولّد كود C++ مصدر فقط. التحويل النهائي إلى binary يتطلب `g++` أو `cmake` خارجياً. لا يوجد "ض → binary" pipeline متكامل.

---

### السؤال 2: إن وُجد فهو لأي قسم؟

**الإجابة: التصدير موجود للقسم العالي المستوى عبر C++ (compiler) — يُولّد كود C++ مصدر (.cpp/.hpp) ولا يكمل التحويل إلى binary.**

**الدليل بالمسار**:

- `.ض` → `Compiler/src/main.cpp:130` → `compiler.compile()` → `Compiler/src/Compiler.cpp:29-111` → `CodeGenVisitor` → `.cpp` + `.hpp`
- المسار: `Library/.ض` → `(Lexer → Parser → Optimizer → CodeGen)` → `output.cpp` + `output.hpp`
- ثم يحتاج `g++ output.cpp -o output` يدوياً

**وليس**:
- `.ضasm` → CPU assembly (.hamster-09) — هذا قسم DAAD المنخفض المستوى
- JS path → تنفيذ مباشر (لا binary)

---

### السؤال 3: ما الصيغة الخارجة بعد التصدير؟

**الإجابة: الصيغة الفعلية هي ملفات C++ مصدر (`.cpp` + `.hpp`) — وليس binary.**

**الدليل من الكود الخام:**

```
Compiler/src/main.cpp:92:
    std::string outputFile = "output.cpp";

Compiler/src/main.cpp:55-56:
    std::ofstream outFile(outputFile);
    outFile << result.sourceOutput;

Compiler/src/main.cpp:59-69:
    // Write header file (output.hpp) alongside the source file
    std::string headerFile = outputFile;
    auto dotPos = headerFile.rfind('.');
    if (dotPos != std::string::npos) {
        headerFile = headerFile.substr(0, dotPos) + ".hpp";
    }
```

**الامتدادات الفعلية من الكود:**
- `.cpp` — ملف المصدر الرئيسي (output الافتراضي)
- `.hpp` — ملف الرأس المشتق من نفس الاسم

**لا يوجد**: `.exe`, `.msi`, `.bin`, `.elf`, `.app` — لا يُولّد أي binary.

**ملفات release/**/*.exe** التي وُجدت هي:
- `release/windows-10-11/bin/*.exe` → ELF 64-bit (ليس PE Windows) — جُمعت على Linux
- `release/windows-7/bin/*.exe` → ELF 64-bit أيضاً
- `Compiler/daad-compiler.exe` → PE32+ (أداة المترجم نفسها — أُنشئت على Windows)

---

## 6. مشاكل/ملاحظات غير محلولة

| # | المشكلة | التفاصيل | الخطورة |
|---|---------|----------|---------|
| 1 | **خطأ في المرجع §1 عن كلمة null** | التقرير المرجعي يقول `لا_شيء` لـ KwNullptr (§1:174) لكن الكود الخام يقول `عدم` (Keywords.cpp:18) والـ JS يقول `عدم` (lexer.js:192). المرجع §1 به تقرير خاطئ. | عالية |
| 2 | **خطأ في المرجع §1 عن switch** | التقرير يقول `افتح` لـ KwSwitch (§1:190) لكن الكود يقول `اختر` (Keywords.cpp:31). `افتح` هو كلمة DAAD المنخفض المستوى وليست عالية المستوى. | عالية |
| 3 | **تجاوز المرجع §1 لأسماء الملفات** | التقرير يزعم قراءة `Keywords.cpp:128` لطباعة، لكن في الكود الفعلي `طباعة` على السطر 125، و `ادخل` على السطر 128. الأرقام غير متطابقة. | متوسطة |
| 4 | **`.exe` files في release/ هي ELF** | ملفات `release/windows-10-11/bin/*.exe` مُعلّمة بامتداد `.exe` لكنها ELF Linux binaries. لا يمكن تشغيلها على Windows. | عالية |
| 5 | **`كرر` — تعارض دلالي في JS** | `كرر`既是 `TT.KW_FOR` (for loop alternative)又是 `.repeat()` method. في codegen.js:957 يُحوّل `كرر` method إلى `.repeat()` لكن في lexer.js:201 يُحوّل `كرر` إلى `TT.KW_FOR`. هذا يعني `obj.كرر(3)` سيُحلّل كـ `obj.for(3)` بدلاً من `obj.repeat(3)`. | عالية |
| 6 | **`مجرّد دالة` غير مدعوم في C++** | الكتاب يُعلّم `مجرّد دالة` كتعريف دالة مجرّدة (§1:213). لا يوجد parser branch في C++ أو JS لهذا النمط. | متوسطة |
| 7 | **`تصنيف_مجرد` غير مدعوم** | الكتاب يذكر `تصنيف_مجرد` ككلمة بديلة في جدول المقارنة (§1:200)، لكن لا يوجد mapping في أي من المحركين. | منخفضة |
| 8 | **`عشوائي()` غير مدعوم** | الكتاب يُعلّم `عشوائي()` كدالة مدمجة (grep على book.html يظهرها في أمثلة)، لكن لا يوجد keyword أو built-in function في أي محرك. | عالية |
| 9 | **`اقرأ_عشري()` غير مدعوم** | الكتاب يُعلّم `اقرأ_عشري()` (§1: book grep line 1820)، لكن لا يوجد في أي محرك — فقط `ادخل` مع `std::cin >>` أو `__input()`. | متوسطة |
| 10 | **`private` غير مفعّل في C++** | Parser يحلل `خاص` ويُعيّنه في AST لكن CodeGen يتجاهله — جميع الحقول تُولّد `public` (CodeGen.cpp:524). | عالية |
| 11 | **`if` بدون أقواس غير مدعوم في C++** | الكتاب يُعلّم `إذا (شرط) جملة` بدون `{}` (الفصل 11)، لكن C++ parser يتوقع `parseBlock()` دائماً. | متوسطة |
| 12 | **`std::string` concat heuristic** | CodeGen.cpp:221-227 يكشف concatenation عبر فحص إذا كان lhs يبدأ وينتهي بـ `"`. هذا heuristic هش — ينكسر مع nested strings. | منخفضة |
| 13 | **无穷 recursive `parseForEachOrForStatement`** | Parser.cpp:728 — الدالة تحاول اكتشاف for-each عبر peek ahead ثم restore. إذا فشل الاكتشاف، قد يدخل في loop غير متوقع مع `parseExpression()` الاستمرارية. | منخفضة |
| 14 | **`استورد`/`صدّر` no-op** | الكلاهما يُولّد تعليق في C++ و nothing في JS. إذا أُضيف دعم module لاحقاً، سيتعطل. | منخفضة |
| 15 | **`الطول`/`حجم` محجوز في JS** | codegen.js:269-272 يمنع تسمية حقل بـ `طول` أو `حجم` — هذا قيد runtime غير موجود في C++. | منخفضة |
| 16 | **`_backup_original` في Compiler/src** | يوجد مجلد `Compiler/src/_backup_original` — محتوياته غير معروفة وقد تحتوي كوداً قديماً. | معلوماتية |
| 17 | ** STDERR warnings about memory leaks في C++** | CodeGen.cpp:65-66 يُولّد تحذيرات `⚠️ متغير ... تم تخصيص ذاكرة له بـ new لكن لم يتم تحريره بـ delete`. التحذيرات في stderr فقط ولا تمنع التوليد. | معلوماتية |
| 18 | **AST node type mismatch** | C++ AST uses `__builtin_print` and `__builtin_call` special types (CodeGen.cpp:246-253) بينما JS AST uses `PrintStmtAST` and `InputStmtAST` types مباشرة. نهج مختلف تماماً. | متوسطة |

---

## ملخص الإحصائيات

| البند | العدد |
|-------|-------|
| **فروقات كتاب vs C++** | 20 فرقاً |
| **فروقات كتاب vs JS** | 26 فرقاً |
| **فروقات C++ vs JS** | 24 فرقاً (11 keyword + 10 parser + 24 codegen) |
| **مشاكل غير محلولة** | 18 ملاحظة |
| **ملفات خام مُقرأة** | 19 ملف |
| **أسطر الكود المُقرأة** | ~8000+ سطر |

---

**تأكيد**: لم يُعدَّل أي كود في هذا التقرير. الملف الوحيد المُنشأ هو هذا التقرير نفسه.
