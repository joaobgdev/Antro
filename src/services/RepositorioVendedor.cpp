#include "services/RepositorioVendedor.hpp"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

namespace {

const char *kConexaoVendedor =
    "antro_vendor_connection";

QSqlDatabase bancoVendedor()
{
    return QSqlDatabase::database(
        kConexaoVendedor
    );
}

}

RepositorioVendedor::~RepositorioVendedor()
{
    {
        QSqlDatabase conexao =
            QSqlDatabase::database(
                kConexaoVendedor,
                false
            );

        if (
            conexao.isValid()
            && conexao.isOpen()
        ) {
            conexao.close();
        }
    }

    QSqlDatabase::removeDatabase(
        kConexaoVendedor
    );
}

bool RepositorioVendedor::abrir()
{
    const QString pasta =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
        );

    QDir().mkpath(pasta);

    const QString caminho =
        pasta + "/antro.db";

    QSqlDatabase conexao;

    if (
        QSqlDatabase::contains(
            kConexaoVendedor
        )
    ) {
        conexao =
            QSqlDatabase::database(
                kConexaoVendedor
            );
    } else {
        conexao =
            QSqlDatabase::addDatabase(
                "QSQLITE",
                kConexaoVendedor
            );
    }

    conexao.setDatabaseName(caminho);

    if (!conexao.open()) {
        m_ultimoErro =
            conexao.lastError().text();

        return false;
    }

    QSqlQuery pragma(conexao);

    if (!pragma.exec(
        "PRAGMA foreign_keys = ON"
    )) {
        m_ultimoErro =
            pragma.lastError().text();

        return false;
    }

    QSqlQuery feira(conexao);

    const bool feiraOk = feira.exec(
        "CREATE TABLE IF NOT EXISTS seller_markets ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  seller_phone TEXT NOT NULL,"
        "  market_name TEXT NOT NULL COLLATE NOCASE,"
        "  FOREIGN KEY (seller_phone) "
        "      REFERENCES users(phone) "
        "      ON DELETE CASCADE,"
        "  UNIQUE(seller_phone, market_name)"
        ")"
    );

    if (!feiraOk) {
        m_ultimoErro =
            feira.lastError().text();

        return false;
    }

    QSqlQuery produto(conexao);

    const bool produtoOk = produto.exec(
        "CREATE TABLE IF NOT EXISTS seller_products ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  seller_phone TEXT NOT NULL,"
        "  name TEXT NOT NULL COLLATE NOCASE,"
        "  sale_type TEXT NOT NULL "
        "      DEFAULT 'unidade' "
        "      CHECK(sale_type IN ('unidade', '100g')),"
        "  price REAL NOT NULL DEFAULT 0 "
        "      CHECK(price >= 0),"
        "  FOREIGN KEY (seller_phone) "
        "      REFERENCES users(phone) "
        "      ON DELETE CASCADE,"
        "  UNIQUE(seller_phone, name)"
        ")"
    );

    if (!produtoOk) {
        m_ultimoErro =
            produto.lastError().text();

        return false;
    }

    return true;
}

QStringList RepositorioVendedor::listarFeiras(
    const QString &telefone
)
{
    QStringList resultado;

    QSqlQuery query(
        bancoVendedor()
    );

    query.prepare(
        "SELECT market_name "
        "FROM seller_markets "
        "WHERE seller_phone = ? "
        "ORDER BY id"
    );

    query.addBindValue(
        telefone
    );

    if (!query.exec()) {
        m_ultimoErro =
            query.lastError().text();

        return resultado;
    }

    while (query.next()) {
        resultado.append(
            query.value(0).toString()
        );
    }

    return resultado;
}

QList<RegistroProdutoVendedor>
RepositorioVendedor::listarProdutos(
    const QString &telefone
)
{
    QList<RegistroProdutoVendedor>
        resultado;

    QSqlQuery query(
        bancoVendedor()
    );

    query.prepare(
        "SELECT id, name, sale_type, price "
        "FROM seller_products "
        "WHERE seller_phone = ? "
        "ORDER BY id"
    );

    query.addBindValue(
        telefone
    );

    if (!query.exec()) {
        m_ultimoErro =
            query.lastError().text();

        return resultado;
    }

    while (query.next()) {

        RegistroProdutoVendedor produto;

        produto.id =
            query.value(0).toInt();

        produto.nome =
            query.value(1).toString();

        produto.tipoVenda =
            query.value(2).toString();

        produto.preco =
            query.value(3).toDouble();

        resultado.append(
            produto
        );
    }

    return resultado;
}

bool RepositorioVendedor::salvarPerfil(
    const QString &telefone,
    const QStringList &feiras,
    const QList<RegistroProdutoVendedor> &produtos
)
{
    QSqlDatabase banco =
        bancoVendedor();

    if (!banco.transaction()) {
        m_ultimoErro =
            banco.lastError().text();

        return false;
    }

    QSqlQuery apagarFeiras(banco);

    apagarFeiras.prepare(
        "DELETE FROM seller_markets "
        "WHERE seller_phone = ?"
    );

    apagarFeiras.addBindValue(
        telefone
    );

    if (!apagarFeiras.exec()) {
        m_ultimoErro =
            apagarFeiras.lastError().text();

        banco.rollback();
        return false;
    }

    for (const QString &nomeFeira : feiras) {

        QSqlQuery inserir(banco);

        inserir.prepare(
            "INSERT INTO seller_markets "
            "(seller_phone, market_name) "
            "VALUES (?, ?)"
        );

        inserir.addBindValue(
            telefone
        );

        inserir.addBindValue(
            nomeFeira.trimmed()
        );

        if (!inserir.exec()) {
            m_ultimoErro =
                inserir.lastError().text();

            banco.rollback();
            return false;
        }
    }

    QList<int> idsMantidos;

    for (
        const RegistroProdutoVendedor &produto :
        produtos
    ) {
        if (produto.id > 0) {
            idsMantidos.append(
                produto.id
            );
        }
    }

    if (idsMantidos.isEmpty()) {

        QSqlQuery apagarTodos(banco);

        apagarTodos.prepare(
            "DELETE FROM seller_products "
            "WHERE seller_phone = ?"
        );

        apagarTodos.addBindValue(
            telefone
        );

        if (!apagarTodos.exec()) {
            m_ultimoErro =
                apagarTodos.lastError().text();

            banco.rollback();
            return false;
        }

    } else {

        QStringList placeholders;

        for (
            int i = 0;
            i < idsMantidos.size();
            ++i
        ) {
            placeholders.append("?");
        }

        QSqlQuery apagarRemovidos(banco);

        apagarRemovidos.prepare(
            "DELETE FROM seller_products "
            "WHERE seller_phone = ? "
            "AND id NOT IN ("
            + placeholders.join(",")
            + ")"
        );

        apagarRemovidos.addBindValue(
            telefone
        );

        for (int id : idsMantidos) {
            apagarRemovidos.addBindValue(
                id
            );
        }

        if (!apagarRemovidos.exec()) {
            m_ultimoErro =
                apagarRemovidos
                    .lastError()
                    .text();

            banco.rollback();
            return false;
        }
    }

    for (
        const RegistroProdutoVendedor &produto :
        produtos
    ) {

        if (produto.id > 0) {

            QSqlQuery atualizar(banco);

            atualizar.prepare(
                "UPDATE seller_products "
                "SET name = ?, "
                "    sale_type = ?, "
                "    price = ? "
                "WHERE id = ? "
                "AND seller_phone = ?"
            );

            atualizar.addBindValue(
                produto.nome.trimmed()
            );

            atualizar.addBindValue(
                produto.tipoVenda
            );

            atualizar.addBindValue(
                produto.preco
            );

            atualizar.addBindValue(
                produto.id
            );

            atualizar.addBindValue(
                telefone
            );

            if (!atualizar.exec()) {
                m_ultimoErro =
                    atualizar
                        .lastError()
                        .text();

                banco.rollback();
                return false;
            }

        } else {

            QSqlQuery inserir(banco);

            inserir.prepare(
                "INSERT INTO seller_products "
                "(seller_phone, name, sale_type, price) "
                "VALUES (?, ?, ?, ?)"
            );

            inserir.addBindValue(
                telefone
            );

            inserir.addBindValue(
                produto.nome.trimmed()
            );

            inserir.addBindValue(
                produto.tipoVenda
            );

            inserir.addBindValue(
                produto.preco
            );

            if (!inserir.exec()) {
                m_ultimoErro =
                    inserir
                        .lastError()
                        .text();

                banco.rollback();
                return false;
            }
        }
    }

    if (!banco.commit()) {
        m_ultimoErro =
            banco.lastError().text();

        banco.rollback();
        return false;
    }

    return true;
}