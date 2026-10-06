import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: pagina
    aba: "perfil"
    function salvar() {
        for (var i = 0; i < lista.count; i++) if (!lista.itemAt(i).gravar()) return
        if (VendedorController.salvarPerfil()) {
            var pilha = pagina.StackView.view
            pilha.pop(null)
            pilha.push(Qt.resolvedUrl("PerfilVendedorScreen.qml"))
        }
    }
    ScrollView {
        anchors.fill: parent
        anchors.margins: pagina.width < 800 ? 20 : 36
        contentWidth: availableWidth
        clip: true
        ColumnLayout {
            width: parent.width
            spacing: 16
            Label { text: "Preços e estoque"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
            Label { text: "Informe o preço pela unidade escolhida e a quantidade ainda disponível. O estoque é compartilhado entre suas feiras."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label { text: "Para 100g, informe o preço de 100g e o estoque em kg (ex.: 0,3 kg). Para kg, as reservas são feitas em passos de 0,5 kg."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label { text: VendedorController.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label { visible: lista.count === 0; text: "Nenhum produto na lista. Você pode salvar somente sua participação nas feiras."; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Repeater {
                id: lista
                model: VendedorController.produtos
                delegate: Rectangle {
                    id: card
                    required property var modelData
                    required property int index
                    Layout.fillWidth: true
                    implicitHeight: campos.implicitHeight + 44
                    radius: 14
                    color: "white"
                    border.color: "#E2E7E3"
                    function gravar() { return VendedorController.definirPreco(index, preco.text) && VendedorController.definirEstoque(index, estoque.text) }
                    ColumnLayout {
                        id: campos
                        anchors.fill: parent
                        anchors.margins: 22
                        spacing: 12
                        Label { text: card.modelData.nome; font.pixelSize: 24; font.bold: true; color: "#22543D" }
                        CheckBox {
                            objectName: "produtoAtivo"
                            text: "Disponível no catálogo"
                            checked: card.modelData.ativo
                            onClicked: VendedorController.definirAtivo(card.index, checked)
                        }
                        GridLayout {
                            Layout.fillWidth: true
                            columns: pagina.width < 800 ? 1 : 3
                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                Label { text: "Preço por" }
                                SelecaoAntro {
                                    objectName: "tipoVenda"
                                    Layout.fillWidth: true
                                    model: ["unidade", "kg", "100g"]
                                    currentIndex: model.indexOf(card.modelData.tipoVenda)
                                    onActivated: {
                                        var anterior = card.modelData.tipoVenda
                                        if (!VendedorController.definirTipoVenda(card.index, currentText)) currentIndex = model.indexOf(anterior)
                                    }
                                }
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                Label { text: "Preço (R$)" }
                                CampoAntro {
                                    id: preco
                                    objectName: "precoProduto"
                                    Layout.fillWidth: true
                                    text: Number(card.modelData.preco).toLocaleString(Qt.locale("pt_BR"), 'f', 2)
                                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                                    onEditingFinished: VendedorController.definirPreco(card.index, text)
                                }
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                Label { text: "Disponível (" + (card.modelData.tipoVenda === "unidade" ? "unidades" : "kg") + ")" }
                                CampoAntro {
                                    id: estoque
                                    objectName: "estoqueProduto"
                                    Layout.fillWidth: true
                                    text: Number(card.modelData.estoque).toLocaleString(Qt.locale("pt_BR"), 'f', card.modelData.tipoVenda === "unidade" ? 0 : 1)
                                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                                    onEditingFinished: VendedorController.definirEstoque(card.index, text)
                                }
                            }
                        }
                        Label { text: "Já reservado: " + card.modelData.reservado + (card.modelData.tipoVenda === "unidade" ? " unidades" : " kg") + ". A quantidade reservada não entra no estoque disponível."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    }
                }
            }
            BotaoAntro { objectName: "salvarPerfil"; text: "Salvar"; onClicked: pagina.salvar() }
            Item { Layout.preferredHeight: 16 }
        }
    }
}
