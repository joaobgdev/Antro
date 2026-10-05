#ifndef COMPRADORCONTROLLER_HPP
#define COMPRADORCONTROLLER_HPP

#include <QObject>
#include <QVariantList>
#include <QtQmlIntegration>
#include "services/CatalogoComprador.hpp"
#include "services/RepositorioCatalogo.hpp"

// Singleton do QML "CompradorController". Une o catálogo em memória (C++ puro) ao banco SQLite.
// Atende o comprador (carrinho e reserva) e o feirante (cadastro dos próprios produtos),
// porque os dois usam o mesmo catálogo e a mesma conexão com o banco.
class CompradorController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QVariantList sacola READ sacola NOTIFY sacolaChanged)
    Q_PROPERTY(int tiposNaSacola READ tiposNaSacola NOTIFY sacolaChanged)
    Q_PROPERTY(double totalEstimado READ totalEstimado NOTIFY sacolaChanged)
    Q_PROPERTY(bool bancoPronto READ bancoPronto CONSTANT)
public:
    explicit CompradorController(QObject* parent = nullptr);

    // Catálogo
    Q_INVOKABLE QVariantList feiras() const;
    Q_INVOKABLE QString adicionarFeira(const QString &nome);
    Q_INVOKABLE QVariantMap feira(int id) const;
    Q_INVOKABLE QVariantMap vendedor(int id) const;
    Q_INVOKABLE QVariantList vendedores(int feiraId) const;
    Q_INVOKABLE QVariantList produtos(int feiraId, int vendedorId) const;

    // Carrinho (a propriedade se chama "sacola" por compatibilidade com as telas existentes)
    Q_INVOKABLE bool adicionar(int feiraId, int vendedorId, int produtoId, double quantidade);
    Q_INVOKABLE bool alterarQuantidade(int indice, double novaQuantidade);
    Q_INVOKABLE void remover(int indice);
    Q_INVOKABLE void limpar();

    // Reserva: grava no SQLite, baixa o estoque e devolve o resumo para a tela de feedback.
    // Campos: ok, erro, codigo, total, itens, feiras
    Q_INVOKABLE QVariantMap finalizarReserva(const QString& telefone, const QString& nome, const QString& data, const QString& hora);
    Q_INVOKABLE QVariantList reservas(const QString& telefone, bool vendedor);
    Q_INVOKABLE QString alterarReserva(const QString& telefone, bool vendedor, int id, const QString& status);
    Q_INVOKABLE QString editarProdutoFeirante(const QString& telefone, int id, double preco, double estoque, double estoqueAnterior, const QVariantList& feiras);
    Q_INVOKABLE void atualizar() { if (m_bancoPronto) recarregar(); }
    Q_INVOKABLE QString ultimoErro() const { return repo.ultimoErro(); }

    // Área do feirante: produtos guardados no SQLite. Devolve "" se deu certo, ou a mensagem de erro.
    Q_INVOKABLE QVariantList produtosDoFeirante(const QString& telefone) const;
    Q_INVOKABLE QString adicionarProdutoFeirante(const QString& telefone, const QString& nomeFeirante,
                                                 const QString& banca, const QString& nome, double preco,
                                                 bool porPeso, double estoque, const QVariantList& feiraIds);
    Q_INVOKABLE bool removerProdutoFeirante(const QString& telefone, int produtoId);

    QVariantList sacola() const;
    int tiposNaSacola() const;
    double totalEstimado() const;
    bool bancoPronto() const { return m_bancoPronto; }

signals:
    void sacolaChanged();
    void reservasChanged();
    void produtosChanged();   // catálogo recarregado do banco (novo produto, remoção, reserva)

private:
    void recarregar();
    mutable RepositorioCatalogo repo;
    CatalogoComprador catalogo;
    bool m_bancoPronto = false;
};
#endif
