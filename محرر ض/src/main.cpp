#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "MainWindow.h"
#include "ThemeManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("محرر ض");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("محرر ض");

    // لغة عربية — كل الواجهة RTL
    QLocale::setDefault(QLocale("ar_SA"));
    app.setLayoutDirection(Qt::RightToLeft);

    // ثيم محفوظ
    ThemeManager::instance().applyToApp(&app);

    MainWindow w;
    w.show();
    return app.exec();
}
