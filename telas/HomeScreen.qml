import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: homePage
    readonly property bool isFarmer: AuthController.perfilUsuario === "feirante"
    inicio: true
    mostrarVoltar: false
    mostrarSacola: !isFarmer

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 18
        Label {
            text: homePage.isFarmer ? "Seus produtos" : "Feiras do Recife"
            font.pixelSize: 36
            font.bold: true
            color: "#17201B"
        }
        ColumnLayout {
            visible: homePage.isFarmer
            Label { text: "Você ainda não cadastrou nenhum produto."; color: "#66706A" }
            BotaoAntro { text: "Adicionar produto"  }
            Item { Layout.fillHeight: true }
        }
        Label {
            visible: !homePage.isFarmer
            text: "Escolha uma feira para conhecer os vendedores e seus produtos."
            color: "#66706A"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        Label {
            visible: !homePage.isFarmer
            text: "Demonstração: vendedores e produtos de exemplo. Confirme os horários com a organização."
            color: "#66706A"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        ListView {
            id: listaFeiras
            visible: !homePage.isFarmer
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 18
            model: CompradorController.feiras()
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                required property var modelData
                width: listaFeiras.width
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
                        Label { anchors.centerIn: parent; text: "⌂"; font.pixelSize: 36; color: "#22543D" }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.bairro; color: "#66706A"; font.pixelSize: 13 }
                        Label { text: modelData.nome; font.bold: true; font.pixelSize: 24; color: "#22543D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: modelData.local + " · " + modelData.horario; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    }
                    BotaoAntro {
                        text: "Ver vendedores"
                        onClicked: homePage.StackView.view.push(Qt.resolvedUrl("FeiraScreen.qml"), {feiraId: modelData.id})
                    }
                }
            }
        }
    }
}
