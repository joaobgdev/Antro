#include "services/GerenciadorFeira.hpp"

#include <iomanip>

using namespace std;

// Catálogo de Produtos

void GerenciadorFeira::cadastrarProduto(const Produto& p) {
    catalogo.erase(p.getId());
    catalogo.emplace(p.getId(), p);
}

Produto* GerenciadorFeira::buscarProduto(int id) {
    auto it = catalogo.find(id);
    if (it == catalogo.end()) return nullptr;
    return &it->second;
}

void GerenciadorFeira::exibirCatalogo() const {
    cout << "===== CATALOGO DA FEIRA =====" << endl;
    if (catalogo.empty()) {
        cout << "(catalogo vazio)" << endl;
        return;
    }
    for (const auto& par : catalogo) {
        const Produto& p = par.second;
        cout << "[" << p.getId() << "] " << p.getNome()
             << " | R$ " << fixed << setprecision(2) << p.getPreco()
             << (p.getEhPorPeso() ? "/kg" : "/un")
             << " | Estoque: " << p.getEstoque()
             << (p.getEhPorPeso() ? " kg" : " un") << endl;
    }
}

// Processamento de pedidos

void GerenciadorFeira::receberPedido(Pedido* p) {
    if (!p) return;
    filaProcessamento.push(p);
    cout << "Pedido #" << p->getId() << " entrou na fila de processamento." << endl;
}

void GerenciadorFeira::processarProximoPedido() {
    if (filaProcessamento.empty()) {
        cout << "Nenhum pedido na fila." << endl;
        return;
    }

    Pedido* p = filaProcessamento.front();
    filaProcessamento.pop();

    p->setStatus(StatusPedido::EM_SEPARACAO);
    cout << "Processando pedido #" << p->getId() << "..." << endl;

    p->setStatus(StatusPedido::AGUARDANDO_PAGAMENTO);
    cout << "Total do pedido #" << p->getId() << ": R$ "
         << fixed << setprecision(2) << p->calcularValorTotal() << endl;
}

// Pedidos abandonados

void GerenciadorFeira::tratarAbandonoPedido(Pedido* p) {
    if (!p) return;
    if (p->getStatus() == StatusPedido::RETIRADO ||
        p->getStatus() == StatusPedido::ABANDONADO) {
        return;
    }

    for (auto& item : p->getItens()) {
        Produto* prod = buscarProduto(item.getProduto()->getId());
        if (prod) prod->reporEstoque(item.getQtdSolicitada());
    }

    p->setStatus(StatusPedido::ABANDONADO);
    cout << "Pedido #" << p->getId() << " abandonado. Estoque devolvido." << endl;
}

// Relatório

void GerenciadorFeira::gerarRelatorioColheita() const {
    cout << "===== RELATORIO DE COLHEITA =====" << endl;
    for (const auto& par : catalogo) {
        const Produto& p = par.second;
        cout << p.getNome() << ": " << p.getEstoque()
             << (p.getEhPorPeso() ? " kg" : " un") << " em estoque" << endl;
    }
}
