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

// Tudo o que o catálogo precisa para funcionar; vem do banco (RepositorioCatalogo).
struct DadosCatalogo {
    std::vector<FeiraComprador> feiras;
    std::vector<VendedorComprador> vendedores;
    std::vector<Produto> produtos;
    std::vector<OfertaComprador> ofertas;
};


class CatalogoComprador {
public:
    CatalogoComprador() = default;
    void definirDados(const DadosCatalogo& dados);   // substitui o catálogo (a sacola é revalidada)
    const std::vector<FeiraComprador>& getFeiras() const;
    const std::vector<ItemSacolaComprador>& getSacola() const;
    const FeiraComprador* buscarFeira(int id) const;
    const VendedorComprador* buscarVendedor(int id) const;
    const Produto* buscarProduto(int id) const;
    std::vector<VendedorComprador> vendedoresDaFeira(int feiraId) const;
    std::vector<Produto> produtosDoVendedor(int feiraId, int vendedorId) const;
    bool adicionar(int feiraId, int vendedorId, int produtoId, double quantidade);
    bool alterarQuantidade(int indice, double novaQuantidade);
    void remover(int indice);
    void limpar();
    double totalEstimado() const;
    double quantidadeNaSacola(int produtoId) const;
    // Confirma a reserva: revalida estoque, baixa o estoque, esvazia a sacola e devolve os itens.
    // Devolve lista vazia (sem alterar nada) se a sacola estiver vazia ou inválida.
    std::vector<ItemSacolaComprador> finalizarReserva();

private:
    bool quantidadeValida(const Produto& produto, double quantidade) const;
    void podarSacola();
    Produto* produtoMutavel(int id);
    bool temOferta(int feiraId, int vendedorId, int produtoId) const;
    std::vector<FeiraComprador> feiras;
    std::vector<VendedorComprador> vendedores;
    std::vector<Produto> produtos;
    std::vector<OfertaComprador> ofertas;
    std::vector<ItemSacolaComprador> sacola;
};

#endif
