#pragma once

#include <QObject>
#include <QString>

class CppToDaadTranspiler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(bool available READ isAvailable CONSTANT)

public:
    explicit CppToDaadTranspiler(QObject* parent = nullptr);

    Q_INVOKABLE QString transpile(const QString& cppCode);
    bool isBusy() const { return m_busy; }
    bool isAvailable() const { return true; }

signals:
    void transpileFinished(bool success, const QString& daadOutput, const QString& message);
    void busyChanged();

private:
    bool m_busy = false;

    QString applyRules(const QString& input) const;
};
