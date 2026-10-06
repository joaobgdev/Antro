#include "services/CatalogoComprador.hpp"
#include <cmath>

using namespace std;

//Carrinho = Sacola

// Carrega a base de dados do catálogo recebida do servidor
void CatalogoComprador::definirDados(const DadosCatalogo& dados)
{
    feiras = dados.feiras;
    vendedores = dados.vendedores;
    produtos = dados.produtos;
    ofertas = dados.ofertas;
    participacoes = dados.participacoes;
    
    // Limpeza automática do carrinho
    podarSacola();
}

// Remove do carrinho itens que não existem mais no catálogo ou que excedem o estoque
void CatalogoComprador::podarSacola()
{
    vector<ItemSacolaComprador> mantidos;
    
    for (const ItemSacolaComprador& item : sacola) {
        const Produto* produto = buscarProduto(item.produtoId);
        
        // Se o produto não existe ou não possui oferta ativa na feira/vendedor, ignora
        if (!produto || !temOferta(item.feiraId, item.vendedorId, item.produtoId)) continue;
        
        // Soma a quantidade acumulada do mesmo produto
        double jaMantido = 0;
        for (const ItemSacolaComprador& m : mantidos)
            if (m.produtoId == item.produtoId) jaMantido += m.quantidade;
            
        // Valida se a quantidade não excede o estoque
        if (!quantidadeValida(*produto, item.quantidade) || jaMantido + item.quantidade > produto->getEstoque() + 0.000001) continue;
        
        mantidos.push_back(item);
    }
    
    // Atualiza o carrinho apenas com os itens válidos
    sacola = mantidos;
}

const DadosCatalogo CatalogoComprador::dados() const
{
    return {feiras, vendedores, produtos, ofertas, participacoes};
}

// Lista completa de feiras cadastradas
const vector<FeiraComprador>& CatalogoComprador::getFeiras() const { return feiras; }

// Itens no carrinho de compras
const vector<ItemSacolaComprador>& CatalogoComprador::getSacola() const { return sacola; }

// Busca uma feira específica pelo seu ID (retorna nullptr se não encontrar)
const FeiraComprador* CatalogoComprador::buscarFeira(int id) const
{
    for (const FeiraComprador& feira : feiras)
        if (feira.id == id) return &feira;
    return nullptr;
}

// Busca um vendedor específico pelo seu ID ''
const VendedorComprador* CatalogoComprador::buscarVendedor(int id) const
{
    for (const VendedorComprador& vendedor : vendedores)
        if (vendedor.id == id) return &vendedor;
    return nullptr;
}

// Busca um produto específico pelo seu ID ''
const Produto* CatalogoComprador::buscarProduto(int id) const
{
    for (const Produto& produto : produtos)
        if (produto.getId() == id) return &produto;
    return nullptr;
}

// Verifica se existe uma oferta checando feira, vendedor e produto
bool CatalogoComprador::temOferta(int feiraId, int vendedorId, int produtoId) const
{
    for (const OfertaComprador& oferta : ofertas)
        if (oferta.feiraId == feiraId && oferta.vendedorId == vendedorId && oferta.produtoId == produtoId)
            return true;
    return false;
}

// Retorna todos os vendedores que atuam em uma feira específica
vector<VendedorComprador> CatalogoComprador::vendedoresDaFeira(int feiraId) const
{
    vector<VendedorComprador> resultado;
    for (const VendedorComprador& vendedor : vendedores) {
        bool participa = false;
        
        // Checa presença por participação ou por oferta cadastrada
        for (const ParticipacaoComprador& p : participacoes)
            if (p.feiraId == feiraId && p.vendedorId == vendedor.id) participa = true;
        for (const OfertaComprador& oferta : ofertas)
            if (oferta.feiraId == feiraId && oferta.vendedorId == vendedor.id) participa = true;
            
        if (participa) resultado.push_back(vendedor);
    }
    return resultado;
}

// Retorna a lista de produtos ofertados por um vendedor específico em uma feira
vector<Produto> CatalogoComprador::produtosDoVendedor(int feiraId, int vendedorId) const
{
    vector<Produto> resultado;
    for (const Produto& produto : produtos)
        if (temOferta(feiraId, vendedorId, produto.getId())) resultado.push_back(produto);
    return resultado;
}

// Calcula a quantidade total de unidades/quilos de um produto no carinho
double CatalogoComprador::quantidadeNaSacola(int produtoId) const
{
    double total = 0;
    for (const ItemSacolaComprador& item : sacola)
        if (item.produtoId == produtoId) total += item.quantidade;
    return total;
}

// Adiciona um item no carrinho seguindo todas validações
bool CatalogoComprador::adicionar(int feiraId, int vendedorId, int produtoId, double quantidade)
{
    const Produto* produto = buscarProduto(produtoId);
    
    // Validações essenciais
    if (!produto || !temOferta(feiraId, vendedorId, produtoId)) return false;
    if (!quantidadeValida(*produto, quantidade)) return false;
    
    // Garante que a quantidade não ultrapasse o estoque
    if (quantidadeNaSacola(produtoId) + quantidade > produto->getEstoque() + 0.000001) return false;

    // Se o item já estiver no carrinnho para a mesma feira e vendedor,incrementa
    for (ItemSacolaComprador& item : sacola) {
        if (item.feiraId == feiraId && item.vendedorId == vendedorId && item.produtoId == produtoId) {
            item.quantidade += quantidade;
            return true;
        }
    }
    
    // Caso seja um item novo, adicionar no carrinho
    sacola.push_back({feiraId, vendedorId, produtoId, quantidade});
    return true;
}

// Valida se a quantidade informada respeita os passos fracionados (0.5kg, 1kg, etc)
bool CatalogoComprador::quantidadeValida(const Produto& produto, double quantidade) const
{
    if (!isfinite(quantidade) || quantidade <= 0) return false;
    double passos = quantidade / produto.getPasso();
    return abs(passos - round(passos)) < 0.00001;
}

// Altera a quantidade de um item no carrinho
bool CatalogoComprador::alterarQuantidade(int indice, double novaQuantidade)
{
    if (indice < 0 || indice >= static_cast<int>(sacola.size())) return false;
    
    ItemSacolaComprador& item = sacola[indice];
    const Produto* produto = buscarProduto(item.produtoId);
    
    if (!produto || !quantidadeValida(*produto, novaQuantidade)) return false;
    
    // Valida se o novo valor não excede o estoque
    if (quantidadeNaSacola(item.produtoId) - item.quantidade + novaQuantidade > produto->getEstoque() + 0.000001) return false;
    
    item.quantidade = novaQuantidade;
    return true;
}

// Remove um item do carrinho
void CatalogoComprador::remover(int indice)
{
    if (indice >= 0 && indice < static_cast<int>(sacola.size())) 
        sacola.erase(sacola.begin() + indice);
}

// Esvazia completamente o carrinho
void CatalogoComprador::limpar() { sacola.clear(); }

// Calcula o valor total dos itens
double CatalogoComprador::totalEstimado() const
{
    double total = 0;
    for (const ItemSacolaComprador& item : sacola) {
        const Produto* produto = buscarProduto(item.produtoId);
        if (produto) total += round(produto->getPreco() * item.quantidade * 100) / 100;
    }
    return total;
}
