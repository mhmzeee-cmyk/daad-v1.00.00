#include "Daad/Parser.hpp"
#include <iostream>
#include <cstdlib>

namespace daad {

void Parser::consume() {
    m_currentToken = m_lexer.getNextToken();
}

void Parser::reportError(const std::string& message) {
    m_diag.report(Severity::Error, m_currentToken.line, m_currentToken.column, message, "");
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

    if (m_currentToken.type == TokenType::TOKEN_KEYWORD) {
        auto& kw = m_currentToken.text;

        // أنواع البيانات
        if (kw == "صحيح" || kw == "عشري" || kw == "حرف" ||
            kw == "منطقي" || kw == "فراغ" || kw == "نص" || kw == "تلقائي" ||
            kw == "عرّف" || kw == "مؤشر" || kw == "مرجع") {
            return parseVariableDeclaration();
        }

        // التحكم في التدفق
        if (kw == "إذا") return parseIfStatement();
        if (kw == "طالما") return parseWhileStatement();
        if (kw == "لكل") return parseForStatement();
        if (kw == "افعل") return parseDoWhileStatement();
        if (kw == "اختر") return parseSwitchStatement();
        if (kw == "ارجع") return parseReturnStatement();
        if (kw == "توقف") { consume(); if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume(); return std::make_unique<BreakStmtAST>(); }
        if (kw == "استمر") { consume(); if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume(); return std::make_unique<ContinueStmtAST>(); }

        // تعريف الدوال
        if (kw == "دالة") return parseFunctionDefinition();

        // البرمجة كائنية التوجه
        if (kw == "نطاق") return parseNamespaceDeclaration();
        if (kw == "تعداد") return parseEnumDeclaration();
        if (kw == "قالب") return parseTemplateDeclaration();

        // الاستثناءات
        if (kw == "حاول") return parseTryCatchStatement();

        // الكائنات
        if (kw == "صنف") return parseClassDeclaration();
        if (kw == "هيكل") return parseStructDeclaration();
        if (kw == "ارمِ") return parseThrowStatement();
        if (kw == "جديد") {
            consume();
            return std::make_unique<ExprStmtAST>(parseNewExpression());
        }
        if (kw == "احذف") {
            consume();
            auto operand = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<DeleteStmtAST>(std::move(operand));
        }
    }

    if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        auto& name = m_currentToken.text;
        if (name == "طباعة") return parsePrintStatement();

        std::string nameStr = m_currentToken.text;
        consume();

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
            m_currentToken.type == TokenType::TOKEN_SLASH_EQUALS) {
            std::string op = m_currentToken.text;
            consume();
            auto val = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
            return std::make_unique<CompoundAssignmentAST>(nameStr, op, std::move(val));
        }
        if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
            consume(); // [
            auto idx = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
            if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
                consume(); // =
                auto val = parseExpression();
                if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
                return std::make_unique<ArraySubscriptAssignAST>(nameStr, std::move(idx), std::move(val));
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

std::unique_ptr<ExprAST> Parser::parseExpression() {
    return parseBinaryExpr(1);
}

int Parser::getCurrentPrecedence() const {
    auto t = m_currentToken.type;
    if (t == TokenType::TOKEN_OR)               return 1;
    if (t == TokenType::TOKEN_AND)              return 2;
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
    return -1;
}

std::unique_ptr<ExprAST> Parser::parseUnary() {
    if (m_currentToken.type == TokenType::TOKEN_MINUS) {
        consume();
        auto operand = parseUnary();
        return std::make_unique<UnaryExprAST>("-", std::move(operand));
    }
    if (m_currentToken.type == TokenType::TOKEN_KEYWORD && m_currentToken.text == "ليس") {
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

        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            return parseFunctionCallExpr(name);
        }

        if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
            consume(); // [
            auto idx = parseExpression();
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
            return std::make_unique<ArraySubscriptExprAST>(name, std::move(idx));
        }

        if (m_currentToken.type == TokenType::TOKEN_DOT) {
            consume();
            std::string member = m_currentToken.text;
            consume();
            return std::make_unique<MemberAccessExprAST>(name, member);
        }

        return std::make_unique<VariableExprAST>(name);
    }

    // كلمات مفتاحية كمُنشئات (مثلاً: زر_أمر())
    if (m_currentToken.type == TokenType::TOKEN_KEYWORD) {
        std::string name = m_currentToken.text;
        consume();
        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            return parseFunctionCallExpr(name);
        }
        reportError("تعبير غير متوقع: " + name);
        return std::make_unique<NumberExprAST>(0.0);
    }

    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        consume();
        auto expr = parseExpression();
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        return expr;
    }

    // ناقص أحادي handled by parseUnary()

    // قيم منطقية — emit C++ bool literals, not doubles
    if (m_currentToken.text == "صواب" && m_currentToken.type == TokenType::TOKEN_KEYWORD) {
        consume();
        return std::make_unique<VariableExprAST>("true");
    }
    if (m_currentToken.text == "خطأ" && m_currentToken.type == TokenType::TOKEN_KEYWORD) {
        consume();
        return std::make_unique<VariableExprAST>("false");
    }

    reportError("تعبير غير متوقع: " + m_currentToken.text);
    return std::make_unique<NumberExprAST>(0.0);
}

std::unique_ptr<ExprAST> Parser::parseFunctionCallExpr(const std::string& name) {
    consume(); // '('
    std::vector<std::unique_ptr<ExprAST>> args;

    if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
        args.push_back(parseExpression());
        while (m_currentToken.type == TokenType::TOKEN_UNKNOWN && m_currentToken.text == ",") {
            consume();
            args.push_back(parseExpression());
        }
    }

    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    return std::make_unique<FunctionCallAST>(name, std::move(args));
}

std::unique_ptr<VarDeclStmtAST> Parser::parseVariableDeclaration() {
    std::string type = m_currentToken.text;
    consume();

    // دعم أنواع المصفوفات (صحيح[], نص[], عشري[])
    if (m_currentToken.type == TokenType::TOKEN_LEFT_BRACKET) {
        consume(); // [
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_BRACKET) consume(); // ]
        type = type + "[]";
    }

    std::string name = m_currentToken.text;
    consume();

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
        auto arg = parseExpression();
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();

        std::vector<std::unique_ptr<ExprAST>> args;
        args.push_back(std::move(arg));
        return std::make_unique<VarDeclStmtAST>("__builtin_print", "print_call", std::move(args[0]));
    }

    auto arg = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<VarDeclStmtAST>("__builtin_print", "print_call", std::move(arg));
}

std::unique_ptr<IfStmtAST> Parser::parseIfStatement() {
    consume(); // 'إذا'

    auto cond = parseExpression();

    auto thenBody = parseBlock();
    std::vector<std::unique_ptr<StmtAST>> elseBody;

    if (m_currentToken.type == TokenType::TOKEN_KEYWORD && m_currentToken.text == "وإلا") {
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

std::unique_ptr<ForStmtAST> Parser::parseForStatement() {
    consume(); // 'لكل'
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) consume();

    // Init
    std::unique_ptr<StmtAST> init;
    if (m_currentToken.type == TokenType::TOKEN_KEYWORD &&
        (m_currentToken.text == "صحيح" || m_currentToken.text == "عشري" ||
         m_currentToken.text == "حرف" || m_currentToken.text == "منطقي" || m_currentToken.text == "تلقائي")) {
        init = parseVariableDeclaration();
    } else if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        std::string name = m_currentToken.text;
        consume();
        if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
            init = parseAssignment(name);
        }
    }

    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();

    // Condition
    auto cond = parseExpression();

    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();

    // Update
    std::unique_ptr<StmtAST> update;
    if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        std::string name = m_currentToken.text;
        consume();
        if (m_currentToken.type == TokenType::TOKEN_EQUALS) {
            update = parseAssignment(name);
        }
    }

    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();

    auto body = parseBlock();
    return std::make_unique<ForStmtAST>(std::move(init), std::move(cond), std::move(update), std::move(body));
}

std::unique_ptr<DoWhileStmtAST> Parser::parseDoWhileStatement() {
    consume(); // 'افعل'
    auto body = parseBlock();

    if (m_currentToken.type == TokenType::TOKEN_KEYWORD && m_currentToken.text == "طالما") {
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
            if (m_currentToken.type == TokenType::TOKEN_KEYWORD && m_currentToken.text == "حالة") {
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
            } else if (m_currentToken.type == TokenType::TOKEN_KEYWORD && m_currentToken.text == "افتراضي") {
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
            if (m_currentToken.type == TokenType::TOKEN_IDENTIFIER || m_currentToken.type == TokenType::TOKEN_KEYWORD) {
                values.push_back(m_currentToken.text);
                consume();
            }
            if (m_currentToken.type == TokenType::TOKEN_UNKNOWN && m_currentToken.text == ",") consume();
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

    if (m_currentToken.type == TokenType::TOKEN_KEYWORD && m_currentToken.text == "امسك") {
        consume();
        if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
            consume();
            catchVar = m_currentToken.text;
            consume();
            if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
        }
        catchBody = parseBlock();
    }

    return std::make_unique<TryCatchStmtAST>(std::move(tryBody), std::move(catchVar), std::move(catchBody));
}

std::unique_ptr<TemplateDeclAST> Parser::parseTemplateDeclaration() {
    consume(); // 'قالب'

    if (m_currentToken.type == TokenType::TOKEN_LESS) consume();

    std::vector<std::string> params;
    while (m_currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        params.push_back(m_currentToken.text);
        consume();
        if (m_currentToken.type == TokenType::TOKEN_UNKNOWN && m_currentToken.text == ",") consume();
        else break;
    }

    if (m_currentToken.type == TokenType::TOKEN_GREATER) consume();
    auto body = parseStatement();
    return std::make_unique<TemplateDeclAST>("Template", std::move(params), std::move(body));
}

std::unique_ptr<FunctionDeclAST> Parser::parseFunctionDefinition() {
    consume(); // 'دالة'

    // اسم الدالة
    std::string name = m_currentToken.text;
    consume();

    // الأقواس (اختيارية)
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) consume();

    // المعاملات
    std::vector<std::pair<std::string, std::string>> params;
    if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
        // قراءة النوع
        std::string paramType = m_currentToken.text;
        consume();
        // قراءة الاسم
        std::string paramName = m_currentToken.text;
        consume();
        params.push_back({paramType, paramName});

        while (m_currentToken.type == TokenType::TOKEN_UNKNOWN && m_currentToken.text == ",") {
            consume();
            paramType = m_currentToken.text;
            consume();
            paramName = m_currentToken.text;
            consume();
            params.push_back({paramType, paramName});
        }
    }

    if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();

    // نوع الإرجاع (->)
    std::string returnType = "فراغ";
    if (m_currentToken.type == TokenType::TOKEN_MINUS) {
        consume(); // '-'
        if (m_currentToken.type == TokenType::TOKEN_GREATER) {
            consume(); // '>'
            returnType = m_currentToken.text;
            consume();
        }
    }

    // جسم الدالة
    auto body = parseBlock();

    return std::make_unique<FunctionDeclAST>(returnType, name, std::move(params), std::move(body));
}

std::unique_ptr<ClassDeclAST> Parser::parseClassDeclaration() {
    consume(); // 'فئة'
    std::string name = m_currentToken.text;
    consume();
    auto body = parseBlock();
    return std::make_unique<ClassDeclAST>(std::move(name), std::move(body));
}

std::unique_ptr<StructDeclAST> Parser::parseStructDeclaration() {
    consume(); // 'هيكلا'
    std::string name = m_currentToken.text;
    consume();
    auto body = parseBlock();
    return std::make_unique<StructDeclAST>(std::move(name), std::move(body));
}

std::unique_ptr<ThrowStmtAST> Parser::parseThrowStatement() {
    consume(); // 'ألقِ'
    auto value = parseExpression();
    if (m_currentToken.type == TokenType::TOKEN_SEMICOLON) consume();
    return std::make_unique<ThrowStmtAST>(std::move(value));
}

std::unique_ptr<NewExprAST> Parser::parseNewExpression() {
    std::string typeName = m_currentToken.text;
    consume();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (m_currentToken.type == TokenType::TOKEN_LEFT_PAREN) {
        consume();
        if (m_currentToken.type != TokenType::TOKEN_RIGHT_PAREN) {
            args.push_back(parseExpression());
            while (m_currentToken.type == TokenType::TOKEN_UNKNOWN && m_currentToken.text == ",") {
                consume();
                args.push_back(parseExpression());
            }
        }
        if (m_currentToken.type == TokenType::TOKEN_RIGHT_PAREN) consume();
    }
    return std::make_unique<NewExprAST>(std::move(typeName), std::move(args));
}

} // namespace daad
