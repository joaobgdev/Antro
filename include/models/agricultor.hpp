#ifndef AGRICULTOR_HPP
#define AGRICULTOR_HPP

#include "models/usuario.hpp"

using namespace std;
// diferente do cadastro de usuario, o agricultor precisa registra o codigo da ocs e o nome da banca
//o codigo da ocs permite que a agricultura familiar venda produtos orgânicos sem a necessidade de um selo tradicional de certificação
class Agricultor : public Usuario {
private:
    string nomeBanca;
    string codigoOCS;

public:
    Agricultor(string nome, string telefone,
               string nomeBanca, string codigoOCS);

// Acesso aos dados específicos do agricultor

    string getNomeBanca() const;
    string getCodigoOCS() const;
//comportamentos sobreescritos da classe usuario
    string getPerfil() const override;
    string getSubtitulo() const override;
    string validar() const override;
};

#endif
