import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: loginPage

    // Fundo
    background: Rectangle {
        color: "#F4F8EC"
    }

    // Variáveis Iniciais
    property int caixas_size1: 34
    property string currentRole: "comprador"
    property string mode: "entrar"          // "entrar" ou "cadastrar"
    readonly property bool isRegister: mode === "cadastrar"
    property string errorMessage: ""

    Connections {
        target: AuthController 
        function onFalhaAutenticacao(mensagem) { loginPage.errorMessage = mensagem }
    }

    // Bloco Central
    Rectangle {
        anchors.centerIn: parent
        width: 440
        implicitHeight: mainLayout.implicitHeight + 48
        color: "white"
        radius: 12
        border.color: "#EAE6D6"
        border.width: 2

        ColumnLayout {
            id: mainLayout
            width: parent.width - 48
            anchors.centerIn: parent
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

            // Abas Entrar / Cadastrar
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Button {
                    text: "Entrar"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1   // larguras iguais
                    implicitHeight: 38
                    background: Rectangle {
                        color: !loginPage.isRegister ? "#3B5A3D" : "#F4F8EC"
                        radius: 6
                        border.color: "#EAE6D6"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: !loginPage.isRegister ? "white" : "#3B5A3D"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: { loginPage.mode = "entrar"; loginPage.errorMessage = "" }
                }

                Button {
                    text: "Cadastrar"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1   // larguras iguais
                    implicitHeight: 38
                    background: Rectangle {
                        color: loginPage.isRegister ? "#3B5A3D" : "#F4F8EC"
                        radius: 6
                        border.color: "#EAE6D6"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: loginPage.isRegister ? "white" : "#3B5A3D"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: { loginPage.mode = "cadastrar"; loginPage.errorMessage = "" }
                }
            }

            // Escolher Perfil (só no cadastro)
            Text {
                visible: loginPage.isRegister
                text: "Selecione o seu perfil:"
                font.pixelSize: 14
                font.bold: true
                color: "#3B5A3D"
            }

            RowLayout {
                visible: loginPage.isRegister
                Layout.fillWidth: true
                spacing: 10

                // Botão Comprador
                Button {
                    text: "Sou Comprador"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1   // larguras iguais
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
                    Layout.preferredWidth: 1   // larguras iguais
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
            }

            // Informações Usuário
            TextField {
                id: txtName
                placeholderText: "Nome Completo"
                visible: loginPage.isRegister
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
                id: txtPassword
                placeholderText: "Senha"
                echoMode: TextInput.Password
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
                visible: loginPage.isRegister && loginPage.currentRole === "feirante"
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
                visible: loginPage.isRegister && loginPage.currentRole === "feirante"
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

            // Mensagem de erro
            Text {
                text: loginPage.errorMessage
                visible: text !== ""
                color: "#B23A3A"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Item { Layout.preferredHeight: 6 }

            // Botão principal
            Button {
                text: loginPage.isRegister ? "Cadastrar e Entrar" : "Entrar"
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
                    loginPage.errorMessage = ""
                    let ok = false
                    if (typeof AuthController !== "undefined") {
                        if (loginPage.isRegister) {
                            ok = AuthController.cadastrar(
                                loginPage.currentRole,
                                txtName.text,
                                txtPhone.text,
                                txtPassword.text,
                                txtMarketName.text,
                                txtOCSNumber.text)
                        } else {
                            ok = AuthController.entrar(txtPhone.text, txtPassword.text)
                        }
                    } else {
                        ok = true
                    }
                    if (ok) {
                        // Navegar para a tela do home
                        if (loginPage.StackView.view) {
                            loginPage.StackView.view.replace(Qt.resolvedUrl("HomeScreen.qml"))
                        }
                    }
                    
                }
            }
        }
    }
}