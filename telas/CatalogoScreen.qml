import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: catalogoPage
    property int feiraId: -1
    property int vendedorId: -1
    readonly property var dadosVendedor: CompradorController.vendedor(vendedorId)
    readonly property var dadosFeira: CompradorController.feira(feiraId)
    property string mensagem: ""
    titulo: dadosVendedor.banca || "Produtos do vendedor"
    Connections {
        target: CompradorController
        function onSacolaChanged() {
            Qt.callLater(function() {
                listaProdutos.model = CompradorController.produtos(catalogoPage.feiraId, catalogoPage.vendedorId)
            })
        }
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 18
        Label { text: catalogoPage.titulo; font.pixelSize: 36; font.bold: true; color: "#17201B" }
        Label { text: (catalogoPage.dadosVendedor.nome || "") + " · " + (catalogoPage.dadosFeira.nome || ""); wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: "Selecione os produtos para a sacola. A seleção ainda não confirma uma reserva."; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
        Label { text: catalogoPage.mensagem; visible: text.length > 0; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#22543D" }
        Label { visible: listaProdutos.count === 0; text: "Este vendedor não tem produtos disponíveis nesta feira." }
        ListView {
            id: listaProdutos
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 18
            model: CompradorController.produtos(catalogoPage.feiraId, catalogoPage.vendedorId)
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                id: produtoCard
                required property var modelData
                width: listaProdutos.width
                height: conteudo.implicitHeight + 56
                color: "white"
                radius: 14
                border.color: "#E4E8E5"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 28
                    spacing: 16
                    Rectangle {
                        Layout.preferredWidth: 72
                        Layout.preferredHeight: 72
                        radius: 36
                        color: "#E5EEE8"
                        Image { anchors.fill: parent; anchors.margins: 18; source: Qt.resolvedUrl("../assets/FolhaIcone.svg"); fillMode: Image.PreserveAspectFit }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: produtoCard.modelData.nome; font.pixelSize: 24; font.bold: true; color: "#22543D" }
                        Label { text: "R$ " + Number(produtoCard.modelData.preco).toLocaleString(Qt.locale("pt_BR"), 'f', 2) + " / " + produtoCard.modelData.unidade }
                        Label { text: "Disponível: " + produtoCard.modelData.disponivel + " " + produtoCard.modelData.unidade; color: "#66706A" }
                    }
                    ColumnLayout {
                        Label { text: produtoCard.modelData.porPeso ? "Quantidade (passos de 0,5 kg)" : "Quantidade (unidades)"; font.pixelSize: 12 }
                        SpinBox {
                            id: quantidade
                            from: 1
                            to: Math.max(1, Math.floor(produtoCard.modelData.disponivel * (produtoCard.modelData.porPeso ? 2 : 1)))
                            value: 1
                            enabled: produtoCard.modelData.disponivel >= (produtoCard.modelData.porPeso ? 0.5 : 1)
                            textFromValue: function(valor, locale) {
                                return produtoCard.modelData.porPeso ? (valor / 2).toLocaleString(locale, 'f', 1) : valor.toString()
                            }
                        }
                    }
                    BotaoAntro {
                        text: "Adicionar à sacola"
                        enabled: quantidade.enabled
                        onClicked: {
                            var qtd = quantidade.value / (produtoCard.modelData.porPeso ? 2 : 1)
                            var nome = produtoCard.modelData.nome
                            var ok = CompradorController.adicionar(catalogoPage.feiraId, catalogoPage.vendedorId, produtoCard.modelData.id, qtd)
                            catalogoPage.mensagem = ok ? nome + " adicionado à sacola." : "Não foi possível adicionar. Confira a quantidade disponível."
                        }
                    }
                }
            }
        }
    }
}
