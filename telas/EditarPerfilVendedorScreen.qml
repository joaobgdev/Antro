import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: pagina
    aba: "perfil"
    property var feiras: CompradorController.feiras()
    Component.onCompleted: VendedorController.carregarPerfil()
    ScrollView {
        anchors.fill: parent
        anchors.margins: pagina.width < 800 ? 20 : 36
        contentWidth: availableWidth
        clip: true
        ColumnLayout {
            width: parent.width
            spacing: 16
            Label { text: "Alterar perfil"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
            Label { text: "Atualize as feiras onde você participa e os produtos que vende."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label { text: VendedorController.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: selecao.implicitHeight + 44
                color: "white"
                radius: 14
                border.color: "#E2E7E3"
                ColumnLayout {
                    id: selecao
                    anchors.fill: parent
                    anchors.margins: 22
                    spacing: 12
                    Label { text: "Quais feiras você participa?"; font.pixelSize: 28; font.bold: true; color: "#17201B"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    Label { text: "Seus produtos serão oferecidos nas feiras selecionadas."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    GridLayout {
                        Layout.fillWidth: true
                        columns: pagina.width < 900 ? 1 : 2
                        rowSpacing: 8
                        columnSpacing: 16
                        Repeater {
                            model: pagina.feiras
                            delegate: CheckBox {
                                required property var modelData
                                objectName: "selecionarFeira"
                                text: modelData.nome
                                checked: VendedorController.feiras.indexOf(modelData.id) >= 0
                                Layout.fillWidth: true
                                onClicked: VendedorController.alternarFeira(modelData.id)
                            }
                        }
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: produtos.implicitHeight + 44
                color: "white"
                radius: 14
                border.color: "#E2E7E3"
                ColumnLayout {
                    id: produtos
                    anchors.fill: parent
                    anchors.margins: 22
                    spacing: 12
                    Label { text: "Produtos que você vende"; font.pixelSize: 28; font.bold: true; color: "#17201B"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    RowLayout {
                        Layout.fillWidth: true
                        CampoAntro { id: novoProduto; objectName: "nomeNovoProduto"; placeholderText: "Digite o nome do produto"; Layout.fillWidth: true; onAccepted: if (VendedorController.adicionarProduto(text)) clear() }
                        BotaoAntro { objectName: "incluirProduto"; text: "+"; onClicked: if (VendedorController.adicionarProduto(novoProduto.text)) novoProduto.clear() }
                    }
                    Flow {
                        Layout.fillWidth: true
                        spacing: 10
                        Repeater {
                            model: VendedorController.produtos
                            delegate: BotaoAntro {
                                required property var modelData
                                required property int index
                                text: modelData.nome + "  ×"
                                secundario: true
                                selecionado: true
                                onClicked: VendedorController.removerProduto(index)
                            }
                        }
                    }
                    Label { text: "Remover um produto tira a oferta do catálogo e mantém o histórico das reservas."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                }
            }
            BotaoAntro { objectName: "avancarPrecos"; text: "Avançar"; onClicked: pagina.StackView.view.push(Qt.resolvedUrl("DefinirPrecosVendedorScreen.qml")) }
            Item { Layout.preferredHeight: 16 }
        }
    }
}
