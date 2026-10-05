#include "ui/CompradorController.hpp"
#include <QDebug>
#include <QString>
#include <QStringList>
#include <cmath>

CompradorController::CompradorController(QObject* parent) : QObject(parent)
{
    m_bancoPronto = repo.abrir();
    if (!m_bancoPronto)
        qWarning() << "Não foi possível abrir o banco do catálogo:" << repo.ultimoErro();
    else
        catalogo.definirDados(repo.carregar());
}

void CompradorController::recarregar()
{
    catalogo.definirDados(repo.carregar());   // também revalida o carrinho
    emit produtosChanged();
    emit sacolaChanged();
}

QVariantMap CompradorController::feira(int id) const
{
    const FeiraComprador* f = catalogo.buscarFeira(id);
    if (!f) return {};
    return {{"id", f->id}, {"nome", QString::fromStdString(f->nome)},
            {"bairro", QString::fromStdString(f->bairro)}, {"local", QString::fromStdString(f->local)},
            {"horario", QString::fromStdString(f->horario)}};
}

QVariantList CompradorController::feiras() const
{
    QVariantList lista;
    for (const FeiraComprador& f : catalogo.getFeiras()) lista.append(feira(f.id));
    return lista;
}

QVariantMap CompradorController::vendedor(int id) const
{
    const VendedorComprador* v = catalogo.buscarVendedor(id);
    if (!v) return {};
    return {{"id", v->id}, {"nome", QString::fromStdString(v->nome)},
            {"banca", QString::fromStdString(v->banca)}, {"descricao", QString::fromStdString(v->descricao)}};
}

QVariantList CompradorController::vendedores(int feiraId) const
{
    QVariantList lista;
    for (const VendedorComprador& v : catalogo.vendedoresDaFeira(feiraId)) lista.append(vendedor(v.id));
    return lista;
}

QVariantList CompradorController::produtos(int feiraId, int vendedorId) const
{
    QVariantList lista;
    for (const Produto& p : catalogo.produtosDoVendedor(feiraId, vendedorId)) {
        lista.append(QVariantMap{{"id", p.getId()}, {"nome", QString::fromStdString(p.getNome())},
                     {"preco", p.getPreco()}, {"porPeso", p.getEhPorPeso()},
                     {"unidade", p.getEhPorPeso() ? "kg" : "unidade"},
                     {"disponivel", p.getEstoque() - catalogo.quantidadeNaSacola(p.getId())}});
    }
    return lista;
}

QVariantList CompradorController::sacola() const
{
    QVariantList lista;
    const std::vector<ItemSacolaComprador>& itens = catalogo.getSacola();
    for (size_t i = 0; i < itens.size(); ++i) {
        const ItemSacolaComprador& item = itens[i];
        const Produto* p = catalogo.buscarProduto(item.produtoId);
        if (!p) continue;
        const double passo = p->getEhPorPeso() ? 0.5 : 1.0;
        // quanto ainda cabe neste item: estoque menos o que está nas outras linhas do mesmo produto
        const double maximo = p->getEstoque() - catalogo.quantidadeNaSacola(item.produtoId) + item.quantidade;
        lista.append(QVariantMap{{"indice", static_cast<int>(i)},
                     {"nome", QString::fromStdString(p->getNome())},
                     {"feira", feira(item.feiraId).value("nome")},
                     {"vendedor", vendedor(item.vendedorId).value("nome")},
                     {"unidade", p->getEhPorPeso() ? "kg" : "unidade"},
                     {"preco", p->getPreco()},
                     {"passo", passo},
                     {"podeAumentar", item.quantidade + passo <= maximo},
                     {"quantidade", item.quantidade}, {"subtotal", p->getPreco() * item.quantidade}});
    }
    return lista;
}

bool CompradorController::adicionar(int feiraId, int vendedorId, int produtoId, double quantidade)
{
    if (!catalogo.adicionar(feiraId, vendedorId, produtoId, quantidade)) return false;
    emit sacolaChanged();
    return true;
}

bool CompradorController::alterarQuantidade(int indice, double novaQuantidade)
{
    if (!catalogo.alterarQuantidade(indice, novaQuantidade)) return false;
    emit sacolaChanged();
    return true;
}

QVariantMap CompradorController::finalizarReserva(const QString& telefone, const QString& nome, const QString& data, const QString& hora)
{
    auto falha = [](const QString& erro) { return QVariantMap{{"ok", false}, {"erro", erro}}; };
    if (!m_bancoPronto) return falha("O banco de dados não está disponível.");
    if (catalogo.getSacola().empty()) return falha("Seu carrinho está vazio.");

    const auto antes = catalogo.getSacola();
    recarregar();
    if (catalogo.getSacola().size() != antes.size()) return falha("Alguns itens não estão mais disponíveis. Revise o carrinho.");
    // Guarda o resumo antes de esvaziar o carrinho.
    const double total = catalogo.totalEstimado();
    QVariantList itensResumo;
    QStringList feirasResumo;
    const std::vector<ItemSacolaComprador> copia = catalogo.getSacola();
    for (const QVariant& v : sacola()) itensResumo.append(v);
    for (const ItemSacolaComprador& item : copia) {
        const QString f = feira(item.feiraId).value("nome").toString();
        if (!feirasResumo.contains(f)) feirasResumo.append(f);
    }

    const std::vector<ItemSacolaComprador> reservados = catalogo.finalizarReserva();
    if (reservados.empty()) {
        recarregar();
        return falha("Alguns itens não estão mais disponíveis. Revise o carrinho.");
    }
    const int codigo = repo.salvarReserva(telefone, nome, reservados, total, data, hora);
    if (codigo == 0) {
        // Não gravou: volta ao estado do banco e restaura o carrinho.
        const QString erro = repo.ultimoErro();
        recarregar();
        for (const ItemSacolaComprador& item : reservados)
            catalogo.adicionar(item.feiraId, item.vendedorId, item.produtoId, item.quantidade);
        emit sacolaChanged();
        return falha("Não foi possível salvar a reserva: " + erro);
    }
    recarregar();   // estoque novo vem do banco
    return {{"ok", true}, {"erro", QString()}, {"codigo", codigo}, {"total", total},
            {"itens", itensResumo}, {"feiras", feirasResumo}, {"status", "SOLICITADA"}, {"data", data}, {"hora", hora}};
}

QVariantList CompradorController::produtosDoFeirante(const QString& telefone) const
{
    QVariantList lista;
    const QVector<RegistroProdutoFeirante> registros = repo.produtosDoFeirante(telefone);
    for (const RegistroProdutoFeirante& r : registros) {
        QStringList nomesFeiras;
        QVariantList feiraIds;
        for (int id : r.feiraIds) { nomesFeiras.append(feira(id).value("nome").toString()); feiraIds.append(id); }
        lista.append(QVariantMap{{"id", r.id}, {"nome", r.nome}, {"preco", r.preco}, {"porPeso", r.porPeso},
                     {"unidade", r.porPeso ? "kg" : "unidade"}, {"estoque", r.estoque},
                     {"feiras", nomesFeiras.join(", ")}, {"feiraIds", feiraIds}});
    }
    return lista;
}

QString CompradorController::adicionarProdutoFeirante(const QString& telefone, const QString& nomeFeirante,
                                                      const QString& banca, const QString& nome, double preco,
                                                      bool porPeso, double estoque, const QVariantList& feiraIds)
{
    if (!m_bancoPronto) return "O banco de dados não está disponível.";
    const QString nomeLimpo = nome.trimmed();
    if (nomeLimpo.isEmpty()) return "Informe o nome do produto.";
    if (!std::isfinite(preco) || preco <= 0) return "Informe um preço maior que zero.";
    if (!std::isfinite(estoque) || estoque <= 0) return "Informe uma quantidade em estoque maior que zero.";
    if (porPeso ? std::floor(estoque * 2) != estoque * 2 : std::floor(estoque) != estoque)
        return porPeso ? "O estoque por kg deve ser múltiplo de 0,5." : "O estoque por unidade deve ser um número inteiro.";

    QVector<int> ids;
    for (const QVariant& v : feiraIds) {
        const int id = v.toInt();
        if (catalogo.buscarFeira(id) && !ids.contains(id)) ids.append(id);
    }
    if (ids.isEmpty()) return "Escolha pelo menos uma feira.";

    if (!repo.inserirProduto(telefone, nomeFeirante, banca, nomeLimpo, preco, porPeso, estoque, ids))
        return "Não foi possível salvar o produto: " + repo.ultimoErro();
    recarregar();
    return QString();
}

bool CompradorController::removerProdutoFeirante(const QString& telefone, int produtoId)
{
    if (!repo.removerProduto(telefone, produtoId)) return false;
    recarregar();
    return true;
}

void CompradorController::remover(int indice) { catalogo.remover(indice); emit sacolaChanged(); }
void CompradorController::limpar() { catalogo.limpar(); emit sacolaChanged(); }
int CompradorController::tiposNaSacola() const { return static_cast<int>(catalogo.getSacola().size()); }
double CompradorController::totalEstimado() const { return catalogo.totalEstimado(); }

QString CompradorController::editarProdutoFeirante(const QString& telefone, int id, double preco, double estoque, double estoqueAnterior, const QVariantList& feiras)
{
    if (!m_bancoPronto) return "O banco de dados não está disponível.";
    bool encontrado = false, peso = false;
    for (const auto &produto : repo.produtosDoFeirante(telefone))
        if (produto.id == id) { encontrado = true; peso = produto.porPeso; }
    if (!encontrado) return "Produto não encontrado para este vendedor.";
    if (!std::isfinite(preco) || preco <= 0 || !std::isfinite(estoque) || estoque < 0)
        return "Informe preço positivo e estoque igual ou maior que zero.";
    if (peso ? std::floor(estoque * 2) != estoque * 2 : std::floor(estoque) != estoque)
        return "Informe estoque inteiro por unidade ou múltiplo de 0,5 kg.";
    QVector<int> ids;
    for (const auto &f : feiras) if (catalogo.buscarFeira(f.toInt()) && !ids.contains(f.toInt())) ids.append(f.toInt());
    if (ids.isEmpty()) return "Escolha pelo menos uma feira.";
    if (!repo.editarProduto(telefone, id, preco, estoque, estoqueAnterior, ids)) return repo.ultimoErro();
    recarregar(); return {};
}


QString CompradorController::adicionarFeira(const QString &nome)
{
    if (!m_bancoPronto) return "O banco de dados não está disponível.";
    if (!repo.inserirFeira(nome)) return repo.ultimoErro();
    recarregar();
    return {};
}
