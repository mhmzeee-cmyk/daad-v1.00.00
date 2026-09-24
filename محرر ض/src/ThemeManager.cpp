#include "ThemeManager.h"
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTextStream>

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {
    QSettings s("محرر ض", "DaadEditor");
    QString saved = s.value("theme", "فاتح").toString();
    m_theme = (saved == "غامق") ? Theme::Dark : Theme::Light;
}

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

QString ThemeManager::currentThemeName() const {
    return (m_theme == Theme::Dark) ? "غامق" : "فاتح";
}

QString ThemeManager::loadQss(Theme t) const {
    QString path = (t == Theme::Dark) ? ":/themes/dark.qss" : ":/themes/light.qss";
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    return ts.readAll();
}

void ThemeManager::setTheme(Theme t) {
    if (m_theme == t) return;
    m_theme = t;
    QSettings s("محرر ض", "DaadEditor");
    s.setValue("theme", currentThemeName());
    emit themeChanged(m_theme);
}

void ThemeManager::toggle() {
    setTheme(m_theme == Theme::Light ? Theme::Dark : Theme::Light);
}

void ThemeManager::applyToApp(QApplication* app) {
    QString qss = loadQss(m_theme);
    app->setStyleSheet(qss);
    // اتجاه عربي عام
    app->setLayoutDirection(Qt::RightToLeft);
}
