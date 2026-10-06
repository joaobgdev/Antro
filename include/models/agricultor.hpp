#ifndef AGRICULTOR_HPP
#define AGRICULTOR_HPP

#include "models/usuario.hpp"

using namespace std;

class Agricultor : public Usuario {
private:
    string nomeBanca;
    string codigoOCS;

public:
    Agricultor(string nome, string telefone,
               string nomeBanca, string codigoOCS);

    string getNomeBanca() const;
    string getCodigoOCS() const;

    string getPerfil() const override;
    string getSubtitulo() const override;
    string validar() const override;
};

#endif
