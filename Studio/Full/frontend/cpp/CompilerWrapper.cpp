/**
 * @file CompilerWrapper.cpp
 * @brief تنفيذ غلاف المحرك للخيوط المتعددة
 */

#include "CompilerWrapper.hpp"
#include <QVariant>

CompilerWrapper::CompilerWrapper(QObject* parent)
    : QObject(parent)
{
    connect(&m_watcher, &QFutureWatcher<daad::CompileResult>::finished, this, [this]() {
        auto result = m_watcher.result();
        m_busy = false;
        emit busyChanged();

        QVariantList diagList;
        for (const auto& d : result.diagnostics) {
            QVariantMap m;
            m["severity"] = static_cast<int>(d.severity);
            m["line"] = static_cast<qint64>(d.line);
            m["column"] = static_cast<qint64>(d.column);
            m["message"] = QString::fromStdString(d.message);
            m["sourceLine"] = QString::fromStdString(d.sourceLine);
            diagList.append(m);
        }

        emit compilationFinished(
            result.success,
            QString::fromStdString(result.headerOutput),
            QString::fromStdString(result.sourceOutput),
            diagList
        );
    });
}

CompilerWrapper::~CompilerWrapper() {
    m_watcher.waitForFinished();
}

void CompilerWrapper::compileAsync(const QString& sourceCode) {
    if (m_busy) return;
    m_busy = true;
    emit busyChanged();

    auto future = QtConcurrent::run([this, sourceCode]() {
        return m_compiler.compile(sourceCode.toStdString());
    });
    m_watcher.setFuture(future);
}

bool CompilerWrapper::isBusy() const {
    return m_busy;
}