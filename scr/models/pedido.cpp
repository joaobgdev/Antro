#include "models/pedido.hpp"

using namespace std;
// obs.: ainda vai ajeitar o consumidor
Pedido::Pedido(int id, const Consumidor* cliente)
    : id(id), consumidor(cliente), status(StatusPedido::SOLICITADO) {}

int Pedido::getId() const { return id; }
const Consumidor* Pedido::getConsumidor() const { return consumidor; }
StatusPedido Pedido::getStatus() const { return status; }
void Pedido::setStatus(StatusPedido novoStatus) { status = novoStatus; }
vector<ItemPedido>& Pedido::getItens() { return itens; }

bool Pedido::adicionarItem(Produto& prod, float qtd) {
    if (prod.deduzirEstoque(qtd)) {
        itens.push_back(ItemPedido(&prod, qtd));
        cout << " -> Item " << prod.getNome() << " adicionado ao pedido #" << id << endl;
        return true;
    } else {
        cout << " !Falha ao adicionar " << prod.getNome() << ": estoque insuficiente!" << endl;
        return false;
    }
}

float Pedido::calcularValorTotal() const {
    float total = 0.0f;
    for (const auto& item : itens) {
        total += item.calcularSubtotal();
    }
    return total;
}

void Pedido::imprimirResumo() const {
    cout << "\n==========================================" << endl;
    cout << "Resumo do Pedido #" << id;
    if (consumidor) {
        cout << " | Cliente: " << consumidor->getNome();
    }
    cout << endl;

    cout << "Status: ";
    switch (status) {
        case StatusPedido::SOLICITADO: cout << "SOLICITADO"; break;
        case StatusPedido::EM_SEPARACAO: cout << "EM SEPARACAO"; break;
        case StatusPedido::AGUARDANDO_PAGAMENTO: cout << "AGUARDANDO PAGAMENTO"; break;
        case StatusPedido::PRONTO_PARA_RETIRADA: cout << "PRONTO PARA RETIRADA"; break;
        case StatusPedido::RETIRADO: cout << "RETIRADO"; break;
        case StatusPedido::ABANDONADO: cout << "ABANDONADO"; break;
    }
    cout << "\n------------------------------------------" << endl;

    for (const auto& item : itens) {
        cout << "- " << item.getProduto()->getNome()
             << " | Solicitado: " << item.getQtdSolicitada()
             << " | Pesa Real: " << item.getQtdRealPesa()
             << " | Subtotal: R$ " << fixed << setprecision(2)
             << item.calcularSubtotal() << endl;
    }
    cout << "------------------------------------------" << endl;
    cout << "VALOR TOTAL: R$ " << fixed << setprecision(2) << calcularValorTotal() << endl;
    cout << "==========================================" << endl;
}