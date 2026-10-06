#ifndef PEDIDO_HPP
#define PEDIDO_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include "models/itempedido.hpp"
#include "models/consumidor.hpp"

using namespace std;

enum class StatusPedido {
    SOLICITADO,
    CONFIRMADO,
    RECUSADO,
    RETIRADO,
    CANCELADO
};

class Pedido {
private:
    int id;
    const Consumidor* consumidor;
    vector<ItemPedido> itens;
    StatusPedido status;

public:
    Pedido(int id, const Consumidor* cliente);

    int getId() const;
    const Consumidor* getConsumidor() const;
    StatusPedido getStatus() const;
    void setStatus(StatusPedido novoStatus);
    vector<ItemPedido>& getItens();

    bool adicionarItem(Produto& prod, float qtd);
    float calcularValorTotal() const;
    void imprimirResumo() const;
};

#endif
