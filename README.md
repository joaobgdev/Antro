# Antro

Aplicativo para aproximar pequenos produtores agroecológicos e compradores das feiras do Recife. O Antro permite divulgar produtos, consultar a disponibilidade e reservar itens com data e horário para retirada na feira.

Projeto acadêmico desenvolvido para a disciplina de Estruturas de Dados e Orientação a Objetos do CIn UFPE.

## Como funciona

O sistema possui dois perfis: **Comprador** e **Feirante**. As feiras são previamente cadastradas na base de dados, e os feirantes escolhem em quais delas desejam disponibilizar seus produtos.

O comprador consulta as feiras e as bancas, escolhe os produtos e adiciona as quantidades desejadas à sacola. Para concluir a reserva, seleciona uma data e um horário de retirada. O feirante recebe a solicitação e pode aceitá-la ou recusá-la. O comprador acompanha a situação pela área de reservas e pode cancelar conforme o estado do pedido.

**O pagamento acontece diretamente entre comprador e produtor, fora do aplicativo.** O valor apresentado é uma estimativa dos produtos selecionados.

## Funcionalidades

### Comprador

- Cadastro, login e saída da conta.
- Consulta de feiras, produtores e produtos disponíveis.
- Seleção de quantidades por unidade ou peso.
- Sacola com cálculo do valor estimado.
- Agendamento da retirada dos produtos.
- Consulta e cancelamento de reservas.

### Feirante

- Cadastro com nome da banca e identificação OCS.
- Participação nas feiras cadastradas.
- Cadastro e edição de produtos, preços e estoque.
- Consulta dos pedidos recebidos.
- Aceitação, recusa e registro da retirada das reservas.

### Reservas e estoque

O estoque é descontado quando a solicitação de reserva é registrada. Reservas canceladas ou recusadas devolvem a quantidade ao estoque. O sistema confere a disponibilidade antes de concluir a operação e registra as alterações em uma transação no banco de dados.

## Tecnologias

| Tecnologia | Uso |
| --- | --- |
| C++17 | Classes de domínio, regras e controladores |
| Qt 6 | Framework do aplicativo |
| Qt Quick e QML | Interface e navegação entre telas |
| Qt SQL e SQLite | Persistência local dos dados |
| CMake | Configuração e compilação |
| CTest | Execução dos testes automatizados |
| GitHub Actions | Compilação e testes em pushes na `main` e pull requests |

## Organização do projeto

| Arquivo ou pasta | Responsabilidade |
| --- | --- |
| `main.cpp` | Inicialização do Qt, conexão dos controladores e carregamento da interface |
| `Main.qml` | Estrutura principal e navegação do aplicativo |
| `telas/` | Páginas e componentes visuais em QML |
| `include/models/` e `src/models/` | Classes de domínio, como usuário, agricultor, consumidor e produto |
| `include/services/` e `src/services/` | Catálogo, agenda e acesso ao banco de dados |
| `include/ui/` e `src/ui/` | Controladores que conectam as ações da interface às regras do sistema |
| `assets/` | Marca e recursos gráficos |
| `tests/` | Testes do catálogo, da persistência e dos fluxos das telas |
| `docs/` | Página de apresentação do projeto em HTML e CSS |
| `CMakeLists.txt` | Dependências, arquivos e alvos de compilação |

### Principais classes

- **AuthController:** cadastro, login e gerenciamento da sessão.
- **CompradorController:** consulta do catálogo, sacola, agendamento e reservas do comprador.
- **VendedorController:** perfil da banca, participação nas feiras, produtos e pedidos do feirante.
- **RepositorioUsuario:** armazenamento e consulta das contas.
- **RepositorioCatalogo:** persistência de feiras, produtos e reservas.
- **AgendaFeira:** cálculo das datas e dos horários disponíveis para retirada.

### Orientação a objetos

`Usuario` é uma classe abstrata com os dados e comportamentos comuns aos perfis. `Agricultor` e `Consumidor` herdam dessa classe e implementam seus comportamentos específicos por meio de métodos virtuais e `override`.

Classes como `Produto` mantêm seus atributos privados e oferecem métodos para consultar e alterar os dados. Esse encapsulamento concentra operações como deduzir e repor estoque na própria classe.

## Como executar

### Requisitos

- Compilador com suporte a C++17.
- CMake 3.16 ou superior.
- Qt 6.5 ou superior, com os módulos Qt Quick e Qt SQL.
- Plugin SQLite do Qt disponível no ambiente de execução.

O Qt Creator pode ser usado para configurar, compilar e executar o projeto. A integração contínua utiliza Qt 6.8.3 no Linux.

### Pelo Qt Creator

1. Clone o repositório:

   ```bash
   git clone https://github.com/joaobgdev/Antro.git
   ```

2. Abra o arquivo `CMakeLists.txt` no Qt Creator.
3. Selecione um kit Desktop compatível com a versão exigida do Qt.
4. Configure e compile o projeto.
5. Execute o alvo `appAntro`.

### Pelo terminal

Na pasta do repositório, com o Qt disponível para o CMake:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Se o CMake não encontrar o Qt, informe a pasta do kit instalado:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="CAMINHO_DO_KIT_QT"
```

No Linux, execute:

```bash
./build/appAntro
```

No Windows, o executável é `appAntro.exe`; sua localização depende do gerador e da configuração de compilação. No macOS, o alvo é gerado como um pacote `.app`. O Qt Creator também permite executar o alvo nessas plataformas.

## Testes

Para compilar e executar os testes:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Os testes estão divididos em três alvos:

| Teste | Escopo |
| --- | --- |
| `catalogo` | Operações do catálogo e da sacola |
| `repositorio` | Persistência e regras de produtos e reservas |
| `telas` | Integração entre interface e controladores |

O teste das telas está configurado para execução sem janela, usando o modo `offscreen` do Qt.

## Armazenamento e limitações

Os dados são armazenados localmente em um arquivo SQLite chamado `antro.db`, criado na pasta de dados do aplicativo. Esse arquivo não precisa ser incluído no repositório. As senhas são armazenadas como hashes com sal.

A versão atual utiliza a base local e não possui sincronização entre dispositivos. Para demonstrar os dois perfis, é possível alternar entre contas na mesma instalação.

O cadastro realiza validações básicas dos dados informados. A plataforma não verifica automaticamente a identidade dos usuários nem a autenticidade da identificação OCS. Uma evolução possível é adicionar verificação administrativa dos cadastros e integração com serviços externos.

## Documentação

A descrição da proposta, dos fluxos e da arquitetura está disponível na [Documentação do Antro](https://docs.google.com/document/d/1f-76UdAjjs8JKjpYygxYDOpW0VoO2aVOu--zfqjKBiU/edit?usp=sharing).

## Equipe 7

- Lucas Medrado Santos
- João Bernardo Gomes Alves de Carvalho
- Maria Luiza de Paula Portela
- Luis Eduardo Lustosa Brito
