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
        anchors.margins: 24
        spacing: 12
        Label { text: catalogoPage.titulo; font.pixelSize: 26; font.bold: true; color: "#3B5A3D" }
        Label { text: (catalogoPage.dadosVendedor.nome || "") + " · " + (catalogoPage.dadosFeira.nome || ""); wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: "Selecione os produtos para a sacola. A seleção ainda não confirma uma reserva."; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#61705F" }
        Label { text: catalogoPage.mensagem; visible: text.length > 0; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#3B5A3D" }
        Label { visible: listaProdutos.count === 0; text: "Este vendedor não tem produtos disponíveis nesta feira." }
        ListView {
            id: listaProdutos
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 12
            model: CompradorController.produtos(catalogoPage.feiraId, catalogoPage.vendedorId)
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                id: produtoCard
                required property var modelData
                width: listaProdutos.width
                height: conteudo.implicitHeight + 32
                color: "white"
                radius: 10
                border.color: "#EAE6D6"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: produtoCard.modelData.nome; font.pixelSize: 20; font.bold: true; color: "#3B5A3D" }
                        Label { text: "R$ " + Number(produtoCard.modelData.preco).toLocaleString(Qt.locale("pt_BR"), 'f', 2) + " / " + produtoCard.modelData.unidade }
                        Label { text: "Disponível: " + produtoCard.modelData.disponivel + " " + produtoCard.modelData.unidade; color: "#61705F" }
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
                    Button {
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
