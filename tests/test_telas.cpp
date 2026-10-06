#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QTemporaryDir>
#include <QEventLoop>
#include <QTimer>
#include <cassert>
#include <iostream>
#include "ui/AuthController.hpp"
#include "ui/CompradorController.hpp"
#include "ui/VendedorController.hpp"

static void aguardar()
{
    QEventLoop eventos;
    QTimer::singleShot(500, &eventos, &QEventLoop::quit);
    eventos.exec();
}

static QQuickItem *encontrar(QQuickItem *item, const QString &nome)
{
    if (!item->isVisible()) return nullptr;
    if (item->objectName() == nome) return item;
    for (QQuickItem *filho : item->childItems())
        if (QQuickItem *achado = encontrar(filho,nome)) return achado;
    return nullptr;
}

static QQuickItem *controle(QQuickWindow *janela, const QString &nome)
{
    QQuickItem *item = encontrar(janela->contentItem(),nome);
    if (!item) std::cerr << "Controle não encontrado: " << nome.toStdString() << '\n';
    assert(item);
    return item;
}

static void clicar(QQuickWindow *janela, const QString &nome)
{
    QQuickItem *item = controle(janela,nome);
    assert(item->property("enabled").toBool());
    assert(QMetaObject::invokeMethod(item,"clicked"));
    aguardar();
}

static void preencher(QQuickWindow *janela, const QString &nome, const QString &texto)
{
    assert(controle(janela,nome)->setProperty("text",texto));
}

static void selecionar(QQuickWindow *janela, const QString &nome, int indice)
{
    QQuickItem *item = controle(janela,nome);
    assert(item->setProperty("currentIndex",indice));
    assert(QMetaObject::invokeMethod(item,"activated",Q_ARG(int,indice)));
    aguardar();
}

int main(int argc, char *argv[])
{
    QTemporaryDir dados; assert(dados.isValid());
    qputenv("XDG_DATA_HOME",dados.path().toUtf8());
    QGuiApplication app(argc,argv);
    app.setApplicationName("AntroTeste");
    QQmlApplicationEngine engine;
    int erros = 0;
    QObject::connect(&engine,&QQmlEngine::warnings,[&](const QList<QQmlError> &avisos) {
        for (const QQmlError &aviso : avisos) { std::cerr << aviso.toString().toStdString() << '\n'; ++erros; }
    });
    auto auth = engine.singletonInstance<AuthController *>("Antro","AuthController");
    auto comprador = engine.singletonInstance<CompradorController *>("Antro","CompradorController");
    auto vendedor = engine.singletonInstance<VendedorController *>("Antro","VendedorController");
    assert(auth && comprador && vendedor);
    comprador->definirAutenticacao(auth); vendedor->definirAutenticacao(auth);
    QObject::connect(vendedor,&VendedorController::catalogoChanged,comprador,&CompradorController::recarregar);
    QObject::connect(comprador,&CompradorController::reservasChanged,vendedor,&VendedorController::atualizarPedidos);
    engine.loadFromModule("Antro","Main");
    assert(!engine.rootObjects().isEmpty());
    auto janela = qobject_cast<QQuickWindow *>(engine.rootObjects().first()); assert(janela);
    aguardar();
    clicar(janela,"abaCadastro"); clicar(janela,"perfilFeirante");
    preencher(janela,"nomeCadastro","Maria Silva"); preencher(janela,"telefone","81999991001");
    preencher(janela,"senha","123456"); preencher(janela,"bancaCadastro","Banca Maria"); preencher(janela,"ocsCadastro","OCS1");
    clicar(janela,"entrar"); assert(auth->perfilUsuario() == "feirante");
    clicar(janela,"abrirFeira");
    assert(!vendedor->feiras().isEmpty());
    clicar(janela,"editarPerfil"); preencher(janela,"nomeNovoProduto","Alface"); clicar(janela,"incluirProduto");
    preencher(janela,"nomeNovoProduto","Couve"); clicar(janela,"incluirProduto");
    assert(vendedor->definirAtivo(1,false));
    assert(vendedor->definirPreco(1,"0,00"));
    assert(!vendedor->definirPreco(1,"-1,00"));
    assert(vendedor->definirAtivo(1,true));
    assert(!vendedor->definirPreco(1,"0,00")); assert(vendedor->erro().contains("Couve"));
    assert(vendedor->definirAtivo(1,false));
    clicar(janela,"avancarPrecos"); preencher(janela,"precoProduto","3,50"); preencher(janela,"estoqueProduto","10");
    clicar(janela,"salvarPerfil");
    assert(vendedor->erro().isEmpty());
    assert(vendedor->produtos().size() == 2);
    assert(!vendedor->produtos()[1].toMap().value("ativo").toBool());
    assert(vendedor->produtos()[1].toMap().value("preco").toDouble() == 0);
    assert(vendedor->produtos().first().toMap().value("estoque").toDouble() == 10);
    janela->setWidth(640); aguardar();
    clicar(janela,"reservas"); assert(vendedor->pedidos().isEmpty());
    clicar(janela,"sair"); assert(!auth->logado()); assert(vendedor->produtos().isEmpty());
    janela->setWidth(1280); aguardar();
    clicar(janela,"abaCadastro"); preencher(janela,"nomeCadastro","Lucas Medrado");
    preencher(janela,"telefone","81999991002"); preencher(janela,"senha","123456"); clicar(janela,"entrar");
    assert(auth->perfilUsuario() == "comprador");
    clicar(janela,"abrirFeira"); clicar(janela,"abrirProdutor"); clicar(janela,"adicionarProduto");
    assert(comprador->tiposNaSacola() == 1);
    clicar(janela,"carrinho");
    auto semHorario = comprador->finalizarReserva(); assert(!semHorario.value("ok").toBool()); assert(comprador->tiposNaSacola() == 1);
    selecionar(janela,"dataRetirada",1); selecionar(janela,"horarioRetirada",0);
    clicar(janela,"solicitarReserva"); assert(comprador->tiposNaSacola() == 0);
    assert(comprador->reservas().size() == 1);
    assert(comprador->reservas().first().toMap().value("status").toString() == "SOLICITADA");
    clicar(janela,"reservas");
    janela->setWidth(640); aguardar();
    clicar(janela,"sair"); preencher(janela,"telefone","81999991001"); preencher(janela,"senha","123456"); clicar(janela,"entrar");
    clicar(janela,"reservas"); assert(vendedor->pedidos().size() == 1); clicar(janela,"aceitarPedido");
    assert(vendedor->pedidos().first().toMap().value("status").toString() == "ACEITA");
    clicar(janela,"sair"); preencher(janela,"telefone","81999991002"); preencher(janela,"senha","123456"); clicar(janela,"entrar");
    clicar(janela,"reservas"); assert(comprador->reservas().first().toMap().value("status").toString() == "ACEITA");
    clicar(janela,"cancelarReserva"); assert(comprador->reservas().first().toMap().value("status").toString() == "CANCELADA");
    clicar(janela,"inicio"); clicar(janela,"abrirFeira"); clicar(janela,"abrirProdutor"); clicar(janela,"adicionarProduto");
    assert(comprador->tiposNaSacola() == 1);
    clicar(janela,"sair"); assert(comprador->tiposNaSacola() == 0); assert(comprador->retiradas().isEmpty());
    preencher(janela,"telefone","81999991001"); preencher(janela,"senha","123456"); clicar(janela,"entrar");
    assert(vendedor->produtos().first().toMap().value("estoque").toDouble() == 10);
    assert(!comprador->adicionar(1,1,1,1));
    assert(erros == 0);
    std::cout << "Cadastro, navegação, edição, agendamento, pedidos e troca de sessão passaram.\n";
}
