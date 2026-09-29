#ifndef PEDIDO_HPP
#define PEDIDO_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include "models/ItemPedido.hpp"
#include "models/Consumidor.hpp"

using namespace std;

// Tabela de Estados do Pedido
enum class StatusPedido {
    SOLICITADO,
    EM_SEPARACAO,
    AGUARDANDO_PAGAMENTO,
    PRONTO_PARA_RETIRADA,
    RETIRADO,
    ABANDONADO
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

#endif // PEDIDO_HPP