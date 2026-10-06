// Includes
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "ui/AuthController.hpp"
#include "ui/CompradorController.hpp"
#include "ui/VendedorController.hpp"

int main(int argc, char *argv[])
{
    // Inicialização essencial
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
<<<<<<< HEAD
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
=======

    // Singletons (Sincronizam QML e o CPP)
    AuthController *auth = engine.singletonInstance<AuthController *>("Antro", "AuthController");
    CompradorController *comprador = engine.singletonInstance<CompradorController *>("Antro", "CompradorController");
    VendedorController *vendedor = engine.singletonInstance<VendedorController *>("Antro", "VendedorController");
    
    // Definir tipo de usuário
    comprador->definirAutenticacao(auth);
    vendedor->definirAutenticacao(auth);

    // Conecta dados vendedor e comprador automaticamente
    QObject::connect(vendedor, &VendedorController::catalogoChanged, comprador, &CompradorController::recarregar); // Adicionar produto
    QObject::connect(comprador, &CompradorController::reservasChanged, vendedor, &VendedorController::atualizarPedidos); // Fazer reserva
    
    // Erros fecham programa
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    
    // "Draw" da tela
    engine.loadFromModule("Antro", "Main");
    return QGuiApplication::exec(); // Loop de execução

>>>>>>> 59b3808 (Adição de Comentários Main.cpp, Main.qml, CMakeLists)
}
