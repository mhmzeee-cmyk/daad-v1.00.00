#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "SchoolOnboarder.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qmlRegisterType<SchoolOnboarder>("DaadBridge", 1, 0, "SchoolOnboarder");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/bridge_main.qml")));
    if (engine.rootObjects().isEmpty()) return -1;
    return app.exec();
}
