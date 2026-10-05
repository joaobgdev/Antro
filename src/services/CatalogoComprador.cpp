#include "services/CatalogoComprador.hpp"
#include <cmath>

void CatalogoComprador::definirDados(const DadosCatalogo& dados)
{
    feiras = dados.feiras;
    vendedores = dados.vendedores;
    produtos = dados.produtos;
    ofertas = dados.ofertas;
    podarSacola();
}

void CatalogoComprador::podarSacola()
{
    // Remove da sacola o que deixou de existir ou não cabe mais no estoque.
    std::vector<ItemSacolaComprador> mantidos;
    for (const ItemSacolaComprador& item : sacola) {
        const Produto* produto = buscarProduto(item.produtoId);
        if (!produto || !temOferta(item.feiraId, item.vendedorId, item.produtoId)) continue;
        double jaMantido = 0;
        for (const ItemSacolaComprador& m : mantidos)
            if (m.produtoId == item.produtoId) jaMantido += m.quantidade;
        if (jaMantido + item.quantidade > produto->getEstoque()) continue;
        mantidos.push_back(item);
    }
    sacola = mantidos;
}

Produto* CatalogoComprador::produtoMutavel(int id)
{
    for (Produto& produto : produtos)
        if (produto.getId() == id) return &produto;
    return nullptr;
}

const std::vector<FeiraComprador>& CatalogoComprador::getFeiras() const { return feiras; }
const std::vector<ItemSacolaComprador>& CatalogoComprador::getSacola() const { return sacola; }

const FeiraComprador* CatalogoComprador::buscarFeira(int id) const
{
    for (const FeiraComprador& feira : feiras)
        if (feira.id == id) return &feira;
    return nullptr;
}

const VendedorComprador* CatalogoComprador::buscarVendedor(int id) const
{
    for (const VendedorComprador& vendedor : vendedores)
        if (vendedor.id == id) return &vendedor;
    return nullptr;
}

const Produto* CatalogoComprador::buscarProduto(int id) const
{
    for (const Produto& produto : produtos)
        if (produto.getId() == id) return &produto;
    return nullptr;
}

bool CatalogoComprador::temOferta(int feiraId, int vendedorId, int produtoId) const
{
    for (const OfertaComprador& oferta : ofertas)
        if (oferta.feiraId == feiraId && oferta.vendedorId == vendedorId && oferta.produtoId == produtoId)
            return true;
    return false;
}

std::vector<VendedorComprador> CatalogoComprador::vendedoresDaFeira(int feiraId) const
{
    std::vector<VendedorComprador> resultado;
    for (const VendedorComprador& vendedor : vendedores) {
        for (const OfertaComprador& oferta : ofertas) {
            if (oferta.feiraId == feiraId && oferta.vendedorId == vendedor.id) {
                resultado.push_back(vendedor);
                break;
            }
        }
    }
    return resultado;
}

std::vector<Produto> CatalogoComprador::produtosDoVendedor(int feiraId, int vendedorId) const
{
    std::vector<Produto> resultado;
    for (const Produto& produto : produtos)
        if (temOferta(feiraId, vendedorId, produto.getId())) resultado.push_back(produto);
    return resultado;
}

double CatalogoComprador::quantidadeNaSacola(int produtoId) const
{
    double total = 0;
    for (const ItemSacolaComprador& item : sacola)
        if (item.produtoId == produtoId) total += item.quantidade;
    return total;
}

bool CatalogoComprador::adicionar(int feiraId, int vendedorId, int produtoId, double quantidade)
{
    const Produto* produto = buscarProduto(produtoId);
    if (!produto || !temOferta(feiraId, vendedorId, produtoId)) return false;
    if (!quantidadeValida(*produto, quantidade)) return false;
    if (quantidadeNaSacola(produtoId) + quantidade > produto->getEstoque()) return false;

    for (ItemSacolaComprador& item : sacola) {
        if (item.feiraId == feiraId && item.vendedorId == vendedorId && item.produtoId == produtoId) {
            item.quantidade += quantidade;
            return true;
        }
    }
    sacola.push_back({feiraId, vendedorId, produtoId, quantidade});
    return true;
}

bool CatalogoComprador::quantidadeValida(const Produto& produto, double quantidade) const
{
    if (!std::isfinite(quantidade) || quantidade <= 0) return false;
    double passos = produto.getEhPorPeso() ? quantidade * 2 : quantidade;   // kg: passos de 0,5
    return std::floor(passos) == passos;
}

bool CatalogoComprador::alterarQuantidade(int indice, double novaQuantidade)
{
    if (indice < 0 || indice >= static_cast<int>(sacola.size())) return false;
    ItemSacolaComprador& item = sacola[indice];
    const Produto* produto = buscarProduto(item.produtoId);
    if (!produto || !quantidadeValida(*produto, novaQuantidade)) return false;
    if (quantidadeNaSacola(item.produtoId) - item.quantidade + novaQuantidade > produto->getEstoque()) return false;
    item.quantidade = novaQuantidade;
    return true;
}

std::vector<ItemSacolaComprador> CatalogoComprador::finalizarReserva()
{
    if (sacola.empty()) return {};
    for (const ItemSacolaComprador& item : sacola) {
        const Produto* produto = buscarProduto(item.produtoId);
        if (!produto || !temOferta(item.feiraId, item.vendedorId, item.produtoId)) return {};
        if (quantidadeNaSacola(item.produtoId) > produto->getEstoque()) return {};
    }
    for (const ItemSacolaComprador& item : sacola)
        produtoMutavel(item.produtoId)->deduzirEstoque(static_cast<float>(item.quantidade));
    std::vector<ItemSacolaComprador> reservados = sacola;
    sacola.clear();
    return reservados;
}

void CatalogoComprador::remover(int indice)
{
    if (indice >= 0 && indice < static_cast<int>(sacola.size())) sacola.erase(sacola.begin() + indice);
}

void CatalogoComprador::limpar() { sacola.clear(); }

double CatalogoComprador::totalEstimado() const
{
    double total = 0;
    for (const ItemSacolaComprador& item : sacola) {
        const Produto* produto = buscarProduto(item.produtoId);
        if (produto) total += produto->getPreco() * item.quantidade;
    }
    return total;
}
