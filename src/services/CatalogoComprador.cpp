#include "services/CatalogoComprador.hpp"
#include <cmath>

CatalogoComprador::CatalogoComprador()
{

    feiras = {
        {1, "Feira Agroecológica da Várzea", "Várzea", "Praça da Várzea", "Sábados, 7h às 10h"},
        {2, "Espaço Agroecológico das Graças", "Graças", "Rua Andrade de Souza", "Sábados, 4h às 11h"},
        {3, "Feira de Produtos Orgânicos de Casa Forte", "Casa Forte", "Praça da Vitória Régia", "Sábados, 5h às 11h"},
        {4, "Espaço Agroecológico do Sítio da Trindade", "Casa Amarela", "Sítio da Trindade", "Sábados, 5h às 11h"}
    };

    vendedores = {
        {1, "Ana Oliveira", "Sítio Boa Vista", "Hortaliças cultivadas em família."},
        {2, "João Batista", "Sítio Riacho Verde", "Frutas e raízes da estação."},
        {3, "Rosa Ferreira", "Quintal da Rosa", "Temperos frescos e legumes."}
    };
    produtos = {
        Produto(101, "Alface crespa", 3.50f, false, 20),
        Produto(102, "Tomate cereja", 12.00f, true, 10),
        Produto(103, "Banana-prata", 8.50f, true, 15),
        Produto(104, "Macaxeira", 7.00f, true, 12),
        Produto(105, "Coentro", 3.00f, false, 30),
        Produto(106, "Abóbora", 6.00f, true, 10)
    };

    ofertas = {{1,1,101}, {1,1,102}, {2,1,101}, {2,1,102},
               {1,2,103}, {1,2,104}, {3,2,103}, {3,2,104},
               {2,3,105}, {2,3,106}, {3,3,105}, {3,3,106}};
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
    if (!std::isfinite(quantidade) || quantidade <= 0) return false;

    double passos = produto->getEhPorPeso() ? quantidade * 2 : quantidade;
    if (std::floor(passos) != passos) return false;
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
