#include "ui/VendedorController.hpp"

VendedorController::VendedorController(
    QObject *parent
)
    : QObject(parent)
{
    m_bancoPronto =
        m_repo.abrir();
}

QVariantList
VendedorController::feiras() const
{
    QVariantList lista;

    for (
        const QString &nome :
        m_feiras
    ) {
        lista.append(
            QVariantMap{
                {"nome", nome}
            }
        );
    }

    return lista;
}

QVariantList
VendedorController::produtos() const
{
    QVariantList lista;

    for (
        const RegistroProdutoVendedor &produto :
        m_produtos
    ) {
        lista.append(
            QVariantMap{
                {"id", produto.id},
                {"nome", produto.nome},
                {
                    "tipoVenda",
                    produto.tipoVenda
                },
                {
                    "preco",
                    produto.preco
                }
            }
        );
    }

    return lista;
}

bool VendedorController::carregarPerfil(
    const QString &telefone
)
{
    if (!m_bancoPronto)
        return false;

    const QString telefoneLimpo =
        telefone.trimmed();

    if (telefoneLimpo.isEmpty())
        return false;

    m_telefone =
        telefoneLimpo;

    m_feiras =
        m_repo.listarFeiras(
            m_telefone
        );

    m_produtos =
        m_repo.listarProdutos(
            m_telefone
        );

    emit perfilChanged();

    return true;
}

bool VendedorController::feiraSelecionada(
    const QString &nome
) const
{
    for (
        const QString &feira :
        m_feiras
    ) {
        if (
            feira.compare(
                nome.trimmed(),
                Qt::CaseInsensitive
            ) == 0
        ) {
            return true;
        }
    }

    return false;
}

bool VendedorController::alternarFeira(
    const QString &nome
)
{
    const QString nomeLimpo =
        nome.trimmed();

    if (nomeLimpo.isEmpty())
        return false;

    for (
        int i = 0;
        i < m_feiras.size();
        ++i
    ) {
        if (
            m_feiras[i].compare(
                nomeLimpo,
                Qt::CaseInsensitive
            ) == 0
        ) {
            m_feiras.removeAt(i);

            emit perfilChanged();

            return true;
        }
    }

    m_feiras.append(
        nomeLimpo
    );

    emit perfilChanged();

    return true;
}

bool VendedorController::adicionarFeira(
    const QString &nome
)
{
    const QString nomeLimpo =
        nome.trimmed();

    if (
        nomeLimpo.isEmpty()
        || feiraSelecionada(nomeLimpo)
    ) {
        return false;
    }

    m_feiras.append(
        nomeLimpo
    );

    emit perfilChanged();

    return true;
}

bool VendedorController::removerFeira(
    const QString &nome
)
{
    for (
        int i = 0;
        i < m_feiras.size();
        ++i
    ) {
        if (
            m_feiras[i].compare(
                nome.trimmed(),
                Qt::CaseInsensitive
            ) == 0
        ) {
            m_feiras.removeAt(i);

            emit perfilChanged();

            return true;
        }
    }

    return false;
}

bool VendedorController::produtoExiste(
    const QString &nome
) const
{
    for (
        const RegistroProdutoVendedor &produto :
        m_produtos
    ) {
        if (
            produto.nome.compare(
                nome.trimmed(),
                Qt::CaseInsensitive
            ) == 0
        ) {
            return true;
        }
    }

    return false;
}

bool VendedorController::adicionarProduto(
    const QString &nome
)
{
    const QString nomeLimpo =
        nome.trimmed();

    if (
        nomeLimpo.isEmpty()
        || produtoExiste(nomeLimpo)
    ) {
        return false;
    }

    RegistroProdutoVendedor produto;

    produto.id = -1;
    produto.nome = nomeLimpo;
    produto.tipoVenda = "unidade";
    produto.preco = 0.0;

    m_produtos.append(
        produto
    );

    emit perfilChanged();

    return true;
}

bool VendedorController::removerProduto(
    int indice
)
{
    if (
        indice < 0
        || indice >= m_produtos.size()
    ) {
        return false;
    }

    m_produtos.removeAt(
        indice
    );

    emit perfilChanged();

    return true;
}

bool VendedorController::definirTipoVenda(
    int indice,
    const QString &tipo
)
{
    if (
        indice < 0
        || indice >= m_produtos.size()
    ) {
        return false;
    }

    if (
        tipo != "unidade"
        && tipo != "100g"
    ) {
        return false;
    }

    m_produtos[indice]
        .tipoVenda = tipo;

    emit perfilChanged();

    return true;
}

bool VendedorController::definirPreco(
    int indice,
    const QString &texto
)
{
    if (
        indice < 0
        || indice >= m_produtos.size()
    ) {
        return false;
    }

    QString valor =
        texto.trimmed();

    valor.remove("R$");

    valor =
        valor.trimmed();

    valor.replace(
        ",",
        "."
    );

    bool ok = false;

    const double preco =
        valor.toDouble(
            &ok
        );

    if (
        !ok
        || preco < 0
    ) {
        return false;
    }

    m_produtos[indice]
        .preco = preco;

    emit perfilChanged();

    return true;
}

bool VendedorController::salvarPerfil()
{
    if (
        !m_bancoPronto
        || m_telefone.isEmpty()
    ) {
        return false;
    }

    const bool sucesso =
        m_repo.salvarPerfil(
            m_telefone,
            m_feiras,
            m_produtos
        );

    if (!sucesso)
        return false;

    // Recarrega para receber os IDs
    // dos produtos recém-criados.

    m_feiras =
        m_repo.listarFeiras(
            m_telefone
        );

    m_produtos =
        m_repo.listarProdutos(
            m_telefone
        );

    emit perfilChanged();

    return true;
}

QString
VendedorController::ultimoErro() const
{
    return m_repo.ultimoErro();
}