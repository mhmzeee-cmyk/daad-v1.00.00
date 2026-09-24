#pragma once

#include "Daad/AST.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace daad {

class CodeGenVisitor : public ExprASTVisitor, public StmtVisitor {
public:
    CodeGenVisitor();

    // Configuration
    void setUserFunctionNames(const std::unordered_set<std::string>& names);
    void enableDebugInfo(bool enable) { m_debugInfo = enable; }
    void setSourceFileName(const std::string& name) { m_sourceFileName = name; }
    void setDebugSourcePath(const std::string& p) { m_debugSourcePath = p; }

    // Output access
    std::string getSourceCode() const { return m_sourceStream.str(); }
    std::string getHeaderCode() const { return m_headerStream.str(); }
    const std::vector<std::string>& getWarnings() const { return m_warnings; }

    // Main body wrapping
    void beginMainBody() {
        target() << "int main() {\n";
    }
    void endMainBody() {
        checkMemoryLeaks();
        target() << "return 0;\n}\n";
    }

    // Stdlib
    static const std::vector<std::string>& stdlibExportedNames();

    // ── ExprAST visitors ──────────────────────────────────────────────────────
    void visit(NumberExprAST&) override;
    void visit(StringExprAST&) override;
    void visit(VariableExprAST&) override;
    void visit(BoolExprAST&) override;
    void visit(NullExprAST&) override;
    void visit(BinaryExprAST&) override;
    void visit(UnaryExprAST&) override;
    void visit(RawExprAST&) override;
    void visit(FunctionCallAST&) override;
    void visit(MemberAccessExprAST&) override;
    void visit(ArraySubscriptExprAST&) override;
    void visit(TernaryExprAST&) override;
    void visit(ImageSizeAST&) override;
    void visit(PixelAST&) override;
    void visit(NewExprAST&) override;
    void visit(SizeofExprAST&) override;
    void visit(TypeofExprAST&) override;

    // ── StmtAST visitors ──────────────────────────────────────────────────────
    void visit(VarDeclStmtAST&) override;
    void visit(AssignmentAST&) override;
    void visit(CompoundAssignmentAST&) override;
    void visit(IfStmtAST&) override;
    void visit(WhileStmtAST&) override;
    void visit(ForStmtAST&) override;
    void visit(DoWhileStmtAST&) override;
    void visit(SwitchStmtAST&) override;
    void visit(ReturnStmtAST&) override;
    void visit(BreakStmtAST&) override;
    void visit(ContinueStmtAST&) override;
    void visit(FunctionDeclAST&) override;
    void visit(ClassDeclAST&) override;
    void visit(StructDeclAST&) override;
    void visit(NamespaceDeclAST&) override;
    void visit(EnumDeclAST&) override;
    void visit(TryCatchStmtAST&) override;
    void visit(TemplateDeclAST&) override;
    void visit(MemberAssignmentAST&) override;
    void visit(ArraySubscriptAssignAST&) override;
    void visit(ThrowStmtAST&) override;
    void visit(DeleteStmtAST&) override;
    void visit(ImportStmtAST&) override;
    void visit(ExportStmtAST&) override;
    void visit(PrintStmtAST&) override;
    void visit(InputStmtAST&) override;
    void visit(ExprStmtAST&) override;
    void visit(ForEachStmtAST&) override;
    void visit(ConstructorDeclAST&) override;
    void visit(LoadImageAST&) override;
    void visit(DrawImageAST&) override;
    void visit(SaveImageAST&) override;
    void visit(CropImageAST&) override;
    void visit(ResizeAST&) override;
    void visit(RotateImageAST&) override;
    void visit(FlipImageAST&) override;
    void visit(OpacityAST&) override;
    void visit(FilterAST&) override;
    void visit(OverlayAST&) override;
    void visit(BackgroundAST&) override;
    void visit(DrawAST&) override;
    void visit(FillAST&) override;
    void visit(RectangleAST&) override;
    void visit(CircleAST&) override;
    void visit(LineAST&) override;
    void visit(TextOnCanvasAST&) override;
    void visit(ClearAST&) override;

private:
    std::ostream& target() { return *m_activeStream; }
    std::string mapType(const std::string& type) const;
    void emitSourceComment(int line);
    void trackAllocation(const std::string& varName);
    void trackDeallocation(const std::string& varName);
    void checkMemoryLeaks();

    std::stringstream m_sourceStream;
    std::stringstream m_headerStream;
    std::ostream* m_activeStream;
    std::string m_lastExpr;
    std::string m_currentClassScope;
    std::string m_sourceFileName;
    std::string m_debugSourcePath; // مسار .ض الحقيقي لتوجيهات #line (يُضبط فقط مع -g)
    bool m_debugInfo = false;
    bool m_inForContext = false;
    std::unordered_map<std::string, std::string> m_typeMap;
    std::unordered_set<std::string> m_userFunctionNames;
    std::unordered_set<std::string> m_allocatedVars;
    std::unordered_set<std::string> m_freedVars;
    std::vector<std::string> m_warnings;
};

} // namespace daad
