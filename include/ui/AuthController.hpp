#ifndef AUTHCONTROLLER_HPP
#define AUTHCONTROLLER_HPP

#include <QObject>
#include <QString>
#include <QtQmlIntegration>
#include <memory>

#include "models/usuario.hpp"
#include "services/RepositorioUsuario.hpp"

// Singleton acessível no QML como "AuthController".
// Só esta classe conhece Qt; as classes de domínio (Usuario...) continuam C++ puro.
class AuthController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool logado READ logado NOTIFY usuarioChanged)
    Q_PROPERTY(QString nomeUsuario READ nomeUsuario NOTIFY usuarioChanged)
    Q_PROPERTY(QString perfilUsuario READ perfilUsuario NOTIFY usuarioChanged)
    Q_PROPERTY(QString subtituloUsuario READ subtituloUsuario NOTIFY usuarioChanged)
    Q_PROPERTY(QString telefoneUsuario READ telefoneUsuario NOTIFY usuarioChanged
)

public:
    explicit AuthController(QObject *parent = nullptr);

    bool logado() const { return m_usuario != nullptr; }
    QString nomeUsuario() const;
    QString perfilUsuario() const;
    QString subtituloUsuario() const;
    QString telefoneUsuario() const;

    Q_INVOKABLE bool cadastrar(const QString &perfil, const QString &nome, const QString &telefone,
                               const QString &senha, const QString &nomeBanca = {},
                               const QString &codigoOCS = {});
    Q_INVOKABLE bool entrar(const QString &telefone, const QString &senha);
    Q_INVOKABLE void sair();

signals:
    void usuarioChanged();
    void falhaAutenticacao(const QString &mensagem);   // erros de cadastro e de login

private:
    bool falhar(const QString &mensagem);
    void definirUsuario(std::unique_ptr<Usuario> usuario);
    static std::unique_ptr<Usuario> criarUsuario(const QString &perfil, const QString &nome,
                                                 const QString &telefone, const QString &nomeBanca,
                                                 const QString &codigoOCS);

    std::unique_ptr<Usuario> m_usuario;
    RepositorioUsuario m_repo;
    bool m_bancoPronto = false;
};

#endif // AUTHCONTROLLER_HPP
