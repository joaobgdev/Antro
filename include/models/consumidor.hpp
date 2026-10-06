#ifndef CONSUMIDOR_HPP
#define CONSUMIDOR_HPP

#include "models/usuario.hpp"

class Consumidor : public Usuario {
public:
    using Usuario::Usuario;

    std::string getPerfil() const override;
    std::string getSubtitulo() const override;
};

#endif
