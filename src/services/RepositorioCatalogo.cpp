#include "services/RepositorioCatalogo.hpp"

#include <QDateTime>
#include <QTime>
#include <QStringList>
#include <cmath>
#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace {
const char *kConexao = "antro_catalogo_connection";

QSqlDatabase banco() { return QSqlDatabase::database(kConexao); }
}

RepositorioCatalogo::~RepositorioCatalogo()
{
    {
        QSqlDatabase conexao = QSqlDatabase::database(kConexao, false);
        if (conexao.isValid() && conexao.isOpen())
            conexao.close();
    }
    QSqlDatabase::removeDatabase(kConexao);
}

bool RepositorioCatalogo::abrir()
{
    const QString pasta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(pasta);

    QSqlDatabase conexao = QSqlDatabase::addDatabase("QSQLITE", kConexao);
    conexao.setDatabaseName(pasta + "/antro.db");
    if (!conexao.open()) {
        m_ultimoErro = conexao.lastError().text();
        return false;
    }
    QSqlQuery pragma(conexao);
    pragma.exec("PRAGMA foreign_keys = ON");
    pragma.exec("PRAGMA busy_timeout = 3000");   // a conexão de usuários usa o mesmo arquivo

    if (!criarTabelas()) return false;

    QSqlQuery q(conexao);
    if (!q.exec("SELECT COUNT(*) FROM feiras") || !q.next()) {
        m_ultimoErro = q.lastError().text();
        return false;
    }
    if (q.value(0).toInt() == 0 && !inserirFeirasIniciais()) return false;
    return migrar();
}

bool RepositorioCatalogo::criarTabelas()
{
    const char *comandos[] = {
        "CREATE TABLE IF NOT EXISTS feiras ("
        "  id INTEGER PRIMARY KEY, nome TEXT NOT NULL, bairro TEXT NOT NULL,"
        "  local TEXT NOT NULL, horario TEXT NOT NULL)",

        // user_phone liga o vendedor a um feirante cadastrado (NULL = vendedor de exemplo)
        "CREATE TABLE IF NOT EXISTS vendedores ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT, user_phone TEXT UNIQUE,"
        "  nome TEXT NOT NULL, banca TEXT NOT NULL, descricao TEXT NOT NULL DEFAULT '')",

        "CREATE TABLE IF NOT EXISTS produtos ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  vendedor_id INTEGER NOT NULL REFERENCES vendedores(id),"
        "  nome TEXT NOT NULL, preco REAL NOT NULL, por_peso INTEGER NOT NULL, estoque REAL NOT NULL)",

        "CREATE TABLE IF NOT EXISTS ofertas ("
        "  feira_id INTEGER NOT NULL REFERENCES feiras(id),"
        "  produto_id INTEGER NOT NULL REFERENCES produtos(id) ON DELETE CASCADE,"
        "  PRIMARY KEY (feira_id, produto_id))",

        "CREATE TABLE IF NOT EXISTS reservas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT, comprador_phone TEXT NOT NULL,"
        "  comprador_nome TEXT NOT NULL, criada_em TEXT NOT NULL, total REAL NOT NULL,"
        "  status TEXT NOT NULL DEFAULT 'SOLICITADA')",

        "CREATE TABLE IF NOT EXISTS reserva_itens ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  reserva_id INTEGER NOT NULL REFERENCES reservas(id) ON DELETE CASCADE,"
        "  feira_id INTEGER NOT NULL, vendedor_id INTEGER NOT NULL, produto_id INTEGER NOT NULL,"
        "  produto_nome TEXT NOT NULL, unidade TEXT NOT NULL,"
        "  quantidade REAL NOT NULL, preco REAL NOT NULL)",
    };
    QSqlQuery q(banco());
    for (const char *sql : comandos) {
        if (!q.exec(QString::fromUtf8(sql))) {
            m_ultimoErro = q.lastError().text();
            return false;
        }
    }
    return true;
}

bool RepositorioCatalogo::inserirFeirasIniciais()
{
    QSqlDatabase db = banco();
    if (!db.transaction()) { m_ultimoErro = db.lastError().text(); return false; }
    QSqlQuery q(db);
    const char *comandos[] = {
        "INSERT INTO feiras VALUES (1, 'Feira Agroecológica da Várzea', 'Várzea', 'Praça da Várzea', 'Sábados, 7h às 10h')",
        "INSERT INTO feiras VALUES (2, 'Espaço Agroecológico das Graças', 'Graças', 'Rua Andrade de Souza', 'Sábados, 4h às 11h')",
        "INSERT INTO feiras VALUES (3, 'Feira de Produtos Orgânicos de Casa Forte', 'Casa Forte', 'Praça da Vitória Régia', 'Sábados, 5h às 11h')",
        "INSERT INTO feiras VALUES (4, 'Espaço Agroecológico do Sítio da Trindade', 'Casa Amarela', 'Sítio da Trindade', 'Sábados, 5h às 11h')",
    };
    for (const char *sql : comandos) {
        if (!q.exec(QString::fromUtf8(sql))) {
            m_ultimoErro = q.lastError().text();
            db.rollback();
            return false;
        }
    }
    if (!db.commit()) { m_ultimoErro = db.lastError().text(); return false; }
    return true;
}

DadosCatalogo RepositorioCatalogo::carregar()
{
    DadosCatalogo d;
    QSqlQuery q(banco());

    if (q.exec("SELECT id, nome, bairro, local, horario FROM feiras ORDER BY id")) {
        while (q.next())
            d.feiras.push_back({q.value(0).toInt(), q.value(1).toString().toStdString(),
                                q.value(2).toString().toStdString(), q.value(3).toString().toStdString(),
                                q.value(4).toString().toStdString()});
    } else m_ultimoErro = q.lastError().text();

    if (q.exec("SELECT id, nome, banca, descricao FROM vendedores WHERE user_phone IS NOT NULL ORDER BY id")) {
        while (q.next())
            d.vendedores.push_back({q.value(0).toInt(), q.value(1).toString().toStdString(),
                                    q.value(2).toString().toStdString(), q.value(3).toString().toStdString()});
    } else m_ultimoErro = q.lastError().text();

    if (q.exec("SELECT p.id, p.nome, p.preco, p.por_peso, p.estoque FROM produtos p JOIN vendedores v ON v.id = p.vendedor_id WHERE v.user_phone IS NOT NULL ORDER BY p.id")) {
        while (q.next())
            d.produtos.push_back(Produto(q.value(0).toInt(), q.value(1).toString().toStdString(),
                                         static_cast<float>(q.value(2).toDouble()), q.value(3).toInt() != 0,
                                         static_cast<float>(q.value(4).toDouble())));
    } else m_ultimoErro = q.lastError().text();

    if (q.exec("SELECT o.feira_id, p.vendedor_id, o.produto_id FROM ofertas o "
               "JOIN produtos p ON p.id = o.produto_id JOIN vendedores v ON v.id = p.vendedor_id WHERE v.user_phone IS NOT NULL ORDER BY o.feira_id, o.produto_id")) {
        while (q.next())
            d.ofertas.push_back({q.value(0).toInt(), q.value(1).toInt(), q.value(2).toInt()});
    } else m_ultimoErro = q.lastError().text();

    return d;
}

bool RepositorioCatalogo::inserirProduto(const QString &telefone, const QString &nomeFeirante,
                                         const QString &banca, const QString &nome, double preco,
                                         bool porPeso, double estoque, const QVector<int> &feiraIds)
{
    QSqlDatabase db = banco();
    if (!db.transaction()) { m_ultimoErro = db.lastError().text(); return false; }

    auto falhar = [&](const QSqlQuery &q) {
        m_ultimoErro = q.lastError().text();
        db.rollback();
        return false;
    };

    QSqlQuery q(db);
    // O vendedor é criado na primeira vez que o feirante cadastra um produto.
    q.prepare("INSERT OR IGNORE INTO vendedores (user_phone, nome, banca) VALUES (?, ?, ?)");
    q.addBindValue(telefone);
    q.addBindValue(nomeFeirante);
    q.addBindValue(banca);
    if (!q.exec()) return falhar(q);

    q.prepare("SELECT id FROM vendedores WHERE user_phone = ?");
    q.addBindValue(telefone);
    if (!q.exec() || !q.next()) return falhar(q);
    const int vendedorId = q.value(0).toInt();

    q.prepare("INSERT INTO produtos (vendedor_id, nome, preco, por_peso, estoque) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(vendedorId);
    q.addBindValue(nome);
    q.addBindValue(preco);
    q.addBindValue(porPeso ? 1 : 0);
    q.addBindValue(estoque);
    if (!q.exec()) return falhar(q);
    const QVariant produtoId = q.lastInsertId();

    for (int feiraId : feiraIds) {
        q.prepare("INSERT INTO ofertas (feira_id, produto_id) VALUES (?, ?)");
        q.addBindValue(feiraId);
        q.addBindValue(produtoId);
        if (!q.exec()) return falhar(q);
    }
    if (!db.commit()) { m_ultimoErro = db.lastError().text(); return false; }
    return true;
}

QVector<RegistroProdutoFeirante> RepositorioCatalogo::produtosDoFeirante(const QString &telefone)
{
    QVector<RegistroProdutoFeirante> lista;
    QSqlQuery q(banco());
    q.prepare("SELECT p.id, p.nome, p.preco, p.por_peso, p.estoque FROM produtos p "
              "JOIN vendedores v ON v.id = p.vendedor_id WHERE v.user_phone = ? ORDER BY p.id");
    q.addBindValue(telefone);
    if (!q.exec()) { m_ultimoErro = q.lastError().text(); return lista; }
    while (q.next())
        lista.push_back({q.value(0).toInt(), q.value(1).toString(), q.value(2).toDouble(),
                         q.value(3).toInt() != 0, q.value(4).toDouble(), {}});

    for (RegistroProdutoFeirante &p : lista) {
        QSqlQuery f(banco());
        f.prepare("SELECT feira_id FROM ofertas WHERE produto_id = ? ORDER BY feira_id");
        f.addBindValue(p.id);
        if (f.exec())
            while (f.next()) p.feiraIds.push_back(f.value(0).toInt());
    }
    return lista;
}

bool RepositorioCatalogo::removerProduto(const QString &telefone, int produtoId)
{
    QSqlDatabase db = banco();
    if (!db.transaction()) { m_ultimoErro = db.lastError().text(); return false; }
    auto falhar = [&](const QString &erro) { m_ultimoErro = erro; db.rollback(); return false; };
    QSqlQuery pendente(db);
    pendente.prepare("SELECT 1 FROM reserva_itens WHERE produto_id = ? AND status IN ('SOLICITADA', 'ACEITA')");
    pendente.addBindValue(produtoId);
    if (!pendente.exec()) return falhar(pendente.lastError().text());
    if (pendente.next()) return falhar("Este produto tem reservas em andamento. Finalize ou cancele as reservas antes de removê-lo.");
    pendente.finish();
    // Só remove produtos do próprio feirante.
    QSqlQuery q(banco());
    q.prepare("DELETE FROM produtos WHERE id = ? AND vendedor_id IN "
              "(SELECT id FROM vendedores WHERE user_phone = ?)");
    q.addBindValue(produtoId);
    q.addBindValue(telefone);
    if (!q.exec()) return falhar(q.lastError().text());
    if (q.numRowsAffected() != 1) return falhar("Produto não encontrado para este vendedor.");
    if (!db.commit()) return falhar(db.lastError().text());
    return true;
}

int RepositorioCatalogo::salvarReserva(const QString &telefoneComprador, const QString &nomeComprador,
                                       const std::vector<ItemSacolaComprador> &itens, double total, const QString &data, const QString &hora)
{
    const QDate dia = QDate::fromString(data, "yyyy-MM-dd");
    const QTime horario = QTime::fromString(hora, "HH:mm");
    if (telefoneComprador.trimmed().isEmpty() || itens.empty() || !dia.isValid() || !horario.isValid()
        || QDateTime(dia, horario) <= QDateTime::currentDateTime()) {
        m_ultimoErro = "Informe data e horário futuros para a retirada (AAAA-MM-DD e HH:MM)."; return 0;
    }
    QSqlDatabase db = banco();
    if (!db.transaction()) { m_ultimoErro = db.lastError().text(); return 0; }

    auto falhar = [&](const QString &erro) {
        m_ultimoErro = erro;
        db.rollback();
        return 0;
    };

    QSqlQuery q(db);
    q.prepare("INSERT INTO reservas (comprador_phone, comprador_nome, criada_em, total, retirada_data, retirada_hora) VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(telefoneComprador);
    q.addBindValue(nomeComprador);
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(total);
    q.addBindValue(data);
    q.addBindValue(hora);
    if (!q.exec()) return falhar(q.lastError().text());
    const int reservaId = q.lastInsertId().toInt();

    double totalAtual = 0;
    for (const ItemSacolaComprador &item : itens) {
        if (!std::isfinite(item.quantidade) || item.quantidade <= 0) return falhar("Quantidade inválida.");
        QSqlQuery p(db);
        p.prepare("SELECT p.nome, p.preco, p.por_peso FROM produtos p JOIN ofertas o ON o.produto_id = p.id "
                  "JOIN vendedores v ON v.id = p.vendedor_id WHERE p.id = ? AND p.vendedor_id = ? AND o.feira_id = ? AND v.user_phone IS NOT NULL");
        p.addBindValue(item.produtoId);
        p.addBindValue(item.vendedorId);
        p.addBindValue(item.feiraId);
        if (!p.exec() || !p.next()) return falhar("Produto não encontrado: " + QString::number(item.produtoId));

        const double passos = p.value(2).toBool() ? item.quantidade * 2 : item.quantidade;
        if (std::floor(passos) != passos) return falhar("Quantidade inválida para a unidade do produto.");
        totalAtual += p.value(1).toDouble() * item.quantidade;
        // O WHERE estoque >= ? impede estoque negativo se outro processo mexeu no banco.
        QSqlQuery baixa(db);
        baixa.prepare("UPDATE produtos SET estoque = estoque - ? WHERE id = ? AND estoque >= ?");
        baixa.addBindValue(item.quantidade);
        baixa.addBindValue(item.produtoId);
        baixa.addBindValue(item.quantidade);
        if (!baixa.exec() || baixa.numRowsAffected() != 1)
            return falhar("Estoque insuficiente para " + p.value(0).toString());

        QSqlQuery ins(db);
        ins.prepare("INSERT INTO reserva_itens (reserva_id, feira_id, vendedor_id, produto_id, produto_nome,"
                    " unidade, quantidade, preco) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        ins.addBindValue(reservaId);
        ins.addBindValue(item.feiraId);
        ins.addBindValue(item.vendedorId);
        ins.addBindValue(item.produtoId);
        ins.addBindValue(p.value(0));
        ins.addBindValue(p.value(2).toInt() != 0 ? "kg" : "unidade");
        ins.addBindValue(item.quantidade);
        ins.addBindValue(p.value(1));
        if (!ins.exec()) return falhar(ins.lastError().text());
    }
    q.prepare("UPDATE reservas SET total = ? WHERE id = ?");
    q.addBindValue(totalAtual); q.addBindValue(reservaId);
    if (!q.exec()) return falhar(q.lastError().text());
    if (!db.commit()) return falhar(db.lastError().text());
    return reservaId;
}

bool RepositorioCatalogo::migrar()
{
    QSqlDatabase db = banco();
    if (!db.transaction()) { m_ultimoErro = db.lastError().text(); return false; }
    auto falhar = [&](const QString &erro) { m_ultimoErro = erro; db.rollback(); return false; };
    QSqlQuery q(db);
    auto coluna = [&](const QString &tabela, const QString &nome) {
        QSqlQuery c(db); c.exec("PRAGMA table_info(" + tabela + ")");
        while (c.next()) if (c.value(1).toString() == nome) return true;
        return false;
    };
    for (const QString &nome : {QString("retirada_data"), QString("retirada_hora")}) {
        if (!coluna("reservas", nome) && !q.exec("ALTER TABLE reservas ADD COLUMN " + nome + " TEXT"))
            return falhar(q.lastError().text());
    }
    if (!coluna("reserva_itens", "status")) {
        if (!q.exec("ALTER TABLE reserva_itens ADD COLUMN status TEXT NOT NULL DEFAULT 'SOLICITADA'"))
            return falhar(q.lastError().text());
        if (!q.exec("UPDATE reserva_itens SET status = (SELECT status FROM reservas WHERE id = reserva_id)"))
            return falhar(q.lastError().text());
    }
    const QStringList tabelas = db.tables();
    if (tabelas.contains("seller_products") || tabelas.contains("seller_markets")) {
        QString sql = "SELECT DISTINCT seller_phone FROM ";
        if (tabelas.contains("seller_products") && tabelas.contains("seller_markets"))
            sql += "(SELECT seller_phone FROM seller_products UNION SELECT seller_phone FROM seller_markets)";
        else sql += tabelas.contains("seller_products") ? "seller_products" : "seller_markets";
        QSqlQuery vendedores(db);
        if (!vendedores.exec(sql)) return falhar(vendedores.lastError().text());
        while (vendedores.next()) {
            const QString telefone = vendedores.value(0).toString();
            q.prepare("INSERT OR IGNORE INTO vendedores (user_phone, nome, banca) SELECT phone, name, COALESCE(market_name, '') FROM users WHERE phone = ?");
            q.addBindValue(telefone); if (!q.exec()) return falhar(q.lastError().text());
            q.prepare("SELECT id FROM vendedores WHERE user_phone = ?"); q.addBindValue(telefone);
            if (!q.exec() || !q.next()) return falhar("Vendedor antigo sem usuário correspondente.");
            const int vendedorId = q.value(0).toInt();
            QVector<int> feiras;
            if (tabelas.contains("seller_markets")) {
                QSqlQuery mercados(db);
                mercados.prepare("SELECT market_name FROM seller_markets WHERE seller_phone = ?"); mercados.addBindValue(telefone);
                if (!mercados.exec()) return falhar(mercados.lastError().text());
                while (mercados.next()) {
                    const QString nome = mercados.value(0).toString().trimmed();
                    q.prepare("SELECT id FROM feiras WHERE nome = ? COLLATE NOCASE OR (bairro != '' AND instr(lower(?), lower(bairro)) > 0) ORDER BY id LIMIT 1");
                    q.addBindValue(nome); q.addBindValue(nome);
                    if (!q.exec()) return falhar(q.lastError().text());
                    int id;
                    if (q.next()) id = q.value(0).toInt();
                    else {
                        q.prepare("INSERT INTO feiras (nome, bairro, local, horario) VALUES (?, '', '', '')"); q.addBindValue(nome);
                        if (!q.exec()) return falhar(q.lastError().text());
                        id = q.lastInsertId().toInt();
                    }
                    if (!feiras.contains(id)) feiras.append(id);
                }
            }
            if (tabelas.contains("seller_products")) {
                QSqlQuery produtos(db);
                produtos.prepare("SELECT name, sale_type, price FROM seller_products WHERE seller_phone = ?"); produtos.addBindValue(telefone);
                if (!produtos.exec()) return falhar(produtos.lastError().text());
                while (produtos.next()) {
                    q.prepare("SELECT id FROM produtos WHERE vendedor_id = ? AND nome = ? COLLATE NOCASE");
                    q.addBindValue(vendedorId); q.addBindValue(produtos.value(0));
                    if (!q.exec()) return falhar(q.lastError().text());
                    int id;
                    if (q.next()) id = q.value(0).toInt();
                    else {
                        const bool peso = produtos.value(1).toString() == "100g";
                        q.prepare("INSERT INTO produtos (vendedor_id, nome, preco, por_peso, estoque) VALUES (?, ?, ?, ?, 0)");
                        q.addBindValue(vendedorId); q.addBindValue(produtos.value(0));
                        q.addBindValue(produtos.value(2).toDouble() * (peso ? 10 : 1)); q.addBindValue(peso ? 1 : 0);
                        if (!q.exec()) return falhar(q.lastError().text());
                        id = q.lastInsertId().toInt();
                    }
                    for (int feiraId : feiras) {
                        q.prepare("INSERT OR IGNORE INTO ofertas (feira_id, produto_id) VALUES (?, ?)");
                        q.addBindValue(feiraId); q.addBindValue(id); if (!q.exec()) return falhar(q.lastError().text());
                    }
                }
            }
        }
        vendedores.finish();
        for (const QString &t : {QString("seller_products"), QString("seller_markets")})
            if (tabelas.contains(t) && !q.exec("DROP TABLE " + t)) return falhar(q.lastError().text());
    }
    if (!db.commit()) return falhar(db.lastError().text());
    return true;
}

bool RepositorioCatalogo::editarProduto(const QString &telefone, int id, double preco, double estoque, double estoqueAnterior, const QVector<int> &feiras)
{
    QSqlDatabase db = banco();
    if (!db.transaction()) { m_ultimoErro = db.lastError().text(); return false; }
    auto falhar = [&](const QString &erro) { m_ultimoErro = erro; db.rollback(); return false; };
    QSqlQuery q(db);
    q.prepare("UPDATE produtos SET preco = ?, estoque = ? WHERE id = ? AND vendedor_id IN (SELECT id FROM vendedores WHERE user_phone = ?) AND estoque = ?");
    q.addBindValue(preco); q.addBindValue(estoque); q.addBindValue(id); q.addBindValue(telefone); q.addBindValue(estoqueAnterior);
    if (!q.exec()) return falhar(q.lastError().text());
    if (q.numRowsAffected() != 1) return falhar("O estoque mudou ou o produto foi removido. Reabra o produto antes de editar.");
    q.prepare("DELETE FROM ofertas WHERE produto_id = ?"); q.addBindValue(id);
    if (!q.exec()) return falhar(q.lastError().text());
    for (int feira : feiras) {
        q.prepare("INSERT INTO ofertas (feira_id, produto_id) VALUES (?, ?)"); q.addBindValue(feira); q.addBindValue(id);
        if (!q.exec()) return falhar(q.lastError().text());
    }
    if (!db.commit()) return falhar(db.lastError().text());
    return true;
}

QVariantList RepositorioCatalogo::reservas(const QString &telefone, bool vendedor)
{
    m_ultimoErro.clear();
    QVariantList lista;
    QSqlQuery q(banco());
    q.prepare("SELECT DISTINCT r.id, r.comprador_nome, r.criada_em, r.retirada_data, r.retirada_hora, r.status "
              "FROM reservas r JOIN reserva_itens i ON i.reserva_id = r.id JOIN vendedores v ON v.id = i.vendedor_id WHERE "
              + QString(vendedor ? "v.user_phone = ?" : "r.comprador_phone = ?") + " ORDER BY r.id DESC");
    q.addBindValue(telefone);
    if (!q.exec()) { m_ultimoErro = q.lastError().text(); return lista; }
    while (q.next()) {
        QVariantList itens; double total = 0;
        QSqlQuery i(banco());
        i.prepare("SELECT i.produto_nome, i.quantidade, i.unidade, i.preco, i.status, v.nome, f.nome "
                  "FROM reserva_itens i JOIN vendedores v ON v.id = i.vendedor_id JOIN feiras f ON f.id = i.feira_id "
                  "WHERE i.reserva_id = ?" + QString(vendedor ? " AND v.user_phone = ?" : "") + " ORDER BY i.id");
        i.addBindValue(q.value(0)); if (vendedor) i.addBindValue(telefone);
        if (!i.exec()) { m_ultimoErro = i.lastError().text(); return {}; }
        QString status;
        while (i.next()) {
            const QString atual = i.value(4).toString();
            if (status.isEmpty()) status = atual; else if (status != atual) status = "PARCIAL";
            const double subtotal = i.value(1).toDouble() * i.value(3).toDouble(); total += subtotal;
            itens.append(QVariantMap{{"nome", i.value(0)}, {"quantidade", i.value(1)}, {"unidade", i.value(2)},
                         {"subtotal", subtotal}, {"status", atual}, {"vendedor", i.value(5)}, {"feira", i.value(6)}});
        }
        lista.append(QVariantMap{{"id", q.value(0)}, {"comprador", q.value(1)}, {"criadaEm", q.value(2)},
                     {"data", q.value(3)}, {"hora", q.value(4)}, {"status", status}, {"itens", itens}, {"total", total}});
    }
    return lista;
}

bool RepositorioCatalogo::alterarReserva(const QString &telefone, bool vendedor, int id, const QString &status)
{
    if ((vendedor && status != "ACEITA" && status != "RECUSADA" && status != "RETIRADA")
        || (!vendedor && status != "CANCELADA")) { m_ultimoErro = "Ação inválida."; return false; }
    QSqlDatabase db = banco();
    if (!db.transaction()) { m_ultimoErro = db.lastError().text(); return false; }
    auto falhar = [&](const QString &erro) { m_ultimoErro = erro; db.rollback(); return false; };
    QSqlQuery q(db);
    q.prepare("SELECT i.id, i.produto_id, i.quantidade, i.status FROM reserva_itens i "
              "JOIN reservas r ON r.id = i.reserva_id JOIN vendedores v ON v.id = i.vendedor_id "
              "WHERE r.id = ? AND " + QString(vendedor ? "v.user_phone = ?" : "r.comprador_phone = ?"));
    q.addBindValue(id); q.addBindValue(telefone);
    if (!q.exec()) return falhar(q.lastError().text());
    struct Item { int id; int produto; double quantidade; QString status; };
    QVector<Item> itens;
    while (q.next()) itens.append({q.value(0).toInt(), q.value(1).toInt(), q.value(2).toDouble(), q.value(3).toString()});
    q.finish();
    int alterados = 0;
    for (const Item &item : itens) {
        const bool pode = status == "RETIRADA" ? item.status == "ACEITA"
                        : status == "CANCELADA" ? (item.status == "SOLICITADA" || item.status == "ACEITA")
                        : item.status == "SOLICITADA";
        if (!pode) continue;
        q.prepare("UPDATE reserva_itens SET status = ? WHERE id = ? AND status = ?");
        q.addBindValue(status); q.addBindValue(item.id); q.addBindValue(item.status);
        if (!q.exec() || q.numRowsAffected() != 1) return falhar("A reserva foi alterada. Atualize a lista.");
        if (status == "RECUSADA" || status == "CANCELADA") {
            q.prepare("UPDATE produtos SET estoque = estoque + ? WHERE id = ?");
            q.addBindValue(item.quantidade); q.addBindValue(item.produto);
            if (!q.exec() || q.numRowsAffected() != 1) return falhar("Não foi possível devolver o estoque.");
        }
        ++alterados;
    }
    if (!alterados) return falhar("A reserva não permite esta ação para este usuário.");
    q.prepare("UPDATE reservas SET status = (SELECT CASE WHEN COUNT(DISTINCT status) = 1 THEN MIN(status) ELSE 'PARCIAL' END "
              "FROM reserva_itens WHERE reserva_id = ?) WHERE id = ?"); q.addBindValue(id); q.addBindValue(id);
    if (!q.exec()) return falhar(q.lastError().text());
    if (!db.commit()) return falhar(db.lastError().text());
    return true;
}


bool RepositorioCatalogo::inserirFeira(const QString &nome)
{
    const QString nomeLimpo = nome.trimmed();
    if (nomeLimpo.isEmpty()) {
        m_ultimoErro = "Informe o nome da feira.";
        return false;
    }
    QSqlQuery q(banco());
    q.prepare("INSERT INTO feiras (nome, bairro, local, horario) SELECT ?, '', '', '' "
              "WHERE NOT EXISTS (SELECT 1 FROM feiras WHERE nome = ? COLLATE NOCASE)");
    q.addBindValue(nomeLimpo);
    q.addBindValue(nomeLimpo);
    if (!q.exec()) {
        m_ultimoErro = q.lastError().text();
        return false;
    }
    if (q.numRowsAffected() != 1) {
        m_ultimoErro = "Esta feira já está no catálogo. Selecione-a na lista.";
        return false;
    }
    return true;
}
