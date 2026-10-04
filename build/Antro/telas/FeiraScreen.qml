import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: feiraPage
    property int feiraId: -1
    readonly property var dadosFeira: CompradorController.feira(feiraId)
    titulo: dadosFeira.nome || "Feira não encontrada"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 18
        Label { text: feiraPage.titulo; font.pixelSize: 36; font.bold: true; color: "#17201B"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: (feiraPage.dadosFeira.local || "") + " · " + (feiraPage.dadosFeira.horario || ""); wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: "Vendedores participantes"; font.pixelSize: 24; color: "#22543D" }
        Label {
            visible: listaVendedores.count === 0
            text: "Ainda não há vendedores cadastrados nesta feira. Você pode voltar e escolher outra."
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        ListView {
            id: listaVendedores
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18
            clip: true
            model: CompradorController.vendedores(feiraPage.feiraId)
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                required property var modelData
                width: listaVendedores.width
                height: conteudo.implicitHeight + 56
                color: "white"
                radius: 14
                border.color: "#E4E8E5"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 28
                    Rectangle {
                        Layout.preferredWidth: 72
                        Layout.preferredHeight: 72
                        radius: 36
                        color: "#E5EEE8"
                        Label { anchors.centerIn: parent; text: modelData.nome.charAt(0); font.pixelSize: 30; font.bold: true; color: "#22543D" }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.banca; font.pixelSize: 24; font.bold: true; color: "#22543D" }
                        Label { text: modelData.nome; font.bold: true }
                        Label { text: modelData.descricao; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
                    }
                    BotaoAntro {
                        text: "Ver produtos"
                        onClicked: feiraPage.StackView.view.push(Qt.resolvedUrl("CatalogoScreen.qml"), {feiraId: feiraPage.feiraId, vendedorId: modelData.id})
                    }
                }
            }
        }
    }
}
