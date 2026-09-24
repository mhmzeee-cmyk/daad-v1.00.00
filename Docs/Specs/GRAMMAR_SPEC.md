# قواعد نحو لغة ض عالية المستوى — EBNF من المحللين الفعليين

> **من الكود الفعلي فقط:** `Compiler/src/{Lexer.cpp:45-231,Parser.cpp:26-1683}` + `Compiler/include/Daad/{Token.hpp:8-57,Parser.hpp:25-93,AST.hpp}` + `Studio/Web/js/{dhad-lexer.js:537-690,dhad-parser.js:97-1555,dhad-ast.js}`.
> **الاصطلاح:** `=` تعريف، `|` بديل، `{ }` تكرار صفر+، `[ ]` اختياري، `( )` تجميع، `"نص"` حرفي، `(* تعليق *)`.
> **ملاحظة التوحيد:** حيث يختلف الطرفان وُسم الفرع بـ `[C++]` / `[JS]` مع المصدر. القاعدة المشتركة بلا وسم.

```ebnf
(* ═══ 1. البرنامج والكتل ═══ *)
(* C++: Compiler.cpp:36-43 حلقة parseStatement حتى isAtEnd؛ JS: dhad-parser.js:97 parseProgram *)
program        = { statement } EOF ;
(* C++: Parser.cpp:26-37 parseBlock — { فقط؛ JS: dhad-parser.js:865-876 parseBlock → ProgramAST *)
block          = "{" { statement } "}" ;

(* ═══ 2. المعجم (مثبت) ═══ *)
(* C++: Lexer.cpp:45-118 + UnicodeUtils.cpp:60-73؛ JS: dhad-lexer.js:379-407,448-690 *)
letter         = latin_letter | arabic_letter | "_" ;
arabic_letter  = (* U+0600-U+06FF عدا 060C 060D 061B 061F *)
                 (* + U+0750-077F + U+FB50-FDFF + U+FE70-FEFE *)
                 "ا" | "ب" | "ت" | "ث" | "ج" | "ح" | "خ" | "د"
               | "ذ" | "ر" | "ز" | "س" | "ش" | "ص" | "ض"
               | "ط" | "ظ" | "ع" | "غ" | "ف" | "ق" | "ك"
               | "ل" | "م" | "ن" | "ه" | "ي" ;
               (* ملاحظة: "و" حرف لكنها كلمة منطقية محجوزة المعنى؛ "س" "ص" محوران *)
latin_letter   = "a".."z" | "A".."Z" ;
digit          = "0".."9" ;   (* المشرقية ١٢٣ غير مدعومة في الطرفين — قيد *)
identifier     = letter { letter | digit } ;
               (* C++: scanIdentifier:218-231 عبر isIdentifierPart؛ *)
               (* JS: readIdentifier:416-446؛ الكلمات تطابق KEYWORDS أولًا *)
keyword        = (* 116 كلمة — انظر KEYWORDS_REGISTRY.json؛ "ليس" JS فقط *) ;
number         = digits [ "." digits ] ;
               (* C++ فقط هذا الشكل: Lexer.cpp:120-132؛ *)
               (* JS يضيف: "0x" hex | digits ["." digits] ["e" ["+"/"-"] digits] | "." digits *)
digits         = digit { digit } ;
string_lit     = '"' { escape_seq | character - ('"' | "\\") } '"' ;
               (* C++: "..." فقط + ثماني \0-\777 + مجهول يُحفظ — Lexer.cpp:134-191؛ *)
               (* JS: "..." أو '...' + \n \t \r \\ \' \" \0 — بلا ثماني — dhad-lexer.js:502-535 *)
escape_seq     = "\\n" | "\\t" | "\\r" | "\\\\" | "\\\"" | octal_escape [C++] | "'" [JS] ;
octal_escape   = "\\" ("0".."7") [("0".."7") [("0".."7")]] ;  (* C++ فقط *)
comment        = "//" { character - newline } newline
               | "/*" { character } "*/"   (* C++: غير مغلق → خطأ Lexer.cpp:83-85 *)
               | "#" { character - newline } newline ;  (* JS فقط dhad-lexer.js:575-579 *)
semi           = ";" | "؛" ;        (* U+061B — Lexer.cpp:58 *)
comma          = "," | "،" ;        (* U+060C — Lexer.cpp:106-107 *)
stmt_end       = [ semi ] ;         (* الفاصلة اختيارية فعليًا في معظم الجمل *)

(* ═══ 3. الأنواع ═══ *)
(* C++: mapType CodeGen.cpp:88-106؛ JS: أنواع نصية غير منمطة *)
type           = base_type { "[]" } ;
base_type      = "صحيح" | "عشري" | "منطقي" | "حرف" | "نص"
               | "فراغ" | "تلقائي" | "ثابت" | "عرّف"
               | "مؤشر" | "مرجع" | class_name ;
class_name     = identifier ;  (* C++: m_knownClassNames Parser.cpp:161-163,1132 *)

(* ═══ 4. العبارات (بالأسبقية المثبتة — Parser.cpp:343-361 = dhad-parser.js:1008-1015) ═══ *)
expression     = binary_expr [ "?" expression ":" expression ] ;
               (* C++: parseExpression:328-341؛ JS: parseExpression:994-997 *)
binary_expr    = unary_expr { bin_op unary_expr } ;  (* ترابط يساري، أسبقية تالية *)
bin_op         = "||" | "أو"                      (* 1 *)
               | "&&" | "و"                       (* 2 *)
               | "==" | "!="                      (* 3 *)
               | "<" | ">" | "<=" | ">="          (* 4 *)
               | "+" | "-"                        (* 5 *)
               | "*" | "/" | "%"                  (* 6 *)
               | "^" ;                            (* 7 أس — C++: std::pow، JS: Math.pow/** *)
unary_expr     = "-" unary_expr | "!" unary_expr | "ليس" unary_expr [JS فعالة؛ C++ ميتة*]
               | primary ;
               (* C++: parseUnary:364-380؛ JS: parseUnary:1036-1079 + * & تمرير + ++ -- لاحقة *)
primary        = number | string_lit | "صواب" | "خطأ" | "عدم"
               | identifier [ call_suffix | index_suffix | member_suffix ]
               | keyword_as_value
               | "(" expression ")"
               | "[" "]"                           (* C++: Raw "{}" Parser.cpp:468-472؛ *)
                                                 (* JS: __array() dhad-parser.js:1242 *)
               | "س" | "ص"                         (* محوران → x/y في JS؛ C++ الفرع ميت *)
               | "جديد" type "(" [ expression { comma expression } ] ")" [JS]
               | ("حجم_الـ" | "نوع_الـ") "(" expression ")" [JS:1203-1216] ;
keyword_as_value = (* C++: Parser.cpp:450-457 أي كلمة كاسم/استدعاء؛ *)
                 (* JS: dhad-parser.js:1225 أي KW عدا المحجوزات كمتغير *)
                 "هذا" | "ذاتي" | keyword "(" [ expression { comma expression } ] ")" ;
call_suffix    = "(" [ expression { comma expression } ] ")" ;
index_suffix   = "[" expression "]" { "[" expression "]" } ;
               (* C++: متسلسلة بـ base Parser.cpp:410-423؛ JS: مفردة *)
member_suffix  = "." identifier ;
(* * فرع "ليس" في C++ ميت: parseUnary:375 يطلب TOKEN_KEYWORD لكن الليكسر لا ينتجها (غير مسجلة) *)

(* ═══ 5. الجمل ═══ *)
statement      = var_decl | const_decl [JS] | assignment | compound_assignment
               | inc_stmt | if_stmt | while_stmt | for_stmt | do_while_stmt
               | switch_stmt | return_stmt | break_stmt | continue_stmt
               | func_def | class_decl | struct_decl | enum_decl | namespace_decl
               | template_decl | try_catch_stmt | throw_stmt | delete_stmt
               | import_stmt | export_stmt | print_stmt | input_stmt
               | widget_decl [JS] | image_stmt | expr_stmt | block | semi ;
               (* C++ التوجيه: Parser.cpp:39-293؛ JS: dhad-parser.js:107-233 *)

var_decl       = type identifier [ { "[" "]" } ]   (* C++: Parser.cpp:506-575 *)
                 [ "(" param_list ")" [ "->" type ] block ]   (* صيغة النوع-ك-دالة *)
                 [ "=" expression ] stmt_end ;
               (* C++: بلا تهيئة → 0.0 (نص → "")؛ الاسم يقبل كلمات *)
               (* JS: parseVarDecl:237-283 + * بادئة + [][] + ,a,b متعدد *)
const_decl     = "ثابت" ( type identifier "=" expression
                        | type identifier
                        | identifier "=" expression ) stmt_end ;  (* JS فقط :284-328 *)
assignment     = identifier "=" expression stmt_end ;
               (* C++: parseAssignment:577-582 + صيغة النوع=قيمة :518-520 *)
compound_assignment = identifier ("+=" | "-=" | "*=" | "/=" | "%=" | "^=") expression stmt_end ;
               (* C++: كلها Parser.cpp:191-201؛ JS: كلها عدا %= *)
inc_stmt       = ("زد" | "انقص") identifier stmt_end ;  (* C++: Parser.cpp:77-94 → +=1/-=1 *)
if_stmt        = ("إذا" | "اذا") expression block [ "وإلا" block ] ;
               (* C++: كتل إجبارية، بلا else-if Parser.cpp:637-651؛ *)
               (* JS: أقواس اختيارية + مفردة + else-if dhad-parser.js:330-358 *)
while_stmt     = ("طالما" | "بينما") expression block ;
for_stmt       = ("لكل" | "كرر") [ "(" ]
                 ( type identifier "في" expression        (* نطاق *)
                 | identifier "في" expression              (* نطاق بلا نوع → تلقائي *)
                 | [ type identifier [ "=" expression ] ] ";" expression ";" [ update ]  (* عداد *)
                 | expression [";" [ update ]]            (* مبسط: كرر (شرط) *)
                 ) [ ")" ] block ;
               (* C++: parseForEachOrForStatement:660-829 باستشراف؛ JS: parseFor:376 + parseForEach:437 *)
update         = assignment | compound_assignment | identifier ("++" | "--")
               | ("زد" | "انقص") identifier ;
do_while_stmt  = "افعل" block ["طالما"] [ "(" ] expression [ ")" ] stmt_end ;
               (* C++: طالما الذيل اختيارية Parser.cpp:835 *)
switch_stmt    = "اختر" [ "(" ] expression [ ")" ] "{"
                 { "حالة" expression ":" { statement } }
                 [ "افتراضي" ":" { statement } ] "}" ;
return_stmt    = "ارجع" [ expression ] stmt_end ;
break_stmt     = ("توقف" | "اكسر") stmt_end ;
continue_stmt  = ("استمر" | "تابع") stmt_end ;
(* "انتقل" مرفوضة بالتصميم: C++ خطأ صريح Parser.cpp:63-75؛ JS بلا فرع *)

func_def       = "دالة" [ "." ] name "(" [ param_list ] ")" [ "->" type ] block ;
               (* C++: أقواس اختيارية + أسماء بمسافات Parser.cpp:1003-1047، افتراضي فراغ *)
               (* JS: ( إجبارية + defaults + بانتظار dhad-parser.js:522-575 *)
name           = identifier { identifier } ;  (* C++ فقط المسافات *)
param_list     = param { comma param } ;
param          = [ type ] identifier [ "[" "]" ] [ "=" expression ] ;  (* JS فقط الافتراضي *)
               (* C++ قواعد parseParam:1072-1123: []→فراغ فارغ؛ مفردة→صحيح؛ الاسم:النوع أو النوع الاسم *)

class_decl     = ("صنف" | "فئة") identifier "{" { class_member } "}" ;
class_member   = ( type identifier ";" )                    (* عضو *)
               | ( type identifier "(" param_list ")" [ "->" type ] block )  (* طريقة *)
               | ( identifier ":" type ";" )                 (* عضو معكوس *)
               | access_mod [ "ساكن" ] ( member | method ) ; (* JS فقط *)
access_mod     = "عام" | "خاص" | "محمي" ;
               (* C++ التفصيل: Parser.cpp:1126-1268؛ الاسم:النوع() مرفوضة :1182-1185 *)
struct_decl    = "هيكل" identifier "{" { statement } "}" ;
               (* C++: الاسم فقط فعليًا AST.hpp:385 — الأعضاء مفقودة (مكسور) *)
enum_decl      = "تعداد" identifier "{" [ identifier { comma identifier } ] "}" [ semi ] ;
namespace_decl = "نطاق" identifier block ;
template_decl  = "قالب" "<" identifier { comma identifier } ">" statement ;
               (* C++: < إجبارية وإلا متغير/خطأ Parser.cpp:960-986 *)
try_catch_stmt = "حاول" block [ "امسك" [ "(" [ identifier ] ")" ] block ]
                 [ "أخيراً" block ] ;
throw_stmt     = "ارمِ" [ expression ] stmt_end ;
delete_stmt    = "احذف" expression stmt_end ;
import_stmt    = "استورد" [ string_lit ] stmt_end ;   (* no-op الطرفان *)
export_stmt    = "صدّر" [ identifier { comma identifier } ] stmt_end ;  (* no-op *)
print_stmt     = "طباعة" ( "(" [ expression { comma expression } ] ")" | expression ) stmt_end ;
               (* "اطبع" بديل مسجل؛ فرع C++ يفحص طباعة Parser.cpp:113 *)
input_stmt     = "ادخل" "(" identifier [JS: { comma identifier }] ")" stmt_end ;
               (* C++ مفرد فقط Parser.cpp:612-635؛ JS متعدد dhad-parser.js:802-847 *)
widget_decl    = gui_keyword identifier [ "=" expression ] stmt_end ;  (* JS فقط :850-863 *)
gui_keyword    = "زر_أمر" | "حقل_نص" | "قائمة_خيارات" | "صورة" | "مربع_اختيار"
               | "شريط_تمرير" | "قائمة_منسدلة" | "لوحة" | "تسمية" | "عمود"
               | "صف" | "شبكة" | "شريط_تلوين" | "علامة_تبويب" ;
expr_stmt      = expression stmt_end ;
               (* C++: استدعاء ( → VarDecl(__builtin_call) Parser.cpp:228-232,257-261 *)

(* ═══ 6. الصور (20 — التوقيعات من المحللين) ═══ *)
(* C++: Parser.cpp:1353-1681 (تعبيرات comma)؛ JS: dhad-parser.js:1291-1555 *)
(* ملاحظة: كلمات الصور تقبل أيضًا الصيغة العامة kw(...) كاستدعاء عادي *)
(* C++: Parser.cpp:119-131؛ التوجيه المخصص 133-153 *)
image_stmt     = load_image | draw_image | image_size_expr | save_image | crop_image
               | resize_stmt | rotate_image | flip_image | opacity_stmt | filter_stmt
               | overlay_stmt | background_stmt | pixel_expr | draw_stmt | fill_stmt
               | rectangle_stmt | circle_stmt | line_stmt | text_on_canvas | clear_stmt ;
load_image     = "حمّل_صورة" "(" string_lit comma identifier ")" stmt_end ;
draw_image     = "ارسم_صورة" "(" identifier comma expression comma expression comma expression comma expression ")" stmt_end ;
image_size_expr = "حجم_صورة" "(" identifier ")" ;   (* تعبير في C++؛ جملة/تعبير في JS *)
save_image     = "احفظ_صورة" "(" identifier comma string_lit ")" stmt_end ;
crop_image     = "قص_صورة" "(" identifier comma expression comma expression comma expression comma expression ")" stmt_end ;
resize_stmt    = "غيّر_حجم" "(" identifier comma expression comma expression ")" stmt_end ;
rotate_image   = "لف_صورة" "(" identifier comma expression ")" stmt_end ;
flip_image     = "قلب_صورة" "(" identifier comma (identifier | string_lit) ")" stmt_end ;
opacity_stmt   = "شفافية" "(" identifier comma expression ")" stmt_end ;
filter_stmt    = "فلتر" "(" identifier comma (identifier | string_lit) ")" stmt_end ;
overlay_stmt   = "تراكب" "(" identifier comma identifier comma expression comma expression ")" stmt_end ;
background_stmt = "خلفية" "(" identifier comma identifier ")" stmt_end ;
pixel_expr     = "بكسل" "(" identifier comma expression comma expression ")" ;  (* تعبير *)
draw_stmt      = "ارسم" shape [ "(" [ expression { comma expression } ] ")" ] stmt_end ;
fill_stmt      = "ملء" shape [ "(" [ expression { comma expression } ] ")" ] stmt_end ;
rectangle_stmt = "مستطيل" "(" expression comma expression comma expression comma expression ")" stmt_end ;
circle_stmt    = "دائرة" "(" expression comma expression comma expression ")" stmt_end ;
line_stmt      = "خط" "(" expression comma expression comma expression comma expression ")" stmt_end ;
text_on_canvas = "نص_على_لوحة" "(" (identifier | string_lit) comma expression comma expression comma expression ")" stmt_end ;
clear_stmt     = "مسح" [ "(" ")" ] stmt_end ;
shape          = identifier | string_lit ;
```

## الملاحق المعجمية (مثبتة)

```ebnf
operator       = "+" | "-" | "*" | "/" | "%" | "=" | "==" | "!" | "!="
               | "&&" | "||" | "<" | ">" | "<=" | ">=" | "?" | "^" | "^="
               | "+=" | "-=" | "*=" | "/=" | "%=" | "++" | "--" | "." | ":" ;
               (* C++: Token.hpp:8-57 + Lexer.cpp:64-114 — الكل؛ *)
               (* JS: TT:130-175 + tokenize:600-676 — الكل عدا %=؛ وتضيف -> :: @ # & *)
arabic_op      = "و" | "أو" | "ليس" ;  (* ليس فعالة JS فقط — انظر الملاحظة أعلاه *)
```
