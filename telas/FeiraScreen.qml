import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: feiraPage
    property int feiraId: -1
    property var dadosFeira: ({})
    property var vendedores: []

    function atualizar() {
        dadosFeira = CompradorController.feira(feiraId)
        vendedores = CompradorController.vendedores(feiraId)
    }
    Component.onCompleted: atualizar()
    StackView.onActivated: { CompradorController.recarregar(); atualizar() }
    Connections { target: CompradorController; function onProdutosChanged() { feiraPage.atualizar() } }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: feiraPage.width < 800 ? 20 : 36
        spacing: 16
        Label { text: feiraPage.dadosFeira.nome || "Feira não encontrada"; font.pixelSize: 32; font.bold: true; color: "#17201B"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: (feiraPage.dadosFeira.local || "") + " · " + (feiraPage.dadosFeira.horario || ""); wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
        Label { text: "Produtores participantes"; font.pixelSize: 24; color: "#22543D" }
        Label { visible: lista.count === 0; text: "Ainda não há produtores cadastrados nesta feira. Volte para escolher outra."; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        ListView {
            id: lista
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16
            clip: true
            model: feiraPage.vendedores
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                id: card
                required property var modelData
                width: lista.width
                height: conteudo.implicitHeight + 44
                color: "white"
                radius: 14
                border.color: "#E2E7E3"
                GridLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 22
                    columns: feiraPage.width < 800 ? 1 : 2
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: card.modelData.banca; font.pixelSize: 24; font.bold: true; color: "#22543D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: card.modelData.nome; color: "#17201B" }
                        Label { text: card.modelData.descricao; visible: text.length > 0; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
                        Label { text: "Cadastro de exemplo antigo · não recebe reservas"; visible: card.modelData.exemplo; color: "#66706A" }
                    }
                    BotaoAntro {
                        objectName: "abrirProdutor"
                        text: "Ver produtos"
                        onClicked: feiraPage.StackView.view.push(Qt.resolvedUrl("CatalogoScreen.qml"), {feiraId: feiraPage.feiraId, vendedorId: card.modelData.id})
                    }
                }
            }
        }
    }
}
