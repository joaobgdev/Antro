#ifndef ITEMPEDIDO_HPP
#define ITEMPEDIDO_HPP

#include "models/produto.hpp"

using namespace std;
//representa um produto dentro do pedido
class ItemPedido {
// o ponteiro produto é do tipo const pq ela acessa o valor mas não pode alterar
private:
    const Produto* produto;
    float qtdSolicitada;
    float qtdRealPesa;

public:
//recebe qual produto e qual a quantidade e cria um novo item
    ItemPedido(const Produto* prod, float qtd);
//ve a quantidade solicitada, quanto foi pesado e modifica o peso
    const Produto* getProduto() const;
    float getQtdSolicitada() const;
    float getQtdRealPesa() const;
    void setQtdRealPesa(float novaQtd);
//calcula o preço com o novo peso do 
    float calcularSubtotal() const;
};

#endif
