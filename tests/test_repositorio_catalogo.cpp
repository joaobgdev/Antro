#include "services/RepositorioCatalogo.hpp"
#include "services/AgendaFeira.hpp"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimeZone>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

static void executar(QSqlDatabase db, const QString &sql)
{
    QSqlQuery q(db);
    if (!q.exec(sql)) { cerr << q.lastError().text().toStdString() << '\n'; abort(); }
}

static void usuarios(QSqlDatabase db)
{
    executar(db, "CREATE TABLE users(id INTEGER PRIMARY KEY, phone TEXT UNIQUE, role TEXT, name TEXT, market_name TEXT, ocs_number TEXT, password_hash TEXT, salt TEXT)");
    executar(db, "INSERT INTO users VALUES (1,'81999990001','feirante','Maria Silva','Banca Maria','OCS1','hash','sal'), (2,'81999990002','feirante','João Souza','Banca João','OCS2','hash','sal'), (3,'81999990003','comprador','Lucas Medrado','','','hash','sal'), (4,'81999990004','comprador','Ana Lima','','','hash','sal')");
}

static RegistroProdutoFeirante produto(const QString &nome, const QString &tipo, double preco, double estoque)
{
    RegistroProdutoFeirante p;
    p.nome = nome; p.tipoVenda = tipo; p.preco = preco; p.estoque = estoque;
    return p;
}

static bool salvar(RepositorioCatalogo &repo, const QString &telefone, const QVector<int> &feiras,
                  const QVector<RegistroProdutoFeirante> &produtos)
{
    return repo.salvarPerfil(telefone, feiras, produtos, repo.produtosDoFeirante(telefone));
}

static QVector<AgendamentoReserva> horarios(const DadosCatalogo &dados, const QVector<int> &ids)
{
    QVector<AgendamentoReserva> lista;
    for (int id : ids) {
        for (const FeiraComprador &f : dados.feiras) if (f.id == id) {
            QString data = AgendaFeira::datas(f).at(1);
            lista.append(AgendaFeira::janelas(f, data).first());
        }
    }
    return lista;
}

static void testarAgenda()
{
    FeiraComprador feira{1,"Várzea","Várzea","Praça","Sábado",6,"07:00","10:00"};
    QTimeZone zona("America/Recife");
    QDateTime antes(QDate(2026,10,10),QTime(6,59),zona);
    QDateTime durante(QDate(2026,10,10),QTime(7,30),zona);
    QDateTime depois(QDate(2026,10,10),QTime(10,0),zona);
    assert(!AgendaFeira::aberta(feira, antes));
    assert(AgendaFeira::aberta(feira, durante));
    assert(!AgendaFeira::aberta(feira, depois));
    assert(AgendaFeira::janelas(feira,"2026-10-10",durante).size() == 3);
    assert(AgendaFeira::janelas(feira,"2026-10-10",depois).isEmpty());
    assert(AgendaFeira::janelas(feira,"2026-10-11",antes).isEmpty());
    assert(AgendaFeira::datas(feira,depois).first() == "2026-10-17");
    assert(!AgendaFeira::validar(feira,{1,"2026-10-10","06:00","07:00"},antes));
    assert(!AgendaFeira::validar(feira,{2,"2026-10-10","07:00","08:00"},antes));
}

static void testarReservas(const QString &arquivo)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","preparar");
        db.setDatabaseName(arquivo); assert(db.open()); usuarios(db); db.close();
    }
    QSqlDatabase::removeDatabase("preparar");
    RepositorioCatalogo repo(arquivo), outro(arquivo);
    assert(repo.abrir()); assert(outro.abrir());
    assert(repo.carregar().vendedores.empty());
    assert(!salvar(repo,"81999990003",{1},{produto("Alface","unidade",3.5,10)}));
    assert(salvar(repo,"81999990001",{1,2},{produto("Alface","unidade",3.5,10),produto("Gengibre","100g",2,0.3)}));
    assert(salvar(repo,"81999990002",{1},{produto("Tomate","kg",12,5)}));
    auto maria = repo.produtosDoFeirante("81999990001");
    auto joao = repo.produtosDoFeirante("81999990002");
    DadosCatalogo dados = repo.carregar();
    assert(dados.vendedores.size() == 2);
    int vm = dados.vendedores[0].id, vj = dados.vendedores[1].id;
    int alface = maria[0].id, gengibre = maria[1].id, tomate = joao[0].id;
    CatalogoComprador catalogo;
    catalogo.definirDados(dados);
    assert(catalogo.produtosDoVendedor(1,vm).size() == 2);
    assert(abs(catalogo.buscarProduto(gengibre)->getPreco() - 20) < 0.000001);
    assert(abs(catalogo.buscarProduto(gengibre)->getPasso() - 0.1) < 0.000001);
    vector<ItemSacolaComprador> itens{{1,vm,alface,2},{2,vm,alface,1},{1,vj,tomate,0.5},{1,vm,gengibre,0.3}};
    auto agenda = horarios(dados,{1,2});
    assert(repo.salvarReservas("81999990001",itens,agenda,dados).isEmpty());
    assert(repo.salvarReservas("81999990003",itens,{},dados).isEmpty());
    auto invalidos = itens; invalidos.back().quantidade = -0.3;
    assert(repo.salvarReservas("81999990003",invalidos,agenda,dados).isEmpty());
    invalidos.back().quantidade = numeric_limits<double>::infinity();
    assert(repo.salvarReservas("81999990003",invalidos,agenda,dados).isEmpty());
    invalidos.back().quantidade = 0.05;
    assert(repo.salvarReservas("81999990003",invalidos,agenda,dados).isEmpty());
    auto insuficientes = itens; insuficientes[2].quantidade = 50;
    assert(repo.salvarReservas("81999990003",insuficientes,agenda,dados).isEmpty());
    assert(repo.produtosDoFeirante("81999990001")[0].estoque == 10);
    assert(repo.reservasDoUsuario("81999990003",false).isEmpty());
    auto ids = repo.salvarReservas("81999990003",itens,agenda,dados);
    if (ids.isEmpty()) cerr << repo.ultimoErro().toStdString() << '\n';
    assert(ids.size() == 3);
    assert(repo.produtosDoFeirante("81999990001")[0].estoque == 7);
    assert(repo.produtosDoFeirante("81999990001")[1].estoque == 0);
    assert(!repo.salvarPerfil("81999990001",{1,2},maria,maria));
    auto novas = repo.produtosDoFeirante("81999990001");
    auto outraUnidade = novas; outraUnidade[1].tipoVenda = "kg";
    assert(!repo.salvarPerfil("81999990001",{1,2},outraUnidade,novas));
    auto compras = repo.reservasDoUsuario("81999990003",false);
    assert(compras.size() == 3);
    double total = 0; for (const RegistroReserva &r : compras) { total += r.total; assert(r.status == "SOLICITADA"); assert(!r.data.isEmpty()); }
    assert(abs(total - 22.5) < 0.000001);
    assert(repo.reservasDoUsuario("81999990001",true).size() == 2);
    assert(repo.reservasDoUsuario("81999990002",true).size() == 1);
    assert(repo.reservasDoUsuario("81999990004",false).isEmpty());
    assert(!repo.alterarReserva("81999990002",true,ids[0],"ACEITA"));
    assert(!repo.alterarReserva("81999990004",false,ids[0],"CANCELADA"));
    assert(!repo.alterarReserva("81999990003",false,ids[0],"ACEITA"));
    assert(repo.alterarReserva("81999990001",true,ids[0],"ACEITA"));
    assert(!repo.alterarReserva("81999990001",true,ids[0],"RECUSADA"));
    assert(!repo.alterarReserva("81999990001",true,ids[0],"RETIRADA"));
    assert(repo.alterarReserva("81999990003",false,ids[0],"CANCELADA"));
    assert(!outro.alterarReserva("81999990003",false,ids[0],"CANCELADA"));
    assert(repo.produtosDoFeirante("81999990001")[0].estoque == 9);
    assert(abs(repo.produtosDoFeirante("81999990001")[1].estoque - 0.3) < 0.000001);
    assert(repo.alterarReserva("81999990001",true,ids[1],"RECUSADA"));
    assert(!repo.alterarReserva("81999990001",true,ids[1],"RECUSADA"));
    assert(repo.produtosDoFeirante("81999990001")[0].estoque == 10);
    auto precoNovo = repo.produtosDoFeirante("81999990002"); precoNovo[0].preco = 14;
    assert(salvar(repo,"81999990002",{1},precoNovo));
    assert(repo.salvarReservas("81999990004",{{1,vj,tomate,0.5}},agenda,dados).isEmpty());
    auto oferta = repo.carregar();
    assert(repo.removerProduto("81999990002",tomate));
    assert(repo.salvarReservas("81999990004",{{1,vj,tomate,0.5}},agenda,oferta).isEmpty());
    assert(!repo.removerProduto("81999990002",alface));
    assert(repo.alterarReserva("81999990002",true,ids[2],"RECUSADA"));
    assert(repo.reservasDoUsuario("81999990002",true).first().itens.first().nome == "Tomate");
    auto venda = repo.produtosDoFeirante("81999990001"); venda[0].estoque = 1;
    assert(salvar(repo,"81999990001",{1,2},venda));
    auto atual = repo.carregar();
    auto primeiro = repo.salvarReservas("81999990003",{{1,vm,alface,1}},agenda,atual);
    assert(primeiro.size() == 1);
    assert(outro.salvarReservas("81999990004",{{2,vm,alface,1}},agenda,atual).isEmpty());
    assert(repo.produtosDoFeirante("81999990001")[0].estoque == 0);
    assert(repo.alterarReserva("81999990003",false,primeiro.first(),"CANCELADA"));
    auto pausados = repo.produtosDoFeirante("81999990001");
    for (RegistroProdutoFeirante &p : pausados) p.ativo = false;
    assert(salvar(repo,"81999990001",{},pausados));
    catalogo.definirDados(repo.carregar());
    assert(catalogo.produtosDoVendedor(1,vm).empty());
    auto reativados = repo.produtosDoFeirante("81999990001");
    assert(reativados.size() == 2);
    reativados[0].ativo = true;
    assert(salvar(repo,"81999990001",{1},reativados));
    catalogo.definirDados(repo.carregar());
    assert(catalogo.produtosDoVendedor(1,vm).size() == 1);
    assert(catalogo.buscarProduto(alface)->getEstoque() == 1);
}

static void testarMigracao(const QString &arquivo)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","antigo"); db.setDatabaseName(arquivo); assert(db.open()); usuarios(db);
        executar(db,"CREATE TABLE seller_markets(id INTEGER PRIMARY KEY, seller_phone TEXT, market_name TEXT)");
        executar(db,"CREATE TABLE seller_products(id INTEGER PRIMARY KEY, seller_phone TEXT, name TEXT, sale_type TEXT, price REAL)");
        executar(db,"INSERT INTO seller_markets VALUES(1,'81999990001','Feira da Várzea'),(2,'81999990001','Feira Agro UFPE'),(3,'81999990001','Feira de Casa Forte')");
        executar(db,"INSERT INTO seller_products VALUES(1,'81999990001','Gengibre','100g',2)");
        executar(db,"CREATE TABLE vendedores(id INTEGER PRIMARY KEY, user_phone TEXT UNIQUE, nome TEXT, banca TEXT, descricao TEXT)");
        executar(db,"INSERT INTO vendedores VALUES(1,'81999990002','João Souza','Banca João',''),(2,NULL,'Exemplo','Exemplo','')");
        executar(db,"CREATE TABLE produtos(id INTEGER PRIMARY KEY, vendedor_id INTEGER, nome TEXT, preco REAL, por_peso INTEGER, estoque REAL)");
        executar(db,"INSERT INTO produtos VALUES(10,1,'Alface',3,0,5),(11,2,'Tomate',12,1,4)");
        executar(db,"CREATE TABLE ofertas(feira_id INTEGER, produto_id INTEGER, PRIMARY KEY(feira_id,produto_id))");
        executar(db,"INSERT INTO ofertas VALUES(1,10),(3,11)");
        executar(db,"CREATE TABLE reservas(id INTEGER PRIMARY KEY, comprador_phone TEXT, comprador_nome TEXT, criada_em TEXT, total REAL, status TEXT)");
        executar(db,"CREATE TABLE reserva_itens(id INTEGER PRIMARY KEY, reserva_id INTEGER, feira_id INTEGER, vendedor_id INTEGER, produto_id INTEGER, produto_nome TEXT, unidade TEXT, quantidade REAL, preco REAL)");
        executar(db,"INSERT INTO reservas VALUES(7,'81999990003','Lucas Medrado','2026-10-01',9,'SOLICITADA')");
        executar(db,"INSERT INTO reserva_itens VALUES(1,7,1,1,10,'Alface','unidade',1,3),(2,7,3,2,11,'Tomate','kg',0.5,12)");
        db.close();
    }
    QSqlDatabase::removeDatabase("antigo");
    int quantidade;
    {
        RepositorioCatalogo repo(arquivo); assert(repo.abrir());
        auto produtos = repo.produtosDoFeirante("81999990001");
        assert(produtos.size() == 1); assert(produtos[0].preco == 2); assert(produtos[0].estoque == 0);
        assert(produtos[0].tipoVenda == "100g"); assert(produtos[0].feiraIds.size() == 2);
        assert(repo.feirasPendentes("81999990001") == QStringList{"Feira Agro UFPE"});
        auto reservas = repo.reservasDoUsuario("81999990003",false);
        assert(reservas.size() == 2);
        double total = 0; for (const auto &r : reservas) { total += r.total; assert(r.itens.size() == 1); }
        assert(total == 9); assert(repo.reservasDoUsuario("81999990002",true).size() == 1);
        quantidade = static_cast<int>(repo.carregar().produtos.size());
        assert(repo.alterarReserva("81999990002",true,7,"ACEITA"));
        assert(repo.alterarReserva("81999990002",true,7,"RETIRADA"));
        assert(!repo.alterarReserva("81999990003",false,7,"CANCELADA"));
    }
    {
        RepositorioCatalogo repo(arquivo); assert(repo.abrir());
        assert(static_cast<int>(repo.carregar().produtos.size()) == quantidade);
        assert(repo.reservasDoUsuario("81999990003",false).size() == 2);
    }
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","verificar"); db.setDatabaseName(arquivo); assert(db.open());
        QSqlQuery q(db); assert(q.exec("SELECT COUNT(*) FROM seller_products")); assert(q.next()); assert(q.value(0).toInt() == 1);
        assert(q.exec("SELECT COUNT(*) FROM users")); assert(q.next()); assert(q.value(0).toInt() == 4);
        q.finish(); db.close();
    }
    QSqlDatabase::removeDatabase("verificar");
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    QTemporaryDir pasta; assert(pasta.isValid());
    testarAgenda(); testarReservas(pasta.path()+"/novo.db"); testarMigracao(pasta.path()+"/antigo.db");
    cout << "Agenda, migração, estoque, permissões e reservas passaram.\n";
}
