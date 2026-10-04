import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: homePage
    readonly property bool isFarmer: AuthController.perfilUsuario === "feirante"
    background: Rectangle { color: "#F4F8EC" }

    header: Rectangle {
        implicitHeight: 72
        color: "white"
        border.color: "#EAE6D6"
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 24
            anchors.rightMargin: 24
            spacing: 16
            Image {
                source: Qt.resolvedUrl("../assets/AntroVerde.svg")
                Layout.preferredWidth: 126
                Layout.preferredHeight: 40
                fillMode: Image.PreserveAspectFit
                sourceSize.width: 252
                sourceSize.height: 80
            }
            Item { Layout.fillWidth: true }
            ColumnLayout {
                spacing: 0
                Text { text: "Olá, " + AuthController.nomeUsuario + "!"; font.bold: true; color: "#3B5A3D" }
                Text { text: AuthController.subtituloUsuario; font.pixelSize: 12; color: "#859B74" }
            }
            Button {
                visible: !homePage.isFarmer
                text: "Sacola (" + CompradorController.tiposNaSacola + ")"
                onClicked: homePage.StackView.view.push(Qt.resolvedUrl("SacolaScreen.qml"))
            }
            Button {
                text: "Sair"
                onClicked: {
                    AuthController.sair()
                    homePage.StackView.view.replace(Qt.resolvedUrl("LoginScreen.qml"))
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 12
        Label {
            text: homePage.isFarmer ? "Seus produtos" : "Feiras do Recife"
            font.pixelSize: 26
            font.bold: true
            color: "#3B5A3D"
        }
        ColumnLayout {
            visible: homePage.isFarmer
            Label { text: "Você ainda não cadastrou nenhum produto."; color: "#61705F" }
            Button { text: "Adicionar produto"  }
            Item { Layout.fillHeight: true }
        }
        Label {
            visible: !homePage.isFarmer
            text: "Escolha uma feira para conhecer os vendedores e seus produtos."
            color: "#61705F"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        Label {
            visible: !homePage.isFarmer
            text: "Demonstração: vendedores e produtos de exemplo. Confirme os horários com a organização."
            color: "#61705F"
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
            spacing: 12
            model: CompradorController.feiras()
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                required property var modelData
                width: listaFeiras.width
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
                        Label { text: modelData.bairro; color: "#61705F"; font.pixelSize: 13 }
                        Label { text: modelData.nome; font.bold: true; font.pixelSize: 20; color: "#3B5A3D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: modelData.local + " · " + modelData.horario; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    }
                    Button {
                        text: "Ver vendedores"
                        onClicked: homePage.StackView.view.push(Qt.resolvedUrl("FeiraScreen.qml"), {feiraId: modelData.id})
                    }
                }
            }
        }
    }
}
