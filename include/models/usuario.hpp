#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>

// Classe base abstrata (herança): Consumidor e Agricultor derivam desta.
class Usuario {
protected:
    std::string nome;
    std::string telefone;   // identificador único (somente dígitos)

public:
    Usuario(std::string nome, std::string telefone);
    virtual ~Usuario() = default;

    std::string getNome() const;
    std::string getTelefone() const;

    // Polimorfismo: cada subclasse responde do seu jeito
    virtual std::string getPerfil() const = 0;      // "comprador" ou "feirante"
    virtual std::string getSubtitulo() const = 0;   // texto de apoio exibido na Home

    // Devolve a mensagem de erro, ou string vazia se estiver tudo válido
    virtual std::string validar() const;
};

#endif // USUARIO_HPP
