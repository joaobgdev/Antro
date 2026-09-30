#include "models/usuario.hpp"

#include <cctype>
#include <sstream>
#include <utility>

Usuario::Usuario(std::string nome, std::string telefone)
    : nome(std::move(nome)), telefone(std::move(telefone)) {}

std::string Usuario::getNome() const { return nome; }
std::string Usuario::getTelefone() const { return telefone; }

std::string Usuario::validar() const {
    // Nome completo: pelo menos duas palavras
    std::istringstream palavras(nome);
    std::string palavra;
    int quantidade = 0;
    while (palavras >> palavra) ++quantidade;
    if (quantidade < 2)
        return "Informe o nome completo.";

    // Telefone: DDD + número = 10 (fixo) ou 11 (celular) dígitos
    int digitos = 0;
    for (unsigned char c : telefone)
        if (std::isdigit(c)) ++digitos;
    if (digitos < 10 || digitos > 11)
        return "Telefone inválido. Use DDD + número.";

    return "";
}
