import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: homePage

    readonly property bool isFarmer: AuthController.perfilUsuario === "feirante"

    background: Rectangle { color: "#F4F8EC" }

    // Cabeçalho
    header: Rectangle {
        implicitHeight: 64
        color: "white"
        border.color: "#EAE6D6"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 24
            anchors.rightMargin: 24
            spacing: 16

            Text {
                text: "antro"
                font.pixelSize: 26
                font.bold: true
                color: "#84C6A8"
            }

            Item { Layout.fillWidth: true }

            ColumnLayout {
                spacing: 0
                Text {
                    text: AuthController.logado ? "Olá, " + AuthController.nomeUsuario + "!" : ""
                    font.bold: true
                    color: "#3B5A3D"
                    Layout.alignment: Qt.AlignRight
                }
                Text {
                    text: AuthController.subtituloUsuario
                    font.pixelSize: 12
                    color: "#859B74"
                    Layout.alignment: Qt.AlignRight
                }
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

    // Conteúdo
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 16

        Text {
            text: homePage.isFarmer ? "Seus produtos" : "Feira agroecológica"
            font.pixelSize: 22
            font.bold: true
            color: "#3B5A3D"
        }

        // --- Visão do feirante (placeholder) ---
        ColumnLayout {
            visible: homePage.isFarmer
            spacing: 12

            Text {
                text: "Você ainda não cadastrou nenhum produto."
                color: "#859B74"
            }
            Button {
                text: "Adicionar produto"
                // TODO: abrir tela de cadastro de produto
            }
        }

        // --- Visão do comprador: leva ao catálogo já existente ---
        ColumnLayout {
            visible: !homePage.isFarmer
            spacing: 12

            Text {
                text: "Veja os produtos disponíveis direto dos produtores."
                color: "#859B74"
            }
            Button {
                text: "Abrir catálogo"
                onClicked: homePage.StackView.view.push(Qt.resolvedUrl("telas/CatalogoScreen.qml"))
            }
        }

        Item { Layout.fillHeight: true }
    }
}
