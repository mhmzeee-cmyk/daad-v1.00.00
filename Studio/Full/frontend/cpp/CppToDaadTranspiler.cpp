#include "CppToDaadTranspiler.hpp"
#include <QRegularExpression>
#include <QStringList>

CppToDaadTranspiler::CppToDaadTranspiler(QObject* parent)
    : QObject(parent)
{
}

static bool isArabicLine(const QString& line) {
    QString t = line.trimmed();
    static const QStringList prefixes = {
        QString::fromUtf8("\u0635\u062D\u064A\u062D"),
        QString::fromUtf8("\u0639\u0634\u0631\u064A"),
        QString::fromUtf8("\u0645\u0646\u0637\u0642\u064A"),
        QString::fromUtf8("\u0646\u0635"),
        QString::fromUtf8("\u062D\u0631\u0641"),
        QString::fromUtf8("\u0645\u0635\u0641\u0648\u0641\u0629"),
        QString::fromUtf8("\u0641\u0631\u0627\u063A"),
        QString::fromUtf8("\u0625\u0630\u0627"),
        QString::fromUtf8("\u0648\u0625\u0644\u0627"),
        QString::fromUtf8("\u0637\u0627\u0644\u0645\u0627"),
        QString::fromUtf8("\u0644\u0643\u0644"),
        QString::fromUtf8("\u062D\u0627\u0648\u0644"),
        QString::fromUtf8("\u0627\u0645\u0633\u0643"),
        QString::fromUtf8("\u0627\u062E\u062A\u0631"),
        QString::fromUtf8("\u062A\u0648\u0642\u0641"),
        QString::fromUtf8("\u0627\u0633\u062A\u0645\u0631"),
        QString::fromUtf8("\u0627\u0631\u062C\u0639"),
        QString::fromUtf8("\u0635\u0646\u0641"),
        QString::fromUtf8("\u0647\u064A\u0643\u0644"),
        QString::fromUtf8("\u062A\u0639\u062F\u0627\u062F"),
        QString::fromUtf8("\u0637\u0628\u0627\u0639\u0628\u0627"),
        QString::fromUtf8("\u0627\u0637\u0628\u0639"),
        QString::fromUtf8("\u0636")
    };
    for (const QString& p : prefixes) {
        if (t.startsWith(p)) return true;
    }
    return false;
}

QString CppToDaadTranspiler::applyRules(const QString& input) const {
    QString result = input;

    result.remove(QRegularExpression("[\u200E\u200F\u202A-\u202E\u2066-\u2069\u200B-\u200F\uFEFF]"));
    result.remove(QRegularExpression(R"(\/\*[\s\S]*?\*\/)"));
    result.remove(QRegularExpression(R"(\/\/.*)"));

    QStringList lines = result.split('\n', Qt::SkipEmptyParts);
    QStringList out;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        if (line.isEmpty()) continue;

        if (line.startsWith("#pragma") || line.startsWith("#include")) continue;
        if (QRegularExpression(R"(^\s*\}\s*$)").match(line).hasMatch()) continue;

        bool isFuncDecl = QRegularExpression(R"(^(?:static\s+)?(?:void|int|double|bool|auto)\s+\w+\s*\(.*\)\s*;\s*$)").match(line).hasMatch();
        if (isFuncDecl) continue;

        bool isFuncDef = QRegularExpression(R"(^(?:static\s+)?(?:void|int|double|bool|auto)\s+\w+\s*\(.*\)\s*\{\s*$)").match(line).hasMatch();
        if (isFuncDef) continue;

        bool alreadyDaad = isArabicLine(line);

        if (alreadyDaad) {
            line.replace(QRegularExpression(R"(;\s*$)"), " " + QString::fromUtf8("\u061B"));
            line = line.trimmed();
            if (!line.isEmpty()) out.append(line);
            continue;
        }

        line.replace(QRegularExpression(R"(daad::runtime::daad_print\s*\()"), QString::fromUtf8("\u0637\u0628\u0627\u0639\u0628\u0627("));
        line.replace(QRegularExpression(R"(daad_print\s*\()"), QString::fromUtf8("\u0637\u0628\u0627\u0639\u0628\u0627("));

        QRegularExpression varRe(R"(^(?:int|double|bool|auto)\s+(\w+)\s*=\s*(.+);\s*$)");
        QRegularExpressionMatch vm = varRe.match(line);
        if (vm.hasMatch()) {
            QString nm = vm.captured(1);
            QString ini = vm.captured(2).trimmed();

            ini.replace("true", QString::fromUtf8("\u0635\u062D\u064A\u062D"));
            ini.replace("false", QString::fromUtf8("\u062E\u0637\u0623"));
            ini.replace("nullptr", QString::fromUtf8("\u0639\u062F\u0645"));
            ini.replace(QRegularExpression(R"(std::to_string\(([^)]*)\))"), "\\1");

            QString dt = QString::fromUtf8("\u0635\u062D\u064A\u062D");
            if (line.startsWith("double")) dt = QString::fromUtf8("\u0639\u0634\u0631\u064A");
            else if (line.startsWith("bool")) dt = QString::fromUtf8("\u0645\u0646\u0637\u0642\u064A");
            else if (line.startsWith("auto")) dt = QString::fromUtf8("\u062A\u0644\u0642\u0627\u0626\u064A");

            out.append(dt + " " + nm + " = " + ini + " " + QString::fromUtf8("\u061B"));
            continue;
        }

        QRegularExpression assignRe(R"(^(\w+)\s*=\s*(.+);\s*$)");
        QRegularExpressionMatch am = assignRe.match(line);
        if (am.hasMatch()) {
            QString nm = am.captured(1);
            QString val = am.captured(2).trimmed();
            val.replace("true", QString::fromUtf8("\u0635\u062D\u064A\u062D"));
            val.replace("false", QString::fromUtf8("\u062E\u0637\u0623"));
            val.replace("nullptr", QString::fromUtf8("\u0639\u062F\u0645"));
            val.replace(QRegularExpression(R"(std::to_string\(([^)]*)\))"), "\\1");
            out.append(nm + " = " + val + " " + QString::fromUtf8("\u061B"));
            continue;
        }

        line.replace(QRegularExpression(R"(return\s+0\s*;)"), "");
        line.replace(QRegularExpression(R"(return\s*;)"), QString::fromUtf8("\u0627\u0631\u062C\u0639") + " " + QString::fromUtf8("\u061B"));
        line.replace(QRegularExpression(R"(return\s+(.+);)"), QString::fromUtf8("\u0627\u0631\u062C\u0639") + " \\1 " + QString::fromUtf8("\u061B"));
        line.replace("true", QString::fromUtf8("\u0635\u062D\u064A\u062D"));
        line.replace("false", QString::fromUtf8("\u062E\u0637\u0623"));
        line.replace("nullptr", QString::fromUtf8("\u0639\u062F\u0645"));
        line.replace("break;", QString::fromUtf8("\u062A\u0648\u0642\u0641") + " " + QString::fromUtf8("\u061B"));
        line.replace("continue;", QString::fromUtf8("\u0627\u0633\u062A\u0645\u0631") + " " + QString::fromUtf8("\u061B"));

        line.replace(QRegularExpression(R"(\belse\s+if\s*\()"), QString::fromUtf8("\u0648\u0625\u0644\u0627") + " " + QString::fromUtf8("\u0625\u0630\u0627") + " (");
        line.replace(QRegularExpression(R"(\belse\s*\{)"), QString::fromUtf8("\u0648\u0625\u0644\u0627"));
        line.replace(QRegularExpression(R"(\belse\b)"), QString::fromUtf8("\u0648\u0625\u0644\u0627"));
        line.replace(QRegularExpression(R"(\bif\s*\()"), QString::fromUtf8("\u0625\u0630\u0627") + " (");
        line.replace(QRegularExpression(R"(\bwhile\s*\()"), QString::fromUtf8("\u0637\u0627\u0644\u0645\u0627") + " (");
        line.replace(QRegularExpression(R"(\bfor\s*\()"), QString::fromUtf8("\u0644\u0643\u0644") + " (");
        line.replace(QRegularExpression(R"(\bdo\s*\{)"), QString::fromUtf8("\u0627\u0641\u0639\u0644"));

        line.replace(QRegularExpression(R"(\btry\s*\{)"), QString::fromUtf8("\u062D\u0627\u0648\u0644"));
        line.replace(QRegularExpression(R"(\bcatch\s*\([^)]*\)\s*\{)"), QString::fromUtf8("\u0627\u0645\u0633\u0643"));
        line.replace(QRegularExpression(R"(throw\s+std::runtime_error\s*\()"), QString::fromUtf8("\u0627\u0631\u0645\u0651") + "(");
        line.replace(QRegularExpression(R"(throw\s+)"), QString::fromUtf8("\u0627\u0631\u0645\u0651") + " ");
        line.replace("std::exception", QString::fromUtf8("\u0627\u0633\u062A\u062B\u0646\u0627\u0621"));

        line.replace("std::string", QString::fromUtf8("\u0646\u0635"));
        line.replace("std::vector<int>", QString::fromUtf8("\u0635\u062D\u064A\u062D[]"));
        line.replace("std::vector<double>", QString::fromUtf8("\u0639\u0634\u0631\u064A[]"));
        line.replace("std::vector<bool>", QString::fromUtf8("\u0645\u0646\u0637\u0642\u064A[]"));
        line.replace("std::vector<std::string>", QString::fromUtf8("\u0646\u0635[]"));
        line.replace("std::vector<char>", QString::fromUtf8("\u062D\u0631\u0641[]"));
        line.replace(QRegularExpression(R"(std::to_string\()"), "(");
        line.replace("std::endl", "");
        line.replace("std::cout", QString::fromUtf8("\u0637\u0628\u0627\u0639\u0628\u0627"));

        line.replace(QRegularExpression(R"(\bint\b)"), QString::fromUtf8("\u0635\u062D\u064A\u062D"));
        line.replace(QRegularExpression(R"(\bdouble\b)"), QString::fromUtf8("\u0639\u0634\u0631\u064A"));
        line.replace(QRegularExpression(R"(\bbool\b)"), QString::fromUtf8("\u0645\u0646\u0637\u0642\u064A"));
        line.replace(QRegularExpression(R"(\bchar\b)"), QString::fromUtf8("\u062D\u0631\u0641"));
        line.replace(QRegularExpression(R"(\bauto\b)"), QString::fromUtf8("\u062A\u0644\u0642\u0627\u0626\u064A"));
        line.replace(QRegularExpression(R"(\bvoid\b)"), QString::fromUtf8("\u0641\u0631\u0627\u063A"));

        line.replace(QRegularExpression(R"(;\s*$)"), " " + QString::fromUtf8("\u061B"));
        line.replace(QRegularExpression(R"(\{\s*\})"), "");

        line = line.trimmed();
        if (!line.isEmpty()) {
            out.append(line);
        }
    }

    QStringList cleaned;
    for (const QString& l : out) {
        QString t = l.trimmed();
        if (!t.isEmpty()) {
            cleaned.append(t);
        }
    }

    return cleaned.join("\n").trimmed();
}

QString CppToDaadTranspiler::transpile(const QString& cppCode) {
    if (m_busy) return QString();
    m_busy = true;
    emit busyChanged();

    QString output = applyRules(cppCode);

    m_busy = false;
    emit busyChanged();

    if (output.isEmpty()) {
        emit transpileFinished(false, QString(),
            QString::fromUtf8("\u274C \u0645\u0627 \u062A\u0645 \u0627\u0643\u062A\u0634\u0627\u0641 \u0642\u064A\u0645\u0627\u062A \u0642\u0627\u0628\u0644\u0629 \u0644\u0644\u062A\u062D\u0648\u064A\u0644 \u0627\u0644\u0639\u0643\u0633"));
        return QString();
    }

    emit transpileFinished(true, output,
        QString::fromUtf8("\u2714 \u062A\u0645 \u0627\u0644\u062A\u062D\u0648\u064A\u0644 \u0628\u0646\u062C\u0627\u062D - \u0645\u0631\u062D\u0644\u0629 \u0627\u0644\u062A\u0637\u0648\u064A\u0631"));

    return output;
}
