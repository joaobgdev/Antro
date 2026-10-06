#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>

using namespace std;

//classe base com os dados e comportamentos comuns do agricultor e do consumidor
class Usuario {
//protected permite que as classes que herdam de usuario acessem o nome e o telefone
protected:
    string nome;
    string telefone;

public:
//cria um usuario com seus dados pessoais
    Usuario(string nome, string telefone);
//destrutor virtual permite destruir a classe derivada corretamente pelo ponteiro de usuario
    virtual ~Usuario() = default;

//acesso ao nome e ao telefone do usuario
    string getNome() const;
    string getTelefone() const;

//cada classe que herda de usuario precisa definir seu perfil e subtitulo
//o = 0 torna a classe abstrata, então não pode criar um usuario diretamente
    virtual string getPerfil() const = 0;
    virtual string getSubtitulo() const = 0;

//confere se o nome tem pelo menos duas palavras e o telefone tem 10 ou 11 digitos
//retorna uma mensagem de erro ou uma string vazia se os dados forem validos
    virtual string validar() const;
};

#endif
