import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: loginPage

    font.family: "Segoe UI"
    background: Rectangle {
        color: "#FAFBFA"
    }

    property int caixas_size1: 34
    property string currentRole: "comprador"
    property string mode: "entrar"
    readonly property bool isRegister: mode === "cadastrar"
    property string errorMessage: ""

    Connections {
        target: AuthController
        function onFalhaAutenticacao(mensagem) { loginPage.errorMessage = mensagem }
    }

    ScrollView {
        id: rolagem
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true
        ColumnLayout {
            width: parent.width
            height: Math.max(implicitHeight, rolagem.availableHeight)
            Item { Layout.fillHeight: true }
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: Math.min(440, loginPage.width - 40)
                Layout.preferredHeight: mainLayout.implicitHeight + 48
                Layout.topMargin: 20
                Layout.bottomMargin: 20
                color: "white"
                radius: 14
                border.color: "#E4E8E5"
                border.width: 1

                ColumnLayout {
                    id: mainLayout
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 12

                    LogoAntro {
                        escala: 1.2
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: "Direto do produtor para a sua mesa"
                        font.pixelSize: 14
                        color: "#66706A"
                        font.weight: Font.Medium
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Item { Layout.preferredHeight: 4 }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        BotaoAntro {
                            text: "Entrar"
                            Layout.fillWidth: true
                            Layout.preferredWidth: 1
                            implicitHeight: 42
                            secundario: loginPage.isRegister
                            onClicked: { loginPage.mode = "entrar"; loginPage.errorMessage = "" }
                        }

                        BotaoAntro {
                            objectName: "abaCadastro"
                            text: "Cadastrar"
                            Layout.fillWidth: true
                            Layout.preferredWidth: 1
                            implicitHeight: 42
                            secundario: !loginPage.isRegister
                            onClicked: { loginPage.mode = "cadastrar"; loginPage.errorMessage = "" }
                        }
                    }

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
                            objectName: "perfilComprador"
                            text: "Sou Comprador"
                            Layout.fillWidth: true
                            Layout.preferredWidth: 1
                            implicitHeight: 42
                            secundario: loginPage.currentRole !== "comprador"
                            onClicked: loginPage.currentRole = "comprador"
                        }

                        BotaoAntro {
                            objectName: "perfilFeirante"
                            text: "Sou Feirante"
                            Layout.fillWidth: true
                            Layout.preferredWidth: 1
                            implicitHeight: 42
                            secundario: loginPage.currentRole !== "feirante"
                            onClicked: loginPage.currentRole = "feirante"
                        }
                    }

                    TextField {
                        id: txtName
                        objectName: "nomeCadastro"
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
                        objectName: "telefone"
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
                        objectName: "senha"
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
                        objectName: "bancaCadastro"
                        placeholderText: "Nome da banca"
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
                        objectName: "ocsCadastro"
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

                    Text {
                        text: loginPage.errorMessage
                        visible: text !== ""
                        color: "#B3261E"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Item { Layout.preferredHeight: 6 }

                    BotaoAntro {
                        objectName: "entrar"
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

                            if (ok) loginPage.StackView.view.replace(Qt.resolvedUrl("HomeScreen.qml"))
                        }
                    }
                }
            }
            Item { Layout.fillHeight: true }
        }
    }
}
