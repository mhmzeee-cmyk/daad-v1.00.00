#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include "CompilerWrapper.hpp"
#include "SyntaxHighlighter.hpp"
#include "Translator.hpp"
#include "CppToDaadTranspiler.hpp"
#include "GeminiBackend.hpp"
#include "FileSystemModel.hpp"
#include "GuiDesignerModel.hpp"
#include "CloudNetworkService.hpp"
#include "ChallengeManager.hpp"
#include "MockDataLoader.hpp"

static void messageHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:   fprintf(stderr, "[QML DEBUG] %s\n", localMsg.constData()); break;
    case QtInfoMsg:    fprintf(stderr, "[QML INFO]  %s\n", localMsg.constData()); break;
    case QtWarningMsg: fprintf(stderr, "[QML WARN]  %s\n", localMsg.constData()); break;
    case QtCriticalMsg:fprintf(stderr, "[QML CRIT]  %s\n", localMsg.constData()); break;
    case QtFatalMsg:   fprintf(stderr, "[QML FATAL] %s\n", localMsg.constData()); break;
    }
}

int main(int argc, char* argv[])
{
    qInstallMessageHandler(messageHandler);

    QGuiApplication app(argc, argv);
    app.setApplicationName("Daad Studio");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("Daad Studio Team");

    qmlRegisterType<CompilerWrapper>("DaadStudio", 1, 0, "Compiler");
    qmlRegisterType<DaadHighlighter>("DaadStudio", 1, 0, "DaadHighlighter");
    qmlRegisterType<CppToDaadTranspiler>("DaadStudio", 1, 0, "CppToDaadTranspiler");
    qmlRegisterType<daad::FileSystemModel>("DaadStudio", 1, 0, "FileSystemModel");
    qmlRegisterType<GuiDesignerModel>("DaadStudio", 1, 0, "GuiDesignerModel");

    QQmlApplicationEngine engine;

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");

    Translator translator;
    engine.rootContext()->setContextProperty("translator", &translator);
    engine.rootContext()->setContextProperty("appVersion", "2.0.0");

    GeminiBackend geminiBackend;
    engine.rootContext()->setContextProperty("geminiBackend", &geminiBackend);

    CloudNetworkService cloudService;
    engine.rootContext()->setContextProperty("cloudService", &cloudService);

    ChallengeManager challengeManager;
    engine.rootContext()->setContextProperty("challengeManager", &challengeManager);

    MockDataLoader mockDataLoader;
    engine.rootContext()->setContextProperty("mockDataLoader", &mockDataLoader);
    mockDataLoader.loadIfEmpty();

    fprintf(stderr, "[INFO] Loading qrc:/qml/Main.qml ...\n");
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    fprintf(stderr, "[INFO] Load complete. Root objects: %d\n", engine.rootObjects().isEmpty() ? 0 : 1);

    if (engine.rootObjects().isEmpty()) {
        fprintf(stderr, "[FATAL] No root objects. Exiting.\n");
        return -1;
    }

    fprintf(stderr, "[INFO] Entering event loop.\n");
    return app.exec();
}
