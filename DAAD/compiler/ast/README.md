# AST Module - ض Core

وحدة شجرة الـ AST (Abstract Syntax Tree) للغة ض Core.

## الملفات

| الملف | الوصف |
|-------|-------|
| `ast_node.h` | تعريف أنواع العقد والبيانات |
| `ast.h` / `ast.c` | واجهة الإدارة الأساسية |
| `ast_builder.h` / `ast_builder.c` | دوال بناء العقد |
| `ast_printer.h` / `ast_printer.c` | طباعة شجرة AST |
| `ast_visitor.h` / `ast_visitor.c` | نمط الزيارة |

## الاستخدام

```c
#include "ast/ast.h"
#include "ast/ast_builder.h"

// بناء برنامج
ASTNodeList* decls = malloc(sizeof(ASTNodeList));
ast_node_list_init(decls);
ast_node_list_add(decls, ast_build_variable_decl("x", NULL, ast_build_literal_int(10)));
ASTNode* program = ast_build_program(decls);

// تحرير الذاكرة
ast_node_destroy(program);
```

## أنواع العقد

- **البرنامج**: الجذر الرئيسي
- **الكتلة**: مجموعة عبارات
- **التعريفات**: متغير، ثابت، دالة
- **العبارات**: إذا، طالما، لكل، ارجع، توقف، أكمل
- **التعبيرات**: عملية ثنائية/أحادية، استدعاء، فهرسة، تحويل نوع
