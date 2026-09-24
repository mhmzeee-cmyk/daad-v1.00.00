/**
 * @file CompilerWrapper.hpp
 * @brief واجهة آمنة للخيوط (Thread-Safe) لمحرك 'ض' للاستخدام من QML
 */

#pragma once

#include <QObject>
#include <QString>
#include <QFutureWatcher>
#include <QtConcurrent>
#include "Daad/Compiler.hpp"

class CompilerWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)

public:
    explicit CompilerWrapper(QObject* parent = nullptr);
    ~CompilerWrapper();

    Q_INVOKABLE void compileAsync(const QString& sourceCode);
    bool isBusy() const;

signals:
    void compilationFinished(
        bool success,
        const QString& headerOutput,
        const QString& sourceOutput,
        const QVariantList& diagnostics
    );
    void busyChanged();

private:
    daad::DaadCompiler m_compiler;
    QFutureWatcher<daad::CompileResult> m_watcher;
    bool m_busy = false;
};