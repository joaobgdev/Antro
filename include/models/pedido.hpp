#ifndef PEDIDO_HPP
#define PEDIDO_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include "models/itempedido.hpp"
#include "models/consumidor.hpp"

using namespace std;

//define os status validos de um pedido
enum class StatusPedido {
    SOLICITADO,
    CONFIRMADO,
    RECUSADO,
    RETIRADO,
    CANCELADO
};

class Pedido {
// define um id pro pedido e faz um ponteiro const pro consumidor, ele vê quem é o consumidos mas não pode alterar pelo ponteiro
//cria um vetor (tipo uma lista), com os itens do pedido, o statuspedido guarda qual o status atual do pedido
private:
    int id;
    const Consumidor* consumidor;
    vector<ItemPedido> itens;
    StatusPedido status;

public:
//cria o pedido com um id e atribui ao cliente

    Pedido(int id, const Consumidor* cliente);

//acesso ao id, ao consumidor e ao status atual do pedido
    int getId() const;
    const Consumidor* getConsumidor() const;
    StatusPedido getStatus() const;
//modifica o status do pedido
    void setStatus(StatusPedido novoStatus);
//retorna a lista de itens por referencia, permitindo alterar os itens do pedido
    vector<ItemPedido>& getItens();

//adiciona o produto ao pedido se conseguir descontar a quantidade do estoque
    bool adicionarItem(Produto& prod, float qtd);
//soma o subtotal de todos os itens para calcular o valor total do pedido
    float calcularValorTotal() const;
//mostra o consumidor, o status, os itens e o valor total do pedido
    void imprimirResumo() const;
};

#endif
