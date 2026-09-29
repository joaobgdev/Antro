#include "models/itempedido.hpp"

using namespace std;

ItemPedido::ItemPedido(const Produto* prod, float qtd)
    : produto(prod), qtdSolicitada(qtd), qtdRealPesa(qtd) {}

const Produto* ItemPedido::getProduto() const { return produto; }
float ItemPedido::getQtdSolicitada() const { return qtdSolicitada; }
float ItemPedido::getQtdRealPesa() const { return qtdRealPesa; }

void ItemPedido::setQtdRealPesa(float novaQtd) {
    qtdRealPesa = novaQtd;
}

float ItemPedido::calcularSubtotal() const {
    if (!produto) return 0.0f;
    return qtdRealPesa * produto->getPreco();
}