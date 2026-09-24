#pragma once

#include <QObject>
#include <QString>

class QApplication;

class ThemeManager : public QObject {
    Q_OBJECT
public:
    enum class Theme { Light, Dark };

    static ThemeManager& instance();

    Theme currentTheme() const { return m_theme; }
    QString currentThemeName() const;
    void setTheme(Theme t);
    void toggle();
    void applyToApp(QApplication* app);

signals:
    void themeChanged(Theme t);

private:
    explicit ThemeManager(QObject* parent = nullptr);
    QString loadQss(Theme t) const;
    Theme m_theme = Theme::Light;
};
