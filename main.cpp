#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "include/ui/AuthController.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AuthController authController;
    engine.rootContext()->setContextProperty("AuthController", &authController);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Antro", "Main");

    return QGuiApplication::exec();
}
