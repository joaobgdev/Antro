#ifndef AGRICULTOR_HPP
#define AGRICULTOR_HPP

#include "models/usuario.hpp"

class Agricultor : public Usuario {
private:
    std::string nomeBanca;
    std::string codigoOCS;

public:
    Agricultor(std::string nome, std::string telefone,
               std::string nomeBanca, std::string codigoOCS);

    std::string getNomeBanca() const;
    std::string getCodigoOCS() const;

    std::string getPerfil() const override;
    std::string getSubtitulo() const override;
    std::string validar() const override;
};

#endif
