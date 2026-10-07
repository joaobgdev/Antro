#include "services/RepositorioUsuario.hpp"

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

using namespace std;

// Namespace anônimo para manter constante e função restritas a este arquivo de tradução
namespace {
// Nome da conexão personalizada para a base de dados do Qt
const char *kConexao = "antro_connection";

// Retorna a instância do banco de dados configurada com o nome da conexão
QSqlDatabase banco() { return QSqlDatabase::database(kConexao); }
}

// Destrutor responsável por fechar e remover a conexão com o banco de dados
RepositorioUsuario::~RepositorioUsuario()
{
    {
        // Obtém a conexão sem tentar abrir caso esteja fechada
        QSqlDatabase conexao = QSqlDatabase::database(kConexao, false);
        if (conexao.isValid() && conexao.isOpen())
            conexao.close();
    }
    // Remove o registro da conexão da memória do Qt
    QSqlDatabase::removeDatabase(kConexao);
}

// Configura o caminho do arquivo SQLite e abre a conexão com o banco
bool RepositorioUsuario::abrir()
{
    // Obtém o diretório de dados apropriado para a aplicação no sistema operacional
    const QString pasta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // Garante que o diretório exista no disco
    QDir().mkpath(pasta);
    const QString caminho = pasta + "/antro.db";

    // Adiciona o driver do SQLite para a conexão nomeada
    QSqlDatabase conexao = QSqlDatabase::addDatabase("QSQLITE", kConexao);
    conexao.setDatabaseName(caminho);
    if (!conexao.open()) {
        m_ultimoErro = conexao.lastError().text();
        return false;
    }
    qDebug() << "Banco de dados em:" << caminho;

    // Executa a instrução DDL para criar a tabela de usuários caso não exista
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

// Verifica se já existe um usuário cadastrado com o número de telefone informado
bool RepositorioUsuario::existe(const QString &telefone)
{
    QSqlQuery q(banco());
    // Utiliza consulta preparada para prevenir injeção de SQL
    q.prepare("SELECT 1 FROM users WHERE phone = ?");
    q.addBindValue(telefone);
    if (!q.exec()) {
        m_ultimoErro = q.lastError().text();
        return false;
    }
    // Retorna true se houver ao menos um registro retornado
    return q.next();
}

// Insere um novo registro de usuário na tabela
bool RepositorioUsuario::inserir(const RegistroUsuario &r)
{
    QSqlQuery q(banco());
    // Prepara a instrução de inserção parametrizada
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

// Busca os dados de um usuário pelo telefone e retorna uma instância de RegistroUsuario caso encontrado
optional<RegistroUsuario> RepositorioUsuario::buscarPorTelefone(const QString &telefone)
{
    QSqlQuery q(banco());
    q.prepare("SELECT role, name, phone, market_name, ocs_number, password_hash, salt "
              "FROM users WHERE phone = ?");
    q.addBindValue(telefone);
    if (!q.exec()) {
        m_ultimoErro = q.lastError().text();
        return nullopt;
    }
    // Retorna nulo caso o usuário não exista no banco
    if (!q.next())
        return nullopt;

    // Mapeia os dados da consulta para a estrutura RegistroUsuario
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
