#include "services/CatalogoComprador.hpp"
#include <cmath>

void CatalogoComprador::definirDados(const DadosCatalogo& dados)
{
    feiras = dados.feiras;
    vendedores = dados.vendedores;
    produtos = dados.produtos;
    ofertas = dados.ofertas;
    participacoes = dados.participacoes;
    podarSacola();
}

void CatalogoComprador::podarSacola()
{

    std::vector<ItemSacolaComprador> mantidos;
    for (const ItemSacolaComprador& item : sacola) {
        const Produto* produto = buscarProduto(item.produtoId);
        if (!produto || !temOferta(item.feiraId, item.vendedorId, item.produtoId)) continue;
        double jaMantido = 0;
        for (const ItemSacolaComprador& m : mantidos)
            if (m.produtoId == item.produtoId) jaMantido += m.quantidade;
        if (!quantidadeValida(*produto, item.quantidade) || jaMantido + item.quantidade > produto->getEstoque() + 0.000001) continue;
        mantidos.push_back(item);
    }
    sacola = mantidos;
}

const DadosCatalogo CatalogoComprador::dados() const
{
    return {feiras, vendedores, produtos, ofertas, participacoes};
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
        bool participa = false;
        for (const ParticipacaoComprador& p : participacoes)
            if (p.feiraId == feiraId && p.vendedorId == vendedor.id) participa = true;
        for (const OfertaComprador& oferta : ofertas)
            if (oferta.feiraId == feiraId && oferta.vendedorId == vendedor.id) participa = true;
        if (participa) resultado.push_back(vendedor);
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
    if (quantidadeNaSacola(produtoId) + quantidade > produto->getEstoque() + 0.000001) return false;

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
    double passos = quantidade / produto.getPasso();
    return std::abs(passos - std::round(passos)) < 0.00001;
}

bool CatalogoComprador::alterarQuantidade(int indice, double novaQuantidade)
{
    if (indice < 0 || indice >= static_cast<int>(sacola.size())) return false;
    ItemSacolaComprador& item = sacola[indice];
    const Produto* produto = buscarProduto(item.produtoId);
    if (!produto || !quantidadeValida(*produto, novaQuantidade)) return false;
    if (quantidadeNaSacola(item.produtoId) - item.quantidade + novaQuantidade > produto->getEstoque() + 0.000001) return false;
    item.quantidade = novaQuantidade;
    return true;
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
        if (produto) total += std::round(produto->getPreco() * item.quantidade * 100) / 100;
    }
    return total;
}
