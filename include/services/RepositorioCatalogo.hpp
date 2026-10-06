#ifndef REPOSITORIOCATALOGO_HPP
#define REPOSITORIOCATALOGO_HPP

#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariantMap>
#include "services/CatalogoComprador.hpp"

using namespace std;

struct RegistroProdutoFeirante {
    int id = 0;
    QString nome;
    QString tipoVenda;
    double preco = 0;
    double estoque = 0;
    double reservado = 0;
    bool ativo = true;
    int versao = 0;
    QVector<int> feiraIds;
};

struct AgendamentoReserva {
    int feiraId = 0;
    QString data;
    QString inicio;
    QString fim;
};

struct ItemReserva {
    QString nome;
    QString unidade;
    double quantidade = 0;
    double preco = 0;
};

struct RegistroReserva {
    int id = 0;
    int feiraId = 0;
    int vendedorId = 0;
    QString comprador;
    QString banca;
    QString feira;
    QString local;
    QString data;
    QString inicio;
    QString fim;
    QString status;
    QString criadaEm;
    double total = 0;
    QVector<ItemReserva> itens;
    QVariantMap comoMapa() const;
};

class RepositorioCatalogo {
public:
    explicit RepositorioCatalogo(const QString &caminho = QString());
    ~RepositorioCatalogo();
    bool abrir();
    DadosCatalogo carregar();
    QStringList feirasPendentes(const QString &telefone);
    QVector<int> feirasDoFeirante(const QString &telefone);
    QVector<RegistroProdutoFeirante> produtosDoFeirante(const QString &telefone);
    bool salvarPerfil(const QString &telefone, const QVector<int> &feiras,
                      const QVector<RegistroProdutoFeirante> &produtos,
                      const QVector<RegistroProdutoFeirante> &anteriores);
    bool removerProduto(const QString &telefone, int produtoId);
    QVector<int> salvarReservas(const QString &telefone, const vector<ItemSacolaComprador> &itens,
                               const QVector<AgendamentoReserva> &agendamentos,
                               const DadosCatalogo &dadosEsperados);
    QVector<RegistroReserva> reservasDoUsuario(const QString &telefone, bool vendedor);
    bool alterarReserva(const QString &telefone, bool vendedor, int id, const QString &status);
    QString ultimoErro() const;

private:
    bool criarTabelas();
    bool migrar();
    bool inserirFeiras();
    bool falhar(const QString &erro);
    int vendedorDoUsuario(const QString &telefone, bool criar = false);
    bool usuarioValido(const QString &telefone, const QString &perfil);
    QString m_caminho;
    QString m_conexao;
    QString m_ultimoErro;
};

#endif
