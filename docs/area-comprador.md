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

Não existe pagamento no aplicativo. Adicionar à sacola ainda não confirma uma reserva. A imagem com a divisão de tarefas deixa a conclusão do carrinho em aberto; por isso, esta implementação entrega a seleção e o acesso à sacola, sem criar uma confirmação fictícia de pedido.

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
| `telas/SacolaScreen.qml` | Itens selecionados, feira, vendedor, quantidade e remoção. |

## Como o C++ funciona

As listas usam `std::vector`, e as buscas usam `for` e `if`. Os IDs ligam as informações: uma oferta contém o ID da feira, do vendedor e do produto. O produto reaproveita a classe `Produto` já existente.

A sacola agrupa itens somente quando feira, vendedor e produto são iguais. Se o mesmo alimento for selecionado para duas feiras, cada retirada fica em uma linha diferente. A soma das quantidades não pode ultrapassar o estoque de exemplo.

O controller usa `QVariantMap` para representar os campos de cada linha e `QVariantList` para enviar listas ao QML. `Q_INVOKABLE` permite chamar um método C++ pela tela. `Q_PROPERTY` expõe informações para o QML, e o sinal `sacolaChanged` avisa quando elas mudam. Esses recursos são a ligação necessária com o Qt; as regras do catálogo continuam em C++ puro.

## Integração com as outras partes

Os vendedores, vínculos, produtos, preços e estoques são fictícios e estão no construtor de `CatalogoComprador`. Não vêm dos cadastros SQLite. Essa é a próxima ligação com a parte do feirante: substituir as listas de exemplo pelos registros reais, mantendo os IDs das ofertas.

A sacola existe apenas em memória. O responsável por pedidos pode usar `getSacola()`, que devolve feira, vendedor, produto e quantidade de cada item, para criar a solicitação de reserva. A validação definitiva do estoque, a data de retirada, a confirmação pelo agricultor e a persistência do pedido pertencem a essa integração. Não chamar o fluxo de pagamento do `GerenciadorFeira` para finalizar esta tela.

A classe `Pedido` e seu status `AGUARDANDO_PAGAMENTO` não foram alterados nesta entrega, pois são da parte de pedidos. A interface nova não usa esse status.

As feiras são referências do Recife; confirmar endereços e horários com a organização antes de uso real. Referências: https://www2.recife.pe.gov.br/servico/feiras-e-pontos-agroecologicos?op=MTI5 e https://www.adagro.pe.gov.br/images/programa-estadual-de-agrotoxicos/feiras-organicas/feiras_organicas.pdf.

## Como testar

Abra o `CMakeLists.txt` no Qt Creator com o mesmo kit Qt 6 usado pelo grupo. Reconfigure o CMake para incluir o controller e os novos arquivos QML. O projeto mantém os requisitos de Qt da main.

1. Cadastre ou entre com um perfil de comprador.
2. Abra a Várzea, escolha Ana e adicione alface ou tomate.
3. Abra a sacola, volte e confira que a seleção continua lá.
4. Volte à home, abra Graças e confira os vendedores dessa feira.
5. Selecione o mesmo produto em outra feira e veja as retiradas em linhas separadas.
6. Remova itens e confira o valor estimado e a disponibilidade.
7. Abra Sítio da Trindade, que tem a situação de lista vazia no exemplo.
8. Saia da conta e entre novamente: a sacola deve estar vazia.
9. Entre como feirante e confira que a área já existente foi preservada.

O teste das regras não precisa do Qt. Na raiz do projeto, com g++ instalado:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -I include tests/test_catalogo_comprador.cpp src/services/CatalogoComprador.cpp src/models/produto.cpp -o test_catalogo_comprador
./test_catalogo_comprador
```

No Windows, execute `test_catalogo_comprador.exe` depois de compilar com o g++ do MinGW. O teste verifica filtros, IDs incorretos, quantidades inválidas, agrupamento, estoque entre feiras, remoção e limpeza.

## Verificações desta entrega

O teste de C++ puro foi compilado e executado com g++ e passou. O percurso home → feira → produtos → sacola → remoção → voltar foi executado no runtime Qt/QML com objetos de teste no lugar dos controllers C++; não houve avisos de execução nesse percurso. Essa verificação separada não substitui a compilação do app completo no Qt Creator. O ambiente de desenvolvimento desta alteração não dispõe do kit de desenvolvimento Qt/C++ para essa compilação completa.
