#include "services/CatalogoComprador.hpp"
#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>

static DadosCatalogo dadosDeExemplo()
{
    DadosCatalogo d;
    d.feiras = {{1, "Várzea", "Várzea", "Praça", "Sáb"}, {2, "Graças", "Graças", "Rua", "Sáb"},
                {3, "Casa Forte", "Casa Forte", "Praça", "Sáb"}, {4, "Trindade", "Casa Amarela", "Sítio", "Sáb"}};
    d.vendedores = {{1, "Ana", "Boa Vista", ""}, {2, "João", "Riacho", ""}, {3, "Rosa", "Quintal", ""}};
    d.produtos = {Produto(101, "Alface", 3.5f, false, 20), Produto(102, "Tomate", 12.0f, true, 10),
                  Produto(103, "Banana", 8.5f, true, 15), Produto(104, "Macaxeira", 7.0f, true, 12),
                  Produto(105, "Coentro", 3.0f, false, 30), Produto(106, "Abóbora", 6.0f, true, 10)};
    d.ofertas = {{1,1,101}, {1,1,102}, {2,1,101}, {2,1,102}, {1,2,103}, {1,2,104},
                 {3,2,103}, {3,2,104}, {2,3,105}, {2,3,106}, {3,3,105}, {3,3,106}};
    return d;
}

int main()
{
    CatalogoComprador catalogo;
    assert(catalogo.getFeiras().empty());
    catalogo.definirDados(dadosDeExemplo());
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

    assert(catalogo.adicionar(1, 1, 101, 2));
    assert(catalogo.alterarQuantidade(0, 5));
    assert(catalogo.getSacola()[0].quantidade == 5);
    assert(!catalogo.alterarQuantidade(0, 0));
    assert(!catalogo.alterarQuantidade(0, 21));
    assert(!catalogo.alterarQuantidade(0, 1.5));
    assert(!catalogo.alterarQuantidade(5, 1));
    assert(catalogo.adicionar(1, 1, 102, 1));
    assert(catalogo.alterarQuantidade(1, 1.5));
    assert(!catalogo.alterarQuantidade(1, 1.25));

    catalogo.limpar();
    Produto produto(200, "Gengibre", 30, true, 0.3, 0.1);
    assert(!produto.deduzirEstoque(-0.1));
    assert(!produto.deduzirEstoque(0.05));
    assert(produto.deduzirEstoque(0.1 + 0.2));
    assert(produto.getEstoque() == 0);
    produto.reporEstoque(-1);
    assert(produto.getEstoque() == 0);
    produto.reporEstoque(0.3);
    assert(std::abs(produto.getEstoque() - 0.3) < 0.000001);
    DadosCatalogo porGrama = dadosDeExemplo();
    porGrama.produtos.push_back(produto);
    porGrama.ofertas.push_back({1,1,200});
    catalogo.definirDados(porGrama);
    assert(catalogo.adicionar(1,1,200,0.1));
    assert(catalogo.adicionar(1,1,200,0.2));
    assert(!catalogo.adicionar(1,1,200,0.1));
    catalogo.limpar();

    assert(catalogo.adicionar(1, 1, 101, 2));
    assert(catalogo.adicionar(2, 1, 101, 3));
    DadosCatalogo novos = dadosDeExemplo();
    novos.ofertas.erase(novos.ofertas.begin() + 2);
    catalogo.definirDados(novos);
    assert(catalogo.getSacola().size() == 1);
    assert(catalogo.getSacola()[0].feiraId == 1);
    std::cout << "Testes do catalogo, do carrinho e da reserva passaram.\n";
}
