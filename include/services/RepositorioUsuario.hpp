#ifndef REPOSITORIOUSUARIO_HPP
#define REPOSITORIOUSUARIO_HPP

#include <QString>
#include <optional>

using namespace std;

struct RegistroUsuario {
    QString perfil;
    QString nome;
    QString telefone;
    QString nomeBanca;
    QString codigoOCS;
    QString hashSenha;
    QString sal;
};

class RepositorioUsuario {
public:
    RepositorioUsuario() = default;
    ~RepositorioUsuario();

    bool abrir();
    bool existe(const QString &telefone);
    bool inserir(const RegistroUsuario &registro);
    optional<RegistroUsuario> buscarPorTelefone(const QString &telefone);

    QString ultimoErro() const { return m_ultimoErro; }

private:
    QString m_ultimoErro;
};

#endif
