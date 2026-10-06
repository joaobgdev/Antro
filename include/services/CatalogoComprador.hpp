#ifndef CATALOGO_COMPRADOR_HPP
#define CATALOGO_COMPRADOR_HPP

#include <string>
#include <vector>
#include "models/produto.hpp"

using namespace std;

struct FeiraComprador {
    int id;
    string nome;
    string bairro;
    string local;
    string horario;
    int diaSemana = 0;
    string inicio;
    string fim;
};

struct VendedorComprador {
    int id;
    string nome;
    string banca;
    string descricao;
    bool exemplo = false;
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

struct ParticipacaoComprador {
    int feiraId;
    int vendedorId;
};

struct DadosCatalogo {
    vector<FeiraComprador> feiras;
    vector<VendedorComprador> vendedores;
    vector<Produto> produtos;
    vector<OfertaComprador> ofertas;
    vector<ParticipacaoComprador> participacoes;
};

class CatalogoComprador {
public:
    CatalogoComprador() = default;
    void definirDados(const DadosCatalogo& dados);
    const DadosCatalogo dados() const;
    const vector<FeiraComprador>& getFeiras() const;
    const vector<ItemSacolaComprador>& getSacola() const;
    const FeiraComprador* buscarFeira(int id) const;
    const VendedorComprador* buscarVendedor(int id) const;
    const Produto* buscarProduto(int id) const;
    vector<VendedorComprador> vendedoresDaFeira(int feiraId) const;
    vector<Produto> produtosDoVendedor(int feiraId, int vendedorId) const;
    bool adicionar(int feiraId, int vendedorId, int produtoId, double quantidade);
    bool alterarQuantidade(int indice, double novaQuantidade);
    void remover(int indice);
    void limpar();
    double totalEstimado() const;
    double quantidadeNaSacola(int produtoId) const;

private:
    bool quantidadeValida(const Produto& produto, double quantidade) const;
    void podarSacola();
    bool temOferta(int feiraId, int vendedorId, int produtoId) const;
    vector<FeiraComprador> feiras;
    vector<VendedorComprador> vendedores;
    vector<Produto> produtos;
    vector<OfertaComprador> ofertas;
    vector<ParticipacaoComprador> participacoes;
    vector<ItemSacolaComprador> sacola;
};

#endif
