#ifndef CONSUMIDOR_HPP
#define CONSUMIDOR_HPP

#include "models/usuario.hpp"

using namespace std;

class Consumidor : public Usuario {
public:

 // Cria um usuario com seus dados pessoais
    using Usuario::Usuario;

//comportamentos sobreescritos da classe usuario
    string getPerfil() const override;
    string getSubtitulo() const override;
};

#endif
