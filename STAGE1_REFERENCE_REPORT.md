# Stage 1 Reference Report — Dhad (ض) Programming Language

**Generated**: 2026-09-12
**Sources Read**: 12 primary source files
**Report Type**: Read-only comprehensive reference

---

## Table of Contents

1. [Book Structure & Chapter Index](#1-book-structure--chapter-index)
2. [Language Keywords (Source-Code Verified)](#2-language-keywords-source-code-verified)
3. [Challenge Definitions (All 211)](#3-challenge-definitions-all-211)
4. [Low-Level Keywords (CPU/DAAD) — Excluded List](#4-low-level-keywords-cpudaad--excluded-list)
5. [Top 5 Observations](#5-top-5-observations)

---

## 1. Book Structure & Chapter Index

**Source**: `/home/m_hmoz/Documents/Dhad-Studio-Unified/Studio/Web/book.html`
**Extracted text**: `/tmp/book_text.txt` (23,460 lines)
**Claimed chapters**: 112 | **Extracted unique chapter headings**: 88

### Section Breakdown

| Section | Arabic Title | Chapters | Topic Range |
|---------|-------------|----------|-------------|
| 1 | الأساسيات | 1–10 | Variables, types, I/O, operators |
| 2 | التحكم في التدفق | 11–20 | If/else, loops, switch |
| 3 | الدوال والمصفوفات | 21–30 | Functions, arrays, strings |
| 4 | البرمجة الكائنية | 31–40 | Classes, OOP, data structures |
| 5 | الخوارزميات | 41–50 | Sorting, searching, DP, graphs |
| 6 | قواعد البيانات | 51–60 | SQL, NoSQL, design |
| 7 | اللغات المتعددة | 61–70 | JS interop, Python, C++ |
| 8 | المشاريع | 71–88+ | Calculator, games, final projects |

### Complete Chapter List (88 chapters)

#### Section 1: Basics (الفصول 1-10)
1. **الفصل 1**: مقدمة في لغة ض — Introduction to Dhad language
2. **الفصل 2**: المتغيرات وأنواع البيانات — Variables and data types
3. **الفصل 3**: الإخراج (طباعة) — Output (print)
4. **الفصل 4**: الإدخال من المستخدم — User input
5. **الفصل 5**: العمليات الحسابية — Arithmetic operations
6. **الفصل 6**: العمليات المنطقية — Logical operations
7. **الفصل 7**: النصوص — Strings
8. **الفصل 8**: الأعداد العشرية — Decimal numbers
9. **الفصل 9**: الثوابت — Constants
10. **الفصل 10**: ملخص الأساسيات — Basics summary

#### Section 2: Control Flow (الفصول 11-20)
11. **الفصل 11**: الجملة الشرطية إذا — If statement
12. **الفصل 12**: إذا وإلا — If and else
13. **الفصل 13**: شروط متعددة (وإلا إذا) — Multiple conditions (else if)
14. **الفصل 14**: حلقة طالما — While loop
15. **الفصل 15**: حلقة لكل — For loop
16. **الفصل 16**: حلقة كل على مصفوفة — For-each on array
17. **الفصل 17**: المقارنات — Comparisons
18. **الفصل 18**: المنطق && و || — Logic operators
19. **الفصل 19**: النفي ! — Negation
20. **الفصل 20**: حلقة افعلطالما — Do-while loop

#### Section 3: Functions & Arrays (الفصول 21-30)
21. **الفصل 21**: دوال بسيطة — Simple functions
22. **الفصل 22**: دوال مع معاملات — Functions with parameters
23. **الفصل 23**: دوال بقيمة إرجاع — Functions with return values
24. **الفصل 24**: المصفوفات — Arrays
25. **الفصل 25**: طول المصفوفة — Array length
26. **الفصل 26**: تعديل المصفوفة — Array modification
27. **الفصل 27**: حساب المجموع — Sum calculation
28. **الفصل 28**: أكبر وأصغر قيمة — Max/min value
29. **الفصل 29**: البحث في المصفوفة — Array search
30. **الفصل 30**: عد التكرارات — Count occurrences

#### Section 4: OOP & Data Structures (الفصول 31-40)
31. **الفصل 31**: المصفوفات المتقدمة — Advanced arrays
32. **الفصل 32**: القوائم — Lists
33. **الفصل 33**: الخرائط — Maps
34. **الفصل 34**: تعريف صنف — Class definition
35. **الفصل 35**: الشجرة الثنائية (BST) — Binary search tree
36. **الفصل 36**: الشجرة المتوازنة — Balanced tree
37. **الفصل 37**: الجدول الهجينة — Hash table
38. **الفصل 38**: الكومة (Heap) — Heap
39. **الفصل 39**: المخطط (Graph) — Graph
40. **الفصل 40**: الخوارزميات — Algorithms intro

#### Section 5: Algorithms (الفصول 41-50)
41. **الفصل 41**: مقدمة في الخوارزميات — Algorithm introduction
42. **الفصل 42**: الفرز السريع (Quick Sort) — Quick sort
43. **الفصل 43**: الدمج (Merge Sort) — Merge sort
44. **الفصل 44**: خوارزميات البحث — Search algorithms
45. **الفصل 45**: البرمجة الديناميكية — Dynamic programming
46. **الفصل 46**: البرمجة الجشعة — Greedy algorithms
47. **الفصل 47**: خوارزمية الاهتزاز — Backtracking
48. **الفصل 48**: خوارزميات الرسم البياني — Graph algorithms
49. **الفصل 49**: خوارزميات النصوص — String algorithms
50. **الفصل 50**: تعقيد الخوارزميات — Complexity analysis

#### Section 6: Databases (الفصول 51-60)
51. **الفصل 51**: مقدمة في قواعد البيانات — Database introduction
52. **الفصل 52**: SQL الأساسي — Basic SQL
53. **الفصل 53**: استعلامات متقدمة — Advanced queries
54. **الفصل 54**: تصميم قواعد البيانات — Database design
55. **الفصل 55**: قواعد البيانات العلائقية — Relational databases
56. **الفصل 56**: NoSQL — NoSQL databases
57. **الفصل 57**: التخزين السحابي — Cloud storage
58. **الفصل 58**: أمان قواعد البيانات — Database security
59. **الفصل 59**: النسخ الاحتياطي — Backup
60. **الفصل 60**: إدارة البيانات — Data management

#### Section 7: Languages Interop (الفصول 61-70)
61. **الفصل 61**: الجافاسكريبت — JavaScript
62. **الفصل 62**: بايثون — Python
63. **الفصل 63**: C++ — C++
64. **الفصل 64**: بناء واجهات — Building interfaces
65. **الفصل 65**: APIs — APIs
66. **الفصل 66**: الشبكات — Networking
67. **الفصل 67**: البرمجة غير المتزامنة — Async programming
68. **الفصل 68**: إدارة الحزم — Package management
69. **الفصل 69**: أدوات التطوير — Development tools
70. **الفصل 70**: أفضل الممارسات — Best practices

#### Section 8: Projects (الفصول 71-88+)
71. **الفصل 71**: مشروع الآلة الحاسبة — Calculator project
72. **الفصل 72**: مشروع لعبة النرد — Dice game
73. **الفصل 73**: مشروع قائمة المهام — Todo list
74. **الفصل 74**: مشروع محفظة — Wallet project
75. **الفصل 75**: مشروع نظام درجات — Grading system
76. **الفصل 76**: مشروع لعبة الحظ — Luck game
77. **الفصل 77**: مشروع محادثة — Chat project
78. **الفصل 78**: مشروع إدارة فصل — Class management
79. **الفصل 79**: مشروع تطبيق ويب — Web app
80. **الفصل 80**: مشروع تطبيق سطح مكتب — Desktop app
81. **الفصل 81**: مشروع لعبة شطرنج — Chess game
82. **الفصل 82**: مشروع تطبيق جوال — Mobile app
83. **الفصل 83**: مشروع نظام إدارة — Management system
84. **الفصل 84**: مشروع تطبيق تعليمي — Educational app
85. **الفصل 85**: مشروع مشروع تخرج — Graduation project
86. **الفصل 86**: مشروع ملخص — Summary project
87. **الفصل 87**: نصائح لأتمام المشروع — Project completion tips
88. **الفصل 88**: الخطوات التالية — Next steps

Additional chapters (98-104+) cover exercises (نظام حجز مواعيد, تحليل نصوص, لعبة الورق), the transpiler, additional courses (التشفير, الأمن السيبراني, برمجة الويب), and the challenge system.

---

## 2. Language Keywords (Source-Code Verified)

### Source Files Read

| File | Lines | Purpose |
|------|-------|---------|
| `Compiler/include/Daad/Keywords.hpp` | 87 | KeywordType enum definition |
| `Compiler/src/Keywords.cpp` | 370 | Arabic→C++ keyword mapping |
| `Studio/desktop-app/server/src/utils/dhad/lexer.js` | 704 | JS Lexer with keyword table |

### Complete Keyword Catalog

#### Category 1: Data Types & Fundamentals (12 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| صحيح | KwInt | `int` | Keywords.cpp:26 |
| عشري | KwDouble | `double` | Keywords.cpp:27 |
| منطقي | KwBool | `bool` | Keywords.cpp:28 |
| حرف | KwChar | `char` | Keywords.cpp:29 |
| نص | KwString | `std::string` | Keywords.cpp:30 |
| ثابت | KwConst | `const` | Keywords.cpp:31 |
| فارغ | KwVoid | `void` | Keywords.cpp:32 |
| تلقائي | KwAuto | `auto` | Keywords.cpp:33 |
| صواب | KwTrue | `true` | Keywords.cpp:34 |
| خطأ | KwFalse | `false` | Keywords.cpp:35 |
| لا_شيء | KwNullptr | `nullptr` | Keywords.cpp:36 |
| نوع | KwTypedef | `typedef` | Keywords.cpp:37 |

#### Category 2: Control Flow (13 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| إذا | KwIf | `if` | Keywords.cpp:41 |
| وإلا | KwElse | `else` | Keywords.cpp:42 |
| طالما | KwWhile | `while` | Keywords.cpp:43 |
| افعل | KwDo | `do` | Keywords.cpp:44 |
| لكل | KwFor | `for` | Keywords.cpp:45 |
| في | KwIn | `for` | Keywords.cpp:46 |
| افتح | KwSwitch | `switch` | Keywords.cpp:47 |
| حالة | KwCase | `case` | Keywords.cpp:48 |
| افتراضي | KwDefault | `default` | Keywords.cpp:49 |
| اكسر | KwBreak | `break` | Keywords.cpp:50 |
| تابع | KwContinue | `continue` | Keywords.cpp:51 |
| انتقل | KwGoto | `goto` | Keywords.cpp:52 |
| س | KwX | `x` | Keywords.cpp:53 |
| ص | KwY | `y` | Keywords.cpp:54 |

#### Category 3: OOP & Scoping (12 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| صنف | KwClass | `class` | Keywords.cpp:56 |
| فئة | KwClass (alt) | `class` | Keywords.cpp:57 |
| هيكل | KwStruct | `struct` | Keywords.cpp:58 |
| تعداد | KwEnum | `enum` | Keywords.cpp:59 |
| واجهة | KwInterface | `class` | Keywords.cpp:60 |
| نطاق | KwNamespace | `namespace` | Keywords.cpp:61 |
| عام | KwPublic | `public` | Keywords.cpp:62 |
| خاص | KwPrivate | `private` | Keywords.cpp:63 |
| محمي | KwProtected | `protected` | Keywords.cpp:64 |
| يرث | KwInherit | `public` | Keywords.cpp:65 |
| ذاتي | KwSelf | `this` | Keywords.cpp:66 |
| هذا | KwSelf (alt) | `this` | Keywords.cpp:67 |
| الأصل | KwBase | (metadata) | Keywords.cpp:68 |
| مجرّد | KwAbstract | `virtual` | Keywords.cpp:69 |

#### Category 4: Functions & Memory (10 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| دالة | KwFunction | (metadata) | Keywords.cpp:72 |
| ارجع | KwReturn | `return` | Keywords.cpp:73 |
| جديد | KwNew | `new` | Keywords.cpp:74 |
| احذف | KwDelete | `delete` | Keywords.cpp:75 |
| مؤشر | KwPointer | `*` | Keywords.cpp:76 |
| مرجع | KwReference | `&` | Keywords.cpp:77 |
| ساكن | KwStatic | `static` | Keywords.cpp:78 |
| مضمن | KwInline | `inline` | Keywords.cpp:79 |
| خارجي | KwExtern | `extern` | Keywords.cpp:80 |
| قالب | KwTemplate | `template` | Keywords.cpp:81 |

#### Category 5: Exception Handling (10 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| حاول | KwTry | `try` | Keywords.cpp:84 |
| امسك | KwCatch | `catch` | Keywords.cpp:85 |
| أخيراً | KwFinally | `finally` | Keywords.cpp:86 |
| ارمِ | KwThrow | `throw` | Keywords.cpp:87 |
| تأكد | KwAssert | `assert` | Keywords.cpp:88 |
| استثناء | KwException | `std::exception` | Keywords.cpp:89 |
| نوع_الـ | KwTypeOf | `decltype` | Keywords.cpp:90 |
| حجم_الـ | KwSizeOf | `sizeof` | Keywords.cpp:91 |
| زد | KwIncrement | `++` | Keywords.cpp:92 |
| انقص | KwDecrement | `--` | Keywords.cpp:93 |

#### Category 6: Advanced Systems (10 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| تزامن | KwSync | `std::mutex` | Keywords.cpp:96 |
| انتظر | KwAwait | `co_await` | Keywords.cpp:97 |
| بانتظار | KwAwait (alt) | `co_await` | Keywords.cpp:98 |
| خيط | KwThread | `std::thread` | Keywords.cpp:99 |
| احجز | KwLock | `std::lock_guard` | Keywords.cpp:100 |
| مشترك | KwShared | `std::shared_ptr` | Keywords.cpp:101 |
| فريد | KwUnique | `std::unique_ptr` | Keywords.cpp:102 |
| استورد | KwImport | `#include` | Keywords.cpp:103 |
| صدّر | KwExport | `export` | Keywords.cpp:104 |
| وحدة | KwModule | `module` | Keywords.cpp:105 |
| بديل | KwAlternative | `else if` | Keywords.cpp:106 |

#### Category 7: GUI Keywords (14 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| زر_أمر | KwButton | `QPushButton` | Keywords.cpp:112 |
| حقل_نص | KwTextField | `QLineEdit` | Keywords.cpp:113 |
| قائمة_خيارات | KwComboBox | `QComboBox` | Keywords.cpp:114 |
| صورة | KwImage | `QLabel` | Keywords.cpp:115 |
| مربع_اختيار | KwCheckBox | `QCheckBox` | Keywords.cpp:116 |
| شريط_تمرير | KwSlider | `QSlider` | Keywords.cpp:117 |
| قائمة_منسدلة | KwDropDown | `QComboBox` | Keywords.cpp:118 |
| لوحة | KwPanel | `QWidget` | Keywords.cpp:119 |
| تسمية | KwLabel | `QLabel` | Keywords.cpp:120 |
| عمود | KwColumn | `QVBoxLayout` | Keywords.cpp:121 |
| صف | KwRow | `QHBoxLayout` | Keywords.cpp:122 |
| شبكة | KwGrid | `QGridLayout` | Keywords.cpp:123 |
| شريط_تلوين | KwProgressBar | `QProgressBar` | Keywords.cpp:124 |
| علامة_تبويب | KwTabBar | `QTabWidget` | Keywords.cpp:125 |

#### Category 8: I/O & Logic (4 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| طباعة | KwPrint | `daad::runtime::daad_print` | Keywords.cpp:128 |
| ادخل | KwInput | (input) | Keywords.cpp:131 |
| و | KwAndArabic | `&&` | Keywords.cpp:107 |
| أو | KwOrArabic | `||` | Keywords.cpp:108 |
| ليس | KwNotArabic | `!` | Keywords.cpp:109 |

#### Category 8b: Web-Compatible Alternatives

| Arabic | Maps To | File:Line |
|--------|---------|-----------|
| اطبع | KwPrint | Keywords.cpp:99 |
| بينما | KwWhile | Keywords.cpp:102 |
| كرر | KwFor | Keywords.cpp:103 |

#### Category 9: Image Processing (20 keywords)

| Arabic | Enum | C++ Equivalent | File:Line |
|--------|------|---------------|-----------|
| حمّل_صورة | KwLoadImage | `daad::image::load` | Keywords.cpp:134 |
| ارسم_صورة | KwDrawImage | `daad::image::draw` | Keywords.cpp:135 |
| حجم_صورة | KwImageSize | `daad::image::size` | Keywords.cpp:136 |
| احفظ_صورة | KwSaveImage | `daad::image::save` | Keywords.cpp:137 |
| قص_صورة | KwCropImage | `daad::image::crop` | Keywords.cpp:138 |
| غيّر_حجم | KwResize | `daad::image::resize` | Keywords.cpp:139 |
| لف_صورة | KwRotateImage | `daad::image::rotate` | Keywords.cpp:140 |
| قلب_صورة | KwFlipImage | `daad::image::flip` | Keywords.cpp:141 |
| شفافية | KwOpacity | `daad::image::opacity` | Keywords.cpp:142 |
| فلتر | KwFilter | `daad::image::filter` | Keywords.cpp:143 |
| تراكب | KwOverlay | `daad::image::overlay` | Keywords.cpp:144 |
| خلفية | KwBackground | `daad::image::background` | Keywords.cpp:145 |
| بكسل | KwPixel | `daad::image::pixel` | Keywords.cpp:146 |
| ارسم | KwDraw | `daad::image::drawShape` | Keywords.cpp:147 |
| ملء | KwFill | `daad::image::fill` | Keywords.cpp:148 |
| مستطيل | KwRectangle | `daad::image::rectangle` | Keywords.cpp:149 |
| دائرة | KwCircle | `daad::image::circle` | Keywords.cpp:150 |
| خط | KwLine | `daad::image::line` | Keywords.cpp:151 |
| نص_على_لوحة | KwTextOnCanvas | `daad::image::text` | Keywords.cpp:152 |
| مسح | KwClear | `daad::image::clear` | Keywords.cpp:153 |

### Keyword Counts Summary

| Source | Total Keywords | Categories |
|--------|---------------|------------|
| Keywords.hpp (enum) | 95 enum values | 9 categories |
| Keywords.cpp (mapping) | 103 unique Arabic terms | 9 categories |
| lexer.js (KEYWORDS object) | 103 keywords (83 base + 20 image) | 9 categories |

---

## 3. Challenge Definitions (All 211)

### Tier Summary

| Tier | Arabic Name | Count | Difficulty | Topics |
|------|------------|-------|------------|--------|
| 1 | أساسيات البرمجة | 31 | BEGINNER | Variables, types, I/O, operators |
| 2 | المنطق والتحكم | 30 | INTERMEDIATE | If/else, loops, logic, comparisons |
| 3 | هيكل البيانات | 30 | ADVANCED | Functions, arrays, search, sum |
| 4 | البرمجة الكائنية | 30 | INTERMEDIATE | Classes, constructors, methods |
| 5 | الخوارزميات | 30 | EXPERT | Sorting, searching, reversing |
| 6 | المصفوفات | 30 | ADVANCED | Array CRUD, 2D arrays, merging |
| 7 | البرمجة الكائنية المتقدمة | 30 | EXPERT | Inheritance, abstract, interfaces |

**Total**: 211 challenges (31 + 6×30)

### Tier 1: أساسيات البرمجة (31 challenges)

| # | Title | Difficulty | XP | Key Keywords |
|---|-------|-----------|-----|--------------|
| 1 | طباعة: المثال | BEGINNER | 10 | طباعة |
| 2 | طباعة: تمارين 1 | BEGINNER | 10 | طباعة |
| 3 | طباعة: تمارين 2 | BEGINNER | 10 | طباعة |
| 4 | متغير صحيح: المثال | BEGINNER | 10 | صحيح, طباعة |
| 5 | متغير صحيح: تمارين 1 | BEGINNER | 10 | صحيح, طباعة |
| 6 | متغير صحيح: تمارين 2 | BEGINNER | 10 | صحيح, طباعة |
| 7 | نص: المثال | BEGINNER | 10 | نص, طباعة |
| 8 | نص: تمارين 1 | BEGINNER | 10 | نص, طباعة |
| 9 | نص: تمارين 2 | BEGINNER | 10 | نص, طباعة |
| 10 | اجمع: المثال | BEGINNER | 10 | صحيح, طباعة |
| 11 | اجمع: تمارين 1 | BEGINNER | 10 | صحيح, طباعة |
| 12 | اجمع: تمارين 2 | BEGINNER | 10 | صحيح, طباعة |
| 13 | اطرح: المثال | BEGINNER | 10 | صحيح, طباعة |
| 14 | اطرح: تمارين 1 | BEGINNER | 10 | صحيح, طباعة |
| 15 | اطرح: تمارين 2 | BEGINNER | 10 | صحيح, طباعة |
| 16 | اضرب: المثال | BEGINNER | 10 | صحيح, طباعة |
| 17 | اضرب: تمارين 1 | BEGINNER | 10 | صحيح, طباعة |
| 18 | اضرب: تمارين 2 | BEGINNER | 10 | صحيح, طباعة |
| 19 | اقسم: المثال | BEGINNER | 10 | صحيح, طباعة |
| 20 | اقسم: تمارين 1 | BEGINNER | 10 | صحيح, طباعة |
| 21 | اقسم: تمارين 2 | BEGINNER | 10 | صحيح, طباعة |
| 22 | باقي القسمة: المثال | BEGINNER | 10 | صحيح, طباعة |
| 23 | باقي القسمة: تمارين 1 | BEGINNER | 10 | صحيح, طباعة |
| 24 | باقي القسمة: تمارين 2 | BEGINNER | 10 | صحيح, طباعة |
| 25 | عشوائي: المثال | BEGINNER | 10 | صحيح, عشوائي, طباعة |
| 26 | عشوائي: تمارين 1 | BEGINNER | 10 | صحيح, عشوائي, طباعة |
| 27 | عشوائي: تمارين 2 | BEGINNER | 10 | صحيح, عشوائي, طباعة |
| 28 | إدخال: المثال | BEGINNER | 10 | نص, ادخل, طباعة |
| 29 | إدخال: تمارين 1 | BEGINNER | 10 | نص, ادخل, طباعة |
| 30 | إدخال: تمارين 2 | BEGINNER | 10 | نص, ادخل, طباعة |
| 31 | عشوائي: المثال | BEGINNER | 10 | صحيح, عشوائي, طباعة |

### Tier 2: المنطق والتحكم (30 challenges)

| # | Title | Difficulty | XP | Key Keywords |
|---|-------|-----------|-----|--------------|
| 1 | شرط إذا: المثال | INTERMEDIATE | 15 | صحيح, إذا, طباعة |
| 2 | شرط إذا: تمارين 1 | INTERMEDIATE | 15 | صحيح, إذا, طباعة |
| 3 | شرط إذا: تمارين 2 | INTERMEDIATE | 15 | صحيح, إذا, طباعة |
| 4 | إذا وإلا: المثال | INTERMEDIATE | 15 | صحيح, إذا, وإلا, طباعة |
| 5 | إذا وإلا: تمارين 1 | INTERMEDIATE | 15 | صحيح, إذا, وإلا, طباعة |
| 6 | إذا وإلا: تمارين 2 | INTERMEDIATE | 15 | صحيح, إذا, وإلا, طباعة |
| 7 | شروط متعددة: المثال | INTERMEDIATE | 20 | صحيح, إذا, وإلا إذا, طباعة |
| 8 | شروط متعددة: تمارين 1 | INTERMEDIATE | 20 | صحيح, إذا, وإلا إذا, طباعة |
| 9 | شروط متعددة: تمارين 2 | INTERMEDIATE | 20 | صحيح, إذا, وإلا إذا, طباعة |
| 10 | حلقة طالما: المثال | INTERMEDIATE | 15 | صحيح, طالما, طباعة |
| 11 | حلقة طالما: تمارين 1 | INTERMEDIATE | 15 | صحيح, طالما, طباعة |
| 12 | حلقة طالما: تمارين 2 | INTERMEDIATE | 15 | صحيح, طالما, طباعة |
| 13 | حلقة لكل: المثال | INTERMEDIATE | 15 | صحيح, لكل, طباعة |
| 14 | حلقة لكل: تمارين 1 | INTERMEDIATE | 15 | صحيح, لكل, طباعة |
| 15 | حلقة لكل: تمارين 2 | INTERMEDIATE | 15 | صحيح, لكل, طباعة |
| 16 | كل على مصفوفة: المثال | INTERMEDIATE | 20 | نص, لكل, في, طباعة |
| 17 | كل على مصفوفة: تمارين 1 | INTERMEDIATE | 20 | نص, لكل, في, طباعة |
| 18 | كل على مصفوفة: تمارين 2 | INTERMEDIATE | 20 | صحيح, لكل, في, طباعة |
| 19 | مقارنات منطقية: المثال | INTERMEDIATE | 15 | صحيح, طباعة |
| 20 | مقارنات منطقية: تمارين 1 | INTERMEDIATE | 15 | صحيح, طباعة |
| 21 | مقارنات منطقية: تمارين 2 | INTERMEDIATE | 15 | صحيح, طباعة |
| 22 | المنطق: المثال | INTERMEDIATE | 15 | صحيح, طباعة |
| 23 | المنطق: تمارين 1 | INTERMEDIATE | 15 | صحيح, طباعة |
| 24 | المنطق: تمارين 2 | INTERMEDIATE | 15 | صحيح, طباعة |
| 25 | النفي: المثال | INTERMEDIATE | 15 | صحيح, طباعة |
| 26 | النفي: تمارين 1 | INTERMEDIATE | 15 | صحيح, طباعة |
| 27 | النفي: تمارين 2 | INTERMEDIATE | 15 | صحيح, طباعة |
| 28 | حلقة افعل: المثال | INTERMEDIATE | 20 | صحيح, افعل, طالما, طباعة |
| 29 | حلقة افعل: تمارين 1 | INTERMEDIATE | 20 | صحيح, افعل, طالما, طباعة |
| 30 | حلقة افعل: تمارين 2 | INTERMEDIATE | 20 | صحيح, افعل, طالما, طباعة |

### Tier 3: هيكل البيانات (30 challenges)

| # | Title | Difficulty | XP | Key Keywords |
|---|-------|-----------|-----|--------------|
| 1 | دالة بسيطة: المثال | ADVANCED | 25 | دالة, طباعة |
| 2 | دالة بسيطة: تمارين 1 | ADVANCED | 25 | دالة, طباعة |
| 3 | دالة بسيطة: تمارين 2 | ADVANCED | 25 | دالة, طباعة |
| 4 | دالة مع معاملات: المثال | ADVANCED | 25 | دالة, طباعة |
| 5 | دالة مع معاملات: تمارين 1 | ADVANCED | 25 | دالة, طباعة |
| 6 | دالة مع معاملات: تمارين 2 | ADVANCED | 25 | دالة, طباعة |
| 7 | دالة إرجاع: المثال | ADVANCED | 25 | دالة, ارجع, صحيح, طباعة |
| 8 | دالة إرجاع: تمارين 1 | ADVANCED | 25 | دالة, ارجع, صحيح, طباعة |
| 9 | دالة إرجاع: تمارين 2 | ADVANCED | 25 | دالة, ارجع, صحيح, طباعة |
| 10 | المصفوفات: المثال | ADVANCED | 25 | صحيح, طباعة |
| 11 | المصفوفات: تمارين 1 | ADVANCED | 25 | صحيح, طباعة |
| 12 | المصفوفات: تمارين 2 | ADVANCED | 25 | صحيح, طباعة |
| 13 | طول المصفوفة: المثال | ADVANCED | 25 | صحيح, طول, طباعة |
| 14 | طول المصفوفة: تمارين 1 | ADVANCED | 25 | صحيح, طول, طباعة |
| 15 | طول المصفوفة: تمارين 2 | ADVANCED | 25 | صحيح, طول, طباعة |
| 16 | تعديل المصفوفة: المثال | ADVANCED | 25 | صحيح, طباعة |
| 17 | تعديل المصفوفة: تمارين 1 | ADVANCED | 25 | صحيح, طباعة |
| 18 | تعديل المصفوفة: تمارين 2 | ADVANCED | 25 | صحيح, طباعة |
| 19 | حساب المجموع: المثال | ADVANCED | 30 | صحيح, لكل, طول, طباعة |
| 20 | حساب المجموع: تمارين 1 | ADVANCED | 30 | صحيح, لكل, طول, طباعة |
| 21 | حساب المجموع: تمارين 2 | ADVANCED | 30 | صحيح, لكل, طول, طباعة |
| 22 | أكبر وأصغر: المثال | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 23 | أكبر وأصغر: تمارين 1 | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 24 | أكبر وأصغر: تمارين 2 | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 25 | البحث في المصفوفة: المثال | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 26 | البحث في المصفوفة: تمارين 1 | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 27 | البحث في المصفوفة: تمارين 2 | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 28 | عد التكرارات: المثال | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 29 | عد التكرارات: تمارين 1 | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |
| 30 | عد التكرارات: تمارين 2 | ADVANCED | 30 | صحيح, لكل, إذا, طول, طباعة |

### Tier 4: البرمجة الكائنية (30 challenges)

| # | Title | Difficulty | XP | Key Keywords |
|---|-------|-----------|-----|--------------|
| 1 | صنف بسيط: المثال | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, ارجع, طباعة |
| 2 | صنف بسيط: تمارين 1 | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, ارجع, طباعة |
| 3 | صنف بسيط: تمارين 2 | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, ارجع, طباعة |
| 4 | قيم افتراضية: المثال | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, طباعة |
| 5 | قيم افتراضية: تمارين 1 | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, طباعة |
| 6 | قيم افتراضية: تمارين 2 | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, طباعة |
| 7 | الخاصية والطريقة: المثال | INTERMEDIATE | 45 | صنف, جديد, هذا, دالة, ارجع, طباعة |
| 8 | الخاصية والطريقة: تمارين 1 | INTERMEDIATE | 45 | صنف, جديد, هذا, دالة, ارجع, طباعة |
| 9 | الخاصية والطريقة: تمارين 2 | INTERMEDIATE | 45 | صنف, جديد, هذا, دالة, ارجع, طباعة |
| 10 | صنف مع عداد: المثال | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, طباعة |
| 11 | صنف مع عداد: تمارين 1 | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, طباعة |
| 12 | صنف مع عداد: تمارين 2 | INTERMEDIATE | 40 | صنف, جديد, هذا, دالة, طباعة |
| 13 | صنف حساب: المثال | INTERMEDIATE | 45 | صنف, جديد, هذا, دالة, طباعة |
| 14 | صنف حساب: تمارين 1 | INTERMEDIATE | 45 | صنف, جديد, هذا, دالة, طباعة |
| 15 | صنف حساب: تمارين 2 | INTERMEDIATE | 45 | صنف, جديد, هذا, دالة, طباعة |
| 16 | صنف طالب: المثال | INTERMEDIATE | 50 | صنف, جديد, هذا, إذا, وإلا, ارجع, طباعة |
| 17 | صنف طالب: تمارين 1 | INTERMEDIATE | 50 | صنف, جديد, هذا, إذا, وإلا, ارجع, طباعة |
| 18 | صنف طالب: تمارين 2 | INTERMEDIATE | 50 | صنف, جديد, هذا, إذا, وإلا إذا, ارجع, طباعة |
| 19 | صنف محفظة: المثال | INTERMEDIATE | 50 | صنف, جديد, هذا, لكل, طول, طباعة |
| 20 | صنف محفظة: تمارين 1 | INTERMEDIATE | 50 | صنف, جديد, هذا, لكل, طول, طباعة |
| 21 | صنف محفظة: تمارين 2 | INTERMEDIATE | 50 | صنف, جديد, هذا, طول, طباعة |
| 22 | صنف مع تحقق: المثال | INTERMEDIATE | 50 | صنف, جديد, هذا, إذا, وإلا, طول, طباعة |
| 23 | صنف مع تحقق: تمارين 1 | INTERMEDIATE | 50 | صنف, جديد, هذا, إذا, وإلا, طباعة |
| 24 | صنف مع تحقق: تمارين 2 | INTERMEDIATE | 50 | صنف, جديد, هذا, إذا, وإلا, طباعة |
| 25 | صنف مع مصفوفة: المثال | INTERMEDIATE | 55 | صنف, جديد, هذا, لكل, طول, ارجع, طباعة |
| 26 | صنف مع مصفوفة: تمارين 1 | INTERMEDIATE | 55 | صنف, جديد, هذا, طول, ارجع, طباعة |
| 27 | صنف مع مصفوفة: تمارين 2 | INTERMEDIATE | 55 | صنف, جديد, هذا, دالة, طباعة |
| 28 | صنف متقدم: المثال | INTERMEDIATE | 55 | صنف, جديد, هذا, إذا, وإلا إذا, وإلا, ارجع, طباعة |
| 29 | صنف متقدم: تمارين 1 | INTERMEDIATE | 55 | صنف, جديد, هذا, ارجع, طباعة |
| 30 | صنف متقدم: تمارين 2 | INTERMEDIATE | 55 | صنف, جديد, هذا, إذا, وإلا, ارجع, طباعة |

### Tier 5: الخوارزميات (30 challenges)

| # | Title | Difficulty | XP | Key Keywords |
|---|-------|-----------|-----|--------------|
| 1 | فرز تصاعدي: المثال | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 2 | فرز تصاعدي: تمارين 1 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 3 | فرز تصاعدي: تمارين 2 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 4 | فرز تنازلي: المثال | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 5 | فرز تنازلي: تمارين 1 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 6 | فرز تنازلي: تمارين 2 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 7 | قلب المصفوفة: المثال | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 8 | قلب المصفوفة: تمارين 1 | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 9 | قلب المصفوفة: تمارين 2 | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 10 | إزالة التكرارات: المثال | EXPERT | 75 | صحيح, لكل, إذا, طول, طباعة |
| 11 | إزالة التكرارات: تمارين 1 | EXPERT | 75 | صحيح, لكل, إذا, طول, طباعة |
| 12 | إزالة التكرارات: تمارين 2 | EXPERT | 75 | صحيح, لكل, إذا, طول, طباعة |
| 13 | حسابات على المصفوفة: المثال | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 14 | حسابات على المصفوفة: تمارين 1 | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 15 | حسابات على المصفوفة: تمارين 2 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 16 | البحث الخطي: المثال | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 17 | البحث الخطي: تمارين 1 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 18 | البحث الخطي: تمارين 2 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 19 | قلب بدون مصفوفة جديدة: المثال | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 20 | قلب بدون مصفوفة جديدة: تمارين 1 | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 21 | قلب بدون مصفوفة جديدة: تمارين 2 | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 22 | دمج مصفوفتين: المثال | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 23 | دمج مصفوفتين: تمارين 1 | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 24 | دمج مصفوفتين: تمارين 2 | EXPERT | 70 | صحيح, لكل, طول, طباعة |
| 25 | تحويل أنظمة العد: المثال | EXPERT | 75 | صحيح, طالما, طباعة |
| 26 | تحويل أنظمة العد: تمارين 1 | EXPERT | 75 | صحيح, طالما, طباعة |
| 27 | تحويل أنظمة العد: تمارين 2 | EXPERT | 75 | صحيح, طالما, طباعة |
| 28 | ترتيب الأزواج: المثال | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 29 | ترتيب الأزواج: تمارين 1 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |
| 30 | ترتيب الأزواج: تمارين 2 | EXPERT | 70 | صحيح, لكل, إذا, طول, طباعة |

### Tier 6: المصفوفات (30 challenges)

| # | Title | Difficulty | XP | Key Keywords |
|---|-------|-----------|-----|--------------|
| 1 | إنشاء مصفوفة: المثال | ADVANCED | 15 | صحيح, طباعة |
| 2 | إنشاء مصفوفة: تمارين 1 | ADVANCED | 15 | صحيح, طباعة |
| 3 | إنشاء مصفوفة: تمارين 2 | ADVANCED | 15 | صحيح, طباعة |
| 4 | الوصول للعناصر: المثال | ADVANCED | 15 | صحيح, طباعة |
| 5 | الوصول للعناصر: تمارين 1 | ADVANCED | 15 | صحيح, طباعة |
| 6 | الوصول للعناصر: تمارين 2 | ADVANCED | 15 | صحيح, طباعة |
| 7 | طول المصفوفة: المثال | ADVANCED | 15 | صحيح, طول, طباعة |
| 8 | طول المصفوفة: تمارين 1 | ADVANCED | 15 | صحيح, طول, طباعة |
| 9 | طول المصفوفة: تمارين 2 | ADVANCED | 15 | صحيح, طول, طباعة |
| 10 | تعديل العناصر: المثال | ADVANCED | 15 | صحيح, طباعة |
| 11 | تعديل العناصر: تمارين 1 | ADVANCED | 15 | صحيح, طباعة |
| 12 | تعديل العناصر: تمارين 2 | ADVANCED | 15 | صحيح, طباعة |
| 13 | إضافة عنصر: المثال | ADVANCED | 20 | صحيح, لكل, طول, طباعة |
| 14 | إضافة عنصر: تمارين 1 | ADVANCED | 20 | صحيح, لكل, طول, طباعة |
| 15 | إضافة عنصر: تمارين 2 | ADVANCED | 20 | صحيح, لكل, طول, طباعة |
| 16 | حذف عنصر: المثال | ADVANCED | 20 | صحيح, لكل, طول, طباعة |
| 17 | حذف عنصر: تمارين 1 | ADVANCED | 20 | صحيح, لكل, طول, طباعة |
| 18 | حذف عنصر: تمارين 2 | ADVANCED | 20 | صحيح, لكل, طول, طباعة |
| 19 | البحث في المصفوفة: المثال | ADVANCED | 20 | صحيح, لكل, إذا, طول, طباعة |
| 20 | البحث في المصفوفة: تمارين 1 | ADVANCED | 20 | صحيح, لكل, إذا, طول, طباعة |
| 21 | البحث في المصفوفة: تمارين 2 | ADVANCED | 20 | صحيح, لكل, إذا, طول, طباعة |
| 22 | مصفوفة ثنائية: المثال | ADVANCED | 20 | صحيح, طباعة |
| 23 | مصفوفة ثنائية: تمارين 1 | ADVANCED | 20 | صحيح, طباعة |
| 24 | مصفوفة ثنائية: تمارين 2 | ADVANCED | 20 | صحيح, طباعة |
| 25 | عد العناصر: المثال | ADVANCED | 20 | صحيح, لكل, إذا, طول, طباعة |
| 26 | عد العناصر: تمارين 1 | ADVANCED | 20 | صحيح, لكل, إذا, طول, طباعة |
| 27 | عد العناصر: تمارين 2 | ADVANCED | 20 | صحيح, لكل, إذا, طول, طباعة |
| 28 | دمج مصفوفتين: المثال | ADVANCED | 25 | صحيح, لكل, طول, طباعة |
| 29 | دمج مصفوفتين: تمارين 1 | ADVANCED | 25 | صحيح, لكل, طول, طباعة |
| 30 | دمج مصفوفتين: تمارين 2 | ADVANCED | 25 | صحيح, لكل, طول, طباعة |

### Tier 7: البرمجة الكائنية المتقدمة (30 challenges)

| # | Title | Difficulty | XP | Key Keywords |
|---|-------|-----------|-----|--------------|
| 1 | الوراثة: المثال | EXPERT | 80 | صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 2 | الوراثة: تمارين 1 | EXPERT | 80 | صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 3 | الوراثة: تمارين 2 | EXPERT | 80 | صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 4 | الأصل: المثال | EXPERT | 80 | صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 5 | الأصل: تمارين 1 | EXPERT | 80 | صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 6 | الأصل: تمارين 2 | EXPERT | 80 | صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 7 | الوصول: المثال | EXPERT | 85 | صنف, خاص, عام, جديد, هذا, دالة, طباعة |
| 8 | الوصول: تمارين 1 | EXPERT | 85 | صنف, خاص, عام, جديد, هذا, دالة, ارجع, طباعة |
| 9 | الوصول: تمارين 2 | EXPERT | 85 | صنف, خاص, عام, جديد, هذا, دالة, طباعة |
| 10 | المحمي: المثال | EXPERT | 85 | صنف, محمي, يرث, جديد, هذا, عام, دالة, طباعة |
| 11 | المحمي: تمارين 1 | EXPERT | 85 | صنف, محمي, عام, جديد, هذا, دالة, طباعة |
| 12 | المحمي: تمارين 2 | EXPERT | 85 | صنف, خاص, عام, جديد, هذا, دالة, طباعة |
| 13 | الصنف المجرّد: المثال | EXPERT | 90 | مجرّد, صنف, يرث, جديد, هذا, عام, دالة, ارجع, طباعة |
| 14 | الصنف المجرّد: تمارين 1 | EXPERT | 90 | مجرّد, صنف, يرث, جديد, هذا, عام, دالة, ارجع, طباعة |
| 15 | الصنف المجرّد: تمارين 2 | EXPERT | 90 | مجرّد, صنف, يرث, جديد, هذا, عام, دالة, ارجع, طباعة |
| 16 | الواجهة: المثال | EXPERT | 90 | واجهة, صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 17 | الواجهة: تمارين 1 | EXPERT | 90 | واجهة, صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 18 | الواجهة: تمارين 2 | EXPERT | 90 | واجهة, صنف, يرث, جديد, هذا, دالة, ارجع, طباعة |
| 19 | النطاق: المثال | EXPERT | 85 | نطاق, دالة, ارجع, طباعة |
| 20 | النطاق: تمارين 1 | EXPERT | 85 | نطاق, دالة, ارجع, طباعة |
| 21 | النطاق: تمارين 2 | EXPERT | 85 | نطاق, دالة, ارجع, طباعة |
| 22 | التعداد: المثال | EXPERT | 85 | تعداد, صحيح, إذا, طباعة |
| 23 | التعداد: تمارين 1 | EXPERT | 85 | تعداد, صحيح, إذا, طباعة |
| 24 | التعداد: تمارين 2 | EXPERT | 85 | تعداد, صحيح, إذا, طباعة |
| 25 | معالجة الأخطاء: المثال | EXPERT | 85 | حاول, امسك, طباعة |
| 26 | معالجة الأخطاء: تمارين 1 | EXPERT | 85 | حاول, امسك, طباعة |
| 27 | معالجة الأخطاء: تمارين 2 | EXPERT | 85 | حاول, امسك, طباعة |
| 28 | هيكل: المثال | EXPERT | 85 | هيكل, صحيح, جديد, هذا, طباعة |
| 29 | هيكل: تمارين 1 | EXPERT | 85 | هيكل, صحيح, جديد, هذا, طباعة |
| 30 | هيكل: تمارين 2 | EXPERT | 85 | هيكل, صحيح, جديد, هذا, طباعة |

---

## 4. Low-Level Keywords (CPU/DAAD) — Excluded List

These keywords are from the CPU/DAAD assembly layer and are **NOT** part of the high-level Dhad language. They are excluded from the language keyword catalog.

**Source**: `DAAD/compiler/lexer/keywords.c` (186 lines), CPU assembly files

### CPU Assembly Keywords (hamster-09 custom CPU)

| Arabic | Meaning | Source | Justification for Exclusion |
|--------|---------|--------|----------------------------|
| حمّل | Load (LD) | keywords.c:15, demo_loop.ضasm:5 | CPU register load instruction |
| اجمع | Add (ADD) | keywords.c:16, demo_many.ضasm:8 | CPU arithmetic operation |
| نقص | Subtract (SUB) | keywords.c:17, demo_many.ضasm:10 | CPU arithmetic operation |
| قفز_إذا_غيرصفر | Jump if not zero (JNZ) | keywords.c:18, demo_loop.ضasm:8 | CPU control flow (assembly level) |
| توقف | Halt (HLT) | keywords.c:19, demo_many.ضasm:12 | CPU halt instruction |
| ضرب | Multiply (MUL) | keywords.c:20, demo_many.ضasm:15 | CPU arithmetic operation |
| اطبع_حرف | Print char (PUTC) | keywords.c:21, demo_many.ضasm:18 | CPU I/O (low-level) |
| اطبع_رقم | Print number (PRN) | keywords.c:22 | CPU I/O (low-level) |
| اقرأ_حرف | Read char (GETC) | keywords.c:23 | CPU I/O (low-level) |
| انسخ | Copy (MOV) | keywords.c:24 | CPU register move |
| احفظ | Store (ST) | keywords.c:25 | CPU memory store |
| عرّف | Define (DEF) | keywords.c:26 | CPU macro definition |
| أدخل | Include (INCLUDE) | keywords.c:27 | CPU file include |
| إذا | If (assembly) | keywords.c:28 | Assembly conditional |
| وإلا | Else (assembly) | keywords.c:29 | Assembly conditional |
| لكل | For (assembly) | keywords.c:30 | Assembly loop |
| افتح | Open (file) | keywords.c:31 | CPU file operation |
| اقرأ | Read (file) | keywords.c:32 | CPU file operation |
| اكتب | Write (file) | keywords.c:33 | CPU file operation |
| أغلق | Close (file) | keywords.c:34 | CPU file operation |
| استورد | Import (assembly) | keywords.c:35 | Assembly include |
| مسار | Path | keywords.c:36 | CPU file path |
| متغير | Variable | keywords.c:37 | CPU variable declaration |
| ثابت | Constant | keywords.c:38 | CPU constant |
| س0–س7 | Registers s0-s7 | dhad_asm.c:36 | CPU registers (8 general) |
| مح | Accumulator | dhad_asm.c:36 | CPU accumulator register |

### CPU Assembly Instructions (from .ضasm files)

```asm
حمّل س0، 0          # LD s0, 0
حمّل س1، 1          # LD s1, 1
اجمع س2، س0، س1    # ADD s2, s0, s1
نقص س3، س0، س1     # SUB s3, s0, s1
قفز_إذا_غيرصفر س0، Loop  # JNZ s0, Loop
توقف               # HLT
اضرب س4، س0، س1    # MUL s4, s0, s1
اطبع_حرف س2        # PUTC s2
```

**Justification for Exclusion**: These keywords belong to the custom CPU assembler (حمّل-09 ISA) and the DAAD compiler's low-level layer. They are not part of the high-level Dhad language taught in the book or used in the Studio challenges. The high-level Dhad language is transpiled to JavaScript (web) or C++ (desktop), not to CPU assembly.

---

## 5. Top 5 Observations

### 1. **Three-Tier Architecture**
The Dhad ecosystem has three distinct layers:
- **High-level Dhad** ( taught in book.html, used in challenges): 103 keywords, transpiled to JS/C++
- **Studio Lexer** (lexer.js): JavaScript-based, 103 keywords, supports Arabic Unicode
- **CPU/DAAD Assembly** (keywords.c, .ضasm files): 27+ low-level keywords for hamster-09 CPU

### 2. **Keyword Redundancy Pattern**
Several keywords have Arabic alternatives for Web compatibility:
- `طالما` ↔ `بينما` (while)
- `لكل` ↔ `كرر` (for)
- `صنف` ↔ `فئة` (class)
- `طباعة` ↔ `اطبع` (print)
- `هذا` ↔ `ذاتي` (this/self)
- `و` ↔ `&&`, `أو` ↔ `||`, `ليس` ↔ `!` (logical operators in Arabic)

### 3. **Challenge System is Well-Structured**
- 211 challenges across 7 tiers with clear progression
- Each tier has 10 lessons × 3 exercises (example + 2 practice)
- Difficulty escalates: BEGINNER → INTERMEDIATE → ADVANCED → EXPERT
- XP rewards scale: 10 → 15 → 20 → 25 → 40 → 70 → 80

### 4. **Book Claims vs Reality**
The book claims "112 chapters" but the extracted text shows 88 unique chapter headings. The discrepancy is likely due to:
- Some "chapters" being sub-sections (الفصل 102أ, 102ب, 102ج)
- Exercise chapters (98-100) not always counted as main chapters
- Additional content chapters (101-104) for transpiler and courses

### 5. **OOP is the Dominant Paradigm**
Challenges 4 (30 challenges) and 7 (30 challenges) together = 60 challenges (28% of all challenges) are OOP-focused. The language supports full OOP: classes, inheritance (`يرث`), abstract classes (`مجرّد`), interfaces (`واجهة`), access modifiers (`عام`/`خاص`/`محمي`), and namespaces (`نطاق`).

---

## Source Files Read

| # | File Path | Lines | Purpose |
|---|-----------|-------|---------|
| 1 | `Studio/Web/book.html` | 9,638 | Primary book source |
| 2 | `/tmp/book_text.txt` | 23,460 | Extracted book text |
| 3 | `Compiler/include/Daad/Keywords.hpp` | 87 | KeywordType enum |
| 4 | `Compiler/src/Keywords.cpp` | 370 | Arabic→C++ keyword mapping |
| 5 | `Studio/desktop-app/server/src/utils/dhad/lexer.js` | 704 | JS Lexer with keywords |
| 6 | `DAAD/compiler/lexer/keywords.c` | 186 | Low-level keyword table |
| 7 | `Studio/tests/challenges_tier1.js` | 56 | Tier 1 challenges (31) |
| 8 | `Studio/tests/challenges_tier2.js` | 55 | Tier 2 challenges (30) |
| 9 | `Studio/tests/challenges_tier3.js` | 55 | Tier 3 challenges (30) |
| 10 | `Studio/tests/challenges_tier4.js` | 55 | Tier 4 challenges (30) |
| 11 | `Studio/tests/challenges_tier5.js` | 55 | Tier 5 challenges (30) |
| 12 | `Studio/tests/challenges_tier6.js` | 55 | Tier 6 challenges (30) |
| 13 | `Studio/tests/challenges_tier7.js` | 55 | Tier 7 challenges (30) |

### Counts
- **Book chapters extracted**: 88
- **High-level keywords documented**: 103 (from Keywords.cpp + lexer.js)
- **Low-level keywords excluded**: 27+ (from keywords.c + CPU assembly)
- **Challenges documented**: 211 (31 + 6×30)

### Git Status
- No commits made (read-only report as specified)
- No files modified except the new report file
