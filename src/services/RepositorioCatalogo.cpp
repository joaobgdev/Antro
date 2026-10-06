#include "services/RepositorioCatalogo.hpp"
#include "services/AgendaFeira.hpp"
#include <QDir>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <cmath>

namespace {
int numeroConexao = 0;

bool colunaExiste(QSqlDatabase db, const QString &tabela, const QString &coluna)
{
    QSqlQuery q(db);
    q.exec("PRAGMA table_info(" + tabela + ")");
    while (q.next()) if (q.value(1).toString() == coluna) return true;
    return false;
}

bool quantidadeValida(double quantidade, const QString &tipo)
{
    if (!std::isfinite(quantidade) || quantidade <= 0) return false;
    double passo = tipo == "100g" ? 0.1 : tipo == "kg" ? 0.5 : 1.0;
    double partes = quantidade / passo;
    return std::abs(partes - std::round(partes)) < 0.00001;
}
}

RepositorioCatalogo::RepositorioCatalogo(const QString &caminho)
    : m_caminho(caminho), m_conexao("antro_catalogo_" + QString::number(++numeroConexao)) {}

RepositorioCatalogo::~RepositorioCatalogo()
{
    {
        QSqlDatabase db = QSqlDatabase::database(m_conexao, false);
        if (db.isValid()) db.close();
    }
    QSqlDatabase::removeDatabase(m_conexao);
}

QString RepositorioCatalogo::ultimoErro() const { return m_ultimoErro; }
bool RepositorioCatalogo::falhar(const QString &erro) { m_ultimoErro = erro; return false; }

bool RepositorioCatalogo::abrir()
{
    if (QSqlDatabase::contains(m_conexao)) return QSqlDatabase::database(m_conexao).isOpen();
    if (m_caminho.isEmpty()) {
        QString pasta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(pasta);
        m_caminho = pasta + "/antro.db";
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_conexao);
    db.setDatabaseName(m_caminho);
    if (!db.open()) return falhar(db.lastError().text());
    QSqlQuery q(db);
    if (!q.exec("PRAGMA foreign_keys = ON") || !q.exec("PRAGMA busy_timeout = 3000"))
        return falhar(q.lastError().text());
    if (!db.transaction()) return falhar(db.lastError().text());
    if (!criarTabelas() || !inserirFeiras() || !migrar()) { db.rollback(); return false; }
    if (!db.commit()) return falhar(db.lastError().text());
    return true;
}

bool RepositorioCatalogo::criarTabelas()
{
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    const QStringList comandos = {
        "CREATE TABLE IF NOT EXISTS feiras (id INTEGER PRIMARY KEY, nome TEXT NOT NULL, bairro TEXT NOT NULL, local TEXT NOT NULL, horario TEXT NOT NULL)",
        "CREATE TABLE IF NOT EXISTS vendedores (id INTEGER PRIMARY KEY AUTOINCREMENT, user_phone TEXT UNIQUE, nome TEXT NOT NULL, banca TEXT NOT NULL, descricao TEXT NOT NULL DEFAULT '')",
        "CREATE TABLE IF NOT EXISTS produtos (id INTEGER PRIMARY KEY AUTOINCREMENT, vendedor_id INTEGER NOT NULL REFERENCES vendedores(id), nome TEXT NOT NULL, preco REAL NOT NULL, por_peso INTEGER NOT NULL, estoque REAL NOT NULL)",
        "CREATE TABLE IF NOT EXISTS ofertas (feira_id INTEGER NOT NULL REFERENCES feiras(id), produto_id INTEGER NOT NULL REFERENCES produtos(id) ON DELETE CASCADE, PRIMARY KEY(feira_id, produto_id))",
        "CREATE TABLE IF NOT EXISTS participacoes (vendedor_id INTEGER NOT NULL REFERENCES vendedores(id), feira_id INTEGER NOT NULL REFERENCES feiras(id), PRIMARY KEY(vendedor_id, feira_id))",
        "CREATE TABLE IF NOT EXISTS reservas (id INTEGER PRIMARY KEY AUTOINCREMENT, comprador_phone TEXT NOT NULL, comprador_nome TEXT NOT NULL, criada_em TEXT NOT NULL, total REAL NOT NULL, status TEXT NOT NULL DEFAULT 'SOLICITADA')",
        "CREATE TABLE IF NOT EXISTS reserva_itens (id INTEGER PRIMARY KEY AUTOINCREMENT, reserva_id INTEGER NOT NULL REFERENCES reservas(id) ON DELETE CASCADE, feira_id INTEGER NOT NULL, vendedor_id INTEGER NOT NULL, produto_id INTEGER NOT NULL, produto_nome TEXT NOT NULL, unidade TEXT NOT NULL, quantidade REAL NOT NULL, preco REAL NOT NULL)",
        "CREATE TABLE IF NOT EXISTS configuracoes (chave TEXT PRIMARY KEY, valor TEXT NOT NULL)",
        "CREATE TABLE IF NOT EXISTS produtos_migrados (id_antigo INTEGER PRIMARY KEY, produto_id INTEGER NOT NULL)",
        "CREATE TABLE IF NOT EXISTS feiras_migracao_pendentes (telefone TEXT NOT NULL, nome TEXT NOT NULL, PRIMARY KEY(telefone, nome))"
    };
    QSqlQuery q(db);
    for (const QString &sql : comandos) if (!q.exec(sql)) return falhar(q.lastError().text());
    const QVector<QStringList> colunas = {
        {"feiras", "dia_semana", "INTEGER NOT NULL DEFAULT 0"},
        {"feiras", "hora_inicio", "TEXT NOT NULL DEFAULT ''"},
        {"feiras", "hora_fim", "TEXT NOT NULL DEFAULT ''"},
        {"produtos", "ativo", "INTEGER NOT NULL DEFAULT 1"},
        {"produtos", "removido", "INTEGER NOT NULL DEFAULT 0"},
        {"produtos", "versao", "INTEGER NOT NULL DEFAULT 0"},
        {"reservas", "vendedor_id", "INTEGER"},
        {"reservas", "feira_id", "INTEGER"},
        {"reservas", "data_retirada", "TEXT NOT NULL DEFAULT ''"},
        {"reservas", "hora_inicio", "TEXT NOT NULL DEFAULT ''"},
        {"reservas", "hora_fim", "TEXT NOT NULL DEFAULT ''"},
        {"reservas", "origem_id", "INTEGER"}
    };
    for (const QStringList &c : colunas) {
        if (!colunaExiste(db, c[0], c[1]) && !q.exec("ALTER TABLE " + c[0] + " ADD COLUMN " + c[1] + " " + c[2]))
            return falhar(q.lastError().text());
    }
    if (!colunaExiste(db, "produtos", "tipo_venda")) {
        if (!q.exec("ALTER TABLE produtos ADD COLUMN tipo_venda TEXT NOT NULL DEFAULT 'unidade'")
            || !q.exec("UPDATE produtos SET tipo_venda = CASE WHEN por_peso = 1 THEN 'kg' ELSE 'unidade' END"))
            return falhar(q.lastError().text());
    }
    return true;
}

bool RepositorioCatalogo::inserirFeiras()
{
    QSqlQuery q(QSqlDatabase::database(m_conexao));
    if (!q.exec("SELECT COUNT(*) FROM feiras") || !q.next()) return falhar(q.lastError().text());
    if (q.value(0).toInt() > 0) return true;
    const QStringList comandos = {
        "INSERT INTO feiras VALUES (1, 'Feira Agroecológica da Várzea', 'Várzea', 'Praça da Várzea', 'Sábados, 7h às 10h', 6, '07:00', '10:00')",
        "INSERT INTO feiras VALUES (2, 'Espaço Agroecológico das Graças', 'Graças', 'Rua Andrade de Souza', 'Sábados, 4h às 11h', 6, '04:00', '11:00')",
        "INSERT INTO feiras VALUES (3, 'Feira de Produtos Orgânicos de Casa Forte', 'Casa Forte', 'Praça da Vitória Régia', 'Sábados, 5h às 11h', 6, '05:00', '11:00')",
        "INSERT INTO feiras VALUES (4, 'Espaço Agroecológico do Sítio da Trindade', 'Casa Amarela', 'Sítio da Trindade', 'Sábados, 5h às 11h', 6, '05:00', '11:00')"
    };
    for (const QString &sql : comandos) if (!q.exec(sql)) return falhar(q.lastError().text());
    return true;
}

bool RepositorioCatalogo::migrar()
{
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    QSqlQuery q(db);
    if (!q.exec("SELECT valor FROM configuracoes WHERE chave = 'catalogo_unificado_1'")) return falhar(q.lastError().text());
    if (q.next()) return true;
    if (!q.exec("UPDATE feiras SET dia_semana = 6, hora_inicio = CASE id WHEN 1 THEN '07:00' WHEN 2 THEN '04:00' ELSE '05:00' END, hora_fim = CASE id WHEN 1 THEN '10:00' ELSE '11:00' END WHERE dia_semana = 0 AND id IN (1,2,3,4)"))
        return falhar(q.lastError().text());
    if (!q.exec("INSERT OR IGNORE INTO participacoes SELECT DISTINCT p.vendedor_id, o.feira_id FROM ofertas o JOIN produtos p ON p.id = o.produto_id"))
        return falhar(q.lastError().text());

    if (db.tables().contains("seller_markets")) {
        QSqlQuery antigos(db);
        if (!antigos.exec("SELECT seller_phone, market_name FROM seller_markets ORDER BY id")) return falhar(antigos.lastError().text());
        while (antigos.next()) {
            QString telefone = antigos.value(0).toString();
            int vendedor = vendedorDoUsuario(telefone, true);
            if (!vendedor) return false;
            QString nome = antigos.value(1).toString().trimmed();
            QString bairro;
            if (nome.compare("Feira da Várzea", Qt::CaseInsensitive) == 0) bairro = "Várzea";
            if (nome.compare("Feira de Casa Forte", Qt::CaseInsensitive) == 0) bairro = "Casa Forte";
            QSqlQuery feira(db);
            feira.prepare("SELECT id FROM feiras WHERE lower(nome) = lower(?) OR (bairro = ? AND ? <> '')");
            feira.addBindValue(nome); feira.addBindValue(bairro); feira.addBindValue(bairro);
            if (!feira.exec()) return falhar(feira.lastError().text());
            if (feira.next()) {
                q.prepare("INSERT OR IGNORE INTO participacoes VALUES (?, ?)");
                q.addBindValue(vendedor); q.addBindValue(feira.value(0));
            } else {
                q.prepare("INSERT OR IGNORE INTO feiras_migracao_pendentes VALUES (?, ?)");
                q.addBindValue(telefone); q.addBindValue(nome);
            }
            if (!q.exec()) return falhar(q.lastError().text());
        }
    }
    if (db.tables().contains("seller_products")) {
        QSqlQuery antigos(db);
        if (!antigos.exec("SELECT id, seller_phone, name, sale_type, price FROM seller_products ORDER BY id")) return falhar(antigos.lastError().text());
        while (antigos.next()) {
            int vendedor = vendedorDoUsuario(antigos.value(1).toString(), true);
            if (!vendedor) return false;
            QString tipo = antigos.value(3).toString();
            double preco = antigos.value(4).toDouble() * (tipo == "100g" ? 10 : 1);
            q.prepare("INSERT INTO produtos(vendedor_id, nome, preco, por_peso, estoque, tipo_venda, ativo) VALUES (?, ?, ?, ?, 0, ?, 1)");
            q.addBindValue(vendedor); q.addBindValue(antigos.value(2)); q.addBindValue(preco);
            q.addBindValue(tipo != "unidade"); q.addBindValue(tipo);
            if (!q.exec()) return falhar(q.lastError().text());
            int id = q.lastInsertId().toInt();
            q.prepare("INSERT INTO produtos_migrados VALUES (?, ?)");
            q.addBindValue(antigos.value(0)); q.addBindValue(id);
            if (!q.exec()) return falhar(q.lastError().text());
            q.prepare("INSERT INTO ofertas SELECT feira_id, ? FROM participacoes WHERE vendedor_id = ?");
            q.addBindValue(id); q.addBindValue(vendedor);
            if (!q.exec()) return falhar(q.lastError().text());
        }
    }

    QSqlQuery reservas(db);
    if (!reservas.exec("SELECT id, comprador_phone, comprador_nome, criada_em, status FROM reservas WHERE vendedor_id IS NULL")) return falhar(reservas.lastError().text());
    while (reservas.next()) {
        int original = reservas.value(0).toInt();
        QSqlQuery grupos(db);
        grupos.prepare("SELECT vendedor_id, feira_id, SUM(quantidade * preco) FROM reserva_itens WHERE reserva_id = ? GROUP BY vendedor_id, feira_id");
        grupos.addBindValue(original);
        if (!grupos.exec()) return falhar(grupos.lastError().text());
        bool primeiro = true;
        while (grupos.next()) {
            int destino = original;
            if (!primeiro) {
                q.prepare("INSERT INTO reservas(comprador_phone, comprador_nome, criada_em, total, status, origem_id) VALUES (?, ?, ?, ?, ?, ?)");
                q.addBindValue(reservas.value(1)); q.addBindValue(reservas.value(2)); q.addBindValue(reservas.value(3));
                q.addBindValue(grupos.value(2)); q.addBindValue(reservas.value(4)); q.addBindValue(original);
                if (!q.exec()) return falhar(q.lastError().text());
                destino = q.lastInsertId().toInt();
            }
            q.prepare("UPDATE reservas SET vendedor_id = ?, feira_id = ?, total = ?, origem_id = ? WHERE id = ?");
            q.addBindValue(grupos.value(0)); q.addBindValue(grupos.value(1)); q.addBindValue(grupos.value(2)); q.addBindValue(original); q.addBindValue(destino);
            if (!q.exec()) return falhar(q.lastError().text());
            if (!primeiro) {
                q.prepare("UPDATE reserva_itens SET reserva_id = ? WHERE reserva_id = ? AND vendedor_id = ? AND feira_id = ?");
                q.addBindValue(destino); q.addBindValue(original); q.addBindValue(grupos.value(0)); q.addBindValue(grupos.value(1));
                if (!q.exec()) return falhar(q.lastError().text());
            }
            primeiro = false;
        }
    }
    if (!q.exec("INSERT INTO configuracoes VALUES ('catalogo_unificado_1', '1')")) return falhar(q.lastError().text());
    return true;
}

bool RepositorioCatalogo::usuarioValido(const QString &telefone, const QString &perfil)
{
    QSqlQuery q(QSqlDatabase::database(m_conexao));
    q.prepare("SELECT 1 FROM users WHERE phone = ? AND role = ?");
    q.addBindValue(telefone); q.addBindValue(perfil);
    if (!q.exec()) return falhar(q.lastError().text());
    if (!q.next()) return falhar("Entre com o perfil correto para realizar esta operação.");
    return true;
}

int RepositorioCatalogo::vendedorDoUsuario(const QString &telefone, bool criar)
{
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    if (!usuarioValido(telefone, "feirante")) return 0;
    QSqlQuery q(db);
    if (criar) {
        q.prepare("INSERT OR IGNORE INTO vendedores(user_phone, nome, banca) SELECT phone, name, COALESCE(market_name, '') FROM users WHERE phone = ?");
        q.addBindValue(telefone);
        if (!q.exec()) { falhar(q.lastError().text()); return 0; }
    }
    q.prepare("SELECT id FROM vendedores WHERE user_phone = ?"); q.addBindValue(telefone);
    if (!q.exec()) { falhar(q.lastError().text()); return 0; }
    return q.next() ? q.value(0).toInt() : 0;
}

DadosCatalogo RepositorioCatalogo::carregar()
{
    DadosCatalogo dados;
    QSqlQuery q(QSqlDatabase::database(m_conexao));
    if (!q.exec("SELECT id, nome, bairro, local, horario, dia_semana, hora_inicio, hora_fim FROM feiras ORDER BY id")) { falhar(q.lastError().text()); return dados; }
    while (q.next()) dados.feiras.push_back({q.value(0).toInt(), q.value(1).toString().toStdString(), q.value(2).toString().toStdString(), q.value(3).toString().toStdString(), q.value(4).toString().toStdString(), q.value(5).toInt(), q.value(6).toString().toStdString(), q.value(7).toString().toStdString()});
    if (!q.exec("SELECT id, nome, banca, descricao, user_phone FROM vendedores ORDER BY id")) { falhar(q.lastError().text()); return {}; }
    while (q.next()) dados.vendedores.push_back({q.value(0).toInt(), q.value(1).toString().toStdString(), q.value(2).toString().toStdString(), q.value(3).toString().toStdString(), q.value(4).toString().isEmpty()});
    if (!q.exec("SELECT id, nome, preco, por_peso, estoque, tipo_venda FROM produtos WHERE ativo = 1 ORDER BY id")) { falhar(q.lastError().text()); return {}; }
    while (q.next()) dados.produtos.push_back(Produto(q.value(0).toInt(), q.value(1).toString().toStdString(), q.value(2).toDouble(), q.value(3).toBool(), q.value(4).toDouble(), q.value(5).toString() == "100g" ? 0.1 : 0));
    if (!q.exec("SELECT o.feira_id, p.vendedor_id, o.produto_id FROM ofertas o JOIN produtos p ON p.id = o.produto_id WHERE p.ativo = 1")) { falhar(q.lastError().text()); return {}; }
    while (q.next()) dados.ofertas.push_back({q.value(0).toInt(), q.value(1).toInt(), q.value(2).toInt()});
    if (!q.exec("SELECT feira_id, vendedor_id FROM participacoes")) { falhar(q.lastError().text()); return {}; }
    while (q.next()) dados.participacoes.push_back({q.value(0).toInt(), q.value(1).toInt()});
    m_ultimoErro.clear();
    return dados;
}

QStringList RepositorioCatalogo::feirasPendentes(const QString &telefone)
{
    QStringList lista;
    QSqlQuery q(QSqlDatabase::database(m_conexao));
    q.prepare("SELECT nome FROM feiras_migracao_pendentes WHERE telefone = ?"); q.addBindValue(telefone);
    if (!q.exec()) { falhar(q.lastError().text()); return lista; }
    while (q.next()) lista.append(q.value(0).toString());
    return lista;
}

QVector<int> RepositorioCatalogo::feirasDoFeirante(const QString &telefone)
{
    QVector<int> lista;
    QSqlQuery q(QSqlDatabase::database(m_conexao));
    q.prepare("SELECT p.feira_id FROM participacoes p JOIN vendedores v ON v.id = p.vendedor_id WHERE v.user_phone = ? ORDER BY p.feira_id"); q.addBindValue(telefone);
    if (!q.exec()) { falhar(q.lastError().text()); return lista; }
    while (q.next()) lista.append(q.value(0).toInt());
    return lista;
}

QVector<RegistroProdutoFeirante> RepositorioCatalogo::produtosDoFeirante(const QString &telefone)
{
    QVector<RegistroProdutoFeirante> lista;
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    QSqlQuery q(db);
    q.prepare("SELECT p.id, p.nome, p.preco, p.tipo_venda, p.estoque, p.ativo, COALESCE((SELECT SUM(i.quantidade) FROM reserva_itens i JOIN reservas r ON r.id = i.reserva_id WHERE i.produto_id = p.id AND r.status IN ('SOLICITADA','ACEITA')),0), p.versao FROM produtos p JOIN vendedores v ON v.id = p.vendedor_id WHERE v.user_phone = ? AND p.removido = 0 ORDER BY p.id"); q.addBindValue(telefone);
    if (!q.exec()) { falhar(q.lastError().text()); return lista; }
    while (q.next()) {
        RegistroProdutoFeirante p;
        p.id = q.value(0).toInt(); p.nome = q.value(1).toString(); p.tipoVenda = q.value(3).toString();
        p.preco = q.value(2).toDouble() / (p.tipoVenda == "100g" ? 10 : 1);
        p.estoque = q.value(4).toDouble(); p.ativo = q.value(5).toBool(); p.reservado = q.value(6).toDouble();
        p.versao = q.value(7).toInt();
        QSqlQuery f(db); f.prepare("SELECT feira_id FROM ofertas WHERE produto_id = ?"); f.addBindValue(p.id);
        if (!f.exec()) { falhar(f.lastError().text()); return {}; }
        while (f.next()) p.feiraIds.append(f.value(0).toInt());
        lista.append(p);
    }
    return lista;
}

bool RepositorioCatalogo::salvarPerfil(const QString &telefone, const QVector<int> &feiras,
                                     const QVector<RegistroProdutoFeirante> &produtos,
                                     const QVector<RegistroProdutoFeirante> &anteriores)
{
    m_ultimoErro.clear();
    if (!usuarioValido(telefone, "feirante")) return false;
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    if (!db.transaction()) return falhar(db.lastError().text());
    int vendedor = vendedorDoUsuario(telefone, true);
    if (!vendedor) { db.rollback(); return false; }
    QSqlQuery q(db);
    q.prepare("SELECT id, versao FROM produtos WHERE vendedor_id = ? AND removido = 0"); q.addBindValue(vendedor);
    if (!q.exec()) { db.rollback(); return falhar(q.lastError().text()); }
    int encontrados = 0;
    while (q.next()) {
        bool confere = false;
        for (const RegistroProdutoFeirante &p : anteriores)
            if (p.id == q.value(0).toInt() && p.versao == q.value(1).toInt()) confere = true;
        if (!confere) { db.rollback(); return falhar("Seus produtos ou estoque mudaram. Reabra o perfil antes de salvar."); }
        ++encontrados;
    }
    if (encontrados != anteriores.size()) { db.rollback(); return falhar("Seus produtos mudaram. Reabra o perfil antes de salvar."); }
    QVector<int> ids;
    for (int feira : feiras) {
        if (ids.contains(feira)) continue;
        q.prepare("SELECT 1 FROM feiras WHERE id = ?"); q.addBindValue(feira);
        if (!q.exec() || !q.next()) { db.rollback(); return falhar("Escolha uma feira pré-cadastrada."); }
        ids.append(feira);
    }
    q.prepare("DELETE FROM participacoes WHERE vendedor_id = ?"); q.addBindValue(vendedor);
    if (!q.exec()) { db.rollback(); return falhar(q.lastError().text()); }
    for (int feira : ids) {
        q.prepare("INSERT INTO participacoes VALUES (?, ?)"); q.addBindValue(vendedor); q.addBindValue(feira);
        if (!q.exec()) { db.rollback(); return falhar(q.lastError().text()); }
    }
    QVector<int> mantidos;
    for (const RegistroProdutoFeirante &p : produtos) {
        if (p.nome.trimmed().isEmpty() || !std::isfinite(p.preco) || p.preco < 0 || (p.ativo && p.preco <= 0)
            || !std::isfinite(p.estoque) || p.estoque < 0
            || (p.tipoVenda != "unidade" && p.tipoVenda != "kg" && p.tipoVenda != "100g")
            || (p.estoque > 0 && !quantidadeValida(p.estoque, p.tipoVenda))) {
            db.rollback(); return falhar("Confira o nome, preço, unidade e estoque de " + p.nome + ".");
        }
        if (ids.isEmpty() && p.ativo) { db.rollback(); return falhar("Selecione uma feira ou pause os produtos antes de salvar."); }
        double preco = p.preco * (p.tipoVenda == "100g" ? 10 : 1);
        int produto = p.id;
        if (produto > 0) {
            q.prepare("SELECT tipo_venda, versao, (SELECT COUNT(*) FROM reserva_itens i JOIN reservas r ON r.id = i.reserva_id WHERE i.produto_id = p.id AND r.status IN ('SOLICITADA','ACEITA')) FROM produtos p WHERE id = ? AND vendedor_id = ? AND removido = 0");
            q.addBindValue(produto); q.addBindValue(vendedor);
            if (!q.exec() || !q.next()) { db.rollback(); return falhar("Produto não encontrado no seu cadastro."); }
            if (q.value(1).toInt() != p.versao) { db.rollback(); return falhar("O estoque foi atualizado. Reabra o perfil antes de salvar."); }
            if (q.value(0).toString() != p.tipoVenda && q.value(2).toInt() > 0) {
                db.rollback(); return falhar("A unidade não pode mudar enquanto houver reservas desse produto.");
            }
            q.prepare("UPDATE produtos SET nome = ?, preco = ?, por_peso = ?, estoque = ?, tipo_venda = ?, ativo = ?, versao = versao + 1 WHERE id = ? AND vendedor_id = ? AND removido = 0 AND versao = ?");
            q.addBindValue(p.nome.trimmed()); q.addBindValue(preco); q.addBindValue(p.tipoVenda != "unidade");
            q.addBindValue(p.estoque); q.addBindValue(p.tipoVenda); q.addBindValue(p.ativo); q.addBindValue(produto); q.addBindValue(vendedor);
            q.addBindValue(p.versao);
            if (!q.exec() || q.numRowsAffected() != 1) { db.rollback(); return falhar("Produto não encontrado no seu cadastro."); }
        } else {
            q.prepare("INSERT INTO produtos(vendedor_id, nome, preco, por_peso, estoque, tipo_venda, ativo) VALUES (?, ?, ?, ?, ?, ?, ?)");
            q.addBindValue(vendedor); q.addBindValue(p.nome.trimmed()); q.addBindValue(preco); q.addBindValue(p.tipoVenda != "unidade"); q.addBindValue(p.estoque); q.addBindValue(p.tipoVenda);
            q.addBindValue(p.ativo);
            if (!q.exec()) { db.rollback(); return falhar(q.lastError().text()); }
            produto = q.lastInsertId().toInt();
        }
        mantidos.append(produto);
        q.prepare("DELETE FROM ofertas WHERE produto_id = ?"); q.addBindValue(produto);
        if (!q.exec()) { db.rollback(); return falhar(q.lastError().text()); }
        for (int feira : ids) {
            q.prepare("INSERT INTO ofertas VALUES (?, ?)"); q.addBindValue(feira); q.addBindValue(produto);
            if (!q.exec()) { db.rollback(); return falhar(q.lastError().text()); }
        }
    }
    QSqlQuery antigos(db);
    antigos.prepare("SELECT id FROM produtos WHERE vendedor_id = ? AND removido = 0"); antigos.addBindValue(vendedor);
    if (!antigos.exec()) { db.rollback(); return falhar(antigos.lastError().text()); }
    QVector<int> removidos;
    while (antigos.next()) if (!mantidos.contains(antigos.value(0).toInt())) removidos.append(antigos.value(0).toInt());
    for (int id : removidos) {
        q.prepare("UPDATE produtos SET ativo = 0, removido = 1, versao = versao + 1 WHERE id = ?"); q.addBindValue(id);
        if (!q.exec()) { db.rollback(); return falhar(q.lastError().text()); }
    }
    if (!db.commit()) { db.rollback(); return falhar(db.lastError().text()); }
    return true;
}

bool RepositorioCatalogo::removerProduto(const QString &telefone, int produtoId)
{
    if (!usuarioValido(telefone, "feirante")) return false;
    QSqlQuery q(QSqlDatabase::database(m_conexao));
    q.prepare("UPDATE produtos SET ativo = 0, removido = 1, versao = versao + 1 WHERE id = ? AND removido = 0 AND vendedor_id IN (SELECT id FROM vendedores WHERE user_phone = ?)");
    q.addBindValue(produtoId); q.addBindValue(telefone);
    if (!q.exec()) return falhar(q.lastError().text());
    return q.numRowsAffected() == 1;
}

QVector<int> RepositorioCatalogo::salvarReservas(const QString &telefone,
    const std::vector<ItemSacolaComprador> &itens, const QVector<AgendamentoReserva> &agendamentos,
    const DadosCatalogo &dadosEsperados)
{
    m_ultimoErro.clear();
    if (!usuarioValido(telefone, "comprador")) return {};
    if (itens.empty()) { falhar("Seu carrinho está vazio."); return {}; }
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    QSqlQuery q(db);
    if (!q.exec("BEGIN IMMEDIATE")) { falhar(q.lastError().text()); return {}; }
    auto erro = [&](const QString &texto) {
        db.rollback();
        falhar(texto);
        return QVector<int>();
    };
    q.prepare("SELECT name FROM users WHERE phone = ?"); q.addBindValue(telefone);
    if (!q.exec() || !q.next()) return erro("Comprador não encontrado.");
    QString comprador = q.value(0).toString();
    QVector<int> ids;
    QMap<QString, int> grupos;
    for (const ItemSacolaComprador &item : itens) {
        const Produto *esperado = nullptr;
        for (const Produto &produto : dadosEsperados.produtos)
            if (produto.getId() == item.produtoId) { esperado = &produto; break; }
        q.prepare("SELECT p.nome, p.preco, p.tipo_venda FROM produtos p JOIN vendedores v ON v.id = p.vendedor_id JOIN users u ON u.phone = v.user_phone AND u.role = 'feirante' JOIN ofertas o ON o.produto_id = p.id WHERE p.id = ? AND p.vendedor_id = ? AND o.feira_id = ? AND p.ativo = 1");
        q.addBindValue(item.produtoId); q.addBindValue(item.vendedorId); q.addBindValue(item.feiraId);
        if (!q.exec()) return erro(q.lastError().text());
        if (!q.next()) return erro("Um produto não está mais disponível nessa feira. Revise o carrinho.");
        QString nome = q.value(0).toString();
        double preco = q.value(1).toDouble();
        QString tipo = q.value(2).toString();
        double passo = tipo == "100g" ? 0.1 : tipo == "kg" ? 0.5 : 1.0;
        if ((tipo != "unidade" && tipo != "kg" && tipo != "100g")
            || !quantidadeValida(item.quantidade, tipo) || !std::isfinite(item.quantidade * preco))
            return erro("A quantidade de " + nome + " é inválida.");
        if (!esperado || !std::isfinite(preco) || preco <= 0
            || std::abs(esperado->getPreco() - preco) > 0.000001
            || std::abs(esperado->getPasso() - passo) > 0.000001)
            return erro("O preço ou a unidade de " + nome + " mudou. Confira o carrinho antes de tentar novamente.");
        q.prepare("SELECT nome, bairro, local, horario, dia_semana, hora_inicio, hora_fim FROM feiras WHERE id = ?");
        q.addBindValue(item.feiraId);
        if (!q.exec() || !q.next()) return erro("Feira não encontrada.");
        FeiraComprador feira{item.feiraId, q.value(0).toString().toStdString(),
            q.value(1).toString().toStdString(), q.value(2).toString().toStdString(),
            q.value(3).toString().toStdString(), q.value(4).toInt(),
            q.value(5).toString().toStdString(), q.value(6).toString().toStdString()};
        AgendamentoReserva retirada;
        for (const AgendamentoReserva &a : agendamentos)
            if (a.feiraId == item.feiraId) { retirada = a; break; }
        if (!AgendaFeira::validar(feira, retirada))
            return erro("Escolha uma data e um horário disponíveis para " + QString::fromStdString(feira.nome) + ".");
        q.prepare("UPDATE produtos SET estoque = MAX(0, ROUND(estoque - ?, 6)), versao = versao + 1 WHERE id = ? AND ativo = 1 AND estoque + 0.000001 >= ?");
        q.addBindValue(item.quantidade); q.addBindValue(item.produtoId); q.addBindValue(item.quantidade);
        if (!q.exec()) return erro(q.lastError().text());
        if (q.numRowsAffected() != 1) return erro("O estoque de " + nome + " mudou. Revise a quantidade no carrinho.");
        QString chave = QString::number(item.vendedorId) + ":" + QString::number(item.feiraId);
        int reserva = grupos.value(chave, 0);
        if (!reserva) {
            q.prepare("INSERT INTO reservas(comprador_phone, comprador_nome, criada_em, total, status, vendedor_id, feira_id, data_retirada, hora_inicio, hora_fim) VALUES (?, ?, ?, 0, 'SOLICITADA', ?, ?, ?, ?, ?)");
            q.addBindValue(telefone); q.addBindValue(comprador);
            q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
            q.addBindValue(item.vendedorId); q.addBindValue(item.feiraId);
            q.addBindValue(retirada.data); q.addBindValue(retirada.inicio); q.addBindValue(retirada.fim);
            if (!q.exec()) return erro(q.lastError().text());
            reserva = q.lastInsertId().toInt();
            grupos.insert(chave, reserva); ids.append(reserva);
        }
        q.prepare("INSERT INTO reserva_itens(reserva_id, feira_id, vendedor_id, produto_id, produto_nome, unidade, quantidade, preco) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        q.addBindValue(reserva); q.addBindValue(item.feiraId); q.addBindValue(item.vendedorId);
        q.addBindValue(item.produtoId); q.addBindValue(nome); q.addBindValue(tipo == "unidade" ? "unidade" : "kg");
        q.addBindValue(item.quantidade); q.addBindValue(preco);
        if (!q.exec()) return erro(q.lastError().text());
        q.prepare("UPDATE reservas SET total = ROUND(total + ?, 2) WHERE id = ?");
        q.addBindValue(item.quantidade * preco); q.addBindValue(reserva);
        if (!q.exec()) return erro(q.lastError().text());
    }
    if (!db.commit()) return erro(db.lastError().text());
    return ids;
}

QVector<RegistroReserva> RepositorioCatalogo::reservasDoUsuario(const QString &telefone, bool vendedor)
{
    m_ultimoErro.clear();
    QVector<RegistroReserva> lista;
    if (!usuarioValido(telefone, vendedor ? "feirante" : "comprador")) return lista;
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    QSqlQuery q(db);
    QString filtro = vendedor ? "v.user_phone = ?" : "r.comprador_phone = ?";
    q.prepare("SELECT r.id, r.feira_id, r.vendedor_id, r.comprador_nome, v.banca, f.nome, f.local, r.data_retirada, r.hora_inicio, r.hora_fim, r.status, r.criada_em, r.total FROM reservas r LEFT JOIN vendedores v ON v.id = r.vendedor_id LEFT JOIN feiras f ON f.id = r.feira_id WHERE " + filtro + " ORDER BY r.id DESC");
    q.addBindValue(telefone);
    if (!q.exec()) { falhar(q.lastError().text()); return lista; }
    while (q.next()) {
        RegistroReserva r;
        r.id = q.value(0).toInt(); r.feiraId = q.value(1).toInt(); r.vendedorId = q.value(2).toInt();
        r.comprador = q.value(3).toString(); r.banca = q.value(4).toString();
        r.feira = q.value(5).toString(); r.local = q.value(6).toString(); r.data = q.value(7).toString();
        r.inicio = q.value(8).toString(); r.fim = q.value(9).toString(); r.status = q.value(10).toString();
        r.criadaEm = q.value(11).toString(); r.total = q.value(12).toDouble();
        QSqlQuery itens(db);
        itens.prepare("SELECT produto_nome, unidade, quantidade, preco FROM reserva_itens WHERE reserva_id = ? ORDER BY id");
        itens.addBindValue(r.id);
        if (!itens.exec()) { falhar(itens.lastError().text()); return {}; }
        while (itens.next()) r.itens.append({itens.value(0).toString(), itens.value(1).toString(), itens.value(2).toDouble(), itens.value(3).toDouble()});
        lista.append(r);
    }
    return lista;
}

bool RepositorioCatalogo::alterarReserva(const QString &telefone, bool vendedor, int id, const QString &status)
{
    m_ultimoErro.clear();
    if (!usuarioValido(telefone, vendedor ? "feirante" : "comprador")) return false;
    QSqlDatabase db = QSqlDatabase::database(m_conexao);
    QSqlQuery q(db);
    if (!q.exec("BEGIN IMMEDIATE")) return falhar(q.lastError().text());
    auto erro = [&](const QString &texto) { db.rollback(); return falhar(texto); };
    QString filtro = vendedor ? "v.user_phone = ?" : "r.comprador_phone = ?";
    q.prepare("SELECT r.status, r.data_retirada, r.hora_inicio, r.hora_fim FROM reservas r LEFT JOIN vendedores v ON v.id = r.vendedor_id WHERE r.id = ? AND " + filtro);
    q.addBindValue(id); q.addBindValue(telefone);
    if (!q.exec()) return erro(q.lastError().text());
    if (!q.next()) return erro("Reserva não encontrada no seu perfil.");
    QString anterior = q.value(0).toString();
    QDate data = QDate::fromString(q.value(1).toString(), Qt::ISODate);
    QTime inicio = QTime::fromString(q.value(2).toString(), "HH:mm");
    QTime fim = QTime::fromString(q.value(3).toString(), "HH:mm");
    QDateTime agora = AgendaFeira::agora();
    bool pendente = anterior == "SOLICITADA";
    bool aceita = anterior == "ACEITA";
    bool permitido = vendedor ? ((pendente && (status == "ACEITA" || status == "RECUSADA"))
        || (aceita && status == "RETIRADA")) : ((pendente || aceita) && status == "CANCELADA");
    if (!permitido) return erro("Essa reserva já mudou de situação. Atualize a lista.");
    if (vendedor && status == "ACEITA" && data.isValid()
        && (data < agora.date() || (data == agora.date() && fim <= agora.time())))
        return erro("O horário de retirada terminou. Recuse o pedido para devolver o estoque.");
    if (vendedor && status == "RETIRADA" && data.isValid()
        && (data > agora.date() || (data == agora.date() && inicio > agora.time())))
        return erro("A retirada só pode ser registrada a partir do horário agendado.");
    q.prepare("UPDATE reservas SET status = ? WHERE id = ? AND status = ?");
    q.addBindValue(status); q.addBindValue(id); q.addBindValue(anterior);
    if (!q.exec()) return erro(q.lastError().text());
    if (q.numRowsAffected() != 1) return erro("Essa reserva já foi atualizada.");
    if (status == "RECUSADA" || status == "CANCELADA") {
        QSqlQuery itens(db);
        itens.prepare("SELECT produto_id, quantidade FROM reserva_itens WHERE reserva_id = ?"); itens.addBindValue(id);
        if (!itens.exec()) return erro(itens.lastError().text());
        while (itens.next()) {
            q.prepare("UPDATE produtos SET estoque = ROUND(estoque + ?, 6), versao = versao + 1 WHERE id = ?");
            q.addBindValue(itens.value(1)); q.addBindValue(itens.value(0));
            if (!q.exec()) return erro(q.lastError().text());
        }
    }
    if (!db.commit()) return erro(db.lastError().text());
    return true;
}

QVariantMap RegistroReserva::comoMapa() const
{
    QVariantList produtos;
    for (const ItemReserva &item : itens)
        produtos.append(QVariantMap{{"nome", item.nome}, {"unidade", item.unidade},
            {"quantidade", item.quantidade}, {"preco", item.preco}, {"subtotal", std::round(item.quantidade * item.preco * 100) / 100}});
    QString situacao = status;
    if (status == "SOLICITADA") situacao = "Aguardando vendedor";
    if (status == "ACEITA") situacao = "Confirmada pelo vendedor";
    if (status == "RECUSADA") situacao = "Recusada";
    if (status == "CANCELADA") situacao = "Cancelada";
    if (status == "RETIRADA") situacao = "Retirada";
    QString retirada = "Horário a combinar (reserva antiga)";
    if (!data.isEmpty()) retirada = QDate::fromString(data, Qt::ISODate).toString("dd/MM/yyyy") + " · " + inicio + " às " + fim;
    QDateTime agora = AgendaFeira::agora();
    QDate dia = QDate::fromString(data, Qt::ISODate);
    bool expirou = dia.isValid() && (dia < agora.date() || (dia == agora.date() && QTime::fromString(fim, "HH:mm") <= agora.time()));
    bool podeRetirar = !dia.isValid() || dia < agora.date()
        || (dia == agora.date() && QTime::fromString(inicio, "HH:mm") <= agora.time());
    return {{"id", id}, {"comprador", comprador}, {"banca", banca}, {"feira", feira},
        {"local", local}, {"retirada", retirada}, {"status", status}, {"situacao", situacao},
        {"total", total}, {"itens", produtos}, {"podeAceitar", status == "SOLICITADA" && !expirou},
        {"podeRecusar", status == "SOLICITADA"}, {"podeRetirar", status == "ACEITA" && podeRetirar},
        {"podeCancelar", status == "SOLICITADA" || status == "ACEITA"}};
}
