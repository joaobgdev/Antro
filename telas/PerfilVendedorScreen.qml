import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: pagina
    aba: "perfil"
    property var minhasFeiras: []
    function atualizar() {
        minhasFeiras = CompradorController.feiras().filter(function(f) { return VendedorController.feiras.indexOf(f.id) >= 0 })
    }
    Component.onCompleted: { VendedorController.carregarPerfil(); atualizar() }
    StackView.onActivated: { VendedorController.carregarPerfil(); atualizar() }
    Connections { target: VendedorController; function onPerfilChanged() { pagina.atualizar() } }
    ScrollView {
        anchors.fill: parent
        anchors.margins: pagina.width < 800 ? 20 : 36
        contentWidth: availableWidth
        clip: true
        ColumnLayout {
            width: parent.width
            spacing: 16
            Label { text: "Meu perfil"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
            Label { text: AuthController.nomeUsuario; font.pixelSize: 24; color: "#22543D" }
            Label { text: AuthController.subtituloUsuario; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label { text: VendedorController.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label {
                visible: VendedorController.feirasPendentes.length > 0
                text: "Feiras do cadastro antigo sem correspondência: " + VendedorController.feirasPendentes.join(", ") + ". Selecione as feiras disponíveis em Alterar perfil. Os registros antigos foram preservados."
                color: "#66706A"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            BotaoAntro { objectName: "editarPerfil"; text: "Alterar perfil"; onClicked: pagina.StackView.view.push(Qt.resolvedUrl("EditarPerfilVendedorScreen.qml")) }
            Label { text: "Feiras onde participo"; font.pixelSize: 24; font.bold: true; color: "#17201B" }
            Label { visible: pagina.minhasFeiras.length === 0; text: "Escolha uma feira na Home ou em Alterar perfil."; color: "#66706A" }
            Repeater {
                model: pagina.minhasFeiras
                delegate: Label { required property var modelData; text: modelData.nome + " · " + modelData.horario; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            }
            Label { text: "Meus produtos"; font.pixelSize: 24; font.bold: true; color: "#17201B" }
            Label { visible: VendedorController.produtos.length === 0; text: "Você ainda não cadastrou produtos."; color: "#66706A" }
            Repeater {
                model: VendedorController.produtos
                delegate: Rectangle {
                    id: card
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: conteudo.implicitHeight + 44
                    radius: 14
                    color: "white"
                    border.color: "#E2E7E3"
                    ColumnLayout {
                        id: conteudo
                        anchors.fill: parent
                        anchors.margins: 22
                        Label { text: card.modelData.nome; font.pixelSize: 22; font.bold: true; color: "#22543D" }
                        Label { text: "Produto pausado"; visible: !card.modelData.ativo; color: "#66706A" }
                        Label { text: "R$ " + Number(card.modelData.preco).toLocaleString(Qt.locale("pt_BR"), 'f', 2) + " / " + card.modelData.tipoVenda }
                        Label { text: "Disponível: " + Number(card.modelData.estoque).toLocaleString(Qt.locale("pt_BR"), 'f', card.modelData.tipoVenda === "unidade" ? 0 : 1) + (card.modelData.tipoVenda === "unidade" ? " unidades" : " kg") + " · reservado: " + Number(card.modelData.reservado).toLocaleString(Qt.locale("pt_BR"), 'f', card.modelData.tipoVenda === "unidade" ? 0 : 1); color: "#66706A" }
                    }
                }
            }
            Item { Layout.preferredHeight: 16 }
        }
    }
}
