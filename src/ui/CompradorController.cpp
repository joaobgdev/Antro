#include "ui/CompradorController.hpp"
#include "ui/AuthController.hpp"
#include "services/AgendaFeira.hpp"
#include <algorithm>
#include <cmath>

CompradorController::CompradorController(QObject *parent) : QObject(parent)
{
    m_bancoPronto = m_repo.abrir();
    if (m_bancoPronto) m_catalogo.definirDados(m_repo.carregar());
    else falhar(m_repo.ultimoErro());
}

void CompradorController::definirAutenticacao(AuthController *auth)
{
    m_auth = auth;
    connect(auth, &AuthController::usuarioChanged, this, [this]() {
        limpar();
        m_erro.clear(); emit erroChanged();
        recarregar();
    });
}

bool CompradorController::falhar(const QString &texto)
{
    m_erro = texto;
    emit erroChanged();
    return false;
}

QString CompradorController::erro() const { return m_erro; }

bool CompradorController::autorizado()
{
    if (!m_bancoPronto) return falhar("Não foi possível abrir o catálogo: " + m_repo.ultimoErro());
    if (!m_auth || m_auth->perfilUsuario() != "comprador") return falhar("Entre com um perfil de comprador.");
    return true;
}

void CompradorController::recarregar()
{
    if (!m_bancoPronto) return;
    DadosCatalogo dados = m_repo.carregar();
    if (!m_repo.ultimoErro().isEmpty()) { falhar(m_repo.ultimoErro()); return; }
    size_t antes = m_catalogo.getSacola().size();
    double totalAntes = m_catalogo.totalEstimado();
    m_catalogo.definirDados(dados);
    if (antes != m_catalogo.getSacola().size()) falhar("Alguns itens saíram do carrinho porque a oferta ou o estoque mudou.");
    else if (std::abs(totalAntes - m_catalogo.totalEstimado()) > 0.000001) falhar("Os preços mudaram. Confira o carrinho antes de solicitar a reserva.");
    podarAgendamentos();
    emit produtosChanged(); emit sacolaChanged(); emit reservasChanged();
}

QVariantMap CompradorController::feira(int id) const
{
    const FeiraComprador *f = m_catalogo.buscarFeira(id);
    if (!f) return {};
    bool aberta = AgendaFeira::aberta(*f);
    QStringList datas = AgendaFeira::datas(*f);
    QString proxima = datas.isEmpty() ? "Sem data disponível" : QDate::fromString(datas.first(), Qt::ISODate).toString("dd/MM");
    return {{"id", f->id}, {"nome", QString::fromStdString(f->nome)}, {"bairro", QString::fromStdString(f->bairro)},
        {"local", QString::fromStdString(f->local)}, {"horario", QString::fromStdString(f->horario)},
        {"aberta", aberta}, {"situacao", aberta ? "Acontecendo agora" : "Próxima feira: " + proxima},
        {"vendedores", static_cast<int>(m_catalogo.vendedoresDaFeira(id).size())}};
}

QVariantList CompradorController::feiras() const
{
    std::vector<FeiraComprador> feiras = m_catalogo.getFeiras();
    std::stable_sort(feiras.begin(), feiras.end(), [](const FeiraComprador &a, const FeiraComprador &b) {
        bool abertaA = AgendaFeira::aberta(a), abertaB = AgendaFeira::aberta(b);
        if (abertaA != abertaB) return abertaA;
        QStringList datasA = AgendaFeira::datas(a), datasB = AgendaFeira::datas(b);
        QString dataA = datasA.isEmpty() ? "9999-12-31" : datasA.first();
        QString dataB = datasB.isEmpty() ? "9999-12-31" : datasB.first();
        if (dataA != dataB) return dataA < dataB;
        return a.inicio < b.inicio;
    });
    QVariantList lista;
    for (const FeiraComprador &f : feiras) lista.append(feira(f.id));
    return lista;
}

QVariantMap CompradorController::vendedor(int id) const
{
    const VendedorComprador *v = m_catalogo.buscarVendedor(id);
    if (!v) return {};
    return {{"id", v->id}, {"nome", QString::fromStdString(v->nome)}, {"banca", QString::fromStdString(v->banca)},
        {"descricao", QString::fromStdString(v->descricao)}, {"exemplo", v->exemplo}};
}

QVariantList CompradorController::vendedores(int feiraId) const
{
    QVariantList lista;
    for (const VendedorComprador &v : m_catalogo.vendedoresDaFeira(feiraId)) lista.append(vendedor(v.id));
    return lista;
}

QVariantList CompradorController::produtos(int feiraId, int vendedorId) const
{
    QVariantList lista;
    for (const Produto &p : m_catalogo.produtosDoVendedor(feiraId, vendedorId)) {
        bool por100g = p.getEhPorPeso() && p.getPasso() < 0.5;
        lista.append(QVariantMap{{"id", p.getId()}, {"nome", QString::fromStdString(p.getNome())},
            {"preco", p.getPreco() / (por100g ? 10 : 1)}, {"unidadePreco", por100g ? "100g" : p.getEhPorPeso() ? "kg" : "unidade"},
            {"unidade", p.getEhPorPeso() ? "kg" : "unidade"}, {"passo", p.getPasso()},
            {"disponivel", std::max(0.0, p.getEstoque() - m_catalogo.quantidadeNaSacola(p.getId()))}});
    }
    return lista;
}

QVariantList CompradorController::sacola() const
{
    QVariantList lista;
    const std::vector<ItemSacolaComprador> &itens = m_catalogo.getSacola();
    for (size_t i = 0; i < itens.size(); ++i) {
        const ItemSacolaComprador &item = itens[i];
        const Produto *p = m_catalogo.buscarProduto(item.produtoId);
        if (!p) continue;
        double maximo = p->getEstoque() - m_catalogo.quantidadeNaSacola(item.produtoId) + item.quantidade;
        lista.append(QVariantMap{{"indice", static_cast<int>(i)}, {"nome", QString::fromStdString(p->getNome())},
            {"feira", feira(item.feiraId).value("nome")}, {"vendedor", vendedor(item.vendedorId).value("banca")},
            {"unidade", p->getEhPorPeso() ? "kg" : "unidade"}, {"preco", p->getPreco()},
            {"passo", p->getPasso()}, {"podeAumentar", item.quantidade + p->getPasso() <= maximo + 0.000001},
            {"quantidade", item.quantidade}, {"subtotal", std::round(p->getPreco() * item.quantidade * 100) / 100}});
    }
    return lista;
}

bool CompradorController::adicionar(int feiraId, int vendedorId, int produtoId, double quantidade)
{
    if (!autorizado()) return false;
    if (vendedor(vendedorId).value("exemplo").toBool()) return falhar("Esse cadastro antigo é apenas um exemplo e não recebe reservas.");
    if (!m_catalogo.adicionar(feiraId, vendedorId, produtoId, quantidade)) return falhar("Confira a quantidade disponível desse produto.");
    m_erro.clear(); emit erroChanged(); emit sacolaChanged(); emit retiradasChanged();
    return true;
}

bool CompradorController::alterarQuantidade(int indice, double quantidade)
{
    if (!autorizado()) return false;
    if (!m_catalogo.alterarQuantidade(indice, quantidade)) return falhar("Não foi possível alterar a quantidade.");
    m_erro.clear(); emit erroChanged(); emit sacolaChanged();
    return true;
}

void CompradorController::podarAgendamentos()
{
    for (int i = m_agendamentos.size() - 1; i >= 0; --i) {
        bool naSacola = false;
        for (const ItemSacolaComprador &item : m_catalogo.getSacola())
            if (item.feiraId == m_agendamentos[i].feiraId) naSacola = true;
        const FeiraComprador *f = m_catalogo.buscarFeira(m_agendamentos[i].feiraId);
        if (!naSacola || !f || !AgendaFeira::validar(*f, m_agendamentos[i])) m_agendamentos.removeAt(i);
    }
    emit retiradasChanged();
}

void CompradorController::remover(int indice)
{
    if (!autorizado()) return;
    m_catalogo.remover(indice); podarAgendamentos(); emit sacolaChanged();
}

void CompradorController::limpar()
{
    m_catalogo.limpar(); m_agendamentos.clear(); emit sacolaChanged(); emit retiradasChanged();
}

QVariantList CompradorController::datasRetirada(int feiraId) const
{
    QVariantList lista;
    const FeiraComprador *f = m_catalogo.buscarFeira(feiraId);
    if (!f) return lista;
    for (const QString &data : AgendaFeira::datas(*f))
        lista.append(QVariantMap{{"valor", data}, {"texto", QDate::fromString(data, Qt::ISODate).toString("dd/MM/yyyy")}});
    return lista;
}

QVariantList CompradorController::janelasRetirada(int feiraId, const QString &data) const
{
    QVariantList lista;
    const FeiraComprador *f = m_catalogo.buscarFeira(feiraId);
    if (!f) return lista;
    for (const AgendamentoReserva &a : AgendaFeira::janelas(*f, data))
        lista.append(QVariantMap{{"inicio", a.inicio}, {"fim", a.fim}, {"texto", a.inicio + " às " + a.fim}});
    return lista;
}

bool CompradorController::agendar(int feiraId, const QString &data, const QString &inicio, const QString &fim)
{
    if (!autorizado()) return false;
    const FeiraComprador *f = m_catalogo.buscarFeira(feiraId);
    AgendamentoReserva retirada{feiraId, data, inicio, fim};
    if (!f || !AgendaFeira::validar(*f, retirada)) return falhar("Esse horário não está disponível. Escolha outra opção.");
    bool existe = false;
    for (AgendamentoReserva &a : m_agendamentos) if (a.feiraId == feiraId) { a = retirada; existe = true; break; }
    if (!existe) m_agendamentos.append(retirada);
    m_erro.clear(); emit erroChanged(); emit retiradasChanged();
    return true;
}

QVariantList CompradorController::retiradas() const
{
    QVariantList lista;
    QVector<int> feiras;
    for (const ItemSacolaComprador &item : m_catalogo.getSacola()) {
        if (feiras.contains(item.feiraId)) continue;
        feiras.append(item.feiraId);
        AgendamentoReserva escolhida;
        for (const AgendamentoReserva &a : m_agendamentos) if (a.feiraId == item.feiraId) escolhida = a;
        lista.append(QVariantMap{{"feiraId", item.feiraId}, {"feira", feira(item.feiraId).value("nome")},
            {"data", escolhida.data}, {"inicio", escolhida.inicio}, {"fim", escolhida.fim}});
    }
    return lista;
}

QVariantMap CompradorController::finalizarReserva()
{
    if (!autorizado()) return {{"ok", false}, {"erro", m_erro}};
    double total = m_catalogo.totalEstimado();
    QVector<int> ids = m_repo.salvarReservas(m_auth->telefoneUsuario(), m_catalogo.getSacola(), m_agendamentos, m_catalogo.dados());
    if (ids.isEmpty()) {
        QString erro = m_repo.ultimoErro();
        recarregar(); falhar(erro);
        return {{"ok", false}, {"erro", erro}};
    }
    QVariantList reservas;
    for (const RegistroReserva &r : m_repo.reservasDoUsuario(m_auth->telefoneUsuario(), false))
        if (ids.contains(r.id)) reservas.append(r.comoMapa());
    limpar(); recarregar();
    m_erro.clear(); emit erroChanged();
    return {{"ok", true}, {"reservas", reservas}, {"total", total}};
}

QVariantList CompradorController::reservas()
{
    QVariantList lista;
    if (!autorizado()) return lista;
    for (const RegistroReserva &r : m_repo.reservasDoUsuario(m_auth->telefoneUsuario(), false)) lista.append(r.comoMapa());
    if (!m_repo.ultimoErro().isEmpty()) falhar(m_repo.ultimoErro());
    return lista;
}

bool CompradorController::cancelarReserva(int id)
{
    if (!autorizado()) return false;
    if (!m_repo.alterarReserva(m_auth->telefoneUsuario(), false, id, "CANCELADA")) {
        emit reservasChanged(); return falhar(m_repo.ultimoErro());
    }
    m_erro.clear(); emit erroChanged(); recarregar();
    return true;
}

int CompradorController::tiposNaSacola() const { return static_cast<int>(m_catalogo.getSacola().size()); }
double CompradorController::totalEstimado() const { return m_catalogo.totalEstimado(); }
