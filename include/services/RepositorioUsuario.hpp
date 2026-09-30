#ifndef REPOSITORIOUSUARIO_HPP
#define REPOSITORIOUSUARIO_HPP

#include <QString>
#include <optional>

// Uma linha da tabela "users"
struct RegistroUsuario {
    QString perfil;       // "comprador" ou "feirante"
    QString nome;
    QString telefone;     // somente dígitos
    QString nomeBanca;    // vazio para compradores
    QString codigoOCS;    // vazio para compradores
    QString hashSenha;
    QString sal;
};

// Única classe que conhece SQL: o resto do programa não sabe como os dados são guardados.
class RepositorioUsuario {
public:
    RepositorioUsuario() = default;
    ~RepositorioUsuario();

    bool abrir();   // abre o arquivo antro.db e cria a tabela se não existir
    bool existe(const QString &telefone);
    bool inserir(const RegistroUsuario &registro);
    std::optional<RegistroUsuario> buscarPorTelefone(const QString &telefone);

    QString ultimoErro() const { return m_ultimoErro; }

private:
    QString m_ultimoErro;
};

#endif // REPOSITORIOUSUARIO_HPP
