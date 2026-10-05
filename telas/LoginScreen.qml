import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: loginPage

    // Fundo
    font.family: "Segoe UI"
    background: Rectangle {
        color: "#FAFBFA"
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
        height: mainLayout.implicitHeight + 48
        color: "white"
        radius: 14
        border.color: "#E4E8E5"
        border.width: 1

        ColumnLayout {
            id: mainLayout
            anchors.fill: parent
            anchors.margins: 24
            spacing: 12

            // Logo (mesma das telas internas)
            LogoAntro {
                escala: 1.2
                Layout.alignment: Qt.AlignHCenter
            }

            // Subtítulo
            Text {
                text: "Direto do produtor para a sua mesa"
                font.pixelSize: 14
                color: "#66706A"
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }

            Item { Layout.preferredHeight: 4 }

            // Abas Entrar / Cadastrar
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                BotaoAntro {
                    text: "Entrar"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1   // larguras iguais
                    implicitHeight: 42
                    secundario: loginPage.isRegister
                    onClicked: { loginPage.mode = "entrar"; loginPage.errorMessage = "" }
                }

                BotaoAntro {
                    text: "Cadastrar"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1   // larguras iguais
                    implicitHeight: 42
                    secundario: !loginPage.isRegister
                    onClicked: { loginPage.mode = "cadastrar"; loginPage.errorMessage = "" }
                }
            }

            // Escolher Perfil (só no cadastro)
            Text {
                visible: loginPage.isRegister
                text: "Selecione o seu perfil:"
                font.pixelSize: 14
                font.bold: true
                color: "#22543D"
            }

            RowLayout {
                visible: loginPage.isRegister
                Layout.fillWidth: true
                spacing: 10

                BotaoAntro {
                    text: "Sou Comprador"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1   // larguras iguais
                    implicitHeight: 42
                    secundario: loginPage.currentRole !== "comprador"
                    onClicked: loginPage.currentRole = "comprador"
                }

                BotaoAntro {
                    text: "Sou Feirante"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1   // larguras iguais
                    implicitHeight: 42
                    secundario: loginPage.currentRole !== "feirante"
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
                color: "#17201B"
                placeholderTextColor: "#8B9590"
                background: Rectangle {
                    color: "white"
                    radius: 10
                    border.color: parent.activeFocus ? "#22543D" : "#E2E7E3"
                }
            }

            TextField {
                id: txtPhone
                placeholderText: "Número de Telefone"
                Layout.fillWidth: true
                implicitHeight: caixas_size1
                verticalAlignment: Text.AlignVCenter
                leftPadding: 12
                color: "#17201B"
                placeholderTextColor: "#8B9590"
                background: Rectangle {
                    color: "white"
                    radius: 10
                    border.color: parent.activeFocus ? "#22543D" : "#E2E7E3"
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
                color: "#17201B"
                placeholderTextColor: "#8B9590"
                background: Rectangle {
                    color: "white"
                    radius: 10
                    border.color: parent.activeFocus ? "#22543D" : "#E2E7E3"
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
                color: "#17201B"
                placeholderTextColor: "#8B9590"
                background: Rectangle {
                    color: "white"
                    radius: 10
                    border.color: parent.activeFocus ? "#22543D" : "#E2E7E3"
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
                color: "#17201B"
                placeholderTextColor: "#8B9590"
                background: Rectangle {
                    color: "white"
                    radius: 10
                    border.color: parent.activeFocus ? "#22543D" : "#E2E7E3"
                }
            }

            // Mensagem de erro
            Text {
                text: loginPage.errorMessage
                visible: text !== ""
                color: "#B3261E"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Item { Layout.preferredHeight: 6 }

            // Botão principal
            BotaoAntro {
                text: loginPage.isRegister ? "Cadastrar e Entrar" : "Entrar"
                Layout.fillWidth: true

                onClicked: {
                    loginPage.errorMessage = ""
                    let ok = false
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

                    if (ok)
                        loginPage.StackView.view.replace(Qt.resolvedUrl("HomeScreen.qml"))
                }
            }
        }
    }
}
