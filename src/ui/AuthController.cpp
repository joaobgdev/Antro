#include "ui/AuthController.hpp"

#include <QCryptographicHash>
#include <QRandomGenerator>

#include "models/agricultor.hpp"
#include "models/consumidor.hpp"

namespace {

QString somenteDigitos(const QString &texto)
{
    QString saida;
    for (QChar c : texto)
        if (c.isDigit())
            saida += c;
    return saida;
}

QString gerarSal()
{
    QByteArray bytes(16, 0);
    QRandomGenerator::system()->generate(bytes.begin(), bytes.end());
    return QString::fromLatin1(bytes.toHex());
}

QString calcularHash(const QString &senha, const QString &sal)
{
    const QByteArray dados = sal.toUtf8() + senha.toUtf8();
    QByteArray h = QCryptographicHash::hash(dados, QCryptographicHash::Sha256);
    for (int i = 0; i < 10000; ++i)
        h = QCryptographicHash::hash(h + dados, QCryptographicHash::Sha256);
    return QString::fromLatin1(h.toHex());
}

} // namespace

AuthController::AuthController(QObject *parent) : QObject(parent)
{
    m_bancoPronto = m_repo.abrir();
}

QString AuthController::nomeUsuario() const
{
    return m_usuario ? QString::fromStdString(m_usuario->getNome()) : QString();
}

QString AuthController::perfilUsuario() const
{
    return m_usuario ? QString::fromStdString(m_usuario->getPerfil()) : QString();
}

QString AuthController::subtituloUsuario() const
{
    return m_usuario ? QString::fromStdString(m_usuario->getSubtitulo()) : QString();
}

std::unique_ptr<Usuario> AuthController::criarUsuario(const QString &perfil, const QString &nome,
                                                      const QString &telefone,
                                                      const QString &nomeBanca,
                                                      const QString &codigoOCS)
{
    if (perfil == QLatin1String("feirante"))
        return std::make_unique<Agricultor>(nome.toStdString(), telefone.toStdString(),
                                            nomeBanca.toStdString(), codigoOCS.toStdString());
    return std::make_unique<Consumidor>(nome.toStdString(), telefone.toStdString());
}

bool AuthController::falhar(const QString &mensagem)
{
    emit falhaAutenticacao(mensagem);
    return false;
}

void AuthController::definirUsuario(std::unique_ptr<Usuario> usuario)
{
    m_usuario = std::move(usuario);
    emit usuarioChanged();
}

bool AuthController::cadastrar(const QString &perfil, const QString &nome, const QString &telefone,
                               const QString &senha, const QString &nomeBanca,
                               const QString &codigoOCS)
{
    if (!m_bancoPronto)
        return falhar("Não foi possível abrir o banco de dados: " + m_repo.ultimoErro());

    const QString digitos = somenteDigitos(telefone);
    auto usuario = criarUsuario(perfil, nome.trimmed(), digitos,
                                nomeBanca.trimmed(), codigoOCS.trimmed());

    const std::string erro = usuario->validar();
    if (!erro.empty())
        return falhar(QString::fromStdString(erro));
    if (senha.size() < 6)
        return falhar("A senha deve ter pelo menos 6 caracteres.");
    if (m_repo.existe(digitos))
        return falhar("Este telefone já está cadastrado. Use a aba Entrar.");

    RegistroUsuario registro;
    registro.perfil    = QString::fromStdString(usuario->getPerfil());
    registro.nome      = QString::fromStdString(usuario->getNome());
    registro.telefone  = digitos;
    registro.nomeBanca = nomeBanca.trimmed();
    registro.codigoOCS = codigoOCS.trimmed();
    registro.sal       = gerarSal();
    registro.hashSenha = calcularHash(senha, registro.sal);

    if (!m_repo.inserir(registro))
        return falhar("Erro ao salvar o cadastro: " + m_repo.ultimoErro());

    definirUsuario(std::move(usuario));
    return true;
}

bool AuthController::entrar(const QString &telefone, const QString &senha)
{
    if (!m_bancoPronto)
        return falhar("Não foi possível abrir o banco de dados: " + m_repo.ultimoErro());

    const auto registro = m_repo.buscarPorTelefone(somenteDigitos(telefone));
    if (!registro || calcularHash(senha, registro->sal) != registro->hashSenha)
        return falhar("Telefone ou senha incorretos.");

    definirUsuario(criarUsuario(registro->perfil, registro->nome, registro->telefone,
                                registro->nomeBanca, registro->codigoOCS));
    return true;
}

void AuthController::sair()
{
    if (!m_usuario)
        return;
    m_usuario.reset();
    emit usuarioChanged();
}
