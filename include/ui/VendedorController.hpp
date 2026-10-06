#ifndef VENDEDORCONTROLLER_HPP
#define VENDEDORCONTROLLER_HPP

#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QtQmlIntegration>
#include "services/RepositorioCatalogo.hpp"

class AuthController;

class VendedorController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QVariantList feiras READ feiras NOTIFY perfilChanged)
    Q_PROPERTY(QVariantList produtos READ produtos NOTIFY perfilChanged)
    Q_PROPERTY(QStringList feirasPendentes READ feirasPendentes NOTIFY perfilChanged)
    Q_PROPERTY(QString erro READ erro NOTIFY erroChanged)

public:
    explicit VendedorController(QObject *parent = nullptr);
    void definirAutenticacao(AuthController *auth);
    QVariantList feiras() const;
    QVariantList produtos() const;
    QStringList feirasPendentes() const;
    QString erro() const;
    Q_INVOKABLE bool carregarPerfil();
    Q_INVOKABLE bool alternarFeira(int id);
    Q_INVOKABLE bool participarFeira(int id);
    Q_INVOKABLE bool adicionarProduto(const QString &nome);
    Q_INVOKABLE bool removerProduto(int indice);
    Q_INVOKABLE bool definirTipoVenda(int indice, const QString &tipo);
    Q_INVOKABLE bool definirPreco(int indice, const QString &texto);
    Q_INVOKABLE bool definirEstoque(int indice, const QString &texto);
    Q_INVOKABLE bool definirAtivo(int indice, bool ativo);
    Q_INVOKABLE bool salvarPerfil();
    Q_INVOKABLE QVariantList pedidos();
    Q_INVOKABLE bool alterarPedido(int id, const QString &status);
    Q_INVOKABLE void atualizarPedidos();

signals:
    void perfilChanged();
    void pedidosChanged();
    void catalogoChanged();
    void erroChanged();

private:
    bool autorizado(); // Verifica se existe uma sessão válida para executar operações do comprador.
    bool falhar(const QString &texto); // Centraliza o tratamento de falhas: armazena a mensagem de erro, notifica o QML e retorna false.
    void limparErro();
    QVector<int> m_feiras;
    QVector<RegistroProdutoFeirante> m_produtos;
    QVector<RegistroProdutoFeirante> m_anteriores;
    QStringList m_pendentes;
    QString m_erro;
    RepositorioCatalogo m_repo;
    QPointer<AuthController> m_auth; //Obs.: Acerca do QPointer, ele é um ponteiro do próprio Qt que herdam o QObject 
    bool m_bancoPronto = false;
};

#endif
