#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>

class Usuario {
protected:
    std::string nome;
    std::string telefone;

public:
    Usuario(std::string nome, std::string telefone);
    virtual ~Usuario() = default;

    std::string getNome() const;
    std::string getTelefone() const;

    virtual std::string getPerfil() const = 0;
    virtual std::string getSubtitulo() const = 0;

    virtual std::string validar() const;
};

#endif
