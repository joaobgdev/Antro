#ifndef ITEMPEDIDO_HPP
#define ITEMPEDIDO_HPP

#include "models/produto.hpp"

using namespace std;

class ItemPedido {
private:
    const Produto* produto;
    float qtdSolicitada;
    float qtdRealPesa;

public:
    ItemPedido(const Produto* prod, float qtd);

    const Produto* getProduto() const;
    float getQtdSolicitada() const;
    float getQtdRealPesa() const;
    void setQtdRealPesa(float novaQtd);

    float calcularSubtotal() const;
};

#endif
