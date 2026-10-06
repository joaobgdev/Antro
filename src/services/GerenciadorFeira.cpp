#include "services/GerenciadorFeira.hpp"

#include <iomanip>

using namespace std;

// Cadastra ou atualiza um produto no catálogo da feira
// Se o produto já existir (mesmo ID), ele é substituído pelo novo
void GerenciadorFeira::cadastrarProduto(const Produto& p) {
    catalogo.erase(p.getId());       // Remove o produto existente com o mesmo ID, se tiver
    catalogo.emplace(p.getId(), p);  // Insere o novo produto no mapa do catálogo
}

// Busca um produto no catálogo pelo seu ID
// Retorna um ponteiro pro produto encontrado ou nullptr se não existir
Produto* GerenciadorFeira::buscarProduto(int id) {
    auto it = catalogo.find(id);
    if (it == catalogo.end()) return nullptr; // Retorna nulo caso o ID não esteja no catálogo
    return &it->second;                       // Retorna o ponteiro para o produto armazenado
}

// Percorre o catálogo de produtos da feira
void GerenciadorFeira::exibirCatalogo() const {
    if (catalogo.empty()) {
        return;
    }
    
    for (const auto& par : catalogo) {
        const Produto& p = par.second;
    }
}

// Adiciona um pedido recebido à fila de processamento
void GerenciadorFeira::receberPedido(Pedido* p) {
    if (!p) return; // Garante que o ponteiro é válido
    
    filaProcessamento.push(p); // Adiciona o pedido no final da fila
}

// Remove o primeiro pedido da fila de processamento e altera o seu status pra CONFIRMADO
void GerenciadorFeira::processarProximoPedido() {
    // Verifica se há pedidos pendentes na fila
    if (filaProcessamento.empty()) {
        return;
    }

    // Pega o próximo pedido do início da fila e remove ele
    Pedido* p = filaProcessamento.front();
    filaProcessamento.pop();

    p->setStatus(StatusPedido::CONFIRMADO);
}

// Desistência/abandono de um pedido
// Devolve os itens solicitados ao estoque e altera o status do pedido para CANCELADO
void GerenciadorFeira::tratarAbandonoPedido(Pedido* p) {
    if (!p) return;
    
    // Ignora se o pedido já estiver em um estado final (não dá pra cancelar)
    if (p->getStatus() == StatusPedido::RETIRADO ||
        p->getStatus() == StatusPedido::CANCELADO ||
        p->getStatus() == StatusPedido::RECUSADO) {
        return;
    }

    // Repõe no catálogo o estoque de cada item presente no pedido
    for (auto& item : p->getItens()) {
        Produto* prod = buscarProduto(item.getProduto()->getId());
        if (prod) prod->reporEstoque(item.getQtdSolicitada());
    }

    // Atualiza o status final do pedido
    p->setStatus(StatusPedido::CANCELADO);
}

// Percorre todo o catálogo atual de produtos
void GerenciadorFeira::gerarRelatorioColheita() const {
    for (const auto& par : catalogo) {
        const Produto& p = par.second;
    }
}
