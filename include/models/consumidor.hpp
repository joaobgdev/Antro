#ifndef CONSUMIDOR_HPP
#define CONSUMIDOR_HPP

#include "models/usuario.hpp"

using namespace std;

class Consumidor : public Usuario {
public:
    using Usuario::Usuario;

    string getPerfil() const override;
    string getSubtitulo() const override;
};

#endif
