# مواصفات لغة ض عالية المستوى — المرجع الوحيد (Single Source of Truth)

![شعار لغة ض](logo-ض.jpeg)

> **الإصدار:** 1.0-factual — **2026-09-11 — من الكود الفعلي فقط، بلا تخمين.**
> **المصادر:** `Compiler/src/{Keywords.cpp,Lexer.cpp,UnicodeUtils.cpp,Parser.cpp (1683),CodeGen.cpp (840),Optimizer.cpp,Diagnostics.cpp,SandboxValidator.cpp,Compiler.cpp,main.cpp}` + `Compiler/include/Daad/{Token.hpp,Keywords.hpp,AST.hpp (677),Parser.hpp,Optimizer.hpp,Diagnostics.hpp,Compiler.hpp}` + `Studio/Web/js/{dhad-lexer.js (703),dhad-parser.js (1572),dhad-ast.js (639),dhad-codegen.js (1065),dhad.js (264),dhad-images.js (512),dhad-highlight.js (280),dhad-libraries.js (2765)}` + `Compiler/stdlib/*.hpp` + `Compiler/stdlib/*/*.ض (100 ملف، 2861 دالة)` + `Compiler/stdlib_arabic/ (100 ملف)`.
> **النطاق:** اللغة عالية المستوى (`.ض` → C++20 / JS). النسخة المنخفضة (`.daad`) خارج النطاق عمدًا.
> **السجل المرافق:** `KEYWORDS_REGISTRY.json` (116 كلمة، كل واحدة بحالتها) — `GRAMMAR_SPEC.md` (النحو EBNF) — `SEMANTIC_RULES.md` (الدلالة) — `STDLIB_SPEC.md` (المكتبة).
> **كيف تقرأ جداول المطابقة:** `C++` = الأصل (`Compiler/`)، `JS` = السحابية (`Web/js`)، `✅` = يدعم ويثبت بملف:سطر، `❌` = لا يدعم، `⚠️` = جزئي/مكسور/صامت (التفصيل في السطر نفسه).

---

## 1. نظرة عامة ومبادئ التصميم (مستنتجة من البنية — موسومة INFERENCE)

- **INFERENCE:** اللغة عربية أصيلة (كلمات عربية + `؛` `،`) تُترجم إلى C++20 (الأصل) أو JS (السحابة). ليست ترجمة حرفية للغات أجنبية: لها محللان مستقلان (Pratt-like ثنائي + recursive-descent) ونظاما توليد مختلفا البنية (C++ نصي مباشر مقابل JS بهيكل `collect→emitRuntime→emitClasses→emitFunctions→emitGlobals→main`).
- **FACT:** خط الأنابيب C++ (`Compiler.cpp:29-109`): `Lexer → Parser.parseStatement()* → OptimizerVisitor → جمع userFns → CodeGenVisitor (فصل global/main) → تحذيرات new/delete → Sandbox.validate → CompileResult{success,diagnostics,header,source}`. المخرجات ملفان دائمًا (`.cpp` + `.hpp` المشتق — `main.cpp:32-80`).
- **FACT:** خط الأنابيب JS (`dhad.js:14-40`): `Lexer.tokenize → Parser.parseProgram → CodeGen.generate` مع إرجاع مبكر عند أخطاء المعجم/التحليل. التنفيذ عبر `new Function(__print,__guiPrint,__array,__widgets,__events,__input,code)` (`dhad.js:127`).
- **FACT:** الفصل العام/التنفيذي في C++ (`Compiler.cpp:18-27` `isGlobalDeclaration`: Function|Class|Struct|Enum|Namespace|Template|Import|Export → عام، الباقي → جسم `main` المخلّق). JS تجمع `FunctionDecl|ClassDecl|StructDecl` (`dhad-codegen.js:72`) وتصدر الباقي كعموميات + جسم رئيسي.

## 2. الترميز والفواصل والتعليقات والمسافات

| الميزة | C++ (FACT) | JS (FACT) | الحكم |
|---|---|---|---|
| الترميز | UTF-8 → `codepoints` يدوي 1-4 بايت مع إصلاح الإيموجي (`UnicodeUtils.cpp:22-58`) | UTF-16 أصلي + أزواج بديلة (`dhad-lexer.js:336-372`) | ✅ كلاهما (بنية مختلفة، سلوك مكافئ للنص العربي) |
| فاصل الجمل | `;` أو `؛` (U+061B) (`Lexer.cpp:58`) — استهلاك اختياري في معظم الجمل (`if (SEMICOLON) consume`) | `;` أو `؛` (`dhad-lexer.js:671-676`) — `match(SEMICOLON)` اختياري غالبًا | ✅ موحد |
| فاصل العناصر | `,` أو `،` (U+060C) (`Lexer.cpp:106-107`) | `,` أو `،` (`dhad-lexer.js:664-669`) | ✅ موحد |
| تعليق سطر | `//` (`Lexer.cpp:73-76`) | `//` (`dhad-lexer.js:559-563`) | ✅ موحد |
| تعليق كتلة | `/* */` مع خطأ `تعليق غير مغلق` (`Lexer.cpp:77-88`) | `/* */` بلا خطأ مثبت (تخطٍ صامت) | ⚠️ C++ يخطئ، JS تصمت |
| تعليق `#` | ❌ (`#` → `رمز غير معروف` `Lexer.cpp:116-117`) | ✅ (`#` → حتى `\n` `dhad-lexer.js:575-579`) | ⚠️ JS-only — للتوحيد أضفه في C++ أو امنعه في JS |
| مسافات | `space \t \n \r FEFF` (`UnicodeUtils.cpp:60-62`)؛ `\t` تُحسب 4 أعمدة (`Lexer.cpp:201`) | `space \t \r \n` فقط (`dhad-lexer.js:547-557`) — بلا FEFF | ⚠️ FEFF مسافة في C++ فقط |
| علامات الاستفهام/الفاصلة العربية في المعرفات | `، ؛ ؟` (060C/061B/061F + 060D) مستبعدة من العربي (`UnicodeUtils.cpp:13-14`) | `، ؛ ؟` مستبعدة (`dhad-lexer.js:388-390`) | ✅ موحد (لكن `؟` لا تُlex في الطرفين — UNKNOWN مقصود؟) |
| الأرقام المشرقية `١٢٣`/`۱۲۳` | ❌ (أرقام `0-9` فقط `Lexer.cpp:122`؛ لا `0660/06F0`) | ❌ (`48-57` فقط `dhad-lexer.js:399-401`) | ❌ غير مدعومة في الطرفين — قيد موثق (لا تخترع دعمًا) |

## 3. الأنواع الأساسية (12)

| الكلمة | C++ (FACT) | JS (FACT) | الحكم |
|---|---|---|---|
| `صحيح` | `int` (`CodeGen.cpp:58`) | `var` (الأنواع غير محفوظة runtime — `emitVarDecl:433`) | ✅ معلن موحد، ممثل مختلف مشروع |
| `عشري` | `double` (`:59`) | `var` | ✅ كأعلاه |
| `منطقي` | `bool` (`:61`) | `var` | ✅ |
| `حرف` | `char` (`:60`) | `var` | ✅ |
| `نص` | `std::string` (`:63`) | `var` | ✅ |
| `ثابت` | `const` (`Keywords.cpp:163`) | `const` إذا بدأ النوع به (`emitVarDecl:436`) + `parseConstDecl` مستقل (`:284`) | ⚠️ JS لها عبارة ثابت مستقلة، C++ تعالجها كنوع |
| `فراغ` | `void` (`:62`) — الافتراضي لإرجاع الدوال (`Parser.cpp:1034`) | `var` — الافتراضي `'void'` نصًا (`:568`) | ✅ |
| `تلقائي` | `auto` (`:64`) | `var` — والنوع الافتراضي لـ for-each (`:767`) | ✅ |
| `صواب`/`خطأ` | `true`/`false` (`CodeGen.cpp:606-608` ← `parsePrimary:437-444`) | `true`/`false` (`genExpr:796` ← `parsePrimary:1181`) | ✅ |
| `عدم` | `nullptr` (`:610-612` ← `:445-448`) | `null` (`:799`)؛ الطباعة `عدم` (`dhad.js:157-160`) | ✅ |
| `عرّف` | `typedef` (تسجيل فقط — لا فرع عبارة) | مسجل `KW_TYPEDEF` بلا فرع عبارة (يسقط لتعبير) | ⚠️ مسجل بلا تنفيذ عبارة في الطرفين — قيد |
| مصفوفات `نوع[]` و`[][]` | ✅ (`CodeGen.cpp:67-72` + `:92-104` متعدد الأبعاد؛ `Parser.cpp:511-515`) | ⚠️ تُقبل نصًا (`parseVarDecl:248` + `IDENT[]IDENT:220`) لكن `emitVarDecl` يتجاهل النوع (`var`) | ⚠️ معلنة في الطرفين، منمطة في C++ فقط |

## 4. القيم الحرفية

| الحرفي | C++ (FACT) | JS (FACT) | الحكم |
|---|---|---|---|
| صحيح عشري `10` | ✅ (`scanNumber:120-132` أرقام + `.` اختيارية؛ `parsePrimary:384-388` بـ `strtod`) | ✅ (`readNumber:449-500` + `parseFloat/parseInt:495-497`) | ✅ |
| عشري `3.14` | ✅ (نفس الفرع؛ `CodeGen:167-172` يطبع `int` إن صحيحًا وإلا `to_string`) | ✅ | ✅ |
| `.5` البادئة بنقطة | ❌ (`.` تُlex `DOT:109` قبل الرقم) | ✅ (`isDigit \|\| (.+digit)` `:588`) | ⚠️ JS-only |
| سداسي `0x10` | ❌ (`0` ثم `x10` توكنان) | ✅ (`0x` `:456` + `parseInt 16` `:497` مع تطبيع `16`) | ⚠️ JS-only (مع تطبيع الشكل) |
| علمي `1e3` | ❌ (`1` ثم `e3`) | ✅ (`e/E±` `:480-492` مع تطبيع `1000`) | ⚠️ JS-only |
| نص `"..."` | ✅ (`"` فقط `:60`؛ escapes `\n\t\r\\\"` + ثماني `\0-\777` `:168-175`؛ مجهول يُحفظ `:183`؛ متعدد الأسطر مسموح؛ EOF → خطأ `نص غير مغلق:150`) | ✅ (`"` أو `'` `:582`؛ `\n\t\r\\\'\"\0` `:518-523`؛ عام يُسقط الشرطة؛ سطر جديد يقطع silently `:526`؛ EOF صامت `:534`) | ⚠️ فروق escapes وإغلاق (الثماني C++ فقط؛ المفرد JS فقط؛ القطع/الصمت JS) |
| `صواب/خطأ/عدم` | ✅ (انظر §3) | ✅ | ✅ |

## 5. المتغيرات والثوابت

```arabic
صحيح العدد = 10 ؛        // ✅ الطرفان (C++: VarDecl int؛ JS: var)
ثابت باي = 3.14 ؛        // ✅ (C++ كنوع؛ JS كـ const عبر parseConstDecl)
تلقائي النتيجة = 10 + 5 ؛ // ✅
نص الاسم = "أحمد" ؛      // ✅ (C++: الصفر الافتراضي لنص يُستبدل "" — CodeGen.cpp:243-246)
صحيح[] أرقام ؛           // ✅ معلن؛ C++ vector، JS var
صحيح م = {} ؛            // ✅ C++ ترقية {} إلى vector (CodeGen.cpp:262-267)؛ JS: Raw غائبة → FunctionCall(__array)
[] الفارغة                // C++: RawExprAST("{}") (Parser.cpp:468-472)؛ JS: FunctionCall(__array,[]) (dhad-parser.js:1242)
```
- **FACT C++:** `parseVariableDeclaration` (`Parser.cpp:506-575`): نوع + `[]` لاحقة + (`= init` | `(params)->return {body}` للدوال المعلنة بصيغة النوع | `;` → تهيئة `0.0`)؛ الاسم يقبل كلمات مفتاحية (`:532-534` مثل `س/ص`)؛ `صحيح=1` تُحلل إسنادًا (`:518-520`)؛ `نوع(` تُحلل استدعاءً (`:521-525`).
- **FACT JS:** `parseVarDecl` (`:237-283`): `*` بادئة (`:242`) + `[]` (`:248`) + اسم (`expectName:256` يقبل كلمات) + `= expr` (`:259`) + `,a,b` متعدد يُغلف `ProgramAST` (`:266-281`)؛ `parseConstDecl` (`:284-328`) بثلاث صيغ (`ثابت صحيح=42`/`ثابت نوع اسم`/`ثابت اسم`).

## 6. المعاملات (الأسبقية والأشكال)

أسبقية موحدة مثبتة (C++ `Parser.cpp:343-361` = JS `dhad-parser.js:1008-1015):

| الأسبقية | المعاملات | C++ توكن | JS توكن | التوليد |
|---|---|---|---|---|
| 1 | `\|\|` `أو` | `TOKEN_OR` + KEYWORD أو | `OR` + `أو` | `\|\|` |
| 2 | `&&` `و` | `TOKEN_AND` + KEYWORD و | `AND` + `و` | `&&` |
| 3 | `==` `!=` | `EQUAL_EQUAL` `NOT_EQUALS` | `EQ` `NEQ` | كما هي |
| 4 | `<` `>` `<=` `>=` | `LESS/GREATER/...` | `LT/GT/LTE/GTE` | كما هي |
| 5 | `+` `-` | `PLUS/MINUS` | `PLUS/MINUS` | كما هي (+ دمج نص C++ `:200-206`) |
| 6 | `*` `/` `%` | `STAR/SLASH/PERCENT` | `STAR/SLASH/PERCENT` | كما هي |
| 7 | `^` (أس) | `POWER` → `std::pow(l,r)` (`CodeGen.cpp:192-193`) | `POWER` → `Math.pow` (`:830`) / `**` للمركب (`:460`) | ⚠️ مكافئ دلاليًا، نص مختلف مشروع |
| أحادي | `-` `!` (`ليس` ⚠️) | `MINUS/NOT` (`parseUnary:364-380` — فرع `ليس` ميت*) | `-/!/ليس/++/--/* /&` (`:1036-1079`) | `(!x)/(-x)` |
| ثلاثي | `? :` | ✅ (`parseExpression:332-338`؛ `CodeGen:614-621`) | ✅ (`parseExpression:997`؛ `genExpr:931`) | ✅ |
| إسناد | `= += -= *= /= %= ^=` | ✅ كلها (`Lexer:64-114`؛ `Parser:191-201,245-256`؛ `CodeGen:290-305` مع `^=`→pow) | ⚠️ كلها **عدا `%=`** (لا `PERCENT_ASSIGN`؛ `parseIdentifierStmt:913` تسقطه) | ⚠️ `%=` C++-only |
| زيادة خاصة | `زد عداد ؛` `انقص عداد ؛` | ✅ جملة → `+=1/-=1` (`Parser.cpp:77-94` + for-update `:734-748`) | ✅ `KW_INCREMENT/DECREMENT` | ✅ |
| `&` المفردة | ❌ (فقط `&&` `:95-96`) | ✅ (`AMPERSAND:172` + تمرير أحادي `:1072`) | ⚠️ JS-only (تكسر `مرجع→&`) | ⚠️ فجوة lexer في C++ |
| `->` | ⚠️ توكنان `-` `>` (`Parser.cpp:555-561` و`:1035-1041` يفحصهما متتابعين) | ✅ توكن `ARROW` (`:152,638-640` + `parseFunctionDecl:566`) | ⚠️ يعمل في الطرفين، هش في C++ | ⚠️ وحّد التوكن |
| `::` | ❌ | ✅ (`DOUBLE_COLON` + `parsePostfix :::1108`) | ⚠️ JS-only | ⚠️ |
| `@` `#` | ❌ | ✅ (`AT/HASH` + `#` تعليق) | ⚠️ JS-only | ⚠️ |

\* `ليس`: انظر `KEYWORDS_REGISTRY.json` (حالة `js-only-registry-gap`) — `Parser.cpp:375-379` يفحص `TOKEN_KEYWORD+ليس` لكن الليكسر لا ينتجها أبدًا (غير مسجلة) فيُlex `ليس` معرفًا. JS صحيحة (`KW_NOT` + `:1051`).

## 7. جمل التحكم

| الجملة | C++ (FACT) | JS (FACT) | الحكم |
|---|---|---|---|
| `إذا (شرط) { } وإلا { }` | ✅ `parseIfStatement:637-651` — شرط تعبير (أقواس اختيارية فعليًا إذ لا تُطلب) + **كتل `{ }` إجبارية** (`parseBlock`) + `وإلا` كتلة فقط (لا else-if) | ✅ `parseIf:330-358` — أقواس اختيارية (`:332`) + **جملة مفردة مسموحة** (`:341,351`) + `else if` معاد (`:348`) | ⚠️ JS أغنى نحويًا (مفردة + else-if) |
| `طالما/بينما (شرط) { }` | ✅ `parseWhileStatement:653-658` | ✅ `parseWhile:360` | ✅ |
| `لكل/كرر` (عداد + نطاق) | ✅ `parseForEachOrForStatement:660-829` — كشف `في` باستشراف (نوع اسم في → ForEach وإلا C-style)؛ تحديث يدعم `++ -- +=... زد انقص` | ✅ `parseFor:376` + `parseForEach:437` منفصلان | ✅ (آليتان، سلوك مكافئ مُدعى — يحتاج parity test) |
| `افعل { } طالما (شرط) ؛` | ✅ `parseDoWhile:831-845` (`طالما` الذيل اختيارية `:835`) + `do{}while()` (`:358-366`) | ✅ `parseDoWhile:455` + `emitDoWhile:553` | ✅ |
| `اختر (قيمة) { حالة X: ... افتراضي: ... }` | ✅ `parseSwitch:847-887` + توليد ذكي (`CodeGen:368-425`: نصوص→if-else، أعداد→switch+break) | ✅ `parseSwitch:474-508` + `switch` خام **بلا break تلقائي** (`:562-570`) | ⚠️ فرق fallthrough للنصوص |
| `توقف ؛` `استمر ؛` (`اكسر/تابع` بدائل) | ✅ مباشر + `؛` (`Parser.cpp:61-62`) + `break/continue` | ✅ مباشر (`:138-139`) | ✅ |
| `انتقل` | ❌ رفض صريح (`Parser.cpp:63-75`) | ❌ فعليًا (مسجلة بلا فرع) لكن بلا رسالة صريحة | ⚠️ وحّد الرسالة الصريحة |
| `ارجع [قيمة] ؛` | ✅ (`parseReturn:889-898`؛ `CodeGen:427-434`) | ✅ (`parseReturn:511`) | ✅ |

## 8. الدوال (تعريف واستدعاء)

```arabic
دالة جمع(صحيح أ ، صحيح ب) -> صحيح { ارجع أ + ب ؛ }   // ✅ الطرفان
جمع(5 ، 3) ؛                                          // ✅ استدعاء
دالة تحية() { طباعة("مرحباً") ؛ }                     // ✅ إرجاع افتراضي فراغ/void
```
- **FACT C++** (`Parser.cpp:1003-1047`): `دالة` + نقطة بادئة اختيارية + اسم يدعم **مسافات** (`إضافة_ خاصية`) + أقواس **اختيارية** (`:1016`) + معاملات `parseParam` (`:1072-1123`: `[]`→فراغ فارغ، مفردة→صحيح افتراضي، `الاسم:النوع` أو `النوع الاسم`) + إرجاع `->` بفحص توكنين (`:1035-1041`, افتراضي `فراغ`) + جسم كتلة. التوليد (`CodeGen.cpp:444-472`): إعلان header + تعريف (مؤهل `Class::` داخل الصفوف `:460`) بأنواع `mapType`.
- **FACT JS** (`dhad-parser.js:522-575`): `دالة` + `IDENTIFIER` فقط (بلا مسافات/نقطة) + `(` إجبارية (`:526`) + معاملات بأنواع وقيم افتراضية (`:529-560`, مفردة→صحيح `:540`) + `[]` (`:545`) + إرجاع `->` عبر توكن `ARROW` (`:566-568`, افتراضي `'void'`) + `بانتظار` اختيارية (`:571`) + جسم كتلة. التوليد (`emitFunctions:333`): `function` مع `=undefined` defaults وإعادة `س/ص→x/y`.
- **الحكم:** ✅ متوافق للأشكال الشائعة؛ ⚠️ فروق شكلية (C++ أسماء مركبة وأقواس اختيارية؛ JS defaults وawait) — وثّقها ولا تكسرها.

## 9. البرمجة الكائنية OOP والنطاقات

| الميزة | C++ (FACT) | JS (FACT) | الحكم |
|---|---|---|---|
| `صنف/فئة اسم { ... }` | ✅ `parseClassDeclaration:1126-1268` — أعضاء `النوع الاسم` أو `الاسم:النوع` + طرق `النوع الاسم(...) -> نوع { }` (صيغة `الاسم:النوع()` **مرفوضة** `:1182-1185`) + `;` اختيارية بسطر جديد + `m_knownClassNames` للاستخدام كنوع | ✅ `parseClass:578-639` — جسم مسطح + `عام/خاص/محمي/ساكن` + ctor بالاسم + `يرث parent` + `الاسم:النوع` معكوسة (`:608-624`) | ⚠️ بنية AST مختلفة جذريًا (C++ منفصلة أعضاء/طرق — `AST.hpp:374-383`؛ JS مسطحة + parent ديناميكي) — التكافؤ سلوكي مُدعى |
| الحقول/الطرق والوصول | ✅ `public:` header + `Class::` تعريفات (`CodeGen:485-511`) | ✅ closures + `this.` + `defineProperty` + prototype (`emitClasses:227`) | ✅ |
| الباني | ⚠️ `ConstructorDeclAST(params,body)` موجود (`AST.hpp:511-517` + `CodeGen:674-687`) **بلا محلل** (لا `parseConstructor` في `Parser.cpp/hpp`) — لا يمكن كتابته نحويًا! | ✅ `parseConstructor:642` (باسم الصف) | ⚠️ JS-only نحويًا |
| `هيكل اسم { ... }` | ⚠️ **مكسور**: الاسم فقط (`AST.hpp:385-390`) + توليد فارغ (`CodeGen:513-515`) — الأعضاء **مفقودة أصلًا** | ✅ `parseStruct:672` (name+body) + `emitStructDecl:696` (حقول) | ❌ C++ مكسور — وسّع AST |
| `تعداد اسم { أ ، ب }` | ✅ (`parseEnum:908-926`؛ `CodeGen:525-533` header enum) | ✅ (`parseEnum:680`؛ `emitEnum:736` كائن `{v:i}`) | ✅ |
| `نطاق اسم { ... }` | ✅ (`parseNamespace:900-906`؛ `CodeGen:517-523` namespace) | ✅ (`parseNamespace:697`؛ `emitNamespace:710` var+IIFE) | ✅ |
| `واجهة/يرث/الأصل/مجرّد` | ⚠️ مسجلة بلا تنفيذ (انظر السجل)؛ الوراثة غير مثبتة في `parseClassDeclaration` المقروء | ⚠️ `يرث` تعمل (`parent`)؛ الباقي مسجل بلا فحص | ⚠️ JS أغنى (وراثة) |
| `قالب <T> ...` | ✅ `parseTemplate:957-1001` (يتطلب `<` وإلا متغير/خطأ) + `template<typename>` header (`:563-571`) | ✅ `parseTemplate:705` + **محو** runtime (`:401`) | ⚠️ دلالي مشروع (compile-time مقابل محو) |
| `هذا/ذاتي.عضو` | ✅ `MemberAssignment` (`Parser:174-186,262-274`؛ `CodeGen:577-580` بنصوص string) | ✅ (`parseIdentifierStmt/parsePostfix` بكائن expr + `::` + دمج `قص من`) | ⚠️ JS أغنى (كائن تعبيري + تسلسل) |

## 10. الاستثناءات والذاكرة والوحدات

| الميزة | C++ | JS | الحكم |
|---|---|---|---|
| `حاول { } امسك [(متغير)] { } أخيراً { }` | ✅ (`parseTryCatch:928-955`؛ `CodeGen:535-561` — **أخيراً كتلة عادية `{ }` لا finally حقيقية!**) | ✅ (`parseTryCatch:720-750`؛ `emitTryCatch:748` بـ `finally` حقيقية) | ⚠️ C++ تُنفذ أخيراً دائمًا كمجال (ليست دلالة finally) |
| `ارمِ expr` | ✅ (`:1307`؛ `throw`) | ✅ (`:753`) | ✅ |
| `احذف expr` | ✅ (`:1317`؛ `delete` + تتبع leak) | ✅ (`:779`) | ✅ |
| `جديد T(args)` | ✅ تعبير (`CodeGen:694-704` + `new` tracking) | ✅ (`parseNewExpr:761` + `new`) | ✅ |
| `حجم_الـ/نوع_الـ` | ✅ `sizeof/decltype` compile-time | ⚠️ `(x).length` / `typeof` runtime | ⚠️ دلالي مختلف مشروع — وثّق |
| `تأكد/استثناء` | ⚠️ مسجلان بلا عبارة فعلية | ⚠️ كذلك | ⚠️ قيد مشترك |
| `استورد/صدّر` | ⚠️ تُحلل وتُتجاهل (`// no-op` `:648-654`) | ⚠️ كذلك (`:402-403`) | ⚠️ صمت متطابق موثق — خطر تضليل |
| `تزامن/انتظر/خيط/احجز/مشترك/فريد/وحدة/مضمن/خارجي` | ⚠️ تسجيل فقط (لا فروع عبارات) — التنفيذ في `stdlib/*.ض` نصًا | ⚠️ كذلك | ⚠️ قيود مشتركة |
| `ساكن` | ⚠️ بلا فرع مستقل | ✅ داخل الصفوف (`:601`) | ⚠️ JS أغنى صفريًا |

## 11. الإدخال والإخراج

| الميزة | C++ | JS | الحكم |
|---|---|---|---|
| `طباعة/اطبع(a ، ب)` | ✅ متعدد (`parsePrint:584-610`؛ `daad_print(args)` `:658-666`) | ✅ متعدد (`parsePrint:787`؛ `__print`) | ✅ |
| `ادخل(x)` | ⚠️ **مفرد فقط** (`parseInput:612-635` — اسم واحد وإلا خطأ؛ `cin>>` `:669-672`) | ✅ **متعدد** (`parseInput:802-847` — `targets[]` + `forEach __input`) | ⚠️ فجوة arity مثبتة |
| `__print` الدلالة | `daad_to_string` (`DaadRuntime.hpp`): نص خام، `bool→صواب/خطأ`، أعداد `ostringstream` | `print` (`dhad.js:154`): `null→عدم`، `true→صواب`، `false→خطأ`، `undefined→عدم`، ضم بمسافات | ✅ مكافئ للقيم الشائعة |

## 12. برمجة الواجهات GUI (14 كلمة — حالة خاصة)

- **FACT:** الكلمات الـ14 مسجلة في الطرفين (`Keywords.cpp:108-121` → `QPushButton...`؛ `dhad-lexer.js:278-292`). **لا يوجد** أي فرع تحليل GUI في C++ (`grep Parser.cpp` صفر) ولا أي `visit(Button)` في `CodeGen.cpp`. JS تحللها كإعلان ودجت (`parseWidgetDecl:850-863` → `VarDecl(widgetType,name,init?)`) بلا مولد خاص (تُعامل كمتغير).
- **الحكم:** ⚠️ `registered-parser-gap` — القرار الموثق: GUI **محللة شكلًا في JS فقط**، وبلا توليد Qt فعلي في أي طرف. لا تدّعِ دعمه في C++؛ إما فعّله (Qt ثقيل) أو ارفضه بخطأ صريح.

## 13. معالجة الصور (20 كلمة — حالة خاصة)

- **FACT:** الـ20 مسجلة ومحللة في الطرفين (C++ `Parser.cpp:119-153,1353-1681` بمسار مزدوج `kw(...)` عام؛ JS `dhad-parser.js:192-211,1291-1555`). التوليد C++ صحيح دائمًا (`daad::image::*` `:718-838`). التوليد JS **مكسور في 17/20** (مسافة بدل نقطة `:959-1054` — انظر السجل) + `حجم_صورة/بكسل` بلا `genExpr` (تُنتجان `/* unknown */` كتعبير).
- **FACT:** التنفيذ: C++ بلا `daad::image` runtime في المستودع (الاستيراد غير موجود — `CodeGen.cpp:79-81` تستورد `Math/DaadStdlib` فقط)؛ JS لها `dhad-images.js` (22 صيغة — `SUPPORTED_FORMATS:10-33` — + 29 دالة عامة `:470-498`) لكن المولد لا يستدعيها بالنقطة الصحيحة.
- **الحكم:** ❌ الصور **مسجلة ومحللة لكن غير قابلة للتشغيل end-to-end في أي طرف اليوم** (C++ بلا runtime، JS ب Emitters مكسورة). الإصلاح الأول: النقاط الـ17.

## 14. التحسين Optimizer (C++ فقط — JS بلا مُحسن)

- **FACT** (`Optimizer.cpp:1-106` + `Optimizer.hpp:7-78`): طي ثوابت `Number op Number` لـ `+ - * / %` فقط (`:47-52`، `%` بتحويل int، `/` بلا فحص صفر) مع `isFolded/foldedValue` (`AST.hpp:159-160`) يستهلكها `CodeGen:184-185`؛ اجتياز (traversal) لـ VarDecl/Assignment/Compound/Unary/If/While/Function/For/DoWhile/Switch/Return/Namespace/TryCatch(بلا finally!)/MemberAssignment/Throw/ExprStmt/Template؛ **كل الزوار الآخرين `{}` فارغة** (35+ فارغًا — Classes/Structs/Enum/Import/Export/Print/Input/ForEach/Constructor/Images...).
- **الحكم:** ⚠️ مُحسّن شكلي (طي + اجتياز) لا تحسين حقيقي (لا حذف ميت/انتشار/تبسيط). JS بلا مُحسن إطلاقًا — التكافؤ غير متأثر (الطي لا يغير الدلالة إلا بقسمة الصفر — UNKNOWN سلوك `/0`).

## 15. التشخيص والأخطاء (رسائل عربية — C++ مثبتة، JS مُدعاة)

- **FACT C++** (`Diagnostics.hpp:10-38` + `Diagnostics.cpp:6-33`): `Severity{Note,Warning,Error,Fatal}` → `ملاحظة/تحذير/خطأ/خطأ فادح`؛ `Diagnostic{severity,line,column,message,sourceLine}`؛ `report` ترفع `hasErrors` على Error/Fatal؛ `printAll` تطبع `[شدة] في السطر line:col - msg` + السطر `^`. أمثلة مثبتة: `انتقل غير مدعومة` (`Parser.cpp:66`)، `تعبير غير متوقع` (`:486`)، `لم يتم التعرف` (`:290`)، `تعليق غير مغلق` (`Lexer.cpp:84`)، `نص غير مغلق` (`:150`)، `رمز غير معروف` (`:117`)، تحذير `new بلا delete` (`CodeGen.cpp:52`)، أخطاء Sandbox العربية (`SandboxValidator.cpp:33-63,152,164,187`).
- **JS:** رسائل عربية مُدعاة (`متوقع:` ونحوها في `dhad-parser.js`) — لم تُحصر سطرًا بسطر هنا (خارج عينة القراءة العميقة — UNKNOWN التغطية الكاملة).
- **الحكم:** ⚠️ C++ موثقة سطرًا؛ JS تحتاج حصرًا لاحقًا (لا تخترعه هنا).

## 16. الأمان Sandbox (مجموعتان منفصلتان — لا توحيد بسذاجة)

- **FACT C++** (`SandboxValidator.cpp:13-78,82-237`): بيضاء 30 header (`<iostream,string,vector...>` + `"output.hpp","Daad/Runtime/DaadRuntime.hpp","stdlib/Math.hpp"` مع تجاوز `"Daad/|"stdlib/|*.hpp` `:146-149`)؛ سوداء 18 regex (`system/popen/exec/remove/rename/unlink/reinterpret_cast/void*/socket/connect/bind/listen/accept/fork/kill/exit/const_cast/#define\`)؛ خطيرة 8 headers (`<cstdlib,cstdio,windows.h,unistd.h,sys/,netinet/,arpa/,dlfcn,signal,setjmp` — **`<cstdlib>` في البيضاء والسوداء معًا!** `:18` و`:68`)؛ heuristics (`void count>20` تحذير `:213` + `new int/char/double[` تحذير `:226`). فحص سطري يتخطى `//` + regex لكل سطر.
- **FACT JS** (`dhad.js:46-92`): 26 نمطًا (`process/require/eval/Function/fetch/XHR/WebSocket/import(/globalThis/__proto__/prototype.constructor/get|setPrototypeOf/callee/window.parent|top|self/frames/opener/location/document.cookie|write/localStorage/sessionStorage/IndexedDB/navigator/window`) + تطبيع (صفر-عرض/تحكم/NBSP→مسافة `:81-84`) + فحص مزدوج (عادي + مضغوط بلا `\n` `:87`). التنفيذ `new Function` بست وسائط فقط (`:127`) + `__print/__array/__input` (`:151-193`).
- **الحكم:** ⚠️ لا تقاطع إطلاقًا (كل مولد يحمي لغته). أبقهما منفصلين؛ أصلح ازدواج `<cstdlib>` فقط.

## 17. القيود والحدود المثبتة (لا تخترع غيرها)

| القيد | الدليل |
|---|---|
| `انتقل` مرفوضة بالتصميم (لا labels) | `Parser.cpp:63-75` |
| `Struct` بلا أعضاء في C++ | `AST.hpp:385-390` + `CodeGen:513-515` |
| `قالب` يتطلب `<` في C++ وإلا متغير/خطأ | `Parser.cpp:960-986` |
| `Import/Export` تُتجاهلان silently في الطرفين | `CodeGen:648-654` + `dhad-codegen.js:402-403` |
| `+` النصوص: C++ تلف الحرفي بـ `std::string()` في حالات (`CodeGen:200-206`) | `:200-206` |
| `Switch` النصوص if-else في C++ فقط | `CodeGen:381-404` |
| `finally` ليست حقيقية في C++ | `CodeGen:552-560` |
| `ImageSize/Pixel` تعبيران في C++، جملتان ناقصتا genExpr في JS | `CodeGen:730/780` مقابل `dhad-codegen.js:786-943` |
| `0x/علمي/.5` و`'` و`#` JS-only؛ الثماني C++-only | §4 |
| المشرقية غير مدعومة في الطرفين | §2 |
| GUI/Wاجهات/تزامن/وحدات مسجلة بلا تنفيذ عبارات | السجل + §10/§12 |
| `مؤشر/*` أحادي و`مرجع/&` مفردة بلا توكنات C++ | §6 + `Lexer.cpp:95-96` |
| `س/ص→x/y` تعمل JS فقط (فرع C++ ميت) | `Parser.cpp:477-484` مقابل الفرع العام `:436-458` |

## 18. أمثلة مرجعية دنيا (مقتبسة حرفيًا من المستودع)

```arabic
طباعة("مرحبا بالعالم!") ؛
```
```arabic
صحيح س = 10 ؛
صحيح مجموع(صحيح أ ، صحيح ب) { ارجع أ + ب ؛ }
إذا (س > 5) { طباعة(س) ؛ } وإلا { طباعة(0) ؛ }
لكل (صحيح ا = 0 ؛ ا < 10 ؛ ا = ا + 1) { طباعة(ا) ؛ }
لكل (عنصر في المجموعة) { طباعة(العنصر) ؛ }
فئة شخص { نص الاسم ؛ صحيح العمر ؛ }
```

## 19. ما ليس في هذه المواصفة (UNKNOWN مُعلن — لا تُفترض)

- تغطية رسائل JS العربية سطرًا بسطر؛ سلوك `/0` بعد الطي؛ تكافؤ `لكل` العداد/النطاق سلوكيًا؛ وراثة C++ (`يرث`)؛ `اطبع` فرع C++ الدقيق؛ `س/ص` داخل `زد/انقص` JS؛ أداء 10K سطر؛ مخطط DB؛ سلوك ` DaadStdlibExt` الـ200 stub عند الاستدعاء الفعلي. كلها تحتاج `real_regression` لغويًا متقاطعًا (نفس `.ض` في الطرفين) — خارج مرحلة التدوين.
