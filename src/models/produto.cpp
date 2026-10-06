#include "models/produto.hpp"
#include <cmath>

using namespace std;

Produto::Produto(int id, string nome, double preco, bool ehPorPeso, double estoque, double passo)
    : id(id), nome(nome), precoUnitarioOuKg(preco), ehPorPeso(ehPorPeso),
      estoqueDisponivel(estoque), passoVenda(ehPorPeso ? (passo > 0 ? passo : 0.5) : 1.0) {}

int Produto::getId() const { return id; }
string Produto::getNome() const { return nome; }
double Produto::getPreco() const { return precoUnitarioOuKg; }
bool Produto::getEhPorPeso() const { return ehPorPeso; }
double Produto::getEstoque() const { return estoqueDisponivel; }
double Produto::getPasso() const { return passoVenda; }

bool Produto::deduzirEstoque(double qtd) {
    if (!isfinite(qtd) || qtd <= 0 || qtd > estoqueDisponivel + 0.000001) return false;
    double passos = qtd / passoVenda;
    if (abs(passos - round(passos)) > 0.00001) return false;
    estoqueDisponivel = round((estoqueDisponivel - qtd) * 1000000) / 1000000;
    return true;
}

void Produto::reporEstoque(double qtd) {
    if (!isfinite(qtd) || qtd <= 0) return;
    double passos = qtd / passoVenda;
    if (abs(passos - round(passos)) > 0.00001) return;
    estoqueDisponivel = round((estoqueDisponivel + qtd) * 1000000) / 1000000;
}
