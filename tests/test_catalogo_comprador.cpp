#include "services/CatalogoComprador.hpp"
#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>

int main()
{
    CatalogoComprador catalogo;
    assert(catalogo.getFeiras().size() == 4);
    assert(catalogo.vendedoresDaFeira(1).size() == 2);
    assert(catalogo.vendedoresDaFeira(4).empty());
    assert(catalogo.vendedoresDaFeira(-1).empty());
    assert(catalogo.produtosDoVendedor(1, 1).size() == 2);
    assert(catalogo.produtosDoVendedor(1, 3).empty());
    assert(!catalogo.adicionar(1, 3, 101, 1));
    assert(!catalogo.adicionar(3, 1, 101, 1));
    assert(!catalogo.adicionar(1, 1, 999, 1));
    assert(!catalogo.adicionar(1, 1, 101, 0));
    assert(!catalogo.adicionar(1, 1, 101, -1));
    assert(!catalogo.adicionar(1, 1, 101, 0.5));
    assert(!catalogo.adicionar(1, 1, 102, 0.25));
    assert(!catalogo.adicionar(1, 1, 102, std::numeric_limits<double>::quiet_NaN()));
    assert(!catalogo.adicionar(1, 1, 102, std::numeric_limits<double>::infinity()));
    assert(catalogo.adicionar(1, 1, 101, 2));
    assert(catalogo.adicionar(1, 1, 101, 3));
    assert(catalogo.getSacola().size() == 1);
    assert(catalogo.getSacola()[0].quantidade == 5);
    assert(catalogo.adicionar(2, 1, 101, 15));
    assert(catalogo.getSacola().size() == 2);
    assert(!catalogo.adicionar(1, 1, 101, 1));
    catalogo.remover(1);
    assert(catalogo.adicionar(1, 1, 102, 0.5));
    assert(std::abs(catalogo.totalEstimado() - 23.5) < 0.001);
    catalogo.remover(-1);
    catalogo.remover(100);
    assert(catalogo.getSacola().size() == 2);
    catalogo.remover(0);
    assert(catalogo.quantidadeNaSacola(101) == 0);
    assert(catalogo.buscarProduto(101)->getEstoque() == 20);
    catalogo.limpar();
    assert(catalogo.getSacola().empty());
    assert(catalogo.totalEstimado() == 0);
    std::cout << "Testes do catalogo e da sacola passaram.\n";
}
