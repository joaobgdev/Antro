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
        anchors.margins: 24
        spacing: 12
        Label { text: feiraPage.titulo; font.pixelSize: 26; font.bold: true; color: "#3B5A3D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: (feiraPage.dadosFeira.local || "") + " · " + (feiraPage.dadosFeira.horario || ""); wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: "Vendedores participantes"; font.pixelSize: 20; color: "#3B5A3D" }
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
            spacing: 12
            clip: true
            model: CompradorController.vendedores(feiraPage.feiraId)
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                required property var modelData
                width: listaVendedores.width
                height: conteudo.implicitHeight + 32
                color: "white"
                radius: 10
                border.color: "#EAE6D6"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 16
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.banca; font.pixelSize: 21; font.bold: true; color: "#3B5A3D" }
                        Label { text: modelData.nome; font.bold: true }
                        Label { text: modelData.descricao; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#61705F" }
                    }
                    Button {
                        text: "Ver produtos"
                        onClicked: feiraPage.StackView.view.push(Qt.resolvedUrl("CatalogoScreen.qml"), {feiraId: feiraPage.feiraId, vendedorId: modelData.id})
                    }
                }
            }
        }
    }
}
