/**
 * @file main.cpp
 * @brief نقطة الدخول لأداة سطر الأوامر DaadCompiler CLI
 * @author فريق ض استديو
 * @version 1.0.0
 */

#include "Daad/Compiler.hpp"
#include "Daad/Diagnostics.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void printBanner() {
    std::cout << "ض استديو - Daad Studio Compiler v1.0.0\n";
    std::cout << "==========================================\n\n";
}

void printUsage() {
    std::cout << "الاستخدام: daad-compiler [خيارات] <ملف_مصدري>\n\n";
    std::cout << "الخيارات:\n";
    std::cout << "  -o <ملف>        تحديد ملف الإخراج (output.cpp)\n";
    std::cout << "  -g, --debug     توجيهات #line لربط المخرج بسطور .ض (للتصحيح F5)\n";
    std::cout << "  -h, --help      عرض رسالة المساعدة\n";
    std::cout << "  -v, --version   عرض الإصدار\n";
}

void printVersion() {
    std::cout << "daad-compiler v1.0.0\n";
}

bool compileAndOutput(const std::string& sourceCode, const std::string& outputFile,
                      const std::string& sourcePath, bool debugLines) {
    // اسم ملف الرأس المشتق من ملف الإخراج (e.g. out/foo.cpp -> foo.hpp)
    std::string headerName = "output.hpp";
    auto dotPos = outputFile.rfind('.');
    auto lastSlash = outputFile.find_last_of("/\\");
    if (dotPos != std::string::npos && (lastSlash == std::string::npos || dotPos > lastSlash)) {
        headerName = outputFile.substr(0, dotPos) + ".hpp";
        auto slashPos = headerName.rfind('\\');
        if (slashPos != std::string::npos) headerName = headerName.substr(slashPos + 1);
        auto slashPos2 = headerName.rfind('/');
        if (slashPos2 != std::string::npos) headerName = headerName.substr(slashPos2 + 1);
    }

    daad::DaadCompiler compiler;
    compiler.setDebugLines(debugLines);
    compiler.setSourcePath(sourcePath);
    auto result = compiler.compile(sourceCode, headerName);

    if (result.success) {
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "خطأ: لا يمكن فتح ملف الإخراج: " << outputFile << "\n";
            return false;
        }
        outFile << result.sourceOutput;
        outFile.close();

        // Write header file (output.hpp) alongside the source file
        std::string headerFile = outputFile;
        auto dotPos = headerFile.rfind('.');
        if (dotPos != std::string::npos) {
            headerFile = headerFile.substr(0, dotPos) + ".hpp";
        } else {
            headerFile += ".hpp";
        }
        std::ofstream headerOut(headerFile);
        if (headerOut.is_open()) {
            headerOut << result.headerOutput;
            headerOut.close();
        }

        std::cout << "تم التحويل بنجاح إلى: " << outputFile << "\n";
        return true;
    } else {
        std::cerr << "فشل التحويل مع الأخطاء التالية:\n";
        for (const auto& diag : result.diagnostics) {
            std::cerr << "  [خطأ] السطر " << diag.line << ":" << diag.column << " - " << diag.message << "\n";
        }
        return false;
    }
}

int main(int argc, char* argv[]) {
    printBanner();

    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string sourceFile;
    std::string outputFile = "output.cpp";
    bool debugLines = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "خطأ: يرجى تحديد ملف الإخراج بعد -o\n";
                return 1;
            }
        } else if (arg == "-g" || arg == "--debug") {
            debugLines = true;
        } else if (arg[0] != '-') {
            sourceFile = arg;
        }
    }

    if (sourceFile.empty()) {
        std::cerr << "خطأ: يرجى تحديد ملف مصدري\n";
        return 1;
    }

    std::ifstream file(sourceFile);
    if (!file.is_open()) {
        std::cerr << "خطأ: لا يمكن فتح الملف: " << sourceFile << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();

    std::cout << "جاري ترجمة: " << sourceFile << "\n\n";
    return compileAndOutput(sourceCode, outputFile, sourceFile, debugLines) ? 0 : 1;
}