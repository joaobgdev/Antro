#include "models/agricultor.hpp"

#include <utility>

Agricultor::Agricultor(std::string nome, std::string telefone,
                       std::string nomeBanca, std::string codigoOCS)
    : Usuario(std::move(nome), std::move(telefone)),
      nomeBanca(std::move(nomeBanca)), codigoOCS(std::move(codigoOCS)) {}

std::string Agricultor::getNomeBanca() const { return nomeBanca; }
std::string Agricultor::getCodigoOCS() const { return codigoOCS; }

std::string Agricultor::getPerfil() const { return "feirante"; }
std::string Agricultor::getSubtitulo() const { return nomeBanca; }

std::string Agricultor::validar() const {
    const std::string base = Usuario::validar();
    if (!base.empty()) return base;
    if (nomeBanca.empty()) return "Informe o nome da feira/banca.";
    if (codigoOCS.empty()) return "Informe o código OCS.";
    return "";
}
