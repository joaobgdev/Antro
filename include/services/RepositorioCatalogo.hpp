#ifndef REPOSITORIOCATALOGO_HPP
#define REPOSITORIOCATALOGO_HPP

#include <QString>
#include <QVector>
#include <QVariantList>
#include "services/CatalogoComprador.hpp"

// Uma linha de produto do feirante (tela "Seus produtos")
struct RegistroProdutoFeirante {
    int id;
    QString nome;
    double preco;
    bool porPeso;
    double estoque;
    QVector<int> feiraIds;
};

// Conhece o SQL do catálogo (feiras, vendedores, produtos, ofertas) e das reservas.
// Usa o mesmo arquivo antro.db do RepositorioUsuario, em uma conexão própria.
class RepositorioCatalogo {
public:
    RepositorioCatalogo() = default;
    ~RepositorioCatalogo();

    bool abrir();   // abre antro.db, cria as tabelas e, se estiverem vazias, insere as feiras iniciais
    DadosCatalogo carregar();
    bool inserirFeira(const QString &nome);

    // Produtos cadastrados por feirantes
    bool inserirProduto(const QString &telefone, const QString &nomeFeirante, const QString &banca,
                        const QString &nome, double preco, bool porPeso, double estoque,
                        const QVector<int> &feiraIds);
    QVector<RegistroProdutoFeirante> produtosDoFeirante(const QString &telefone);
    bool removerProduto(const QString &telefone, int produtoId);

    // Reservas: grava a reserva e baixa o estoque na mesma transação. Devolve o código (0 = falhou).
    int salvarReserva(const QString &telefoneComprador, const QString &nomeComprador,
                      const std::vector<ItemSacolaComprador> &itens, double total,
                      const QString &data, const QString &hora);
    bool editarProduto(const QString &telefone, int id, double preco, double estoque, double estoqueAnterior, const QVector<int> &feiras);
    QVariantList reservas(const QString &telefone, bool vendedor);
    bool alterarReserva(const QString &telefone, bool vendedor, int id, const QString &status);

    QString ultimoErro() const { return m_ultimoErro; }

private:
    bool criarTabelas();
    bool inserirFeirasIniciais();
    bool migrar();
    QString m_ultimoErro;
};

#endif // REPOSITORIOCATALOGO_HPP
