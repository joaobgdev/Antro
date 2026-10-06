#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "ui/AuthController.hpp"
#include "ui/CompradorController.hpp"
#include "ui/VendedorController.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    AuthController *auth = engine.singletonInstance<AuthController *>("Antro", "AuthController");
    CompradorController *comprador = engine.singletonInstance<CompradorController *>("Antro", "CompradorController");
    VendedorController *vendedor = engine.singletonInstance<VendedorController *>("Antro", "VendedorController");
    comprador->definirAutenticacao(auth);
    vendedor->definirAutenticacao(auth);
    QObject::connect(vendedor, &VendedorController::catalogoChanged, comprador, &CompradorController::recarregar);
    QObject::connect(comprador, &CompradorController::reservasChanged, vendedor, &VendedorController::atualizarPedidos);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("Antro", "Main");
    return QGuiApplication::exec();
}
