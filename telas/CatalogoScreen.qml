import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: catalogoPage
    property int feiraId: -1
    property int vendedorId: -1
    property var dadosVendedor: ({})
    property var dadosFeira: ({})
    property var produtos: []
    property string mensagem: ""

    function atualizar() {
        dadosVendedor = CompradorController.vendedor(vendedorId)
        dadosFeira = CompradorController.feira(feiraId)
        produtos = CompradorController.produtos(feiraId, vendedorId)
    }
    Component.onCompleted: atualizar()
    // Recarrega os dados sempre que esta tela fica ativa.
    StackView.onActivated: { CompradorController.recarregar(); atualizar() }
    // Atualiza a tela quando o catálogo ou a sacola mudam.
    Connections {
        target: CompradorController
        function onProdutosChanged() { catalogoPage.atualizar() }
        function onSacolaChanged() { Qt.callLater(catalogoPage.atualizar) }
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: catalogoPage.width < 800 ? 20 : 36
        spacing: 16
        Label { text: catalogoPage.dadosVendedor.banca || "Produtos do produtor"; font.pixelSize: 32; font.bold: true; color: "#17201B"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: (catalogoPage.dadosVendedor.nome || "") + " · " + (catalogoPage.dadosFeira.nome || ""); color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: "Adicione os produtos e escolha a retirada no carrinho. O vendedor vai confirmar a disponibilidade."; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
        Label { text: CompradorController.erro || catalogoPage.mensagem; visible: text.length > 0; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: CompradorController.erro ? "#B3261E" : "#22543D" }
        Label { visible: lista.count === 0; text: "Este produtor ainda não oferece produtos nesta feira." }
        ListView {
            id: lista
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 16
            model: catalogoPage.produtos
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                id: card
                required property var modelData
                width: lista.width
                height: conteudo.implicitHeight + 44
                radius: 14
                color: "white"
                border.color: "#E2E7E3"
                GridLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 22
                    columns: catalogoPage.width < 1000 ? 1 : 2
                    rowSpacing: 12
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: card.modelData.nome; font.pixelSize: 24; font.bold: true; color: "#22543D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: "R$ " + Number(card.modelData.preco).toLocaleString(Qt.locale("pt_BR"), 'f', 2) + " / " + card.modelData.unidadePreco }
                        Label { text: "Disponível: " + Number(card.modelData.disponivel).toLocaleString(Qt.locale("pt_BR"), 'f', card.modelData.passo < 1 ? 1 : 0) + " " + card.modelData.unidade; color: "#66706A" }
                    }
                    RowLayout {
                        spacing: 12
                        ColumnLayout {
                            Label { text: "Quantidade (" + card.modelData.unidade + ")"; font.pixelSize: 12 }
                            SpinBox {
                                id: quantidade
                                objectName: "quantidadeProduto"
                                from: 1
                                to: Math.max(1, Math.floor((card.modelData.disponivel + 0.000001) / card.modelData.passo))
                                value: 1
                                enabled: card.modelData.disponivel + 0.000001 >= card.modelData.passo && !catalogoPage.dadosVendedor.exemplo
                                textFromValue: function(valor, locale) { return (valor * card.modelData.passo).toLocaleString(Qt.locale("pt_BR"), 'f', card.modelData.passo < 1 ? 1 : 0) }
                            }
                        }
                        BotaoAntro {
                            objectName: "adicionarProduto"
                            text: "Adicionar"
                            enabled: quantidade.enabled
                            onClicked: {
                                var nome = card.modelData.nome
                                // Converte os passos do seletor para unidades ou kg.
                                if (CompradorController.adicionar(catalogoPage.feiraId, catalogoPage.vendedorId, card.modelData.id, quantidade.value * card.modelData.passo))
                                    catalogoPage.mensagem = nome + " adicionado ao carrinho."
                            }
                        }
                    }
                }
            }
        }
    }
}
