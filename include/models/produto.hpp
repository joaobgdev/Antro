#ifndef PRODUTO_HPP
#define PRODUTO_HPP

#include <string>

using namespace std;

class Produto {
private:
    int id;
    string nome;
    float precoUnitarioOuKg;
    bool ehPorPeso;
    float estoqueDisponivel;

public:
    Produto(int id, string nome, float preco, bool ehPorPeso, float estoque);

    // Getters
    int getId() const;
    string getNome() const;
    float getPreco() const;
    bool getEhPorPeso() const;
    float getEstoque() const;

    // Métodos de negócio
    bool deduzirEstoque(float qtd);
    void reporEstoque(float qtd);
};

#endif 