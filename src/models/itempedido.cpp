#include "models/itempedido.hpp"
#include <cmath>

using namespace std;

ItemPedido::ItemPedido(const Produto* prod, float qtd)
    : produto(prod), qtdSolicitada(std::isfinite(qtd) && qtd > 0 ? qtd : 0), qtdRealPesa(qtdSolicitada) {}

const Produto* ItemPedido::getProduto() const { return produto; }
float ItemPedido::getQtdSolicitada() const { return qtdSolicitada; }
float ItemPedido::getQtdRealPesa() const { return qtdRealPesa; }

void ItemPedido::setQtdRealPesa(float novaQtd) {
    if (!std::isfinite(novaQtd) || novaQtd <= 0) return;
    if (produto && !produto->getEhPorPeso() && std::floor(novaQtd) != novaQtd) return;
    qtdRealPesa = novaQtd;
}

float ItemPedido::calcularSubtotal() const {
    if (!produto) return 0.0f;
    return qtdRealPesa * produto->getPreco();
}
