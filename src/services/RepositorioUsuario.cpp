#include "services/RepositorioUsuario.hpp"

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace {
const char *kConexao = "antro_connection";

QSqlDatabase banco() { return QSqlDatabase::database(kConexao); }
}

RepositorioUsuario::~RepositorioUsuario()
{
    {
        QSqlDatabase conexao = QSqlDatabase::database(kConexao, false);
        if (conexao.isValid() && conexao.isOpen())
            conexao.close();
    }
    QSqlDatabase::removeDatabase(kConexao);
}

bool RepositorioUsuario::abrir()
{
    const QString pasta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(pasta);
    const QString caminho = pasta + "/antro.db";

    QSqlDatabase conexao = QSqlDatabase::addDatabase("QSQLITE", kConexao);
    conexao.setDatabaseName(caminho);
    if (!conexao.open()) {
        m_ultimoErro = conexao.lastError().text();
        return false;
    }
    qDebug() << "Banco de dados em:" << caminho;

    QSqlQuery q(conexao);
    const bool ok = q.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id            INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  phone         TEXT NOT NULL UNIQUE,"
        "  role          TEXT NOT NULL,"
        "  name          TEXT NOT NULL,"
        "  market_name   TEXT,"
        "  ocs_number    TEXT,"
        "  password_hash TEXT NOT NULL,"
        "  salt          TEXT NOT NULL"
        ")");
    if (!ok)
        m_ultimoErro = q.lastError().text();
    return ok;
}

bool RepositorioUsuario::existe(const QString &telefone)
{
    QSqlQuery q(banco());
    q.prepare("SELECT 1 FROM users WHERE phone = ?");
    q.addBindValue(telefone);
    if (!q.exec()) {
        m_ultimoErro = q.lastError().text();
        return false;
    }
    return q.next();
}

bool RepositorioUsuario::inserir(const RegistroUsuario &r)
{
    QSqlQuery q(banco());
    q.prepare("INSERT INTO users (phone, role, name, market_name, ocs_number, password_hash, salt) "
              "VALUES (?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(r.telefone);
    q.addBindValue(r.perfil);
    q.addBindValue(r.nome);
    q.addBindValue(r.nomeBanca);
    q.addBindValue(r.codigoOCS);
    q.addBindValue(r.hashSenha);
    q.addBindValue(r.sal);
    if (!q.exec()) {
        m_ultimoErro = q.lastError().text();
        return false;
    }
    return true;
}

std::optional<RegistroUsuario> RepositorioUsuario::buscarPorTelefone(const QString &telefone)
{
    QSqlQuery q(banco());
    q.prepare("SELECT role, name, phone, market_name, ocs_number, password_hash, salt "
              "FROM users WHERE phone = ?");
    q.addBindValue(telefone);
    if (!q.exec()) {
        m_ultimoErro = q.lastError().text();
        return std::nullopt;
    }
    if (!q.next())
        return std::nullopt;

    RegistroUsuario r;
    r.perfil    = q.value(0).toString();
    r.nome      = q.value(1).toString();
    r.telefone  = q.value(2).toString();
    r.nomeBanca = q.value(3).toString();
    r.codigoOCS = q.value(4).toString();
    r.hashSenha = q.value(5).toString();
    r.sal       = q.value(6).toString();
    return r;
}
