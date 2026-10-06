#include "ui/VendedorController.hpp"
#include "ui/AuthController.hpp"
#include <cmath>

using namespace std;

VendedorController::VendedorController(QObject *parent) : QObject(parent)
{
    m_bancoPronto = m_repo.abrir();
    if (!m_bancoPronto) falhar(m_repo.ultimoErro());
}

void VendedorController::definirAutenticacao(AuthController *auth)
{
    m_auth = auth;
    connect(auth, &AuthController::usuarioChanged, this, [this]() {
        m_feiras.clear(); m_produtos.clear(); m_anteriores.clear(); m_pendentes.clear(); limparErro();
        if (m_auth->perfilUsuario() == "feirante") carregarPerfil();
        emit perfilChanged(); emit pedidosChanged();
    });
}

bool VendedorController::falhar(const QString &texto)
{
    m_erro = texto;
    emit erroChanged();
    return false;
}

void VendedorController::limparErro() { m_erro.clear(); emit erroChanged(); }
QString VendedorController::erro() const { return m_erro; }
QStringList VendedorController::feirasPendentes() const { return m_pendentes; }

bool VendedorController::autorizado()
{
    if (!m_bancoPronto) return falhar("Não foi possível abrir o catálogo: " + m_repo.ultimoErro());
    if (!m_auth || m_auth->perfilUsuario() != "feirante") return falhar("Entre com um perfil de feirante.");
    return true;
}

bool VendedorController::carregarPerfil()
{
    if (!autorizado()) return false;
    limparErro();
    m_feiras = m_repo.feirasDoFeirante(m_auth->telefoneUsuario());
    m_produtos = m_repo.produtosDoFeirante(m_auth->telefoneUsuario());
    m_anteriores = m_produtos;
    m_pendentes = m_repo.feirasPendentes(m_auth->telefoneUsuario());
    if (!m_repo.ultimoErro().isEmpty()) return falhar(m_repo.ultimoErro());
    emit perfilChanged();
    return true;
}

QVariantList VendedorController::feiras() const
{
    QVariantList lista;
    for (int id : m_feiras) lista.append(id);
    return lista;
}

QVariantList VendedorController::produtos() const
{
    QVariantList lista;
    for (const RegistroProdutoFeirante &p : m_produtos)
        lista.append(QVariantMap{{"id", p.id}, {"nome", p.nome}, {"tipoVenda", p.tipoVenda},
            {"preco", p.preco}, {"estoque", p.estoque}, {"reservado", p.reservado}, {"ativo", p.ativo}});
    return lista;
}

bool VendedorController::alternarFeira(int id)
{
    if (!autorizado()) return false;
    bool existe = false;
    for (const FeiraComprador &f : m_repo.carregar().feiras) if (f.id == id) existe = true;
    if (!existe) return falhar("Feira não encontrada.");
    if (m_feiras.contains(id)) m_feiras.removeAll(id);
    else m_feiras.append(id);
    limparErro(); emit perfilChanged();
    return true;
}

bool VendedorController::participarFeira(int id)
{
    if (!carregarPerfil()) return false;
    if (!m_feiras.contains(id) && !alternarFeira(id)) return false;
    return salvarPerfil();
}

bool VendedorController::adicionarProduto(const QString &nome)
{
    if (!autorizado()) return false;
    QString texto = nome.trimmed();
    if (texto.isEmpty()) return falhar("Digite o nome do produto.");
    for (const RegistroProdutoFeirante &p : m_produtos)
        if (p.nome.compare(texto, Qt::CaseInsensitive) == 0) return falhar("Esse produto já está na lista.");
    RegistroProdutoFeirante produto;
    produto.nome = texto; produto.tipoVenda = "unidade";
    m_produtos.append(produto);
    limparErro(); emit perfilChanged();
    return true;
}

bool VendedorController::removerProduto(int indice)
{
    if (!autorizado() || indice < 0 || indice >= m_produtos.size()) return false;
    m_produtos.removeAt(indice);
    limparErro(); emit perfilChanged();
    return true;
}

bool VendedorController::definirTipoVenda(int indice, const QString &tipo)
{
    if (!autorizado() || indice < 0 || indice >= m_produtos.size()) return false;
    if (tipo != "unidade" && tipo != "kg" && tipo != "100g") return falhar("Unidade inválida.");
    RegistroProdutoFeirante &p = m_produtos[indice];
    if (p.tipoVenda == tipo) return true;
    if (p.reservado > 0) return falhar("A unidade não pode mudar enquanto houver reservas desse produto.");
    if (p.tipoVenda == "100g") p.preco *= 10;
    if (tipo == "100g") p.preco /= 10;
    p.tipoVenda = tipo;
    limparErro(); emit perfilChanged();
    return true;
}

bool VendedorController::definirPreco(int indice, const QString &texto)
{
    if (!autorizado() || indice < 0 || indice >= m_produtos.size()) return false;
    bool ok;
    QString numero = texto.trimmed();
    if (numero.contains(',')) numero.remove('.');
    double valor = numero.replace(',', '.').toDouble(&ok);
    if (!ok || !isfinite(valor) || valor <= 0) return falhar("Informe um preço maior que zero.");
    valor = round(valor * 100) / 100;
    if (valor <= 0) return falhar("O preço mínimo é R$ 0,01.");
    m_produtos[indice].preco = valor;
    limparErro();
    return true;
}

bool VendedorController::definirEstoque(int indice, const QString &texto)
{
    if (!autorizado() || indice < 0 || indice >= m_produtos.size()) return false;
    bool ok;
    QString numero = texto.trimmed();
    if (numero.contains(',')) numero.remove('.');
    double valor = numero.replace(',', '.').toDouble(&ok);
    double passo = m_produtos[indice].tipoVenda == "100g" ? 0.1 : m_produtos[indice].tipoVenda == "kg" ? 0.5 : 1.0;
    if (!ok || !isfinite(valor) || valor < 0 || abs(valor / passo - round(valor / passo)) > 0.00001)
        return falhar("O estoque deve ser zero ou um múltiplo de " + QString::number(passo) + ".");
    m_produtos[indice].estoque = valor;
    limparErro();
    return true;
}

bool VendedorController::salvarPerfil()
{
    if (!autorizado()) return false;
    if (!m_repo.salvarPerfil(m_auth->telefoneUsuario(), m_feiras, m_produtos, m_anteriores)) return falhar(m_repo.ultimoErro());
    carregarPerfil(); emit catalogoChanged();
    return true;
}

bool VendedorController::definirAtivo(int indice, bool ativo)
{
    if (!autorizado() || indice < 0 || indice >= m_produtos.size()) return false;
    m_produtos[indice].ativo = ativo;
    limparErro();
    return true;
}

QVariantList VendedorController::pedidos()
{
    QVariantList lista;
    if (!autorizado()) return lista;
    for (const RegistroReserva &r : m_repo.reservasDoUsuario(m_auth->telefoneUsuario(), true)) lista.append(r.comoMapa());
    if (!m_repo.ultimoErro().isEmpty()) falhar(m_repo.ultimoErro());
    return lista;
}

bool VendedorController::alterarPedido(int id, const QString &status)
{
    if (!autorizado()) return false;
    if (!m_repo.alterarReserva(m_auth->telefoneUsuario(), true, id, status)) {
        emit pedidosChanged();
        return falhar(m_repo.ultimoErro());
    }
    limparErro(); emit pedidosChanged(); emit catalogoChanged();
    return true;
}

void VendedorController::atualizarPedidos() { emit pedidosChanged(); }
