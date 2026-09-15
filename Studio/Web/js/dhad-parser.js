// ═══════════════════════════════════════════════════════════════════════════════
// Dhad (ض) Language — Recursive Descent Parser
// Parses token stream → AST (32 node types)
// ═══════════════════════════════════════════════════════════════════════════════

var DhadParser = (function () {
  'use strict';

  var TT = DhadLexer.TT;
  var AST = DhadAST;

  // ── Parser ──────────────────────────────────────────────────────────────────
  function Parser(tokens, errors) {
    this.tokens = tokens;
    this.pos = 0;
    this.errors = errors || [];
  }

  Parser.prototype.peek = function (offset) {
    offset = offset || 0;
    return this.tokens[this.pos + offset];
  };

  Parser.prototype.advance = function () {
    var t = this.tokens[this.pos];
    this.pos++;
    return t;
  };

  Parser.prototype.expect = function (type) {
    var t = this.peek();
    if (!t || t.type !== type) {
      var got = t ? '"' + t.value + '"' : 'EOF';
      this.errors.push({
        message: 'متوقع: ' + type + ' — حصل على: ' + got,
        line: t ? t.line : 0,
        col: t ? t.col : 0
      });
      // Recovery: skip to next semicolon or newline-ish token
      while (this.pos < this.tokens.length && this.peek().type !== TT.SEMICOLON && this.peek().type !== TT.EOF) {
        this.pos++;
      }
      return { type: type, value: '', line: t ? t.line : 0, col: t ? t.col : 0 };
    }
    return this.advance();
  };

  Parser.prototype.match = function (type) {
    if (this.peek() && this.peek().type === type) {
      return this.advance();
    }
    return null;
  };

  Parser.prototype.check = function (type) {
    return this.peek() && this.peek().type === type;
  };

  Parser.prototype.isKeyword = function (type) {
    return type && type.indexOf('KW_') === 0;
  };

  // ── P3: Context-specific class names (after صنف/يرث/جديد only) ──────────────
  var CLASS_NAME_TOKENS = [TT.IDENTIFIER, TT.KW_CIRCLE, TT.KW_RECTANGLE, TT.KW_LINE, TT.KW_IMAGE, TT.KW_TEMPLATE];

  Parser.prototype.isClassName = function () {
    var t = this.peek();
    if (!t) return false;
    return CLASS_NAME_TOKENS.indexOf(t.type) !== -1;
  };

  Parser.prototype.expectClassName = function () {
    var t = this.peek();
    if (!t) {
      this.errors.push({ message: 'متوقع اسم صنف', line: 0, col: 0 });
      return { type: 'IDENTIFIER', value: '__error', line: 0, col: 0 };
    }
    if (CLASS_NAME_TOKENS.indexOf(t.type) !== -1) {
      return this.advance();
    }
    this.errors.push({
      message: 'متوقع اسم صنف — حصل على: "' + t.value + '"',
      line: t.line,
      col: t.col
    });
    return { type: 'IDENTIFIER', value: '__error', line: t.line, col: t.col };
  };

  // Accept IDENTIFIER or any keyword as a variable name
  Parser.prototype.isName = function () {
    var t = this.peek();
    if (!t) return false;
    return t.type === TT.IDENTIFIER || this.isKeyword(t.type);
  };

  Parser.prototype.expectName = function () {
    var t = this.peek();
    if (!t) {
      this.errors.push({ message: 'متوقع اسم', line: 0, col: 0 });
      return { type: 'IDENTIFIER', value: '__error', line: 0, col: 0 };
    }
    if (t.type === TT.IDENTIFIER || this.isKeyword(t.type)) {
      return this.advance();
    }
    this.errors.push({
      message: 'متوقع اسم — حصل على: "' + t.value + '"',
      line: t.line,
      col: t.col
    });
    // Recovery
    return { type: 'IDENTIFIER', value: '__error', line: t.line, col: t.col };
  };

  Parser.prototype.isTypeKeyword = function () {
    var t = this.peek();
    if (!t) return false;
    return t.type === TT.KW_INT || t.type === TT.KW_DOUBLE || t.type === TT.KW_BOOL ||
           t.type === TT.KW_CHAR || t.type === TT.KW_STRING || t.type === TT.KW_VOID ||
           t.type === TT.KW_AUTO || t.type === TT.KW_CONST || t.type === TT.KW_FUNCTION;
  };

  // ── Program ─────────────────────────────────────────────────────────────────
  Parser.prototype.parseProgram = function () {
    var body = [];
    while (!this.check(TT.EOF)) {
      var stmt = this.parseStatement();
      if (stmt) body.push(stmt);
    }
    return new AST.ProgramAST(body, 1, 1);
  };

  // ── Statements ──────────────────────────────────────────────────────────────
  Parser.prototype.parseStatement = function () {
    var t = this.peek();
    if (!t) return null;

    // Skip stray semicolons
    if (t.type === TT.SEMICOLON) { this.advance(); return null; }

    switch (t.type) {
      // Variable declarations: صحيح/عشري/منطقي/حرف/نص/فراغ [ثابت] اسم [= expr] ;
      case TT.KW_INT:
      case TT.KW_DOUBLE:
      case TT.KW_BOOL:
      case TT.KW_CHAR:
      case TT.KW_STRING:
      case TT.KW_VOID:
      case TT.KW_AUTO:
        return this.parseVarDecl();

      // ثابت type var = ... ;
      case TT.KW_CONST:
        return this.parseConstDecl();

      // Control flow
      case TT.KW_IF:      return this.parseIf();
      case TT.KW_WHILE:   return this.parseWhile();
      case TT.KW_FOR:     return this.parseFor();
      case TT.KW_DO:      return this.parseDoWhile();
      case TT.KW_SWITCH:  return this.parseSwitch();

      // Jump statements
      case TT.KW_RETURN:  return this.parseReturn();
      case TT.KW_BREAK:   this.advance(); this.match(TT.SEMICOLON); return new AST.BreakStmtAST(t.line, t.col);
      case TT.KW_CONTINUE:this.advance(); this.match(TT.SEMICOLON); return new AST.ContinueStmtAST(t.line, t.col);

      // Function: دالة 이름(...) [-> type] { ... }
      // But if دالة is followed by IDENTIFIER then ASSIGN, it's a variable declaration with type دالة
      case TT.KW_FUNCTION:
        if (this.peek(1) && this.peek(1).type === TT.IDENTIFIER && this.peek(2) && this.peek(2).type === TT.ASSIGN) {
          return this.parseVarDecl();
        }
        return this.parseFunctionDecl();

      // OOP
      case TT.KW_CLASS:    return this.parseClass();
      case TT.KW_STRUCT:   return this.parseStruct();
      case TT.KW_ENUM:     return this.parseEnum();
      case TT.KW_NAMESPACE:return this.parseNamespace();
      case TT.KW_TEMPLATE: return this.parseTemplate();
      case TT.KW_ABSTRACT: return this.parseAbstract();
      case TT.KW_INTERFACE: return this.parseInterface();

      // Error handling
      case TT.KW_TRY:      return this.parseTryCatch();
      case TT.KW_THROW:    return this.parseThrow();

      // Memory
      case TT.KW_NEW:      return this.parseNewExpr();
      case TT.KW_DELETE:   return this.parseDelete();

      // Print
      case TT.KW_PRINT:    return this.parsePrint();

      // Input
      case TT.KW_INPUT:    return this.parseInput();

      // Import/Export
      case TT.KW_IMPORT:   return this.parseImport();
      case TT.KW_EXPORT:   return this.parseExport();

      // GUI widgets
      case TT.KW_BUTTON:
      case TT.KW_TEXTFIELD:
      case TT.KW_COMBOBOX:
      case TT.KW_IMAGE:
      case TT.KW_CHECKBOX:
      case TT.KW_SLIDER:
      case TT.KW_DROPDOWN:
      case TT.KW_PANEL:
      case TT.KW_LABEL:
      case TT.KW_COLUMN:
      case TT.KW_ROW:
      case TT.KW_GRID:
      case TT.KW_PROGRESSBAR:
      case TT.KW_TABBAR:
        return this.parseWidgetDecl();

      // Image Processing (20 keywords)
      case TT.KW_LOAD_IMAGE:   return this.parseLoadImage();
      case TT.KW_DRAW_IMAGE:   return this.parseDrawImage();
      case TT.KW_IMAGE_SIZE:   return this.parseImageSize();
      case TT.KW_SAVE_IMAGE:   return this.parseSaveImage();
      case TT.KW_CROP_IMAGE:   return this.parseCropImage();
      case TT.KW_RESIZE:       return this.parseResize();
      case TT.KW_ROTATE_IMAGE: return this.parseRotateImage();
      case TT.KW_FLIP_IMAGE:   return this.parseFlipImage();
      case TT.KW_OPACITY:      return this.parseOpacity();
      case TT.KW_FILTER:       return this.parseFilter();
      case TT.KW_OVERLAY:      return this.parseOverlay();
      case TT.KW_BACKGROUND:   return this.parseBackground();
      case TT.KW_PIXEL:        return this.parsePixel();
      case TT.KW_DRAW:         return this.parseDrawStmt();
      case TT.KW_FILL:         return this.parseFillStmt();
      case TT.KW_RECTANGLE:    return this.parseRectangle();
      case TT.KW_CIRCLE:       return this.parseCircle();
      case TT.KW_LINE:         return this.parseLine();
      case TT.KW_TEXT_ON_CANVAS: return this.parseTextOnCanvas();
      case TT.KW_CLEAR:        return this.parseClearCanvas();

      // Identifier-based statements (assignment, function call, member access, array subscript)
      case TT.IDENTIFIER:
        // User-defined type: className varName = ... or className[] varName = ...
        // Check: IDENTIFIER IDENTIFIER or IDENTIFIER [ ] IDENTIFIER
        if (this.peek(1) && this.peek(1).type === TT.IDENTIFIER) {
          return this.parseVarDecl();
        }
        if (this.peek(1) && this.peek(1).type === TT.LBRACKET &&
            this.peek(2) && this.peek(2).type === TT.RBRACKET &&
            this.peek(3) && this.peek(3).type === TT.IDENTIFIER) {
          return this.parseVarDecl();
        }
        return this.parseIdentifierStmt();

      case TT.LBRACE:
        return this.parseBlock();

      default:
        // Expression statement (fallback)
        return this.parseExprStatement();
    }
  };

  // ── Variable Declaration ────────────────────────────────────────────────────
  Parser.prototype.parseVarDecl = function () {
    var typeTok = this.advance(); // the type keyword
    var varType = typeTok.value;

    // Handle pointer type: صحيح* → type = "int*"
    if (this.check(TT.STAR)) {
      this.advance(); // *
      varType += '*';
    }

    // Handle array type: صحيح[] → type = "int[]"
    if (this.check(TT.LBRACKET)) {
      this.advance(); // [
      if (this.check(TT.RBRACKET)) {
        this.advance(); // ]
        varType += '[]';
      }
    }

    var nameTok = this.expectName();
    var init = null;

    if (this.match(TT.ASSIGN)) {
      init = this.parseExpression();
    }

    // Return first declaration, then check for comma (multi-var)
    var decls = [new AST.VarDeclStmtAST(varType, nameTok.value, init, typeTok.line, typeTok.col)];

    while (this.match(TT.COMMA)) {
      var nextName = this.expectName();
      var nextInit = null;
      if (this.match(TT.ASSIGN)) {
        nextInit = this.parseExpression();
      }
      decls.push(new AST.VarDeclStmtAST(varType, nextName.value, nextInit, typeTok.line, typeTok.col));
    }

    this.match(TT.SEMICOLON);

    // If single declaration, return it directly
    if (decls.length === 1) return decls[0];

    // Multiple declarations → wrap in ProgramAST block
    return new AST.ProgramAST(decls, typeTok.line, typeTok.col);
  };

  Parser.prototype.parseConstDecl = function () {
    var constTok = this.advance(); // ثابت

    // If next is a type keyword BUT followed by '=', treat keyword as variable name
    // e.g. ثابت صحيح = 42 → const name = 42 (صحيح is the variable name)
    if (this.isTypeKeyword() && this.peek(1) && this.peek(1).type === TT.ASSIGN) {
      var nameTok = this.expectName();
      var init = null;
      if (this.match(TT.ASSIGN)) {
        init = this.parseExpression();
      }
      this.match(TT.SEMICOLON);
      return new AST.VarDeclStmtAST('const', nameTok.value, init, constTok.line, constTok.col);
    }

    if (this.isTypeKeyword()) {
      // Form 1: ثابت نوع اسم = قيمة
      var typeTok = this.advance(); // type
      var varType = typeTok.value;
      if (this.check(TT.LBRACKET)) {
        this.advance(); // [
        if (this.check(TT.RBRACKET)) {
          this.advance(); // ]
          varType += '[]';
        }
      }
      var nameTok = this.expectName();
      var init = null;
      if (this.match(TT.ASSIGN)) {
        init = this.parseExpression();
      }
      this.match(TT.SEMICOLON);
      return new AST.VarDeclStmtAST('const ' + varType, nameTok.value, init, constTok.line, constTok.col);
    }

    // Form 2: ثابت اسم = قيمة
    var nameTok = this.expectName();
    var init = null;
    if (this.match(TT.ASSIGN)) {
      init = this.parseExpression();
    }
    this.match(TT.SEMICOLON);
    return new AST.VarDeclStmtAST('const', nameTok.value, init, constTok.line, constTok.col);
  };

  // ── If Statement ────────────────────────────────────────────────────────────
  Parser.prototype.parseIf = function () {
    var ifTok = this.advance(); // إذا
    var paren = this.match(TT.LPAREN);
    var condition = this.parseExpression();
    if (paren) this.expect(TT.RPAREN);

    // Then body: either a block { ... } or a single statement
    var thenBody;
    if (this.check(TT.LBRACE)) {
      thenBody = this.parseBlock().body;
    } else {
      thenBody = [this.parseStatement()];
    }

    var elseBody = [];
    if (this.check(TT.KW_ELSE)) {
      this.advance();
      if (this.check(TT.KW_IF)) {
        elseBody = [this.parseIf()];
      } else if (this.check(TT.LBRACE)) {
        elseBody = this.parseBlock().body;
      } else {
        elseBody = [this.parseStatement()];
      }
    }

    return new AST.IfStmtAST(condition, thenBody, elseBody, ifTok.line, ifTok.col);
  };

  // ── While Loop ──────────────────────────────────────────────────────────────
  Parser.prototype.parseWhile = function () {
    var whileTok = this.advance(); // طالما
    var paren = this.match(TT.LPAREN);
    var condition = this.parseExpression();
    if (paren) this.expect(TT.RPAREN);

    var body;
    if (this.check(TT.LBRACE)) {
      body = this.parseBlock().body;
    } else {
      body = [this.parseStatement()];
    }
    return new AST.WhileStmtAST(condition, body, whileTok.line, whileTok.col);
  };

  // ── For Loop ────────────────────────────────────────────────────────────────
  Parser.prototype.parseFor = function () {
    var forTok = this.advance(); // لكل
    var paren = this.match(TT.LPAREN);

    // Try to detect for-each: (type name في iterable)
    // Save position to restore if it's not for-each
    var savedPos = this.pos;

    if (this.isTypeKeyword()) {
      var typeTok = this.peek();
      this.advance(); // type
      if (this.isName()) {
        this.advance(); // name
        if (this.check(TT.KW_IN)) {
          // For-each! Restore and parse properly
          this.pos = savedPos;
          return this.parseForEach(forTok, paren);
        }
      }
    }
    this.pos = savedPos; // restore

    // Standard for loop: for (init; condition; update)
    var init = null;
    if (this.isTypeKeyword()) {
      init = this.parseVarDecl();
      // parseVarDecl consumes semicolon
    } else if (this.check(TT.IDENTIFIER)) {
      var nameTok = this.advance();
      if (this.match(TT.ASSIGN)) {
        init = new AST.AssignmentAST(nameTok.value, this.parseExpression(), nameTok.line, nameTok.col);
      }
      this.match(TT.SEMICOLON);
    } else {
      this.match(TT.SEMICOLON);
    }

    // Condition
    var condition = null;
    if (!this.check(TT.SEMICOLON)) {
      condition = this.parseExpression();
    }
    this.expect(TT.SEMICOLON);

    // Update
    var update = null;
    if (!this.check(TT.RPAREN)) {
      update = this.parseExprStatement();
    }
    if (paren) this.expect(TT.RPAREN);

    var body;
    if (this.check(TT.LBRACE)) {
      body = this.parseBlock().body;
    } else {
      body = [this.parseStatement()];
    }
    return new AST.ForStmtAST(init, condition, update, body, forTok.line, forTok.col);
  };

  // ── For-Each Loop ──────────────────────────────────────────────────────────
  Parser.prototype.parseForEach = function (forTok, paren) {
    var typeTok = this.advance(); // the type keyword
    var varType = typeTok.value;
    var nameTok = this.expectName();
    this.expect(TT.KW_IN); // في
    var iterable = this.parseExpression();
    if (paren) this.expect(TT.RPAREN);

    var body;
    if (this.check(TT.LBRACE)) {
      body = this.parseBlock().body;
    } else {
      body = [this.parseStatement()];
    }
    return new AST.ForEachStmtAST(varType, nameTok.value, iterable, body, forTok.line, forTok.col);
  };

  // ── Do-While Loop ───────────────────────────────────────────────────────────
  Parser.prototype.parseDoWhile = function () {
    var doTok = this.advance(); // افعل
    var body;
    if (this.check(TT.LBRACE)) {
      body = this.parseBlock().body;
    } else {
      body = [this.parseStatement()];
    }

    this.expect(TT.KW_WHILE);
    var paren = this.match(TT.LPAREN);
    var condition = this.parseExpression();
    if (paren) this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);

    return new AST.DoWhileStmtAST(condition, body, doTok.line, doTok.col);
  };

  // ── Switch Statement ────────────────────────────────────────────────────────
  Parser.prototype.parseSwitch = function () {
    var swTok = this.advance(); // اختر
    var paren = this.match(TT.LPAREN);
    var expression = this.parseExpression();
    if (paren) this.expect(TT.RPAREN);
    this.expect(TT.LBRACE);

    var cases = [];
    var defaultBody = [];

    while (!this.check(TT.RBRACE) && !this.check(TT.EOF)) {
      if (this.check(TT.KW_CASE)) {
        this.advance();
        var caseValue = this.parseExpression();
        this.expect(TT.COLON);
        var caseBody = [];
        while (!this.check(TT.KW_CASE) && !this.check(TT.KW_DEFAULT) && !this.check(TT.RBRACE) && !this.check(TT.EOF)) {
          caseBody.push(this.parseStatement());
        }
        cases.push(new AST.SwitchCaseAST(caseValue, caseBody));
      } else if (this.check(TT.KW_DEFAULT)) {
        this.advance();
        this.expect(TT.COLON);
        while (!this.check(TT.KW_CASE) && !this.check(TT.RBRACE) && !this.check(TT.EOF)) {
          defaultBody.push(this.parseStatement());
        }
      } else {
        this.errors.push({ message: 'متوقع حالة أو افتراضي', line: this.peek().line, col: this.peek().col });
        this.advance();
      }
    }

    this.expect(TT.RBRACE);
    return new AST.SwitchStmtAST(expression, cases, defaultBody, swTok.line, swTok.col);
  };

  // ── Return Statement ────────────────────────────────────────────────────────
  Parser.prototype.parseReturn = function () {
    var retTok = this.advance(); // ارجع
    var value = null;
    if (!this.check(TT.SEMICOLON) && !this.check(TT.RBRACE) && !this.check(TT.EOF)) {
      value = this.parseExpression();
    }
    this.match(TT.SEMICOLON);
    return new AST.ReturnStmtAST(value, retTok.line, retTok.col);
  };

  // ── Function Declaration ────────────────────────────────────────────────────
  Parser.prototype.parseFunctionDecl = function () {
    var fnTok = this.advance(); // دالة
    var nameTok = this.expect(TT.IDENTIFIER);

    this.expect(TT.LPAREN);
    var params = [];
    if (!this.check(TT.RPAREN)) {
      do {
        // Support: type name, type name = default, دالة name
        if (this.check(TT.KW_FUNCTION)) {
          // Function type parameter: دالة اسم
          this.advance(); // skip دالة
          var paramName = this.expectName();
          params.push({ type: 'دالة', name: paramName.value, defaultValue: null });
        } else if (this.isTypeKeyword() || this.check(TT.IDENTIFIER)) {
          var firstWord = this.advance().value;
          // كلمة واحدة فقط: تُعامل كاسم معامل بالنوع الافتراضي صحيح
          // (توافق مع محرك سطح المكتب — كان يسبب خطأ "متوقع اسم")
          if (this.check(TT.COMMA) || this.check(TT.RPAREN)) {
            params.push({ type: 'صحيح', name: firstWord, defaultValue: null });
          } else {
            var paramType = firstWord;
            // Handle array type: صحيح[] → type = "int[]"
            if (this.check(TT.LBRACKET)) {
              this.advance(); // [
              if (this.check(TT.RBRACKET)) {
                this.advance(); // ]
                paramType += '[]';
              }
            }
            var paramName = this.expectName();
            var defaultValue = null;
            if (this.match(TT.ASSIGN)) {
              defaultValue = this.parseExpression();
            }
            params.push({ type: paramType, name: paramName.value, defaultValue: defaultValue });
          }
        }
      } while (this.match(TT.COMMA));
    }
    this.expect(TT.RPAREN);

    // Optional return type: -> type
    var returnType = 'void';
    if (this.match(TT.ARROW)) {
      returnType = this.advance().value;
    }

    // Optional async: بانتظار (no-op in web interpreter)
    this.match(TT.KW_AWAIT);

    var body = this.parseBlock().body || [];
    return new AST.FunctionDeclAST(returnType, nameTok.value, params, body, fnTok.line, fnTok.col);
  };

  // ── Class Declaration ───────────────────────────────────────────────────────
  Parser.prototype.parseClass = function () {
    var classTok = this.advance(); // صنف
    var nameTok = this.expectClassName();

    // Check for inheritance: صنف اسم يرث اسم2
    var parent = null;
    if (this.check(TT.KW_INHERIT)) {
      this.advance(); // يرث
      parent = this.expectClassName().value;
    }

    this.expect(TT.LBRACE);

    var body = [];
    while (!this.check(TT.RBRACE) && !this.check(TT.EOF)) {
      // Read access modifier
      var access = 'public';
      var isStatic = false;

      if (this.check(TT.KW_PUBLIC)) { this.advance(); access = 'public'; }
      else if (this.check(TT.KW_PRIVATE)) { this.advance(); access = 'private'; }
      else if (this.check(TT.KW_PROTECTED)) { this.advance(); access = 'protected'; }

      if (this.check(TT.KW_STATIC)) { this.advance(); isStatic = true; }

      // P1: دالة جديد(...) → constructor
      if (this.check(TT.KW_FUNCTION) && this.peek(1) && this.peek(1).type === TT.KW_NEW) {
        this.advance(); // دالة
        this.advance(); // جديد
        var ctor = this.parseConstructor(nameTok.value);
        ctor.access = access;
        body.push(ctor);
      }
      // P5: دالة ClassName(...) → constructor
      else if (this.check(TT.KW_FUNCTION) && this.peek(1) && this.peek(1).type === TT.IDENTIFIER && this.peek(1).value === nameTok.value) {
        this.advance(); // دالة
        var ctor = this.parseConstructor(nameTok.value);
        ctor.access = access;
        body.push(ctor);
      }
      // Constructor: same name as class followed by (
      else if (this.check(TT.IDENTIFIER) && this.peek().value === nameTok.value && this.peek(1) && this.peek(1).type === TT.LPAREN) {
        var ctor = this.parseConstructor(nameTok.value);
        ctor.access = access;
        body.push(ctor);
      } else if (this.check(TT.IDENTIFIER) && this.peek(1) && this.peek(1).type === TT.COLON) {
        // الصيغة العكسية لعضو الصف: الاسم : النوع
        var memberName = this.advance().value;
        this.advance(); // :
        var memberTypeTok = this.expectName();
        var commaOk = true;
        while (this.match(TT.COMMA)) {
          if (this.check(TT.SEMICOLON)) { this.advance(); commaOk = false; break; }
          var extraName = this.expectName().value;
          var extraType = this.expectName().value;
          body.push(new AST.VarDeclStmtAST(extraType, extraName, null));
        }
        if (commaOk) this.match(TT.SEMICOLON);
        var member = new AST.VarDeclStmtAST(memberTypeTok.value, memberName, null);
        member.access = access;
        member.isStatic = isStatic;
        body.push(member);
      } else {
        var stmt = this.parseStatement();
        if (stmt) {
          if (stmt.type === 'VarDecl') {
            // P6d: Reject طول/حجم as field names
            if (stmt.name === 'طول' || stmt.name === 'حجم') {
              this.errors.push({
                message: 'لا يمكن استخدام "طول" أو "حجم" كاسم حقل — استخدم اسماً آخر',
                line: stmt.line,
                col: stmt.col
              });
            }
            stmt.access = access;
            stmt.isStatic = isStatic;
          }
          if (stmt.type === 'FunctionDecl') { stmt.access = access; stmt.isStatic = isStatic; }
          body.push(stmt);
        }
      }
    }

    this.expect(TT.RBRACE);
    var node = new AST.ClassDeclAST(nameTok.value, body, classTok.line, classTok.col);
    node.parent = parent;
    return node;
  };

  // ── Constructor Declaration ────────────────────────────────────────────────
  Parser.prototype.parseConstructor = function (className) {
    var nameTok = this.advance(); // className
    this.expect(TT.LPAREN);
    var params = [];
    if (!this.check(TT.RPAREN)) {
      do {
        if (this.isTypeKeyword() || this.check(TT.IDENTIFIER) || this.isName()) {
          var paramType = this.advance().value;
          if (this.check(TT.LBRACKET)) {
            this.advance();
            if (this.check(TT.RBRACKET)) {
              this.advance();
              paramType += '[]';
            }
          }
          var paramName = this.expectName();
          var defaultValue = null;
          if (this.match(TT.ASSIGN)) {
            defaultValue = this.parseExpression();
          }
          params.push({ type: paramType, name: paramName.value, defaultValue: defaultValue });
        }
      } while (this.match(TT.COMMA));
    }
    this.expect(TT.RPAREN);
    var body = this.parseBlock().body || [];
    return new AST.ConstructorDeclAST(params, body, nameTok.line, nameTok.col);
  };

  // ── Struct Declaration ──────────────────────────────────────────────────────
  Parser.prototype.parseStruct = function () {
    var structTok = this.advance(); // هيكل
    var nameTok = this.expect(TT.IDENTIFIER);
    var body = this.parseBlock().body || [];
    return new AST.StructDeclAST(nameTok.value, body, structTok.line, structTok.col);
  };

  // ── Enum Declaration ────────────────────────────────────────────────────────
  Parser.prototype.parseEnum = function () {
    var enumTok = this.advance(); // تعداد
    var nameTok = this.expect(TT.IDENTIFIER);
    this.expect(TT.LBRACE);

    var values = [];
    while (!this.check(TT.RBRACE) && !this.check(TT.EOF)) {
      var val = this.expect(TT.IDENTIFIER);
      values.push(val.value);
      this.match(TT.COMMA);
    }

    this.expect(TT.RBRACE);
    return new AST.EnumDeclAST(nameTok.value, values, enumTok.line, enumTok.col);
  };

  // ── Namespace Declaration ───────────────────────────────────────────────────
  Parser.prototype.parseNamespace = function () {
    var nsTok = this.advance(); // نطاق
    var nameTok = this.expect(TT.IDENTIFIER);
    var body = this.parseBlock().body || [];
    return new AST.NamespaceDeclAST(nameTok.value, body, nsTok.line, nsTok.col);
  };

  // ── Template Declaration ────────────────────────────────────────────────────
  Parser.prototype.parseTemplate = function () {
    var tmplTok = this.advance(); // قالب
    this.expect(TT.LT);
    var params = [];
    do {
      params.push(this.expect(TT.IDENTIFIER).value);
    } while (this.match(TT.COMMA));
    this.expect(TT.GT);

    var nameTok = this.expect(TT.IDENTIFIER);
    var body = this.parseStatement();
    return new AST.TemplateDeclAST(nameTok.value, params, body, tmplTok.line, tmplTok.col);
  };

  // ── P4: Abstract Declaration ────────────────────────────────────────────────
  Parser.prototype.parseAbstract = function () {
    var absTok = this.advance(); // مجرد
    if (this.check(TT.KW_CLASS)) {
      var cls = this.parseClass();
      cls.isAbstract = true;
      return cls;
    }
    if (this.check(TT.KW_FUNCTION)) {
      // مجرد دالة name(); → abstract function declaration (no body)
      this.advance(); // دالة
      var nameTok = this.peek();
      if (nameTok && (nameTok.type === TT.IDENTIFIER || this.isKeyword(nameTok.type))) {
        this.advance();
        this.expect(TT.LPAREN);
        var params = [];
        if (!this.check(TT.RPAREN)) {
          do {
            if (this.isTypeKeyword() || this.check(TT.IDENTIFIER)) {
              var paramType = this.advance().value;
              if (this.check(TT.LBRACKET)) {
                this.advance();
                if (this.check(TT.RBRACKET)) { this.advance(); paramType += '[]'; }
              }
              var paramName = this.expectName();
              var defaultValue = null;
              if (this.match(TT.ASSIGN)) {
                defaultValue = this.parseExpression();
              }
              params.push({ type: paramType, name: paramName.value, defaultValue: defaultValue });
            }
          } while (this.match(TT.COMMA));
        }
        this.expect(TT.RPAREN);
        this.match(TT.SEMICOLON);
        return new AST.FunctionDeclAST('void', nameTok.value, params, [], absTok.line, absTok.col);
      }
    }
    this.errors.push({
      message: 'متوقع صنف أو دالة بعد مجرد',
      line: absTok.line,
      col: absTok.col
    });
    return null;
  };

  // ── P4: Interface Declaration ───────────────────────────────────────────────
  Parser.prototype.parseInterface = function () {
    var ifTok = this.advance(); // واجهة
    var nameTok = this.expectClassName();
    this.expect(TT.LBRACE);

    var body = [];
    while (!this.check(TT.RBRACE) && !this.check(TT.EOF)) {
      var access = 'public';
      if (this.check(TT.KW_PUBLIC)) { this.advance(); access = 'public'; }
      else if (this.check(TT.KW_PRIVATE)) { this.advance(); access = 'private'; }
      else if (this.check(TT.KW_PROTECTED)) { this.advance(); access = 'protected'; }

      if (this.check(TT.KW_FUNCTION)) {
        this.advance(); // دالة
        var fnName = this.expectName();
        this.expect(TT.LPAREN);
        var params = [];
        if (!this.check(TT.RPAREN)) {
          do {
            if (this.isTypeKeyword() || this.check(TT.IDENTIFIER)) {
              var paramType = this.advance().value;
              if (this.check(TT.LBRACKET)) {
                this.advance();
                if (this.check(TT.RBRACKET)) { this.advance(); paramType += '[]'; }
              }
              var paramName = this.expectName();
              params.push({ type: paramType, name: paramName.value, defaultValue: null });
            }
          } while (this.match(TT.COMMA));
        }
        this.expect(TT.RPAREN);
        this.match(TT.SEMICOLON);
        var fn = new AST.FunctionDeclAST('void', fnName.value, params, [], fnName.line, fnName.col);
        fn.access = access;
        body.push(fn);
      } else {
        this.errors.push({ message: 'متوقع دالة في الواجهة', line: this.peek().line, col: this.peek().col });
        this.advance();
      }
    }

    this.expect(TT.RBRACE);
    var node = new AST.ClassDeclAST(nameTok.value, body, ifTok.line, ifTok.col);
    node.isInterface = true;
    return node;
  };

  // ── Try-Catch Statement ─────────────────────────────────────────────────────
  Parser.prototype.parseTryCatch = function () {
    var tryTok = this.advance(); // حاول
    var tryBody = this.parseBlock().body || [];

    var catchVar = '';
    var catchBody = [];
    if (this.check(TT.KW_CATCH)) {
      this.advance();
      if (this.match(TT.LPAREN)) {
        // امسك (نوع اسم) or امسك (اسم)
        var typeOrName = this.expectName();
        if (this.isName()) {
          // Type + name: امسك (خطأ أ)
          catchVar = this.expectName().value;
        } else {
          // Just name: امسك (أ)
          catchVar = typeOrName.value;
        }
        this.expect(TT.RPAREN);
      }
      catchBody = this.parseBlock().body || [];
    }

    var finallyBody = null;
    if (this.check(TT.KW_FINALLY)) {
      this.advance();
      finallyBody = this.parseBlock().body || [];
    }

    return new AST.TryCatchStmtAST(tryBody, catchVar, catchBody, finallyBody, tryTok.line, tryTok.col);
  };

  // ── Throw Statement ─────────────────────────────────────────────────────────
  Parser.prototype.parseThrow = function () {
    var throwTok = this.advance(); // ارمِ
    var value = this.parseExpression();
    this.match(TT.SEMICOLON);
    return new AST.ThrowStmtAST(value, throwTok.line, throwTok.col);
  };

  // ── New Expression ──────────────────────────────────────────────────────────
  Parser.prototype.parseNewExpr = function () {
    var newTok = this.advance(); // جديد
    // P2/P3: Accept class name (IDENTIFIER or 5 context keywords)
    if (!this.isClassName()) {
      this.errors.push({
        message: 'متوقع: اسم صنف بعد صنف/يرث/جديد (مثال: جديد مستطيل())',
        line: newTok.line,
        col: newTok.col
      });
      return new AST.NewExprAST('__error', [], newTok.line, newTok.col);
    }
    var typeName = this.expectClassName().value;

    var args = [];
    if (this.match(TT.LPAREN)) {
      if (!this.check(TT.RPAREN)) {
        do {
          args.push(this.parseExpression());
        } while (this.match(TT.COMMA));
      }
      this.expect(TT.RPAREN);
    }

    return new AST.NewExprAST(typeName, args, newTok.line, newTok.col);
  };

  // ── Delete Statement ────────────────────────────────────────────────────────
  Parser.prototype.parseDelete = function () {
    var delTok = this.advance(); // احذف
    var operand = this.parseExpression();
    this.match(TT.SEMICOLON);
    return new AST.DeleteStmtAST(operand, delTok.line, delTok.col);
  };

  // ── Print Statement ─────────────────────────────────────────────────────────
  Parser.prototype.parsePrint = function () {
    var printTok = this.advance(); // طباعة
    this.expect(TT.LPAREN);
    var args = [];
    if (!this.check(TT.RPAREN)) {
      do {
        args.push(this.parseExpression());
      } while (this.match(TT.COMMA));
    }
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.PrintStmtAST(args, printTok.line, printTok.col);
  };

  // ── Input Statement: ادخل(متغير) ────────────────────────────────────────────
  Parser.prototype.parseInput = function () {
    var inputTok = this.advance(); // ادخل
    if (!this.check(TT.LPAREN)) {
      this.errors.push({
        message: "توقع قوس فاتح '(' بعد 'ادخل'",
        line: inputTok.line,
        col: inputTok.col
      });
      return null;
    }
    this.expect(TT.LPAREN);

    var target = null;
    var tok = this.peek();
    if (tok && (tok.type === TT.IDENTIFIER ||
                (tok.type === TT.KW_X || tok.type === TT.KW_Y))) {
      target = this.advance().value;
    } else {
      this.errors.push({
        message: "توقع اسم متغير داخل 'ادخل(...)'",
        line: inputTok.line,
        col: inputTok.col
      });
      return null;
    }

    // دعم عدة متغيرات: ادخل(أ، ب)
    var targets = [target];
    while (this.match(TT.COMMA)) {
      tok = this.peek();
      if (tok && tok.type === TT.IDENTIFIER) {
        targets.push(this.advance().value);
      } else {
        this.errors.push({
          message: "توقع اسم متغير بعد الفاصلة في 'ادخل(...)'",
          line: inputTok.line,
          col: inputTok.col
        });
        return null;
      }
    }

    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.InputStmtAST(targets, inputTok.line, inputTok.col);
  };

  // ── GUI Widget Declaration ──────────────────────────────────────────────────
  Parser.prototype.parseWidgetDecl = function () {
    var widgetTok = this.advance();
    var widgetType = widgetTok.value;
    var nameTok = this.expect(TT.IDENTIFIER);
    var init = null;

    if (this.match(TT.ASSIGN)) {
      init = this.parseExpression();
    }

    this.match(TT.SEMICOLON);
    return new AST.VarDeclStmtAST(widgetType, nameTok.value, init, widgetTok.line, widgetTok.col);
  };

  // ── Block { ... } ───────────────────────────────────────────────────────────
  Parser.prototype.parseBlock = function () {
    var startTok = this.peek();
    this.expect(TT.LBRACE);

    var body = [];
    while (!this.check(TT.RBRACE) && !this.check(TT.EOF)) {
      var stmt = this.parseStatement();
      if (stmt) body.push(stmt);
    }

    this.expect(TT.RBRACE);
    return new AST.ProgramAST(body, startTok.line, startTok.col);
  };

  // ── Identifier Statement ────────────────────────────────────────────────────
  Parser.prototype.parseIdentifierStmt = function () {
    var nameTok = this.peek();
    var name = nameTok.value;

    // Check if this is a type followed by identifier (variable declaration)
    // e.g. " صحيح x = 5 ;" — but we already handle type keywords above
    // So identifier here means it's a statement starting with an identifier

    // Could be: name = expr (assignment)
    //          name += expr (compound assignment)
    //          name(args) (function call)
    //          name.member (member access)
    //          name[index] (array subscript)
    //          name++ / name-- (increment/decrement)

    // Peek ahead to determine what kind of statement
    var next = this.peek(1);

    if (!next) {
      // Just a bare identifier as expression
      return this.parseExprStatement();
    }

    // Assignment: name = expr
    if (next.type === TT.ASSIGN) {
      this.advance(); // name
      this.advance(); // =
      var value = this.parseExpression();
      this.match(TT.SEMICOLON);
      return new AST.AssignmentAST(name, value, nameTok.line, nameTok.col);
    }

    // Compound assignment: name += -= *= /=
    if (next.type === TT.PLUS_ASSIGN || next.type === TT.MINUS_ASSIGN ||
        next.type === TT.STAR_ASSIGN || next.type === TT.SLASH_ASSIGN ||
        next.type === TT.POWER_ASSIGN) {
      this.advance(); // name
      var op = this.advance(); // operator
      var val = this.parseExpression();
      this.match(TT.SEMICOLON);
      return new AST.CompoundAssignmentAST(name, op.value, val, nameTok.line, nameTok.col);
    }

    // Increment/Decrement: name++ name--
    if (next.type === TT.PLUS_PLUS || next.type === TT.MINUS_MINUS) {
      this.advance(); // name
      var op2 = this.advance();
      this.match(TT.SEMICOLON);
      return new AST.CompoundAssignmentAST(name, op2.value === '++' ? '+=' : '-=', new AST.NumberExprAST(1, op2.line, op2.col), nameTok.line, nameTok.col);
    }

    // Member access or function call
    return this.parseExprStatement();
  };

  // ── Expression Statement ────────────────────────────────────────────────────
  Parser.prototype.parseExprStatement = function () {
    var expr = this.parseExpression();

    // Check for member assignment: obj.member = value  or  arr[idx] = value
    if (this.check(TT.ASSIGN) || this.check(TT.PLUS_ASSIGN) || this.check(TT.MINUS_ASSIGN) ||
        this.check(TT.STAR_ASSIGN) || this.check(TT.SLASH_ASSIGN) || this.check(TT.POWER_ASSIGN)) {

      if (expr.type === 'MemberAccessExpr') {
        var op = this.advance(); // = or += etc.
        var value = this.parseExpression();
        this.match(TT.SEMICOLON);
        if (op.type === TT.ASSIGN) {
          return new AST.MemberAssignmentAST(expr.object, expr.member, value, expr.line, expr.col);
        } else {
          // Compound: obj.member += val → obj.member = obj.member + val
          var binOp = op.value.charAt(0); // += → +
          var binExpr = new AST.BinaryExprAST(binOp, expr, value, expr.line, expr.col);
          return new AST.MemberAssignmentAST(expr.object, expr.member, binExpr, expr.line, expr.col);
        }
      }

      if (expr.type === 'ArraySubscriptExpr') {
        var op = this.advance();
        var value = this.parseExpression();
        this.match(TT.SEMICOLON);
        if (op.type === TT.ASSIGN) {
          return new AST.ArraySubscriptAssignAST(expr.name, expr.index, value, expr.line, expr.col);
        } else {
          var binOp = op.value.charAt(0);
          var fullExpr = new AST.ArraySubscriptExprAST(expr.name, expr.index, expr.line, expr.col);
          var binExpr = new AST.BinaryExprAST(binOp, fullExpr, value, expr.line, expr.col);
          return new AST.ArraySubscriptAssignAST(expr.name, expr.index, binExpr, expr.line, expr.col);
        }
      }

      if (expr.type === 'VariableExpr') {
        var op = this.advance();
        var value = this.parseExpression();
        this.match(TT.SEMICOLON);
        if (op.type === TT.ASSIGN) {
          return new AST.AssignmentAST(expr.name, value, expr.line, expr.col);
        } else {
          var binOp = op.value.charAt(0); // += → +
          var binExpr = new AST.BinaryExprAST(binOp, expr, value, expr.line, expr.col);
          return new AST.AssignmentAST(expr.name, binExpr, expr.line, expr.col);
        }
      }
    }

    this.match(TT.SEMICOLON);
    return new AST.ExprStmtAST(expr, expr.line, expr.col);
  };

  // ═══════════════════════════════════════════════════════════════════════════════
  // EXPRESSIONS (Precedence Climbing)
  // ═══════════════════════════════════════════════════════════════════════════════

  // ── Entry Point ─────────────────────────────────────────────────────────────
  Parser.prototype.parseExpression = function () {
    var expr = this.parseBinaryExpr(0);
    // Ternary: expr ? trueExpr : falseExpr
    if (this.check(TT.QUESTION)) {
      this.advance(); // ?
      var trueExpr = this.parseExpression();
      this.expect(TT.COLON);
      var falseExpr = this.parseExpression();
      return new AST.TernaryExprAST(expr, trueExpr, falseExpr, expr.line, expr.col);
    }
    return expr;
  };

  // ── Binary Expression (Precedence Climbing) ─────────────────────────────────
  var PRECEDENCE = {
    '||': 1, 'أو': 1,
    '&&': 2, 'و': 2,
    '==': 3, '!=': 3, '<': 4, '>': 4, '<=': 4, '>=': 4,
    '+': 5, '-': 5,
    '*': 6, '/': 6, '%': 6,
    '^': 7
  };

  Parser.prototype.parseBinaryExpr = function (minPrec) {
    var left = this.parseUnary();

    while (true) {
      var op = this.peek();
      if (!op) break;

      var prec = PRECEDENCE[op.value];
      if (prec === undefined || prec < minPrec) break;

      var opTok = this.advance();
      var right = this.parseBinaryExpr(prec + 1);
      left = new AST.BinaryExprAST(op.value, left, right, opTok.line, opTok.col);
    }

    return left;
  };

  // ── Unary Expression ────────────────────────────────────────────────────────
  Parser.prototype.parseUnary = function () {
    var t = this.peek();
    if (!t) return new AST.NullExprAST(0, 0);

    // -expr, !expr, ليس expr, --expr, ++expr
    if (t.type === TT.MINUS) {
      this.advance();
      var operand = this.parseUnary();
      return new AST.UnaryExprAST('-', operand, t.line, t.col);
    }
    if (t.type === TT.NOT) {
      this.advance();
      var operand = this.parseUnary();
      return new AST.UnaryExprAST('!', operand, t.line, t.col);
    }
    if (t.type === TT.KW_NOT) { // ليس
      this.advance();
      var operand = this.parseUnary();
      return new AST.UnaryExprAST('!', operand, t.line, t.col);
    }
    if (t.type === TT.PLUS_PLUS) {
      this.advance();
      var operand = this.parseUnary();
      return new AST.UnaryExprAST('++', operand, t.line, t.col);
    }
    if (t.type === TT.MINUS_MINUS) {
      this.advance();
      var operand = this.parseUnary();
      return new AST.UnaryExprAST('--', operand, t.line, t.col);
    }
    // Pointer dereference *x → just return the value
    if (t.type === TT.STAR) {
      this.advance();
      return this.parseUnary();
    }
    // Address-of &x → just return the value
    if (t.type === TT.AMPERSAND) {
      this.advance();
      return this.parseUnary();
    }

    return this.parsePostfix();
  };

  // ── Postfix Expression ──────────────────────────────────────────────────────
  Parser.prototype.parsePostfix = function () {
    var expr = this.parsePrimary();

    while (true) {
      var t = this.peek();
      if (!t) break;

      // Member access: .member (allow keywords too for method names like .احذف)
      if (t.type === TT.DOT) {
        this.advance();
        var memberTok = this.peek();
        if (memberTok && (memberTok.type === TT.IDENTIFIER || this.isKeyword(memberTok.type))) {
          var memberName = memberTok.value;
          this.advance();
          // Handle multi-word method names: .قص من → "قص من"
          if (memberName === 'قص' && this.peek() && this.peek().type === TT.IDENTIFIER && this.peek().value === 'من') {
            this.advance(); // skip "من"
            memberName = 'قص من';
          }
          expr = new AST.MemberAccessExprAST(expr, memberName, t.line, t.col);
          continue;
        }
        this.errors.push({ message: 'متوقع اسم العضو', line: t.line, col: t.col });
        continue;
      }

      // Double colon: ::member
      if (t.type === TT.DOUBLE_COLON) {
        this.advance();
        var member = this.expectName();
        expr = new AST.MemberAccessExprAST(expr, member.value, t.line, t.col);
        continue;
      }

      // Array subscript: [expr]
      if (t.type === TT.LBRACKET) {
        this.advance();
        var index = this.parseExpression();
        this.expect(TT.RBRACKET);
        if (expr.type === 'VariableExpr') {
          expr = new AST.ArraySubscriptExprAST(expr.name, index, t.line, t.col);
        } else if (expr.type === 'MemberAccessExpr') {
          // obj.field[i] — keep the member access as the array reference
          expr = new AST.ArraySubscriptExprAST(expr, index, t.line, t.col);
        }
        continue;
      }

      // Function call: (args)
      if (t.type === TT.LPAREN) {
        this.advance();
        var args = [];
        if (!this.check(TT.RPAREN)) {
          do {
            args.push(this.parseExpression());
          } while (this.match(TT.COMMA));
        }
        this.expect(TT.RPAREN);
        if (expr.type === 'VariableExpr') {
          expr = new AST.FunctionCallAST(expr.name, args, t.line, t.col);
        } else {
          expr = new AST.FunctionCallAST(expr, args, t.line, t.col);
        }
        continue;
      }

      // Postfix ++ or --
      if (t.type === TT.PLUS_PLUS || t.type === TT.MINUS_MINUS) {
        this.advance();
        expr = new AST.UnaryExprAST(t.value, expr, t.line, t.col);
        continue;
      }

      break;
    }

    return expr;
  };

  // ── Primary Expression ──────────────────────────────────────────────────────
  Parser.prototype.parsePrimary = function () {
    var t = this.peek();
    if (!t) {
      this.errors.push({ message: 'متوقع تعبير', line: 0, col: 0 });
      return new AST.NullExprAST(0, 0);
    }

    // Number literal
    if (t.type === TT.NUMBER) {
      this.advance();
      var val = t.value.indexOf('.') >= 0 || t.value.indexOf('e') >= 0 || t.value.indexOf('E') >= 0 || t.value.indexOf('x') >= 0
        ? parseFloat(t.value)
        : parseInt(t.value, t.value.indexOf('x') === 0 ? 16 : 10);
      return new AST.NumberExprAST(val, t.line, t.col);
    }

    // String literal
    if (t.type === TT.STRING) {
      this.advance();
      return new AST.StringExprAST(t.value, t.line, t.col);
    }

    // Boolean
    if (t.type === TT.KW_TRUE) { this.advance(); return new AST.BoolExprAST(true, t.line, t.col); }
    if (t.type === TT.KW_FALSE) { this.advance(); return new AST.BoolExprAST(false, t.line, t.col); }
    if (t.type === TT.KW_NULL) { this.advance(); return new AST.NullExprAST(t.line, t.col); }

    // هذا (self/this)
    if (t.type === TT.KW_SELF) { this.advance(); return new AST.VariableExprAST('this', t.line, t.col); }

    // new ClassName(args)
    if (t.type === TT.KW_NEW) {
      this.advance();
      // P2/P3: Accept class name (IDENTIFIER or 5 context keywords)
      if (!this.isClassName()) {
        this.errors.push({
          message: 'متوقع: اسم صنف بعد صنف/يرث/جديد (مثال: جديد مستطيل())',
          line: t.line,
          col: t.col
        });
        return new AST.NewExprAST('__error', [], t.line, t.col);
      }
      var typeName = this.expectClassName().value;
      var args = [];
      if (this.match(TT.LPAREN)) {
        if (!this.check(TT.RPAREN)) {
          do { args.push(this.parseExpression()); } while (this.match(TT.COMMA));
        }
        this.expect(TT.RPAREN);
      }
      return new AST.NewExprAST(typeName, args, t.line, t.col);
    }

    // sizeof / typeof
    if (t.type === TT.KW_SIZEOF) {
      this.advance();
      this.expect(TT.LPAREN);
      var expr = this.parseExpression();
      this.expect(TT.RPAREN);
      return new AST.SizeofExprAST(expr, t.line, t.col);
    }
    if (t.type === TT.KW_TYPEOF) {
      this.advance();
      this.expect(TT.LPAREN);
      var expr = this.parseExpression();
      this.expect(TT.RPAREN);
      return new AST.TypeofExprAST(expr, t.line, t.col);
    }

    // Identifier or variable
    if (t.type === TT.IDENTIFIER) {
      this.advance();
      return new AST.VariableExprAST(t.value, t.line, t.col);
    }

    // Keywords as variable references (e.g. طباعة(صحيح) where صحيح is a var name)
    if (this.isKeyword(t.type) && t.type !== TT.KW_TRUE && t.type !== TT.KW_FALSE &&
        t.type !== TT.KW_NULL && t.type !== TT.KW_SELF && t.type !== TT.KW_NEW &&
        t.type !== TT.KW_SIZEOF && t.type !== TT.KW_TYPEOF && t.type !== TT.KW_FUNCTION &&
        t.type !== TT.KW_NOT && t.type !== TT.KW_PRINT) {
      this.advance();
      return new AST.VariableExprAST(t.value, t.line, t.col);
    }

    // Parenthesized expression
    if (t.type === TT.LPAREN) {
      this.advance();
      var expr = this.parseExpression();
      this.expect(TT.RPAREN);
      return expr;
    }

    // Array literal: [expr, expr, ...]
    if (t.type === TT.LBRACKET) {
      this.advance();
      var elements = [];
      if (!this.check(TT.RBRACKET)) {
        do {
          elements.push(this.parseExpression());
        } while (this.match(TT.COMMA));
      }
      this.expect(TT.RBRACKET);
      // Represent as function call to Array constructor
      return new AST.FunctionCallAST('__array', elements, t.line, t.col);
    }

    // Unknown token — skip and report error
    this.errors.push({
      message: 'تعبير غير متوقع: "' + t.value + '"',
      line: t.line,
      col: t.col
    });
    this.advance();
    return new AST.NullExprAST(t.line, t.col);
  };

  // ── Import ─────────────────────────────────────────────────────────────────
  Parser.prototype.parseImport = function () {
    var tok = this.advance(); // استورد
    var path = this.expect(TT.STRING);
    this.match(TT.SEMICOLON);
    return new AST.ImportStmtAST(path.value, tok.line, tok.col);
  };

  // ── Export ─────────────────────────────────────────────────────────────────
  Parser.prototype.parseExport = function () {
    var tok = this.advance(); // صدّر
    // export [دالة] name, name, ...
    var names = [];
    this.match(TT.KW_FUNCTION); // optional دالة
    names.push(this.expectName().value);
    while (this.match(TT.COMMA)) {
      this.match(TT.KW_FUNCTION);
      names.push(this.expectName().value);
    }
    this.match(TT.SEMICOLON);
    return new AST.ExportStmtAST(names, tok.line, tok.col);
  };

  // ── Image Processing Parsers (20) ──────────────────────────────────────────

  // حمّل_صورة("path", متغير);
  Parser.prototype.parseLoadImage = function () {
    var tok = this.advance(); // حمّل_صورة
    this.expect(TT.LPAREN);
    var path = this.expect(TT.STRING);
    this.match(TT.COMMA);
    var varName = this.expectName();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.LoadImageAST(path.value, varName.value, tok.line, tok.col);
  };

  // ارسم_صورة(المتغير, x, y, width, height);
  Parser.prototype.parseDrawImage = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var x = this.parseExpression();
    this.match(TT.COMMA);
    var y = this.parseExpression();
    this.match(TT.COMMA);
    var w = this.parseExpression();
    this.match(TT.COMMA);
    var h = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.DrawImageAST(imgVar.value, x, y, w, h, tok.line, tok.col);
  };

  // حجم_صورة(المتغير);
  Parser.prototype.parseImageSize = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.ImageSizeAST(imgVar.value, tok.line, tok.col);
  };

  // احفظ_صورة(المتغير, "path");
  Parser.prototype.parseSaveImage = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var path = this.expect(TT.STRING);
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.SaveImageAST(imgVar.value, path.value, tok.line, tok.col);
  };

  // قص_صورة(المتغير, x, y, w, h);
  Parser.prototype.parseCropImage = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var x = this.parseExpression();
    this.match(TT.COMMA);
    var y = this.parseExpression();
    this.match(TT.COMMA);
    var w = this.parseExpression();
    this.match(TT.COMMA);
    var h = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.CropImageAST(imgVar.value, x, y, w, h, tok.line, tok.col);
  };

  // غيّر_حجم(المتغير, w, h);
  Parser.prototype.parseResize = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var w = this.parseExpression();
    this.match(TT.COMMA);
    var h = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.ResizeAST(imgVar.value, w, h, tok.line, tok.col);
  };

  // لف_صورة(المتغير, زاوية);
  Parser.prototype.parseRotateImage = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var angle = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.RotateImageAST(imgVar.value, angle, tok.line, tok.col);
  };

  // قلب_صورة(المتغير, "أفقي"/"عمودي");
  Parser.prototype.parseFlipImage = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var direction = this.expect(TT.STRING);
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.FlipImageAST(imgVar.value, direction.value, tok.line, tok.col);
  };

  // شفافية(المتغير, قيمة);
  Parser.prototype.parseOpacity = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var value = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.OpacityAST(imgVar.value, value, tok.line, tok.col);
  };

  // فلتر(المتغير, "اسم الفلتر");
  Parser.prototype.parseFilter = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var filterName = this.expect(TT.STRING);
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.FilterAST(imgVar.value, filterName.value, tok.line, tok.col);
  };

  //تراكب(المتغير1, المتغير2, x, y);
  Parser.prototype.parseOverlay = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar1 = this.expectName();
    this.match(TT.COMMA);
    var imgVar2 = this.expectName();
    this.match(TT.COMMA);
    var x = this.parseExpression();
    this.match(TT.COMMA);
    var y = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.OverlayAST(imgVar1.value, imgVar2.value, x, y, tok.line, tok.col);
  };

  // خلفية(المتغير, متغير_الخلفية);
  Parser.prototype.parseBackground = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var bgVar = this.expectName();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.BackgroundAST(imgVar.value, bgVar.value, tok.line, tok.col);
  };

  //بكسل(المتغير, x, y) — returns pixel value
  Parser.prototype.parsePixel = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var imgVar = this.expectName();
    this.match(TT.COMMA);
    var x = this.parseExpression();
    this.match(TT.COMMA);
    var y = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.PixelAST(imgVar.value, x, y, tok.line, tok.col);
  };

  // ارسم("نوع", args...);
  Parser.prototype.parseDrawStmt = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var shape = this.expect(TT.STRING);
    var args = [];
    while (this.match(TT.COMMA)) {
      args.push(this.parseExpression());
    }
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.DrawAST(shape.value, args, tok.line, tok.col);
  };

  // ملء("نوع", args...);
  Parser.prototype.parseFillStmt = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var shape = this.expect(TT.STRING);
    var args = [];
    while (this.match(TT.COMMA)) {
      args.push(this.parseExpression());
    }
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.FillAST(shape.value, args, tok.line, tok.col);
  };

  // مستطيل(x, y, w, h);
  Parser.prototype.parseRectangle = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var x = this.parseExpression();
    this.match(TT.COMMA);
    var y = this.parseExpression();
    this.match(TT.COMMA);
    var w = this.parseExpression();
    this.match(TT.COMMA);
    var h = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.RectangleAST(x, y, w, h, tok.line, tok.col);
  };

  // دائرة(x, y, نصف_القطر);
  Parser.prototype.parseCircle = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var x = this.parseExpression();
    this.match(TT.COMMA);
    var y = this.parseExpression();
    this.match(TT.COMMA);
    var r = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.CircleAST(x, y, r, tok.line, tok.col);
  };

  // خط(x1, y1, x2, y2);
  Parser.prototype.parseLine = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var x1 = this.parseExpression();
    this.match(TT.COMMA);
    var y1 = this.parseExpression();
    this.match(TT.COMMA);
    var x2 = this.parseExpression();
    this.match(TT.COMMA);
    var y2 = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.LineAST(x1, y1, x2, y2, tok.line, tok.col);
  };

  // نص_على_لوحة("النص", x, y, حجم_الخط);
  Parser.prototype.parseTextOnCanvas = function () {
    var tok = this.advance();
    this.expect(TT.LPAREN);
    var text = this.parseExpression();
    this.match(TT.COMMA);
    var x = this.parseExpression();
    this.match(TT.COMMA);
    var y = this.parseExpression();
    this.match(TT.COMMA);
    var fontSize = this.parseExpression();
    this.expect(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.TextOnCanvasAST(text, x, y, fontSize, tok.line, tok.col);
  };

  // مسح();
  Parser.prototype.parseClearCanvas = function () {
    var tok = this.advance();
    this.match(TT.LPAREN);
    this.match(TT.RPAREN);
    this.match(TT.SEMICOLON);
    return new AST.ClearAST(tok.line, tok.col);
  };

  // ── Exports ─────────────────────────────────────────────────────────────────
  return {
    Parser: Parser,
    PRECEDENCE: PRECEDENCE
  };
})();

if (typeof module !== 'undefined' && module.exports) {
  module.exports = DhadParser;
}
