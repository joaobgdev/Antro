import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: pagina
    property string titulo: "Feiras do Recife"
    property bool mostrarVoltar: true
    property bool mostrarSacola: true
    property bool inicio: false
    font.family: "Segoe UI"
    background: Rectangle { color: "#FAFBFA" }
    header: Rectangle {
        implicitHeight: 92
        color: "white"
        border.color: "#E2E7E3"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16
            LogoAntro { }
            BotaoAntro {
                text: "⌂  Home"
                secundario: true
                selecionado: pagina.inicio
                onClicked: {
                    if (!pagina.inicio && pagina.StackView.view)
                        pagina.StackView.view.pop(null)
                }
            }
            BotaoAntro {
                text: "←  Voltar"
                secundario: true
                visible: pagina.mostrarVoltar
                onClicked: pagina.StackView.view.pop()
            }
            Item { Layout.fillWidth: true }
            Label { text: AuthController.nomeUsuario; color: "#59635E"; font.pixelSize: 15 }
            BotaoAntro {
                visible: pagina.mostrarSacola
                text: "Carrinho (" + CompradorController.tiposNaSacola + ")"
                secundario: true
                onClicked: pagina.StackView.view.push(Qt.resolvedUrl("SacolaScreen.qml"))
            }
            BotaoAntro {
                text: "Sair"
                secundario: true
                onClicked: {
                    var pilha = pagina.StackView.view
                    var login = Qt.resolvedUrl("LoginScreen.qml")
                    AuthController.sair()
                    Qt.callLater(function() {
                        pilha.clear()
                        pilha.push(login)
                    })
                }
            }
        }
    }
}
