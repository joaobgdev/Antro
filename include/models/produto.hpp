#ifndef PRODUTO_HPP
#define PRODUTO_HPP

#include <string>

using namespace std;

class Produto {
private:
    int id;
    string nome;
    double precoUnitarioOuKg;
    bool ehPorPeso;
    double estoqueDisponivel;
    double passoVenda;

public:
    Produto(int id, string nome, double preco, bool ehPorPeso, double estoque, double passo = 0);

    int getId() const;
    string getNome() const;
    double getPreco() const;
    bool getEhPorPeso() const;
    double getPasso() const;
    double getEstoque() const;

    bool deduzirEstoque(double qtd);
    void reporEstoque(double qtd);
};

#endif
