#ifndef GERENCIADORFEIRA_HPP
#define GERENCIADORFEIRA_HPP

#include <map>
#include <queue>
#include <unordered_map>
#include <iostream>
#include "models/Produto.hpp"
#include "models/Pedido.hpp"

using namespace std;

class GerenciadorFeira {
private:
    map<int, Produto> catalogo;          // map para busca e ordenação
    queue<Pedido*> filaProcessamento;    // queue para ordem de chegada 

public:
    GerenciadorFeira() = default;

    // --- Catálogo de Produtos 
    void cadastrarProduto(const Produto& p);
    Produto* buscarProduto(int id);
    void exibirCatalogo() const;

    // Processamento de pedidos
    void receberPedido(Pedido* p);
    void processarProximoPedido(); // Retira da fila, ajusta pesos e calcula total

    //Pedidos abandonados
    void tratarAbandonoPedido(Pedido* p);

    void gerarRelatorioColheita() const;
};

#endif // GERENCIADORFEIRA_HPP