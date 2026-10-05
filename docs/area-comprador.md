# Área do comprador

O fluxo desta parte é: entrar como comprador, escolher uma feira, abrir um vendedor e selecionar os produtos dele naquela feira. O botão Voltar retorna à tela anterior. A sacola fica no cabeçalho da home, da feira e do catálogo.

## O que foi feito

- Home do comprador com as feiras pré-cadastradas.
- Logo AntroVerde no cabeçalho e três versões SVG fornecidas pela equipe incluídas nos recursos.
- Página de uma feira com seus vendedores, sem repetir o mesmo vendedor por produto.
- Catálogo filtrado pela feira e pelo vendedor selecionados.
- Sacola compartilhada entre as telas, com remoção de itens e valor estimado.
- Quantidades inteiras para unidades e passos de 0,5 kg para produtos vendidos por peso.
- Validação da oferta e da quantidade em C++, incluindo o estoque do mesmo produto oferecido em duas feiras.
- Limpeza da sacola quando muda o usuário autenticado.

Não existe pagamento no aplicativo. O comprador monta o **carrinho**, vê o total estimado e toca em **Finalizar reserva**; a reserva é gravada no SQLite, o estoque é baixado e aparece a tela de confirmação (feedback).

## Novidades desta etapa

- **Carrinho** (`SacolaScreen.qml`): botões − / + (passo de 1 unidade ou 0,5 kg), remover, total estimado e botão *Finalizar reserva*.
- **Feedback** (`ReservaConfirmadaScreen.qml`): número da reserva, feiras de retirada, resumo dos itens e total. Ao abrir, o carrinho sai da pilha de telas e o carrinho fica vazio.
- **Persistência em SQLite** (`RepositorioCatalogo`, mesmo arquivo `antro.db` dos usuários): feiras, vendedores, produtos, ofertas (produto em cada feira), reservas e itens da reserva. Os dados de exemplo (feiras, Ana, João, Rosa e produtos 101–106) são inseridos só na primeira execução; depois tudo vem do banco.
- **Área do feirante** (`HomeScreen.qml` + `ProdutoFeiranteScreen.qml`): o feirante cadastra produtos (nome, preço, por kg ou unidade, estoque e feiras onde vende) e remove os seus. Ficam salvos e aparecem para os compradores nas feiras escolhidas, mesmo depois de fechar e abrir o app. O vendedor é criado automaticamente no primeiro produto, ligado ao telefone do feirante.
- **Reserva e estoque**: `salvarReserva` grava a reserva e baixa o estoque na mesma transação (`UPDATE ... WHERE estoque >= ?` impede estoque negativo). Os preços e nomes ficam registrados em `reserva_itens`.

## Onde fica cada parte

| Arquivo | Responsabilidade |
| --- | --- |
| `include/services/CatalogoComprador.hpp` | Estruturas de feira, vendedor, oferta e item da sacola; declaração do catálogo. |
| `src/services/CatalogoComprador.cpp` | Dados de exemplo, buscas, filtros e regras da sacola. |
| `include/ui/CompradorController.hpp` e `src/ui/CompradorController.cpp` | Ponte entre C++ e QML, no mesmo padrão do AuthController. |
| `telas/HomeScreen.qml` | Home com feiras para o comprador; preserva a área do feirante. |
| `telas/FeiraScreen.qml` | Vendedores da feira escolhida. |
| `telas/CatalogoScreen.qml` | Produtos do vendedor dentro da feira. |
| `telas/PaginaComprador.qml` | Cabeçalho reutilizado pelas telas. |
| `telas/SacolaScreen.qml` | Carrinho: itens, quantidade (− / +), remoção, total e finalização da reserva. |
| `telas/ReservaConfirmadaScreen.qml` | Feedback de reserva realizada. |
| `telas/ProdutoFeiranteScreen.qml` | Cadastro de produto pelo feirante. |
| `include/services/RepositorioCatalogo.hpp` e `src/services/RepositorioCatalogo.cpp` | Todo o SQL do catálogo e das reservas. |

## Como o C++ funciona

As listas usam `std::vector`, e as buscas usam `for` e `if`. Os IDs ligam as informações: uma oferta contém o ID da feira, do vendedor e do produto. O produto reaproveita a classe `Produto` já existente.

A sacola agrupa itens somente quando feira, vendedor e produto são iguais. Se o mesmo alimento for selecionado para duas feiras, cada retirada fica em uma linha diferente. A soma das quantidades não pode ultrapassar o estoque de exemplo.

O controller usa `QVariantMap` para representar os campos de cada linha e `QVariantList` para enviar listas ao QML. `Q_INVOKABLE` permite chamar um método C++ pela tela. `Q_PROPERTY` expõe informações para o QML, e o sinal `sacolaChanged` avisa quando elas mudam. Esses recursos são a ligação necessária com o Qt; as regras do catálogo continuam em C++ puro.

## Integração com as outras partes

`CatalogoComprador` continua sendo C++ puro: recebe os dados prontos por `definirDados(DadosCatalogo)` e o `RepositorioCatalogo` é quem lê o banco. Depois de cada mudança (produto novo, remoção, reserva) o controller recarrega o catálogo e o carrinho é revalidado (itens que sumiram ou passaram do estoque saem dele).

A reserva é gravada com status `SOLICITADA` em `reservas`. A confirmação pelo agricultor, a data de retirada, a tela "minhas reservas" e a ligação com `Pedido`/`GerenciadorFeira` ficam para a parte de pedidos; as classes `Pedido` e `StatusPedido` não foram alteradas. O carrinho em si não é salvo: se o app fechar antes de finalizar, ele começa vazio.

As feiras são referências do Recife; confirmar endereços e horários com a organização antes de uso real. Referências: https://www2.recife.pe.gov.br/servico/feiras-e-pontos-agroecologicos?op=MTI5 e https://www.adagro.pe.gov.br/images/programa-estadual-de-agrotoxicos/feiras-organicas/feiras_organicas.pdf.

## Como testar

Abra o `CMakeLists.txt` no Qt Creator (Qt 6.5+ com o módulo Sql), reconfigure o CMake e execute.

1. Cadastre um **feirante**, entre e toque em *Adicionar produto*. Preencha, marque uma ou mais feiras e salve.
2. Feche o aplicativo e abra de novo, entre com o mesmo feirante: o produto continua na lista.
3. Cadastre/entre como **comprador**, abra a feira marcada e o vendedor: o produto novo aparece (vendedor = banca do feirante).
4. Adicione itens, abra o **Carrinho**, use − / + e confira o total estimado.
5. Toque em *Finalizar reserva*: aparece a tela de feedback. *Voltar para as feiras* leva à home e o carrinho está vazio.
6. Abra o mesmo vendedor de novo: o estoque disponível diminuiu. Feche e reabra o app: continua diminuído.
7. Remova um produto como feirante e confira que some para o comprador.

O arquivo do banco é `antro.db` em `QStandardPaths::AppDataLocation` (o caminho é impresso no console ao iniciar). Para recomeçar do zero, feche o app e apague esse arquivo.

O teste das regras (carrinho, quantidade, estoque, reserva) não precisa do Qt:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -I include tests/test_catalogo_comprador.cpp src/services/CatalogoComprador.cpp src/models/produto.cpp -o test_catalogo_comprador
./test_catalogo_comprador
```

No Windows, execute `test_catalogo_comprador.exe` depois de compilar com o g++ do MinGW.
