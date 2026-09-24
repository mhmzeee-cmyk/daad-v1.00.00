#include "Daad/Parser.hpp"
#include <iostream>
#include <cstdlib>

namespace daad {

// ═══ isAnyKeyword: accepts TOKEN_KEYWORD + all TOKEN_KW_* types ═══
bool Parser::isAnyKeyword(TokenType type) {
    switch (type) {
        case TokenType::TOKEN_KEYWORD:
        // Data types
        case TokenType::TOKEN_KW_INT:
        case TokenType::TOKEN_KW_DOUBLE:
        case TokenType::TOKEN_KW_BOOL:
        case TokenType::TOKEN_KW_CHAR:
        case TokenType::TOKEN_KW_STRING:
        case TokenType::TOKEN_KW_CONST:
        case TokenType::TOKEN_KW_VOID:
        case TokenType::TOKEN_KW_AUTO:
        // Literals & typedef
        case TokenType::TOKEN_KW_TRUE:
        case TokenType::TOKEN_KW_FALSE:
        case TokenType::TOKEN_KW_NULLPTR:
        case TokenType::TOKEN_KW_TYPEDEF:
        // Control flow
        case TokenType::TOKEN_KW_IF:
        case TokenType::TOKEN_KW_ELSE:
        case TokenType::TOKEN_KW_WHILE:
        case TokenType::TOKEN_KW_DO:
        case TokenType::TOKEN_KW_FOR:
        case TokenType::TOKEN_KW_IN:
        case TokenType::TOKEN_KW_SWITCH:
        case TokenType::TOKEN_KW_CASE:
        case TokenType::TOKEN_KW_DEFAULT:
        case TokenType::TOKEN_KW_BREAK:
        case TokenType::TOKEN_KW_CONTINUE:
        case TokenType::TOKEN_KW_GOTO:
        case TokenType::TOKEN_KW_RETURN:
        // OOP & Scoping
        case TokenType::TOKEN_KW_CLASS:
        case TokenType::TOKEN_KW_STRUCT:
        case TokenType::TOKEN_KW_ENUM:
        case TokenType::TOKEN_KW_INTERFACE:
        case TokenType::TOKEN_KW_NAMESPACE:
        // Access & Inheritance
        case TokenType::TOKEN_KW_PUBLIC:
        case TokenType::TOKEN_KW_PRIVATE:
        case TokenType::TOKEN_KW_PROTECTED:
        case TokenType::TOKEN_KW_INHERIT:
        case TokenType::TOKEN_KW_SELF:
        case TokenType::TOKEN_KW_BASE:
        case TokenType::TOKEN_KW_ABSTRACT:
        // Functions & Memory
        case TokenType::TOKEN_KW_FUNCTION:
        case TokenType::TOKEN_KW_NEW:
        case TokenType::TOKEN_KW_DELETE:
        case TokenType::TOKEN_KW_POINTER:
        case TokenType::TOKEN_KW_REFERENCE:
        case TokenType::TOKEN_KW_STATIC:
        case TokenType::TOKEN_KW_INLINE:
        case TokenType::TOKEN_KW_EXTERN:
        case TokenType::TOKEN_KW_TEMPLATE:
        // Exception Handling
        case TokenType::TOKEN_KW_TRY:
        case TokenType::TOKEN_KW_CATCH:
        case TokenType::TOKEN_KW_FINALLY:
        case TokenType::TOKEN_KW_THROW:
        case TokenType::TOKEN_KW_ASSERT:
        case TokenType::TOKEN_KW_EXCEPTION:
        case TokenType::TOKEN_KW_TYPEOF:
        case TokenType::TOKEN_KW_SIZEOF:
        case TokenType::TOKEN_KW_INCREMENT:
        case TokenType::TOKEN_KW_DECREMENT:
        // Advanced Systems
        case TokenType::TOKEN_KW_SYNC:
        case TokenType::TOKEN_KW_AWAIT:
        case TokenType::TOKEN_KW_THREAD:
        case TokenType::TOKEN_KW_LOCK:
        case TokenType::TOKEN_KW_SHARED:
        case TokenType::TOKEN_KW_UNIQUE:
        case TokenType::TOKEN_KW_IMPORT:
        case TokenType::TOKEN_KW_EXPORT:
        case TokenType::TOKEN_KW_MODULE:
        case TokenType::TOKEN_KW_ALTERNATIVE:
        // GUI
        case TokenType::TOKEN_KW_BUTTON:
        case TokenType::TOKEN_KW_TEXTFIELD:
        case TokenType::TOKEN_KW_COMBOBOX:
        case TokenType::TOKEN_KW_IMAGE:
        case TokenType::TOKEN_KW_CHECKBOX:
        case TokenType::TOKEN_KW_SLIDER:
        case TokenType::TOKEN_KW_DROPDOWN:
        case TokenType::TOKEN_KW_PANEL:
        case TokenType::TOKEN_KW_LABEL:
        case TokenType::TOKEN_KW_COLUMN:
        case TokenType::TOKEN_KW_ROW:
        case TokenType::TOKEN_KW_GRID:
        case TokenType::TOKEN_KW_PROGRESSBAR:
        case TokenType::TOKEN_KW_TABBAR:
        // Print / Input
        case TokenType::TOKEN_KW_PRINT:
        case TokenType::TOKEN_KW_INPUT:
        // Arabic logic
        case TokenType::TOKEN_KW_AND_ARABIC:
        case TokenType::TOKEN_KW_OR_ARABIC:
        // Image Processing
        case TokenType::TOKEN_KW_LOAD_IMAGE:
        case TokenType::TOKEN_KW_DRAW_IMAGE:
        case TokenType::TOKEN_KW_IMAGE_SIZE:
        case TokenType::TOKEN_KW_SAVE_IMAGE:
        case TokenType::TOKEN_KW_CROP_IMAGE:
        case TokenType::TOKEN_KW_RESIZE:
        case TokenType::TOKEN_KW_ROTATE_IMAGE:
        case TokenType::TOKEN_KW_FLIP_IMAGE:
        case TokenType::TOKEN_KW_OPACITY:
        case TokenType::TOKEN_KW_FILTER:
        case TokenType::TOKEN_KW_OVERLAY:
        case TokenType::TOKEN_KW_BACKGROUND:
        case TokenType::TOKEN_KW_PIXEL:
        case TokenType::TOKEN_KW_DRAW:
        case TokenType::TOKEN_KW_FILL:
        case TokenType::TOKEN_KW_RECTANGLE:
        case TokenType::TOKEN_KW_CIRCLE:
        case TokenType::TOKEN_KW_LINE:
        case TokenType::TOKEN_KW_TEXT_ON_CANVAS:
        case TokenType::TOKEN_KW_CLEAR:
        // Coordinate axes
        case TokenType::TOKEN_KW_X:
        case TokenType::TOKEN_KW_Y:
            return true;
        default:
            return false;
    }
}

void Parser::consume() {
    m_currentToken = m_lexer.getNextToken();
}

void Parser::reportError(const std::string& message) {
    m_diag.report(Severity::Error, m_currentToken.line, m_currentToken.column, message, "");
}

bool Parser::isNextLeftParen() {
    Token saved = m_currentToken;
    size_t pos, line, col;
    m_lexer.saveState(pos, line, col);
    consume();
    bool result = (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN);
    m_currentToken = saved;
    m_lexer.restoreState(pos, line, col);
    return result;
}

std::vector<std::unique_ptr<StmtAST>> Parser::parseBlock() {
    std::vector<std::unique_ptr<StmtAST>> stmts;
    if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACE) {
        consume();
        while (m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE && m_currentToken.type != TokenType::TOKEN_EOF) {
            auto stmt = parseStatement();
            if (stmt) stmts.push_back(std::move(stmt));
        }
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACE) consume();
    }
    return stmts;
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    if (m_currentToken.type == TokenType::TOKEN_EOF) {
        return nullptr;
    }
    // ختم كل جملة بسطر رمزها الأول — أساس #line للتصحيح (F5)
    int stmtLine = static_cast<int>(m_currentToken.line);
    auto stmt = parseStatementImpl();
    if (stmt && stmt->line <= 0) {
        stmt->line = stmtLine;
    }
    return stmt;
}

std::unique_ptr<StmtAST> Parser::parseStatementImpl() {
    if (m_currentToken.type == TokenType::TOKEN_EOF) {
        return nullptr;
    }

    if (isAnyKeyword(m_currentToken.type)) {
        auto& kw = m_currentToken.text;

        // أنواع البيانات
        if (kw == "صحيح" || kw == "عشري" || kw == "حرف" ||
            kw == "منطقي" || kw == "فراغ" || kw == "نص" || kw == "تلقائي" ||
            kw == "عرّف" || kw == "مؤشر" || kw == "مرجع") {
            return parseVariableDeclaration();
        }

        // التحكم في التدفق
        if (kw == "إذا" || kw == "اذا") return parseIfStatement();
        if (kw == "طالما" || kw == "بينما") return parseWhileStatement();
        if (kw == "لكل" || kw == "كرر") return parseForEachOrForStatement();
        if (kw == "افعل") return parseDoWhileStatement();
        if (kw == "اختر") return parseSwitchStatement();
        if (kw == "ارجع") return parseReturnStatement();
        if (kw == "توقف" || kw == "اكسر") { consume(); if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume(); return std::make_unique<BreakStmtAST>(); }
        if (kw == "استمر" || kw == "تابع") { consume(); if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume(); return std::make_unique<ContinueStmtAST>(); }
        // 'انتقل' (GOTO) غير مدعومة: لا توجد labels في اللغة، والقبول الصامت كان
        // يولّد C++ غير صالح. نرفض بوضوح مع استرداد حتى نهاية الجملة.
        if (kw == "انتقل") {
            reportError("جملة 'انتقل' (القفز المباشر GOTO) غير مدعومة بعد في لغة ض");
            consume();
            while (m_currentToken.type != TokenType::TOKEN_SEMICOLON &&
                   m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE &&
                   m_currentToken.type != TokenType::TOKEN_EOF) {
                consume();
            }
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return nullptr;
        }

        // زد/انقص كجملة مستقلة: زد عداد ؛
        if (kw == "زد" || kw == "انقص") {
            std::string op = (kw == "زد") ? "+=" : "-=";
            consume();
            std::string varName;
            if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
                varName = m_currentToken.text;
                consume();
            } else if (isAnyKeyword(m_currentToken.type) &&
                       (m_currentToken.text == "س" || m_currentToken.text == "ص")) {
                varName = m_currentToken.text;
                consume();
            }
            if (!varName.empty()) {
                if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
                return std::make_unique<CompoundAssignmentAST>(varName, op, std::make_unique<NumberExprAST>(1.0));
            }
        }

        // تعريف الدوال
        if (kw == "دالة") return parseFunctionDefinition();

        // البرمجة كائنية التوجه
        if (kw == "نطاق") return parseNamespaceDeclaration();
        if (kw == "تعداد") return parseEnumDeclaration();
        if (kw == "قالب") return parseTemplateDeclaration();
        // مجرّد صنف → abstract class
        if (kw == "مجرّد") {
            consume(); // consume 'مجرّد'
            if (isAnyKeyword(m_currentToken.type) &&
                (m_currentToken.text == "صنف" || m_currentToken.text == "فئة")) {
                return parseClassDeclaration(true); // isAbstract=true
            }
            // Not followed by class keyword — error or treat as variable
            reportError("توقع 'صنف' أو 'فئة' بعد 'مجرّد'");
            return nullptr;
        }
        if (kw == "صنف" || kw == "فئة") return parseClassDeclaration();
        if (kw == "هيكل") return parseStructDeclaration();

        // الاستثناءات
        if (kw == "حاول") return parseTryCatchStatement();
        if (kw == "ارمِ") return parseThrowStatement();
        if (kw == "احذف") return parseDeleteStatement();
        if (kw == "استورد") return parseImportStatement();
        if (kw == "صدّر") return parseExportStatement();

        // طباعة
        if (kw == "طباعة") return parsePrintStatement();

        // الإدخال من المستخدم
        if (kw == "ادخل") return parseInputStatement();

        // كلمات الأوامر الصورية قد تُستخدم أيضًا كأسماء دوال عامة (مثل: مسح(مكدس))
        if (kw == "حمّل_صورة" || kw == "ارسم_صورة" || kw == "حجم_صورة" ||
            kw == "احفظ_صورة" || kw == "قص_صورة" || kw == "غيّر_حجم" ||
            kw == "لف_صورة" || kw == "قلب_صورة" || kw == "شفافية" ||
            kw == "فلتر" || kw == "تراكب" || kw == "خلفية" || kw == "بكسل" ||
            kw == "ارسم" || kw == "ملء" || kw == "مستطيل" || kw == "دائرة" ||
            kw == "خط" || kw == "نص_على_لوحة" || kw == "مسح") {
            if (isNextLeftParen()) {
                consume(); // الكلمة
                auto call = parseFunctionCallExpr(kw); // يستهلك '(' بنفسه
                if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
                return std::make_unique<ExprStmtAST>(std::move(call));
            }
        }

        // معالجة الصور (20) — يتوافق مع Web
        if (kw == "حمّل_صورة") return parseLoadImage();
        if (kw == "ارسم_صورة") return parseDrawImage();
        if (kw == "حجم_صورة") return parseImageSize();
        if (kw == "احفظ_صورة") return parseSaveImage();
        if (kw == "قص_صورة") return parseCropImage();
        if (kw == "غيّر_حجم") return parseResize();
        if (kw == "لف_صورة") return parseRotateImage();
        if (kw == "قلب_صورة") return parseFlipImage();
        if (kw == "شفافية") return parseOpacity();
        if (kw == "فلتر") return parseFilter();
        if (kw == "تراكب") return parseOverlay();
        if (kw == "خلفية") return parseBackground();
        if (kw == "بكسل") return parsePixel();
        if (kw == "ارسم") return parseDraw();
        if (kw == "ملء") return parseFill();
        if (kw == "مستطيل") return parseRectangle();
        if (kw == "دائرة") return parseCircle();
        if (kw == "خط") return parseLine();
        if (kw == "نص_على_لوحة") return parseTextOnCanvas();
        if (kw == "مسح") return parseClear();
    }

    if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        auto& name = m_currentToken.text;
        if (name == "طباعة") return parsePrintStatement();
        
        // Check if this identifier is a known class name (used as a type)
        if (m_knownClassNames.count(name)) {
            return parseVariableDeclaration();
        }

        std::string nameStr = m_currentToken.text;
        consume();

        // دمج مكونات الاسم المركب (مثل: إضافة_ خاصية)
        while (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
            nameStr += " " + m_currentToken.text;
            consume();
        }

        if (m_currentToken.type == TokenType::TOKEN_DOT) {
            consume();
            std::string member = m_currentToken.text;
            consume();
            if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
                consume();
                auto val = parseExpression();
                if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
                return std::make_unique<MemberAssignmentAST>(nameStr, member, std::move(val));
            }
            reportError("خطأ في الوصول للعضو: " + nameStr + "." + member);
            return nullptr;
        }

        if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
            return parseAssignment(nameStr);
        }
        if (m_currentToken.type == TokenType::TOKEN_PLUS_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_MINUS_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_STAR_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_SLASH_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_PERCENT_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_POWER_ASSIGN) {
            std::string op = m_currentToken.text;
            consume();
            auto val = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<CompoundAssignmentAST>(nameStr, op, std::move(val));
        }
        if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
            // جمع أدلة الفهرسة كاملة: a[i][j][k] = ...
            std::vector<std::unique_ptr<ExprAST>> indices;
            while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
                consume(); // [
                indices.push_back(parseExpression());
                if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
            }
            if (!indices.empty() && m_currentToken.type == TokenType::TOKEN_EQUALS) {
                consume(); // =
                auto val = parseExpression();
                if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
                if (indices.size() >= 2) {
                    // فهرسة متسلسلة: آخر قوس هو هدف التعيين، وما قبله base (يعرض a[i][j])
                    auto chain = std::make_unique<ArraySubscriptExprAST>(nameStr, std::move(indices[0]), nullptr);
                    for (size_t ci = 1; ci + 1 < indices.size(); ++ci) {
                        chain = std::make_unique<ArraySubscriptExprAST>(nameStr, std::move(indices[ci]), std::move(chain));
                    }
                    return std::make_unique<ArraySubscriptAssignAST>(nameStr, std::move(indices.back()), std::move(val), std::move(chain));
                }
                return std::make_unique<ArraySubscriptAssignAST>(nameStr, std::move(indices[0]), std::move(val));
            }
            reportError("خطأ في الوصول بالمصفوفة");
            return nullptr;
        }
        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            auto call = parseFunctionCallExpr(nameStr);
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<VarDeclStmtAST>("__builtin_call", "call", std::move(call));
        }
        reportError("المتغير '" + nameStr + "' غير معرّف أو ناقص");
    }

    // كلمات مفتاحية كمتغيرات في جملة (س = 10، ص = 20، خيط = ...، حالة = ...)
    // أي كلمة مفتاحية تصل هنا لم تُطابق فرعًا لغويًا أعلاه — تُعامل كاسم متغير/استدعاء
    if (isAnyKeyword(m_currentToken.type)) {
        std::string varName = m_currentToken.text;
        consume();

        if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
            return parseAssignment(varName);
        }
        if (m_currentToken.type == TokenType::TOKEN_PLUS_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_MINUS_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_STAR_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_SLASH_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_PERCENT_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_POWER_ASSIGN) {
            std::string op = m_currentToken.text;
            consume();
            auto val = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<CompoundAssignmentAST>(varName, op, std::move(val));
        }
        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            auto call = parseFunctionCallExpr(varName);
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<VarDeclStmtAST>("__builtin_call", "call", std::move(call));
        }
        if (m_currentToken.type == TokenType::TOKEN_DOT) {
            consume();
            std::string member = m_currentToken.text;
            consume();
            if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
                consume();
                auto val = parseExpression();
                if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
                return std::make_unique<MemberAssignmentAST>(varName, member, std::move(val));
            }
            reportError("خطأ في الوصول للعضو: " + varName + "." + member);
            return nullptr;
        }
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) {
            consume();
            return nullptr;
        }
        // Expression statement starting with a keyword like س/ص
        auto expr = parseExpression();
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        return std::make_unique<VarDeclStmtAST>("__builtin_call", "call", std::move(expr));
    }

    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) {
        consume();
        return nullptr;
    }

    reportError("لم يتم التعرف على بداية الجملة: " + m_currentToken.text);
    consume();
    return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseBinaryExpr(int precedence) {
    auto lhs = parseUnary();
    if (!lhs) return nullptr;

    while (true) {
        int prec = getCurrentPrecedence();
        if (prec < precedence) break;

        std::string op = m_currentToken.text;
        consume();
        auto rhs = parseBinaryExpr(prec + 1);
        lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<ExprAST> Parser::parseBinaryExprFrom(std::unique_ptr<ExprAST> lhs, int precedence) {
    if (!lhs) return nullptr;

    while (true) {
        int prec = getCurrentPrecedence();
        if (prec < precedence) break;

        std::string op = m_currentToken.text;
        consume();
        auto rhs = parseBinaryExpr(prec + 1);
        lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    auto expr = parseBinaryExpr(1);
    
    // ternary: expr ? trueExpr : falseExpr
    if (m_currentToken.type == TokenType::TOKEN_QUESTION) {
        consume(); // ?
        auto trueExpr = parseExpression();
        if (m_currentToken.type == TokenType::TOKEN_COLON || m_currentToken.text == ":") consume();
        auto falseExpr = parseExpression();
        return std::make_unique<TernaryExprAST>(std::move(expr), std::move(trueExpr), std::move(falseExpr));
    }
    
    return expr;
}

int Parser::getCurrentPrecedence() const {
    auto t = m_currentToken.type;
    if (t == TokenType::TOKEN_OR)               return 1;
    if (t == TokenType::TOKEN_AND)              return 2;
    if (isAnyKeyword(t) && m_currentToken.text == "أو")  return 1;
    if (isAnyKeyword(t) && m_currentToken.text == "و")   return 2;
    if (t == TokenType::TOKEN_EQUAL_EQUAL ||
        t == TokenType::TOKEN_NOT_EQUALS)       return 3;
    if (t == TokenType::TOKEN_LESS ||
        t == TokenType::TOKEN_GREATER ||
        t == TokenType::TOKEN_LESS_EQUALS ||
        t == TokenType::TOKEN_GREATER_EQUALS)   return 4;
    if (t == TokenType::TOKEN_PLUS ||
        t == TokenType::TOKEN_MINUS)            return 5;
    if (t == TokenType::TOKEN_STAR ||
        t == TokenType::TOKEN_SLASH ||
        t == TokenType::TOKEN_PERCENT)          return 6;
    if (t == TokenType::TOKEN_POWER)            return 7;  // ^ (قوة)
    return -1;
}

std::unique_ptr<ExprAST> Parser::parseUnary() {
    if (m_currentToken.type == TokenType::TOKEN_MINUS) {
        consume();
        auto operand = parseUnary();
        return std::make_unique<UnaryExprAST>("-", std::move(operand));
    }
    if (m_currentToken.type == TokenType::TOKEN_NOT) {
        consume();
        auto operand = parseUnary();
        return std::make_unique<UnaryExprAST>("!", std::move(operand));
    }
    if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "ليس") {
        consume();
        auto operand = parseUnary();
        return std::make_unique<UnaryExprAST>("!", std::move(operand));
    }
    return parsePrimary();
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    if (m_currentToken.type == TokenType::TOKEN_NUMBER) {
        double val = std::strtod(m_currentToken.text.c_str(), nullptr);
        consume();
        return std::make_unique<NumberExprAST>(val);
    }

    if (m_currentToken.type == TokenType::TOKEN_STRING) {
        std::string str = m_currentToken.text;
        consume();
        return std::make_unique<StringExprAST>(str);
    }

    if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        std::string name = m_currentToken.text;
        consume();

        // دمج مكونات الاسم المركب (مثل: إضافة_ خاصية)
        while (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
            name += " " + m_currentToken.text;
            consume();
        }

        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            return parseFunctionCallExpr(name);
        }

        if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
            consume(); // [
            auto idx = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
            auto expr = std::make_unique<ArraySubscriptExprAST>(name, std::move(idx));
            // فهرسة متسلسلة: a[i][0] ...
            while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
                consume(); // [
                auto idx2 = parseExpression();
                if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
                expr = std::make_unique<ArraySubscriptExprAST>(name, std::move(idx2), std::move(expr));
            }
            return expr;
        }

        if (m_currentToken.type == TokenType::TOKEN_DOT) {
            consume();
            std::string member = m_currentToken.text;
            consume();
            return std::make_unique<MemberAccessExprAST>(name, member);
        }

        return std::make_unique<VariableExprAST>(name);
    }

    // قيم منطقية — يجب أن تأتي قبل فحص TOKEN_KEYWORD العام
    if (isAnyKeyword(m_currentToken.type)) {
        if (m_currentToken.text == "صواب") {
            consume();
            return std::make_unique<BoolExprAST>(true);
        }
        if (m_currentToken.text == "خطأ") {
            consume();
            return std::make_unique<BoolExprAST>(false);
        }
        if (m_currentToken.text == "عدم") {
            consume();
            return std::make_unique<NullExprAST>();
        }

        // كلمات مفتاحية كمُنشئات (مثلاً: زر_ أمر()) أو كمتغيرات
        std::string name = m_currentToken.text;
        consume();
        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            return parseFunctionCallExpr(name);
        }
        // أي كلمة مفتاحية يمكن استخدامها كاسم متغير (مثل Web)
        return std::make_unique<VariableExprAST>(name);
    }

    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        consume();
        auto expr = parseExpression();
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        return expr;
    }

    // قائمة فارغة: [] → {} (تُستخدم لتهيئة مصفوفات فارغة)
    if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
        consume(); // [
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
        return std::make_unique<RawExprAST>("{}");
    }

    // ناقص أحادي handled by parseUnary()

    // محور الإحداثيات: س → x، ص → y
    if (m_currentToken.text == "س" && isAnyKeyword(m_currentToken.type)) {
        consume();
        return std::make_unique<VariableExprAST>("x");
    }
    if (m_currentToken.text == "ص" && isAnyKeyword(m_currentToken.type)) {
        consume();
        return std::make_unique<VariableExprAST>("y");
    }

    reportError("تعبير غير متوقع: " + m_currentToken.text);
    return std::make_unique<NumberExprAST>(0.0);
}

std::unique_ptr<ExprAST> Parser::parseFunctionCallExpr(const std::string& name) {
    consume(); // '('
    std::vector<std::unique_ptr<ExprAST>> args;

    if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
        args.push_back(parseExpression());
        while (m_currentToken.type == TokenType::TOKEN_COMMA) {
            consume();
            args.push_back(parseExpression());
        }
    }

    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    return std::make_unique<FunctionCallAST>(name, std::move(args));
}

std::unique_ptr<StmtAST> Parser::parseVariableDeclaration() {
    std::string type = m_currentToken.text;
    consume();

    // دعم أنواع المصفوفات (صحيح[], نص[], عشري[][])
    while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
        consume(); // [
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
        type = type + "[]";
    }

    // متغير يحمل اسم نوع (مثل: صحيح=1) — إسناد وليس تعريفًا
    if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
        return parseAssignment(type);
    }
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        auto call = parseFunctionCallExpr(type);
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        return std::make_unique<ExprStmtAST>(std::move(call));
    }

    // قراءة الاسم — يدعم الكلمات المفتاحية كأسماء متغيرات (مثل س، ص)
    std::string name;
    if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        name = m_currentToken.text;
        consume();
    } else if (isAnyKeyword(m_currentToken.type)) {
        name = m_currentToken.text;
        consume();
    } else {
        reportError("متوقع اسم متغير بعد النوع");
        name = "__error__";
    }

    // If followed by (, this is a function declaration
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        std::vector<std::pair<std::string, std::string>> params;
        consume(); // (
        if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
            auto [paramType, paramName] = parseParam();
            params.push_back({paramType, paramName});
            while (m_currentToken.type == TokenType::TOKEN_COMMA) {
                consume();
                auto [pt, pn] = parseParam();
                params.push_back({pt, pn});
            }
        }
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume(); // )
        std::string returnType = type;
        if (m_currentToken.type == TokenType::TOKEN_MINUS) {
            consume();
            if (m_currentToken.type == TokenType::TOKEN_GREATER) {
                consume();
                returnType = parseTypeName();
            }
        }
        auto body = parseBlock();
        return std::make_unique<FunctionDeclAST>(returnType, name, std::move(params), std::move(body));
    }

    if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
        consume();
        auto init = parseExpression();
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        return std::make_unique<VarDeclStmtAST>(type, name, std::move(init));
    }

    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<VarDeclStmtAST>(type, name, std::make_unique<NumberExprAST>(0.0));
}

std::unique_ptr<StmtAST> Parser::parseAssignment(const std::string& name) {
    consume(); // '='
    auto val = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<AssignmentAST>(name, std::move(val));
}

std::unique_ptr<StmtAST> Parser::parsePrintStatement() {
    consume(); // 'طباعة'

    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        consume();
        
        // Support multiple arguments: طباعة(expr1, expr2, expr3)
        std::vector<std::unique_ptr<ExprAST>> args;
        if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
            args.push_back(parseExpression());
            while (m_currentToken.type == TokenType::TOKEN_COMMA) {
                consume();
                args.push_back(parseExpression());
            }
        }
        
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        return std::make_unique<PrintStmtAST>(std::move(args));
    }

    auto arg = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::vector<std::unique_ptr<ExprAST>> args;
    args.push_back(std::move(arg));
    return std::make_unique<PrintStmtAST>(std::move(args));
}

std::unique_ptr<StmtAST> Parser::parseInputStatement() {
    consume(); // 'ادخل'

    if (m_currentToken.type != TokenType::TOKEN_LEFT_PAREN) {
        reportError("توقع قوس فاتح '(' بعد 'ادخل'");
        return std::make_unique<InputStmtAST>("");
    }
    consume(); // (

    if (m_currentToken.type != TokenType::TOKEN_IDENTIFIER &&
        !(isAnyKeyword(m_currentToken.type) &&
          (m_currentToken.text == "س" || m_currentToken.text == "ص"))) {
        reportError("توقع اسم متغير داخل 'ادخل(...)'");
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        return std::make_unique<InputStmtAST>("");
    }
    std::string varName = m_currentToken.text;
    consume();

    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<InputStmtAST>(varName);
}

std::unique_ptr<IfStmtAST> Parser::parseIfStatement() {
    consume(); // 'إذا'

    auto cond = parseExpression();

    auto thenBody = parseBlock();
    std::vector<std::unique_ptr<StmtAST>> elseBody;

    if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "وإلا") {
        consume();
        elseBody = parseBlock();
    }

    return std::make_unique<IfStmtAST>(std::move(cond), std::move(thenBody), std::move(elseBody));
}

std::unique_ptr<WhileStmtAST> Parser::parseWhileStatement() {
    consume(); // 'طالما'
    auto cond = parseExpression();
    auto body = parseBlock();
    return std::make_unique<WhileStmtAST>(std::move(cond), std::move(body));
}

std::unique_ptr<StmtAST> Parser::parseForEachOrForStatement() {
    consume(); // 'لكل' or 'كرر'
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) consume();

    // Check if this is a for-each: لكل (نوع اسم في تعبير)
    // We need to look ahead to detect "في" (in) keyword
    // Save current state
    // Simple heuristic: if we see a type keyword, then identifier, then "في" — it's for-each
    
    // Try to detect for-each pattern
    if (isAnyKeyword(m_currentToken.type) &&
        (m_currentToken.text == "صحيح" || m_currentToken.text == "عشري" ||
         m_currentToken.text == "حرف" || m_currentToken.text == "منطقي" ||
         m_currentToken.text == "نص" || m_currentToken.text == "تلقائي")) {
        // Could be for-each or C-style for init
        // Peek ahead: type identifier "في" → for-each
        // We need to check if after "type name" we see "في"
        std::string savedType = m_currentToken.text;
        consume(); // type
        std::string savedName = m_currentToken.text;
        consume(); // identifier
        
        if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "في") {
            // This is for-each!
            consume(); // في
            auto iterExpr = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
            auto body = parseBlock();
            return std::make_unique<ForEachStmtAST>(savedType, savedName, std::move(iterExpr), std::move(body));
        }
        
        // Not for-each — this is C-style for with type init
        // We already consumed type + name, now parse the rest
        std::unique_ptr<StmtAST> init;
        if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
            consume(); // '='
            auto val = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            init = std::make_unique<VarDeclStmtAST>(savedType, savedName, std::move(val));
        } else {
            // Put back as variable declaration
            init = std::make_unique<VarDeclStmtAST>(savedType, savedName, std::make_unique<NumberExprAST>(0.0));
        }
        
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        auto cond = parseExpression();
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        
        std::unique_ptr<StmtAST> update;
        if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER ||
            (isAnyKeyword(m_currentToken.type) &&
             (m_currentToken.text == "س" || m_currentToken.text == "ص"))) {
            std::string uName = m_currentToken.text;
            consume();
            if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
                update = parseAssignment(uName);
            } else if (m_currentToken.type == TokenType::TOKEN_PLUS_PLUS) {
                consume();
                update = std::make_unique<CompoundAssignmentAST>(uName, "+=", std::make_unique<NumberExprAST>(1.0));
            } else if (m_currentToken.type == TokenType::TOKEN_MINUS_MINUS) {
                consume();
                update = std::make_unique<CompoundAssignmentAST>(uName, "-=", std::make_unique<NumberExprAST>(1.0));
            } else if (m_currentToken.type == TokenType::TOKEN_PLUS_EQUALS ||
                       m_currentToken.type == TokenType::TOKEN_MINUS_EQUALS ||
                       m_currentToken.type == TokenType::TOKEN_STAR_EQUALS ||
                       m_currentToken.type == TokenType::TOKEN_SLASH_EQUALS ||
                       m_currentToken.type == TokenType::TOKEN_PERCENT_EQUALS ||
                       m_currentToken.type == TokenType::TOKEN_POWER_ASSIGN) {
                std::string op = m_currentToken.text;
                consume();
                auto val = parseExpression();
                update = std::make_unique<CompoundAssignmentAST>(uName, op, std::move(val));
            }
        } else if (isAnyKeyword(m_currentToken.type) &&
                   (m_currentToken.text == "زد" || m_currentToken.text == "انقص")) {
            std::string op = (m_currentToken.text == "زد") ? "+=" : "-=";
            consume();
            std::string uName;
            if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
                uName = m_currentToken.text;
                consume();
            } else if (isAnyKeyword(m_currentToken.type) &&
                       (m_currentToken.text == "س" || m_currentToken.text == "ص")) {
                uName = m_currentToken.text;
                consume();
            }
            if (!uName.empty()) {
                update = std::make_unique<CompoundAssignmentAST>(uName, op, std::make_unique<NumberExprAST>(1.0));
            }
        }
        
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        auto body = parseBlock();
        return std::make_unique<ForStmtAST>(std::move(init), std::move(cond), std::move(update), std::move(body));
    }
    
    // C-style for or for-each with identifier first (e.g., لكل (اسم في مصفوفة))
    if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        std::string name = m_currentToken.text;
        consume();
        
        if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "في") {
            // for-each: لكل (اسم في تعبير)
            consume(); // في
            auto iterExpr = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
            auto body = parseBlock();
            return std::make_unique<ForEachStmtAST>("تلقائي", name, std::move(iterExpr), std::move(body));
        }
        
        // C-style for with assignment init
        std::unique_ptr<StmtAST> init;
        if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
            init = parseAssignment(name);
        } else if (m_currentToken.type == TokenType::TOKEN_SEMICOLON || m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) {
            // Simplified for: كرر ( ا < 5 ) — no init, just condition
            // Put the consumed identifier back as the start of the condition expression
            auto startExpr = std::make_unique<VariableExprAST>(name);
            auto cond = parseBinaryExprFrom(std::move(startExpr), 1);
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
            auto body = parseBlock();
            return std::make_unique<ForStmtAST>(nullptr, std::move(cond), nullptr, std::move(body));
        } else {
            // Could be condition start: كرر ( ا < 5 )
            auto startExpr = std::make_unique<VariableExprAST>(name);
            auto cond = parseBinaryExprFrom(std::move(startExpr), 1);
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            
            std::unique_ptr<StmtAST> update;
            if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER ||
                (isAnyKeyword(m_currentToken.type) &&
                 (m_currentToken.text == "س" || m_currentToken.text == "ص"))) {
                std::string uName = m_currentToken.text;
                consume();
                if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
                    update = parseAssignment(uName);
                } else if (m_currentToken.type == TokenType::TOKEN_PLUS_PLUS) {
                    consume();
                    update = std::make_unique<CompoundAssignmentAST>(uName, "+=", std::make_unique<NumberExprAST>(1.0));
                } else if (m_currentToken.type == TokenType::TOKEN_MINUS_MINUS) {
                    consume();
                    update = std::make_unique<CompoundAssignmentAST>(uName, "-=", std::make_unique<NumberExprAST>(1.0));
                } else if (m_currentToken.type == TokenType::TOKEN_PLUS_EQUALS ||
                           m_currentToken.type == TokenType::TOKEN_MINUS_EQUALS ||
                           m_currentToken.type == TokenType::TOKEN_STAR_EQUALS ||
                           m_currentToken.type == TokenType::TOKEN_SLASH_EQUALS ||
                           m_currentToken.type == TokenType::TOKEN_PERCENT_EQUALS ||
                           m_currentToken.type == TokenType::TOKEN_POWER_ASSIGN) {
                    std::string op = m_currentToken.text;
                    consume();
                    auto val = parseExpression();
                    update = std::make_unique<CompoundAssignmentAST>(uName, op, std::move(val));
                }
            }
            
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
            auto body = parseBlock();
            return std::make_unique<ForStmtAST>(nullptr, std::move(cond), std::move(update), std::move(body));
        }
    }
    
    // Generic fallback
    std::unique_ptr<StmtAST> init;
    auto cond = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    auto update = std::unique_ptr<StmtAST>();
    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    auto body = parseBlock();
    return std::make_unique<ForStmtAST>(std::move(init), std::move(cond), std::move(update), std::move(body));
}

std::unique_ptr<DoWhileStmtAST> Parser::parseDoWhileStatement() {
    consume(); // 'افعل'
    auto body = parseBlock();

    if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "طالما") {
        consume();
    }

    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) consume();
    auto cond = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();

    return std::make_unique<DoWhileStmtAST>(std::move(cond), std::move(body));
}

std::unique_ptr<SwitchStmtAST> Parser::parseSwitchStatement() {
    consume(); // 'اختر'
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) consume();
    auto expr = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();

    std::vector<SwitchCaseAST> cases;
    std::vector<std::unique_ptr<StmtAST>> defaultBody;

    if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACE) {
        consume();
        while (m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE && m_currentToken.type != TokenType::TOKEN_EOF) {
            if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "حالة") {
                consume();
                auto caseVal = parseExpression();
                if (m_currentToken.type == TokenType::TOKEN_COLON || m_currentToken.text == ":") consume();
                SwitchCaseAST sc;
                sc.value = std::move(caseVal);
                while (m_currentToken.type != TokenType::TOKEN_KEYWORD &&
                       m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE &&
                       m_currentToken.type != TokenType::TOKEN_EOF) {
                    auto s = parseStatement();
                    if (s) sc.body.push_back(std::move(s));
                }
                cases.push_back(std::move(sc));
            } else if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "افتراضي") {
                consume();
                if (m_currentToken.type == TokenType::TOKEN_COLON || m_currentToken.text == ":") consume();
                while (m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE && m_currentToken.type != TokenType::TOKEN_EOF) {
                    auto s = parseStatement();
                    if (s) defaultBody.push_back(std::move(s));
                }
            } else {
                consume();
            }
        }
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACE) consume();
    }

    return std::make_unique<SwitchStmtAST>(std::move(expr), std::move(cases), std::move(defaultBody));
}

std::unique_ptr<ReturnStmtAST> Parser::parseReturnStatement() {
    consume(); // 'ارجع'
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) {
        consume();
        return std::make_unique<ReturnStmtAST>(nullptr);
    }
    auto val = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<ReturnStmtAST>(std::move(val));
}

std::unique_ptr<NamespaceDeclAST> Parser::parseNamespaceDeclaration() {
    consume(); // 'نطاق'
    std::string name = m_currentToken.text;
    consume();
    auto body = parseBlock();
    return std::make_unique<NamespaceDeclAST>(std::move(name), std::move(body));
}

std::unique_ptr<EnumDeclAST> Parser::parseEnumDeclaration() {
    consume(); // 'تعداد'
    std::string name = m_currentToken.text;
    consume();
    std::vector<std::string> values;
    if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACE) {
        consume();
        while (m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE && m_currentToken.type != TokenType::TOKEN_EOF) {
            if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER || isAnyKeyword(m_currentToken.type)) {
                values.push_back(m_currentToken.text);
                consume();
            }
            if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
        }
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACE) consume();
    }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<EnumDeclAST>(std::move(name), std::move(values));
}

std::unique_ptr<TryCatchStmtAST> Parser::parseTryCatchStatement() {
    consume(); // 'حاول'
    auto tryBody = parseBlock();

    std::string catchVar;
    std::vector<std::unique_ptr<StmtAST>> catchBody;
    std::vector<std::unique_ptr<StmtAST>> finallyBody;

    if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "امسك") {
        consume();
        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            consume();
            catchVar = m_currentToken.text;
            consume();
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        }
        catchBody = parseBlock();
    }

    // أخيراً (finally) block
    if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "أخيراً") {
        consume();
        finallyBody = parseBlock();
    }

    return std::make_unique<TryCatchStmtAST>(std::move(tryBody), std::move(catchVar),
                                              std::move(catchBody), std::move(finallyBody));
}

std::unique_ptr<StmtAST> Parser::parseTemplateDeclaration() {
    consume(); // 'قالب'

    // إذا لم يتبعها '<' فهي ليست إعلان قالب بل استخدام الكلمة كاسم متغير
    if (m_currentToken.type != TokenType::TOKEN_LESS) {
        // قالب = ... أو قالب( ... ) أو قالب بجملة أخرى
        std::string varName = "قالب";
        if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
            return parseAssignment(varName);
        }
        if (m_currentToken.type == TokenType::TOKEN_PLUS_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_MINUS_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_STAR_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_SLASH_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_PERCENT_EQUALS ||
            m_currentToken.type == TokenType::TOKEN_POWER_ASSIGN) {
            std::string op = m_currentToken.text;
            consume();
            auto val = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<CompoundAssignmentAST>(varName, op, std::move(val));
        }
        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            auto call = parseFunctionCallExpr(varName);
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<VarDeclStmtAST>("__builtin_call", "call", std::move(call));
        }
        reportError("الكلمة المحجوزة 'قالب' تستخدم بلا معنى: توقع '<' أو تعيين");
        return nullptr;
    }

    consume(); // '<'

    std::vector<std::string> params;
    while (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        params.push_back(m_currentToken.text);
        consume();
        if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
        else break;
    }

    if (m_currentToken.type == TokenType::TOKEN_GREATER) consume();
    auto body = parseStatement();
    return std::make_unique<TemplateDeclAST>("Template", std::move(params), std::move(body));
}

std::unique_ptr<FunctionDeclAST> Parser::parseFunctionDefinition() {
    consume(); // 'دالة'

    // اسم الدالة (يدعم النقطة المسبقة مثل .peek_ والمكونات بمسافة مثل إضافة_ خاصية)
    if (m_currentToken.type == TokenType::TOKEN_DOT) consume();
    std::string name = m_currentToken.text;
    consume();
    while (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        name += " " + m_currentToken.text;
        consume();
    }

    // الأقواس (اختيارية)
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) consume();

    // المعاملات
    std::vector<std::pair<std::string, std::string>> params;
    if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
        auto [paramType, paramName] = parseParam();
        params.push_back({paramType, paramName});

        while (m_currentToken.type == TokenType::TOKEN_COMMA) {
            consume();
            auto [pt, pn] = parseParam();
            params.push_back({pt, pn});
        }
    }

    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();

    // نوع الإرجاع (->)
    std::string returnType = "فراغ";
    if (m_currentToken.type == TokenType::TOKEN_MINUS) {
        consume(); // '-'
        if (m_currentToken.type == TokenType::TOKEN_GREATER) {
            consume(); // '>'
            returnType = parseTypeName();
        }
    }

    // جسم الدالة
    auto body = parseBlock();

    return std::make_unique<FunctionDeclAST>(returnType, name, std::move(params), std::move(body));
}

bool Parser::isKnownTypeName(const std::string& text) const {
    // لاحقات المصفوفات [] لا تغيّر كون الاسم نوعًا معروفًا (عشري[][] → عشري)
    std::string base = text;
    while (base.size() >= 2 && base.compare(base.size() - 2, 2, "[]") == 0) {
        base = base.substr(0, base.size() - 2);
    }
    if (m_knownClassNames.count(base)) return true;
    return base == "صحيح" || base == "عشري" || base == "منطقي" || base == "حرف" ||
           base == "نص" || base == "فراغ" || base == "تلقائي" || base == "دالة" ||
           base == "قالب" || base == "مهمة";
}

std::string Parser::parseTypeName() {
    std::string type = m_currentToken.text;
    consume();
    while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
        consume(); // [
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
        type += "[]";
    }
    return type;
}

std::pair<std::string, std::string> Parser::parseParam() {
    // معامل فارغ: [] (عنصر نائب بدون نوع)
    if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
        consume(); // [
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
        return {"فراغ", ""};
    }

    // تجميع كل كلمات المعامل (مع لاحقات المصفوفات [])
    std::vector<std::string> words;
    while (true) {
        if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER ||
            isAnyKeyword(m_currentToken.type)) {
            std::string word = m_currentToken.text;
            consume();
            while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
                consume(); // [
                if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
                word += "[]";
            }
            words.push_back(word);
        } else {
            break;
        }
    }

    if (words.empty()) return {"صحيح", "عدم"};

    // كلمة واحدة فقط: تُعامل كاسم معامل بالنوع الافتراضي (صحيح)
    // كان هذا يسبب وصولًا لـ words[1] خارج الحدود (انهيار segfault)
    if (words.size() == 1) {
        return {"صحيح", words[0]};
    }

    // القسمة من النهاية: آخر كلمة معروفة نوعًا هي النوع، والباقي الاسم المركب
    size_t last = words.size() - 1;
    std::string type, name;
    if (isKnownTypeName(words[last])) {
        type = words[last];
        name = words[0];
        for (size_t i = 1; i < last; ++i) name += " " + words[i];
    } else if (isKnownTypeName(words[0])) {
        type = words[0];
        name = words[1];
        for (size_t i = 2; i < words.size(); ++i) name += " " + words[i];
    } else {
        // لا نوع معروف: أول كلمة نوع (افتراضي) والباقي اسم مركب
        type = words[0];
        name = words[1];
        for (size_t i = 2; i < words.size(); ++i) name += " " + words[i];
    }
    return {type, name};
}

std::unique_ptr<ClassDeclAST> Parser::parseClassDeclaration(bool isAbstract) {
    consume(); // 'صنف' or 'فئة'
    std::string name = m_currentToken.text;
    consume(); // class name
    
    // Register class name as a known type
    m_knownClassNames.insert(name);
    
    if (m_currentToken.type != TokenType::TOKEN_LEFT_BRACE) {
        reportError("متوقع '{' بعد اسم الفئة");
        return std::make_unique<ClassDeclAST>(name);
    }
    consume(); // {
    
    std::vector<std::pair<std::string, std::string>> members;
    std::vector<std::unique_ptr<FunctionDeclAST>> methods;
    std::vector<AccessLevel> memberAccess;
    std::vector<AccessLevel> methodAccess;
    
    // Default access for class is private (C++ convention)
    AccessLevel currentAccess = AccessLevel::Private;
    
    while (true) {
        // الإنهاء الموثوق في كل الحالات: '}' ينهي الصف و'EOF' يتوقف
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACE) break;
        if (m_currentToken.type == TokenType::TOKEN_EOF) break;

        // ── Access specifiers: خاص / عام / محمي (اختيارياً بـ ':' ) ──
        if (isAnyKeyword(m_currentToken.type)) {
            if (m_currentToken.text == "خاص") {
                currentAccess = AccessLevel::Private;
                consume();
                if (m_currentToken.type == TokenType::TOKEN_COLON) consume(); // optional ':'
                continue;
            }
            if (m_currentToken.text == "عام") {
                currentAccess = AccessLevel::Public;
                consume();
                if (m_currentToken.type == TokenType::TOKEN_COLON) consume(); // optional ':'
                continue;
            }
            if (m_currentToken.text == "محمي") {
                currentAccess = AccessLevel::Protected;
                consume();
                if (m_currentToken.type == TokenType::TOKEN_COLON) consume(); // optional ':'
                continue;
            }
        }

        auto isNameLike = [this]() {
            return isAnyKeyword(m_currentToken.type) ||
                   m_currentToken.type == TokenType::TOKEN_IDENTIFIER;
        };
        if (!isNameLike()) {
            reportError("رمز غير متوقع في جسم الصف: " + m_currentToken.text);
            skipUntilMemberEnd();
            continue;
        }

        // ── مجرّد دالة داخل الصف ──
        bool isMethodAbstract = false;
        if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "مجرّد") {
            isMethodAbstract = true;
            consume(); // consume 'مجرّد'
        }

        std::string first = m_currentToken.text;
        (void)m_currentToken.line; // suppress unused warning, line tracked elsewhere
        consume(); // first token (نوع أو اسم)

        // ── الصيغة (2): الاسم : النوع  —  الاسم ثم ':' ثم النوع
        if (m_currentToken.type == TokenType::TOKEN_COLON) {
            consume(); // :
            if (!isNameLike()) {
                reportError("متوقع نوع العضو بعد ':' في الصف " + name);
                skipUntilMemberEnd();
                continue;
            }
            std::string memberType = m_currentToken.text;
            size_t typeLine = m_currentToken.line;
            consume();
            // دعم مصفوفة: الاسم : النوع[]
            while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
                consume(); // [
                if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
                memberType += "[]";
            }
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) {
                consume(); // ;
                members.push_back({memberType, first});
                memberAccess.push_back(currentAccess);
            } else if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
                // طريقة على الصيغة: الاسم : النوع (معاملات) { ... }  — غير مدعومة، خطأ واضح
                reportError("طريقة الصف بصيغة 'الاسم : النوع' غير مدعومة، استخدم 'النوع الاسم (معاملات)'");
                skipUntilMemberEnd();
            } else if (m_currentToken.line > typeLine) {
                // سطر جديد يفصل الأعضاء؛ الفاصلة المنقوطة اختيارية
                members.push_back({memberType, first});
                memberAccess.push_back(currentAccess);
            } else {
                reportError("متوقع ';' أو نهاية سطر بعد تعريف العضو: " + first);
                skipUntilMemberEnd();
            }
            continue;
        }

        // ── الصيغة (1): النوع الاسم  أو  النوع[] الاسم  أو  النوع الاسم(معاملات){...}
        // ── Special case: "دالة" keyword inside class = method definition
        if (first == "دالة") {
            // Method definition using "دالة" keyword: دالة اسم(معاملات) { ... }
            if (!isNameLike()) {
                reportError("متوقع اسم الطريقة بعد 'دالة'");
                skipUntilMemberEnd();
                continue;
            }
            std::string methodName = m_currentToken.text;
            consume(); // method name

            if (m_currentToken.type != TokenType::TOKEN_LEFT_PAREN) {
                reportError("متوقع '(' بعد اسم الطريقة: " + methodName);
                skipUntilMemberEnd();
                continue;
            }
            consume(); // (
            std::vector<std::pair<std::string, std::string>> params;
            auto isNameAndTypeFn = [this]() {
                return isAnyKeyword(m_currentToken.type) ||
                       m_currentToken.type == TokenType::TOKEN_IDENTIFIER;
            };
            if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
                while (isNameAndTypeFn()) {
                    auto [pt, pn] = parseParam();
                    params.push_back({pt, pn});
                    if (m_currentToken.type == TokenType::TOKEN_COMMA) {
                        consume();
                        continue;
                    }
                    break;
                }
            }
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume(); // )
            std::string returnType = "فراغ";
            if (m_currentToken.type == TokenType::TOKEN_MINUS) {
                consume();
                if (m_currentToken.type == TokenType::TOKEN_GREATER) {
                    consume();
                    if (isNameAndTypeFn()) {
                        returnType = parseTypeName();
                    }
                }
            }
            // Check for pure virtual: مجرّد دالة → body is just ';' 
            bool methodAbstract = isMethodAbstract;
            auto body = parseBlock();
            // For pure virtual: consume trailing ';' if present
            if (methodAbstract && m_currentToken.type == TokenType::TOKEN_SEMICOLON) {
                consume(); // consume ';' after pure virtual declaration
            }
            auto method = std::make_unique<FunctionDeclAST>(returnType, methodName, std::move(params), std::move(body));
            method->isAbstract = methodAbstract;
            methodAccess.push_back(currentAccess);
            methods.push_back(std::move(method));
            continue;
        }

        std::string memberType = first;
        // دعم مصفوفة: النوع[] الاسم  (بما في ذلك [][])
        while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
            consume(); // [
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
            memberType += "[]";
        }

        if (!isNameLike()) {
            reportError("متوقع اسم العضو بعد النوع: " + memberType);
            skipUntilMemberEnd();
            continue;
        }
        std::string memberName = m_currentToken.text;
        size_t nameLine = m_currentToken.line;
        consume(); // member name

        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) {
            consume(); // ;
            members.push_back({memberType, memberName});
            memberAccess.push_back(currentAccess);
            continue;
        }

        if (m_currentToken.line > nameLine) {
            // سطر جديد يفصل الأعضاء؛ الفاصلة المنقوطة اختيارية
            members.push_back({memberType, memberName});
            memberAccess.push_back(currentAccess);
            continue;
        }

        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            // ── طريقة: النوع الاسم (معاملات) { ... }
            consume(); // (
            std::vector<std::pair<std::string, std::string>> params;
            auto isNameAndType = [this]() {
                return isAnyKeyword(m_currentToken.type) ||
                       m_currentToken.type == TokenType::TOKEN_IDENTIFIER;
            };
            if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
                while (isNameAndType()) {
                    auto [pt, pn] = parseParam();
                    params.push_back({pt, pn});
                    if (m_currentToken.type == TokenType::TOKEN_COMMA) {
                        consume();
                        continue;
                    }
                    break;
                }
            }
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume(); // )
            std::string returnType = memberType;
            if (m_currentToken.type == TokenType::TOKEN_MINUS) {
                consume();
                if (m_currentToken.type == TokenType::TOKEN_GREATER) {
                    consume();
                    if (isNameAndType()) {
                        returnType = parseTypeName();
                    }
                }
            }
            auto body = parseBlock();
            auto method = std::make_unique<FunctionDeclAST>(returnType, memberName, std::move(params), std::move(body));
            method->isAbstract = isMethodAbstract;
            methodAccess.push_back(currentAccess);
            methods.push_back(std::move(method));
            continue;
        }

        reportError("متوقع ';' أو '(' بعد اسم العضو: " + memberName);
        skipUntilMemberEnd();
    }
    
    if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACE) consume(); // }
    
    auto classDecl = std::make_unique<ClassDeclAST>(name, std::move(members), std::move(methods));
    classDecl->isAbstract = isAbstract;
    classDecl->memberAccess = std::move(memberAccess);
    classDecl->methodAccess = std::move(methodAccess);
    return classDecl;
}

void Parser::skipUntilMemberEnd() {
    // يتجاوز الرموز حتى نهاية تعريف العضو: ';' أو '}' أو نهاية الملف — لا يبتلع الملف أبدًا
    while (m_currentToken.type != TokenType::TOKEN_SEMICOLON &&
           m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE &&
           m_currentToken.type != TokenType::TOKEN_EOF) {
        consume();
    }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
}

std::unique_ptr<StructDeclAST> Parser::parseStructDeclaration() {
    consume(); // 'هيكل'
    if (m_currentToken.type != TokenType::TOKEN_IDENTIFIER && !isAnyKeyword(m_currentToken.type)) {
        reportError("متوقع اسم الهيكل بعد 'هيكل'");
        return std::make_unique<StructDeclAST>("__error__");
    }
    std::string name = m_currentToken.text;
    consume();
    m_knownClassNames.insert(name);

    if (m_currentToken.type != TokenType::TOKEN_LEFT_BRACE) {
        reportError("متوقع '{' بعد اسم الهيكل");
        return std::make_unique<StructDeclAST>(name);
    }
    consume(); // {

    ParamList members;
    while (m_currentToken.type != TokenType::TOKEN_RIGHT_BRACE && m_currentToken.type != TokenType::TOKEN_EOF) {
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) { consume(); continue; }
        // member: type name ;
        // type may be like صحيح, عشري, نص, or array type with []
        std::string type;
        if (isAnyKeyword(m_currentToken.type) || m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
            type = m_currentToken.text;
            consume();
            while (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
                consume(); // [
                if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
                type += "[]";
            }
        } else {
            reportError("متوقع نوع العضو في الهيكل");
            skipUntilMemberEnd();
            continue;
        }
        if (m_currentToken.type != TokenType::TOKEN_IDENTIFIER && !isAnyKeyword(m_currentToken.type)) {
            reportError("متوقع اسم العضو بعد النوع: " + type);
            skipUntilMemberEnd();
            continue;
        }
        std::string memName = m_currentToken.text;
        consume();
        // optional ; or newline
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
        members.push_back({type, memName});
    }

    if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACE) consume(); // }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume(); // optional ;

    return std::make_unique<StructDeclAST>(name, std::move(members));
}

std::unique_ptr<ForEachStmtAST> Parser::parseForEachStatement() {
    consume(); // 'لكل'
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) consume();
    
    std::string varType = "تلقائي";
    if (isAnyKeyword(m_currentToken.type) &&
        (m_currentToken.text == "صحيح" || m_currentToken.text == "عشري" ||
         m_currentToken.text == "حرف" || m_currentToken.text == "منطقي" ||
         m_currentToken.text == "نص" || m_currentToken.text == "تلقائي")) {
        varType = m_currentToken.text;
        consume();
    }
    
    std::string varName = m_currentToken.text;
    consume();
    
    if (isAnyKeyword(m_currentToken.type) && m_currentToken.text == "في") {
        consume();
    }
    
    auto iterExpr = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    auto body = parseBlock();
    
    return std::make_unique<ForEachStmtAST>(varType, varName, std::move(iterExpr), std::move(body));
}

std::unique_ptr<ThrowStmtAST> Parser::parseThrowStatement() {
    consume(); // 'ارمِ'
    std::unique_ptr<ExprAST> val;
    if (m_currentToken.type != TokenType::TOKEN_SEMICOLON) {
        val = parseExpression();
    }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<ThrowStmtAST>(std::move(val));
}

std::unique_ptr<DeleteStmtAST> Parser::parseDeleteStatement() {
    consume(); // 'احذف'
    auto operand = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<DeleteStmtAST>(std::move(operand));
}

std::unique_ptr<ImportStmtAST> Parser::parseImportStatement() {
    consume(); // 'استورد'
    std::string path;
    if (m_currentToken.type == TokenType::TOKEN_STRING) {
        path = m_currentToken.text;
        consume();
    }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<ImportStmtAST>(std::move(path));
}

std::unique_ptr<ExportStmtAST> Parser::parseExportStatement() {
    consume(); // 'صدّر'
    std::vector<std::string> names;
    if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER || isAnyKeyword(m_currentToken.type)) {
        names.push_back(m_currentToken.text);
        consume();
        while (m_currentToken.type == TokenType::TOKEN_COMMA) {
            consume();
            names.push_back(m_currentToken.text);
            consume();
        }
    }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<ExportStmtAST>(std::move(names));
}

// ── Image Processing Parsers (20) — يتوافق مع Web ──────────────────────────

std::unique_ptr<StmtAST> Parser::parseLoadImage() {
    consume(); // 'حمّل_صورة'
    auto path = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    std::string varName = m_currentToken.text;
    consume();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    // Convert path expression to string for LoadImageAST
    std::string pathStr;
    if (auto strExpr = dynamic_cast<StringExprAST*>(path.get())) {
        pathStr = strExpr->value;
    } else {
        pathStr = "__dynamic_path__";
    }
    return std::make_unique<LoadImageAST>(pathStr, varName);
}

std::unique_ptr<StmtAST> Parser::parseDrawImage() {
    consume(); // 'ارسم_صورة'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto x = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto w = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto h = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<DrawImageAST>(imgVarStr, std::move(x), std::move(y), std::move(w), std::move(h));
}

std::unique_ptr<StmtAST> Parser::parseImageSize() {
    consume(); // 'حجم_صورة'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<ExprStmtAST>(std::make_unique<ImageSizeAST>(imgVarStr));
}

std::unique_ptr<StmtAST> Parser::parseSaveImage() {
    consume(); // 'احفظ_صورة'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto path = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    std::string pathStr;
    if (auto strExpr = dynamic_cast<StringExprAST*>(path.get())) {
        pathStr = strExpr->value;
    } else {
        pathStr = "__dynamic_path__";
    }
    return std::make_unique<SaveImageAST>(imgVarStr, pathStr);
}

std::unique_ptr<StmtAST> Parser::parseCropImage() {
    consume(); // 'قص_صورة'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto x = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto w = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto h = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<CropImageAST>(imgVarStr, std::move(x), std::move(y), std::move(w), std::move(h));
}

std::unique_ptr<StmtAST> Parser::parseResize() {
    consume(); // 'غيّر_حجم'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto w = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto h = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<ResizeAST>(imgVarStr, std::move(w), std::move(h));
}

std::unique_ptr<StmtAST> Parser::parseRotateImage() {
    consume(); // 'لف_صورة'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto angle = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<RotateImageAST>(imgVarStr, std::move(angle));
}

std::unique_ptr<StmtAST> Parser::parseFlipImage() {
    consume(); // 'قلب_صورة'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    std::string direction = m_currentToken.text;
    consume();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<FlipImageAST>(imgVarStr, direction);
}

std::unique_ptr<StmtAST> Parser::parseOpacity() {
    consume(); // 'شفافية'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto value = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<OpacityAST>(imgVarStr, std::move(value));
}

std::unique_ptr<StmtAST> Parser::parseFilter() {
    consume(); // 'فلتر'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    std::string filterName = m_currentToken.text;
    consume();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<FilterAST>(imgVarStr, filterName);
}

std::unique_ptr<StmtAST> Parser::parseOverlay() {
    consume(); // 'تراكب'
    auto imgVar1 = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto imgVar2 = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto x = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVar1Str, imgVar2Str;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar1.get())) {
        imgVar1Str = varExpr->name;
    } else {
        imgVar1Str = "__img1__";
    }
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar2.get())) {
        imgVar2Str = varExpr->name;
    } else {
        imgVar2Str = "__img2__";
    }
    return std::make_unique<OverlayAST>(imgVar1Str, imgVar2Str, std::move(x), std::move(y));
}

std::unique_ptr<StmtAST> Parser::parseBackground() {
    consume(); // 'خلفية'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto bgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr, bgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    if (auto varExpr = dynamic_cast<VariableExprAST*>(bgVar.get())) {
        bgVarStr = varExpr->name;
    } else {
        bgVarStr = "__bg__";
    }
    return std::make_unique<BackgroundAST>(imgVarStr, bgVarStr);
}

std::unique_ptr<StmtAST> Parser::parsePixel() {
    consume(); // 'بكسل'
    auto imgVar = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto x = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    std::string imgVarStr;
    if (auto varExpr = dynamic_cast<VariableExprAST*>(imgVar.get())) {
        imgVarStr = varExpr->name;
    } else {
        imgVarStr = "__img__";
    }
    return std::make_unique<ExprStmtAST>(std::make_unique<PixelAST>(imgVarStr, std::move(x), std::move(y)));
}

std::unique_ptr<StmtAST> Parser::parseDraw() {
    consume(); // 'ارسم'
    std::string shape = m_currentToken.text;
    consume();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        consume();
        if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
            args.push_back(parseExpression());
            while (m_currentToken.type == TokenType::TOKEN_COMMA) {
                consume();
                args.push_back(parseExpression());
            }
        }
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<DrawAST>(shape, std::move(args));
}

std::unique_ptr<StmtAST> Parser::parseFill() {
    consume(); // 'ملء'
    std::string shape = m_currentToken.text;
    consume();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        consume();
        if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
            args.push_back(parseExpression());
            while (m_currentToken.type == TokenType::TOKEN_COMMA) {
                consume();
                args.push_back(parseExpression());
            }
        }
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    }
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<FillAST>(shape, std::move(args));
}

std::unique_ptr<StmtAST> Parser::parseRectangle() {
    consume(); // 'مستطيل'
    auto x = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto w = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto h = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<RectangleAST>(std::move(x), std::move(y), std::move(w), std::move(h));
}

std::unique_ptr<StmtAST> Parser::parseCircle() {
    consume(); // 'دائرة'
    auto x = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto r = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<CircleAST>(std::move(x), std::move(y), std::move(r));
}

std::unique_ptr<StmtAST> Parser::parseLine() {
    consume(); // 'خط'
    auto x1 = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y1 = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto x2 = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y2 = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<LineAST>(std::move(x1), std::move(y1), std::move(x2), std::move(y2));
}

std::unique_ptr<StmtAST> Parser::parseTextOnCanvas() {
    consume(); // 'نص_على_لوحة'
    std::string text = m_currentToken.text;
    consume();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto x = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto y = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_COMMA) consume();
    auto fontSize = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<TextOnCanvasAST>(text, std::move(x), std::move(y), std::move(fontSize));
}

std::unique_ptr<StmtAST> Parser::parseClear() {
    consume(); // 'مسح'
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<ClearAST>();
}

} // namespace daad
