/**
 * @file CodeGen.hpp
 * @brief مولد كود C++ من شجرة النحو
 * @author فريق ض استديو
 */

#pragma once

#include "Daad/AST.hpp"
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

namespace daad {

class CodeGenVisitor : public ASTVisitor {
public:
    CodeGenVisitor();

    // استرجاع الكود المولد لكل ملف
    std::string getHeaderCode() const { return m_headerStream.str(); }
    std::string getSourceCode() const {
        std::string result;
        std::string headerName = m_sourceFileName.empty() ? "output.hpp" : m_sourceFileName;
        result += "#include \"" + headerName + "\"\n";
        result += m_sourceStream.str();
        if (m_hasMainBody) {
            result += "\nint main() {\n";
            result += m_entryStream.str();
            result += "return 0;\n}\n";
        }
        return result;
    }
    
    // Switch emission target to the synthesized main() body
    // (executable top-level statements go here; declarations stay at global scope)
    void beginMainBody() { m_hasMainBody = true; m_activeStream = &m_entryStream; }
    void endMainBody()   { m_activeStream = &m_sourceStream; }
    
    // Get warnings generated during codegen (e.g., missing delete)
    const std::vector<std::string>& getWarnings() const { return m_warnings; }
    
    // Enable source line comments for debugging
    void enableDebugInfo(bool enabled) { m_debugInfo = enabled; }
    // أسماء الدوال المعرفة في ملف المصدر الحالي — لتخطي رفعها من المكتبة
    void setUserFunctionNames(const std::unordered_set<std::string>& names);
    static const std::vector<std::string>& stdlibExportedNames();
    void setSourceFileName(const std::string& name) { m_sourceFileName = name; }

    void visit(NumberExprAST& node) override;
    void visit(VariableExprAST& node) override;
    void visit(StringExprAST& node) override;
    void visit(BinaryExprAST& node) override;
    void visit(UnaryExprAST& node) override;
    void visit(RawExprAST& node) override;
    void visit(VarDeclStmtAST& node) override;
    void visit(AssignmentAST& node) override;
    void visit(CompoundAssignmentAST& node) override;
    void visit(IfStmtAST& node) override;
    void visit(WhileStmtAST& node) override;
    void visit(ForStmtAST& node) override;
    void visit(DoWhileStmtAST& node) override;
    void visit(SwitchStmtAST& node) override;
    void visit(ReturnStmtAST& node) override;
    void visit(BreakStmtAST& node) override;
    void visit(ContinueStmtAST& node) override;
    void visit(FunctionDeclAST& node) override;
    void visit(FunctionCallAST& node) override;
    void visit(ClassDeclAST& node) override;
    void visit(StructDeclAST& node) override;
    void visit(NamespaceDeclAST& node) override;
    void visit(EnumDeclAST& node) override;
    void visit(TryCatchStmtAST& node) override;
    void visit(TemplateDeclAST& node) override;
    void visit(MemberAccessExprAST& node) override;
    void visit(MemberAssignmentAST& node) override;
    void visit(ArraySubscriptExprAST& node) override;
    void visit(ArraySubscriptAssignAST& node) override;
    void visit(BoolExprAST& node) override;
    void visit(NullExprAST& node) override;
    void visit(TernaryExprAST& node) override;
    void visit(ForEachStmtAST& node) override;
    void visit(ThrowStmtAST& node) override;
    void visit(DeleteStmtAST& node) override;
    void visit(ImportStmtAST& node) override;
    void visit(ExportStmtAST& node) override;

    // New AST Node Visitors — يتوافق مع Web
    void visit(PrintStmtAST& node) override;
    void visit(InputStmtAST& node) override;
    void visit(ConstructorDeclAST& node) override;
    void visit(ExprStmtAST& node) override;
    void visit(NewExprAST& node) override;
    void visit(SizeofExprAST& node) override;
    void visit(TypeofExprAST& node) override;

    // Image Processing Visitors (20)
    void visit(LoadImageAST& node) override;
    void visit(DrawImageAST& node) override;
    void visit(ImageSizeAST& node) override;
    void visit(SaveImageAST& node) override;
    void visit(CropImageAST& node) override;
    void visit(ResizeAST& node) override;
    void visit(RotateImageAST& node) override;
    void visit(FlipImageAST& node) override;
    void visit(OpacityAST& node) override;
    void visit(FilterAST& node) override;
    void visit(OverlayAST& node) override;
    void visit(BackgroundAST& node) override;
    void visit(PixelAST& node) override;
    void visit(DrawAST& node) override;
    void visit(FillAST& node) override;
    void visit(RectangleAST& node) override;
    void visit(CircleAST& node) override;
    void visit(LineAST& node) override;
    void visit(TextOnCanvasAST& node) override;
    void visit(ClearAST& node) override;

    std::string getLastExpr() const { return m_lastExpr; }

private:
    std::ostringstream m_headerStream;
    std::ostringstream m_sourceStream;
    std::ostringstream m_entryStream;
    std::ostringstream* m_activeStream;
    bool m_hasMainBody = false;
    std::ostream& target() { return *m_activeStream; }
    // اسم الصف الحالي أثناء توليد طرق الأعضاء (لتأهيل التعريفات بـ Class::)
    std::string m_currentClassScope;
    std::unordered_map<std::string, std::string> m_typeMap;
    std::string m_lastExpr;
    bool m_inForContext = false;
    
    // Debug info
    bool m_debugInfo = false;
    std::string m_sourceFileName;
    std::unordered_set<std::string> m_userFunctionNames;
    int m_currentLine = 0;
    
    // Memory tracking for delete enforcement
    std::unordered_set<std::string> m_allocatedVars;
    std::unordered_set<std::string> m_freedVars;
    std::vector<std::string> m_warnings;
    
    void emitSourceComment(int line);
    void trackAllocation(const std::string& varName);
    void trackDeallocation(const std::string& varName);
    void checkMemoryLeaks();
    // ترجمة اسم نوع ض إلى C++ (يدعم الأنواع متعددة الأبعاد مثل عشري[][])
    std::string mapType(const std::string& type) const;
};

} // namespace daad
