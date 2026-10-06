#ifndef GERENCIADORFEIRA_HPP
#define GERENCIADORFEIRA_HPP

#include <map>
#include <queue>
#include <unordered_map>
#include <iostream>
#include "models/produto.hpp"
#include "models/pedido.hpp"

using namespace std;

class GerenciadorFeira {
private:
    map<int, Produto> catalogo;
    queue<Pedido*> filaProcessamento;

public:
    GerenciadorFeira() = default;

    void cadastrarProduto(const Produto& p);
    Produto* buscarProduto(int id);
    void exibirCatalogo() const;

    void receberPedido(Pedido* p);
    void processarProximoPedido();

    void tratarAbandonoPedido(Pedido* p);

    void gerarRelatorioColheita() const;
};

#endif
