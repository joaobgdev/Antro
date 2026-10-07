#ifndef COMPRADORCONTROLLER_HPP
#define COMPRADORCONTROLLER_HPP

#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QtQmlIntegration>
#include "services/CatalogoComprador.hpp"
#include "services/RepositorioCatalogo.hpp"

class AuthController;

class CompradorController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QVariantList sacola READ sacola NOTIFY sacolaChanged)
    Q_PROPERTY(QVariantList retiradas READ retiradas NOTIFY retiradasChanged)
    Q_PROPERTY(int tiposNaSacola READ tiposNaSacola NOTIFY sacolaChanged)
    Q_PROPERTY(double totalEstimado READ totalEstimado NOTIFY sacolaChanged)
    Q_PROPERTY(QString erro READ erro NOTIFY erroChanged)

public:
    explicit CompradorController(QObject *parent = nullptr);
    void definirAutenticacao(AuthController *auth);
    Q_INVOKABLE QVariantList feiras() const; // Q_INVOKABLE permite que este método C++ seja chamado diretamente pelo QML.
    Q_INVOKABLE QVariantMap feira(int id) const;
    Q_INVOKABLE QVariantMap vendedor(int id) const;
    // Métodos expostos ao QML para operações realizadas pelo comprador.
    Q_INVOKABLE QVariantList vendedores(int feiraId) const;
    Q_INVOKABLE QVariantList produtos(int feiraId, int vendedorId) const;
    Q_INVOKABLE bool adicionar(int feiraId, int vendedorId, int produtoId, double quantidade);
    Q_INVOKABLE bool alterarQuantidade(int indice, double quantidade);
    Q_INVOKABLE void remover(int indice);
    Q_INVOKABLE void limpar();
    Q_INVOKABLE QVariantList datasRetirada(int feiraId) const;
    Q_INVOKABLE QVariantList janelasRetirada(int feiraId, const QString &data) const;
    Q_INVOKABLE bool agendar(int feiraId, const QString &data, const QString &inicio, const QString &fim);
    Q_INVOKABLE QVariantMap finalizarReserva();
    Q_INVOKABLE QVariantList reservas();
    Q_INVOKABLE bool cancelarReserva(int id);
    Q_INVOKABLE void recarregar();
    QVariantList sacola() const;
    QVariantList retiradas() const;
    int tiposNaSacola() const;
    double totalEstimado() const;
    QString erro() const;

signals:
    void sacolaChanged();
    void retiradasChanged();
    void produtosChanged();
    void reservasChanged();
    void erroChanged();

private:
    bool autorizado(); // Confere se o banco está aberto e o usuário é comprador.
    bool falhar(const QString &texto);
    void podarAgendamentos(); // Remove retiradas inválidas ou de feiras que saíram da sacola.
    RepositorioCatalogo m_repo;
    CatalogoComprador m_catalogo;
    QVector<AgendamentoReserva> m_agendamentos;
    // Fica nulo se o objeto de autenticação for destruído.
    QPointer<AuthController> m_auth;
    bool m_bancoPronto = false;
    QString m_erro;
};

#endif
