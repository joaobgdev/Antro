#include "services/GerenciadorFeira.hpp"
#include <iomanip>

using namespace std;

// Gestão do Catálogo

void GerenciadorFeira::cadastrarProduto(const Produto& p) {
    catalogo.insert({p.getId(), p});
    cout << " [Catálogo] Produto '" << p.getNome() << "' (ID: " << p.getId() << ") cadastrado com sucesso!" << endl;
}

Produto* GerenciadorFeira::buscarProduto(int id) {
    auto it = catalogo.find(id);
    if (it != catalogo.end()) {
        return &(it->second);
    }
    return nullptr;
}

void GerenciadorFeira::exibirCatalogo() const {
    cout << "        CATÁLOGO DE PRODUTOS DA FEIRA     " << endl;
    if (catalogo.empty()) {
        cout << "Nenhum produto cadastrado no catálogo." << endl;
        return;
    }
    for (const auto& pair : catalogo) {
        const Produto& p = pair.second;
        cout << "ID: " << p.getId()
             << " | " << p.getNome()
             << " | R$ " << fixed << setprecision(2) << p.getPreco()
             << (p.getEhPorPeso() ? " /kg" : " /un")
             << " | Estoque: " << p.getEstoque() << endl;
    }

}

// Processamento da Fila de Pedidos

void GerenciadorFeira::receberPedido(Pedido* p) {
    if (!p) return;
    filaProcessamento.push(p);
    p->setStatus(StatusPedido::SOLICITADO);
    cout << " [Fila] Pedido #" << p->getId() << " recebido e adicionado à fila de espera." << endl;
}

void GerenciadorFeira::processarProximoPedido() {
    if (filaProcessamento.empty()) {
        cout << "\n [Fila] Nenhum pedido pendente para processamento!" << endl;
        return;
    }

    Pedido* ped = filaProcessamento.front();
    filaProcessamento.pop();

    cout << " Processando Pedido #" << ped->getId() << " (FIFO)" << endl;


    ped->setStatus(StatusPedido::EM_SEPARACAO);

    // Ajuste dos pesos reais medidos na balança do agricultor 
    for (auto& item : ped->getItens()) {
        if (item.getProduto() && item.getProduto()->getEhPorPeso()) {
            // Exemplo simulação: variação real de peso (+8% da estimativa)
            float pesoAjustado = item.getQtdSolicitada() * 1.08f;
            item.setQtdRealPesa(pesoAjustado);
            cout << " -> " << item.getProduto()->getNome() 
                 << ": Peso ajustado na balança de " << item.getQtdSolicitada() 
                 << "kg para " << fixed << setprecision(2) << pesoAjustado << "kg" << endl;
        }
    }

    ped->setStatus(StatusPedido::AGUARDANDO_PAGAMENTO);
    cout << "\n Sacola montada! Status alterado para AGUARDANDO_PAGAMENTO." << endl;
    ped->imprimirResumo();
}

// Tratamento de No-Show (Cancelamento) 

void GerenciadorFeira::tratarAbandonoPedido(Pedido* p) {
    if (!p) return;

    p->setStatus(StatusPedido::ABANDONADO);
    cout << "\n [No-Show] Pedido #" << p->getId() << " marcado como ABANDONADO." << endl;

    // Reintegrar os itens ao estoque do catálogo (RF05 / US04)
    for (const auto& item : p->getItens()) {
        if (item.getProduto()) {
            Produto* prodNoCatalogo = buscarProduto(item.getProduto()->getId());
            if (prodNoCatalogo) {
                prodNoCatalogo->reporEstoque(item.getQtdSolicitada());
                cout << " -> " << item.getQtdSolicitada() << " unidade(s)/kg de '" 
                     << prodNoCatalogo->getNome() << "' devolvido(s) ao estoque." << endl;
            }
        }
    }
}

// Relatório Consolidado de Colheita

void GerenciadorFeira::gerarRelatorioColheita() const {
    cout << "RELATÓRIO GERAL DE COLHEITA" << endl;

    // std::unordered_map para somar instantaneamente as quantidades em O(1)
    unordered_map<int, float> totaisPorProduto;

    // Cópia da fila para percorrer sem alterar a fila original
    queue<Pedido*> copiaFila = filaProcessamento;

    while (!copiaFila.empty()) {
        Pedido* ped = copiaFila.front();
        copiaFila.pop();

        for (const auto& item : ped->getItens()) {
            if (item.getProduto()) {
                totaisPorProduto[item.getProduto()->getId()] += item.getQtdSolicitada();
            }
        }
    }

    if (totaisPorProduto.empty()) {
        cout << " Nenhuma demanda pendente na fila de colheita." << endl;
    } else {
        for (const auto& pair : totaisPorProduto) {
            int idProd = pair.first;
            float totalQtd = pair.second;

            auto it = catalogo.find(idProd);
            if (it != catalogo.end()) {
                const Produto& p = it->second;
                cout << " • " << p.getNome() << ": Total a colher = " 
                     << fixed << setprecision(2) << totalQtd 
                     << (p.getEhPorPeso() ? " kg" : " un") << endl;
            }
        }
    }
}
