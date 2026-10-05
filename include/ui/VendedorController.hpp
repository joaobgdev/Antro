#ifndef VENDEDORCONTROLLER_HPP
#define VENDEDORCONTROLLER_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QtQmlIntegration>

#include "services/RepositorioVendedor.hpp"

class VendedorController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(
        QVariantList feiras
        READ feiras
        NOTIFY perfilChanged
    )

    Q_PROPERTY(
        QVariantList produtos
        READ produtos
        NOTIFY perfilChanged
    )

public:
    explicit VendedorController(
        QObject *parent = nullptr
    );

    QVariantList feiras() const;
    QVariantList produtos() const;

    Q_INVOKABLE bool carregarPerfil(
        const QString &telefone
    );

    Q_INVOKABLE bool feiraSelecionada(
        const QString &nome
    ) const;

    Q_INVOKABLE bool alternarFeira(
        const QString &nome
    );

    Q_INVOKABLE bool adicionarFeira(
        const QString &nome
    );

    Q_INVOKABLE bool removerFeira(
        const QString &nome
    );

    Q_INVOKABLE bool adicionarProduto(
        const QString &nome
    );

    Q_INVOKABLE bool removerProduto(
        int indice
    );

    Q_INVOKABLE bool definirTipoVenda(
        int indice,
        const QString &tipo
    );

    Q_INVOKABLE bool definirPreco(
        int indice,
        const QString &texto
    );

    Q_INVOKABLE bool salvarPerfil();

    Q_INVOKABLE QString ultimoErro() const;

signals:
    void perfilChanged();

private:
    bool produtoExiste(
        const QString &nome
    ) const;

    QString m_telefone;

    QStringList m_feiras;

    QList<RegistroProdutoVendedor>
        m_produtos;

    RepositorioVendedor m_repo;

    bool m_bancoPronto = false;
};

#endif