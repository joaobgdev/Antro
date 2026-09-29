import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: loginPage
    anchors.fill: parent

    // Fundo
    background: Rectangle {
        anchors.fill: parent
        color: "#F4F8EC"
    }

    // Variáveis Iniciais
    property int caixas_size1: 34
    property string currentRole: "comprador"

    // Bloco Central
    Rectangle {
        anchors.centerIn: parent
        width: 440
        height: mainLayout.implicitHeight + 48
        color: "white"
        radius: 12
        border.color: "#EAE6D6"
        border.width: 2

        ColumnLayout {
            id: mainLayout
            anchors.fill: parent
            anchors.margins: 24
            spacing: 12

            // Logo/Título
            Text {
                text: "antro"
                font.pixelSize: 38
                font.bold: true
                color: "#84C6A8"
                Layout.alignment: Qt.AlignHCenter
            }

            // Subtítulo
            Text {
                text: "Direto do produtor para a sua mesa"
                font.pixelSize: 13
                color: "#859B74"
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }

            Item { Layout.preferredHeight: 4 }

            // Escolher Perfil
            Text {
                text: "Selecione o seu perfil:"
                font.pixelSize: 14
                font.bold: true
                color: "#3B5A3D"
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // Botão Comprador
                Button {
                    text: "Sou Comprador"
                    Layout.fillWidth: true
                    implicitHeight: 38

                    background: Rectangle {
                        color: loginPage.currentRole === "comprador" ? "#859B74" : "#F4F8EC"
                        radius: 6
                        border.color: "#EAE6D6"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: loginPage.currentRole === "comprador" ? "white" : "#3B5A3D"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: loginPage.currentRole = "comprador"
                }

                // Botão Feirante
                Button {
                    text: "Sou Feirante"
                    Layout.fillWidth: true
                    implicitHeight: 38

                    background: Rectangle {
                        color: loginPage.currentRole === "feirante" ? "#859B74" : "#F4F8EC"
                        radius: 6
                        border.color: "#EAE6D6"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: loginPage.currentRole === "feirante" ? "white" : "#3B5A3D"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: loginPage.currentRole = "feirante"
                }
                Button {
                    text: "Entrar"
                    onClicked: {
                        // Navega para a tela do catálogo trocando o item atual do StackView
                        stackView.push("CatalogoScreen.qml")
                    }
                }
            }

        // Informações Usuário
            TextField {
                id: txtName
                placeholderText: "Nome Completo"
                Layout.fillWidth: true
                implicitHeight: caixas_size1
                verticalAlignment: Text.AlignVCenter
                leftPadding: 12
                color: "#2B3A2C"
                placeholderTextColor: "#859B74"
                background: Rectangle {
                    color: "#F4F8EC"
                    radius: 6
                    border.color: "#EAE6D6"
                }
            }

            TextField {
                id: txtPhone
                placeholderText: "Número de Telefone"
                Layout.fillWidth: true
                implicitHeight: caixas_size1
                verticalAlignment: Text.AlignVCenter
                leftPadding: 12
                color: "#2B3A2C"
                placeholderTextColor: "#859B74"
                background: Rectangle {
                    color: "#F4F8EC"
                    radius: 6
                    border.color: "#EAE6D6"
                }
            }

            TextField {
                id: txtMarketName
                placeholderText: "Nome da Feira / Banca"
                visible: loginPage.currentRole === "feirante"
                Layout.fillWidth: true
                implicitHeight: caixas_size1
                verticalAlignment: Text.AlignVCenter
                leftPadding: 12
                color: "#2B3A2C"
                placeholderTextColor: "#859B74"
                background: Rectangle {
                    color: "#F4F8EC"
                    radius: 6
                    border.color: "#EAE6D6"
                }
            }

            TextField {
                id: txtOCSNumber
                placeholderText: "Código OCS"
                visible: loginPage.currentRole === "feirante"
                Layout.fillWidth: true
                implicitHeight: caixas_size1
                verticalAlignment: Text.AlignVCenter
                leftPadding: 12
                color: "#2B3A2C"
                placeholderTextColor: "#859B74"
                background: Rectangle {
                    color: "#F4F8EC"
                    radius: 6
                    border.color: "#EAE6D6"
                }
            }

            Item { Layout.preferredHeight: 6 }

            // Botão Cadastro
            Button {
                text: "Cadastrar e Entrar"
                Layout.fillWidth: true
                implicitHeight: 44

                background: Rectangle {
                    color: parent.down ? "#2B3A2C" : "#3B5A3D"
                    radius: 6
                }

                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    if (loginPage.currentRole === "feirante") {
                        stackView.replace("feiranteDashboard.qml")
                    } else {
                        stackView.replace("BuyerDashboard.qml")
                    }
                }
            }
        }
    }
}