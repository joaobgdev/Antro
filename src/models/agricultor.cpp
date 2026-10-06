#include "models/agricultor.hpp"

#include <utility>

using namespace std;

Agricultor::Agricultor(string nome, string telefone,
                       string nomeBanca, string codigoOCS)
    : Usuario(move(nome), move(telefone)),
      nomeBanca(move(nomeBanca)), codigoOCS(move(codigoOCS)) {}

string Agricultor::getNomeBanca() const { return nomeBanca; }
string Agricultor::getCodigoOCS() const { return codigoOCS; }

string Agricultor::getPerfil() const { return "feirante"; }
string Agricultor::getSubtitulo() const { return nomeBanca; }

string Agricultor::validar() const {
    const string base = Usuario::validar();
    if (!base.empty()) return base;
    if (nomeBanca.empty()) return "Informe o nome da feira/banca.";
    if (codigoOCS.empty()) return "Informe o código OCS.";
    return "";
}
