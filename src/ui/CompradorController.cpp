#include "ui/CompradorController.hpp"
#include <QString>

CompradorController::CompradorController(QObject* parent) : QObject(parent) {}

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
    for (const ItemSacolaComprador& item : catalogo.getSacola()) {
        const Produto* p = catalogo.buscarProduto(item.produtoId);
        if (!p) continue;
        lista.append(QVariantMap{{"nome", QString::fromStdString(p->getNome())},
                     {"feira", feira(item.feiraId).value("nome")},
                     {"vendedor", vendedor(item.vendedorId).value("nome")},
                     {"unidade", p->getEhPorPeso() ? "kg" : "unidade"},
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

void CompradorController::remover(int indice) { catalogo.remover(indice); emit sacolaChanged(); }
void CompradorController::limpar() { catalogo.limpar(); emit sacolaChanged(); }
int CompradorController::tiposNaSacola() const { return static_cast<int>(catalogo.getSacola().size()); }
double CompradorController::totalEstimado() const { return catalogo.totalEstimado(); }
