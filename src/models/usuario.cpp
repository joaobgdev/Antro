#include "models/usuario.hpp"

#include <cctype>
#include <sstream>
#include <utility>

using namespace std;

Usuario::Usuario(string nome, string telefone)
    : nome(move(nome)), telefone(move(telefone)) {}

string Usuario::getNome() const { return nome; }
string Usuario::getTelefone() const { return telefone; }

string Usuario::validar() const {

    istringstream palavras(nome);
    string palavra;
    int quantidade = 0;
    while (palavras >> palavra) ++quantidade;
    if (quantidade < 2)
        return "Informe o nome completo.";

    int digitos = 0;
    for (unsigned char c : telefone)
        if (isdigit(c)) ++digitos;
    if (digitos < 10 || digitos > 11)
        return "Telefone inválido. Use DDD + número.";

    return "";
}
