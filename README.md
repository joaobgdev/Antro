# Antro

![Antro](docs/images/hero.png)

Aplicativo desenvolvido pela **Equipe 7**, para a disciplina de Estruturas de Dados e Orientação a Objetos do CIn UFPE. O projeto aproxima pequenos produtores agroecológicos e compradores das feiras do Recife.

## Funcionalidades

- **Comprador:** consulta feiras e produtores, adiciona produtos à sacola, agenda a retirada e acompanha suas reservas.
- **Feirante:** seleciona as feiras em que participa, cadastra produtos, define preços e estoque e gerencia os pedidos recebidos.
- **Contas:** cadastro e login nos dois perfis, com armazenamento local em SQLite.

O pagamento é combinado diretamente entre comprador e produtor, fora do aplicativo.

## Tecnologias

C++17, Qt 6, Qt Quick/QML, Qt SQL, SQLite e CMake.

## Como executar

1. Clone o repositório:

   ```bash
   git clone https://github.com/joaobgdev/Antro.git
   ```

2. Abra o arquivo `CMakeLists.txt` no Qt Creator.
3. Selecione um kit Desktop com Qt 6.5 ou superior e os módulos Qt Quick e Qt SQL.
4. Compile e execute o alvo `appAntro`.

Os dados são salvos localmente no arquivo `antro.db`, criado na pasta de dados do aplicativo. A versão atual não sincroniza dados entre dispositivos.

## Documentação

Consulte a [documentação do projeto em PDF](docs/Documentacao-Antro.pdf) para conhecer os fluxos, a arquitetura, as classes e o histórico do desenvolvimento.

## Equipe 7

- Lucas Medrado Santos
- João Bernardo Gomes Alves de Carvalho
- Maria Luiza de Paula Portela
- Luis Eduardo Lustosa Brito
