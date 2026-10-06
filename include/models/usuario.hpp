#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>

using namespace std;

class Usuario {
protected:
    string nome;
    string telefone;

public:
    Usuario(string nome, string telefone);
    virtual ~Usuario() = default;

    string getNome() const;
    string getTelefone() const;

    virtual string getPerfil() const = 0;
    virtual string getSubtitulo() const = 0;

    virtual string validar() const;
};

#endif
