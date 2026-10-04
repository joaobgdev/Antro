#ifndef CATALOGO_COMPRADOR_HPP
#define CATALOGO_COMPRADOR_HPP

#include <string>
#include <vector>
#include "models/produto.hpp"


struct FeiraComprador {
    int id;
    std::string nome;
    std::string bairro;
    std::string local;
    std::string horario;
};

struct VendedorComprador {
    int id;
    std::string nome;
    std::string banca;
    std::string descricao;
};

struct OfertaComprador {
    int feiraId;
    int vendedorId;
    int produtoId;
};

struct ItemSacolaComprador {
    int feiraId;
    int vendedorId;
    int produtoId;
    double quantidade;
};



class CatalogoComprador {
public:
    CatalogoComprador();
    const std::vector<FeiraComprador>& getFeiras() const;
    const std::vector<ItemSacolaComprador>& getSacola() const;
    const FeiraComprador* buscarFeira(int id) const;
    const VendedorComprador* buscarVendedor(int id) const;
    const Produto* buscarProduto(int id) const;
    std::vector<VendedorComprador> vendedoresDaFeira(int feiraId) const;
    std::vector<Produto> produtosDoVendedor(int feiraId, int vendedorId) const;
    bool adicionar(int feiraId, int vendedorId, int produtoId, double quantidade);
    void remover(int indice);
    void limpar();
    double totalEstimado() const;
    double quantidadeNaSacola(int produtoId) const;

private:
    bool temOferta(int feiraId, int vendedorId, int produtoId) const;
    std::vector<FeiraComprador> feiras;
    std::vector<VendedorComprador> vendedores;
    std::vector<Produto> produtos;
    std::vector<OfertaComprador> ofertas;
    std::vector<ItemSacolaComprador> sacola;
};

#endif
