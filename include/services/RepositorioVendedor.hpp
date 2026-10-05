#ifndef REPOSITORIOVENDEDOR_HPP
#define REPOSITORIOVENDEDOR_HPP

#include <QString>
#include <QStringList>
#include <QList>

struct RegistroProdutoVendedor {
    int id = -1;
    QString nome;
    QString tipoVenda; // "unidade" ou "100g"
    double preco = 0.0;
};

class RepositorioVendedor
{
public:
    RepositorioVendedor() = default;
    ~RepositorioVendedor();

    bool abrir();

    QStringList listarFeiras(
        const QString &telefone
    );

    QList<RegistroProdutoVendedor> listarProdutos(
        const QString &telefone
    );

    bool salvarPerfil(
        const QString &telefone,
        const QStringList &feiras,
        const QList<RegistroProdutoVendedor> &produtos
    );

    QString ultimoErro() const {
        return m_ultimoErro;
    }

private:
    QString m_ultimoErro;
};

#endif