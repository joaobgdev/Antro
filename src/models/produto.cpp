#include "models/produto.hpp"

using namespace std;

Produto::Produto(int id, string nome, float preco, bool ehPorPeso, float estoque)
    : id(id), nome(nome), precoUnitarioOuKg(preco), ehPorPeso(ehPorPeso), estoqueDisponivel(estoque) {}

int Produto::getId() const { return id; }
string Produto::getNome() const { return nome; }
float Produto::getPreco() const { return precoUnitarioOuKg; }
bool Produto::getEhPorPeso() const { return ehPorPeso; }
float Produto::getEstoque() const { return estoqueDisponivel; }

bool Produto::deduzirEstoque(float qtd) {
    if (estoqueDisponivel >= qtd) {
        estoqueDisponivel -= qtd;
        return true;
    }
    return false;
}

void Produto::reporEstoque(float qtd) {
    estoqueDisponivel += qtd;
}