#ifndef PRODUTO_HPP
#define PRODUTO_HPP

#include <string>

using namespace std;

//representa um produto que pode ser vendido por unidade ou por peso
class Produto {
//guarda o id, o nome, o preco e a quantidade disponivel do produto
//ehPorPeso indica se a venda é por peso, e passoVenda define de quanto em quanto pode vender
private:
    int id;
    string nome;
    double precoUnitarioOuKg;
    bool ehPorPeso;
    double estoqueDisponivel;
    double passoVenda;

public:
//cria o produto com seus dados, usando passo de 0.5 kg por padrao para peso e 1 para unidade
    Produto(int id, string nome, double preco, bool ehPorPeso, double estoque, double passo = 0);

//acesso aos dados do produto, ao passo de venda e ao estoque disponivel
    int getId() const;
    string getNome() const;
    double getPreco() const;
    bool getEhPorPeso() const;
    double getPasso() const;
    double getEstoque() const;

//desconta uma quantidade valida do estoque, respeitando o passo de venda e o estoque disponivel
//retorna true se conseguiu descontar e false se a quantidade não for aceita
    bool deduzirEstoque(double qtd);
//devolve uma quantidade ao estoque se ela for positiva e respeitar o passo de venda
    void reporEstoque(double qtd);
};

#endif
