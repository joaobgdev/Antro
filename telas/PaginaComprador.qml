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
    property string aba: ""
    readonly property bool feirante: AuthController.perfilUsuario === "feirante"
    font.family: "Segoe UI"
    background: Rectangle { color: "#FAFBFA" }

    function abrir(tela) {
        var pilha = pagina.StackView.view
        if (pilha) pilha.push(Qt.resolvedUrl(tela))
    }

    header: Rectangle {
        implicitHeight: pagina.width < 1050 ? 146 : 92
        color: "white"
        border.color: "#E2E7E3"
        GridLayout {
            columns: pagina.width < 1050 ? 1 : 2
            anchors.fill: parent
            anchors.margins: 16
            rowSpacing: 6
            columnSpacing: 12
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                LogoAntro { escala: pagina.width < 950 ? 0.75 : 1 }
                BotaoAntro {
                    objectName: "inicio"
                    text: "Home"
                    secundario: true
                    selecionado: pagina.inicio
                    onClicked: if (!pagina.inicio && pagina.StackView.view) pagina.StackView.view.pop(null)
                }
                BotaoAntro {
                    text: "Voltar"
                    secundario: true
                    visible: pagina.mostrarVoltar
                    onClicked: pagina.StackView.view.pop()
                }
                Item { Layout.fillWidth: true }
                Label {
                    visible: pagina.width >= 1100
                    text: AuthController.nomeUsuario
                    color: "#59635E"
                    elide: Text.ElideRight
                    Layout.maximumWidth: 180
                }

            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                BotaoAntro {
                    text: pagina.feirante ? "Pedidos" : "Minhas reservas"
                    objectName: "reservas"
                    secundario: true
                    selecionado: pagina.aba === "reservas"
                    onClicked: if (pagina.aba !== "reservas") pagina.abrir(pagina.feirante ? "PedidosVendedorScreen.qml" : "MinhasReservasScreen.qml")
                }
                BotaoAntro {
                    text: "Perfil"
                    objectName: "perfil"
                    visible: pagina.feirante
                    secundario: true
                    selecionado: pagina.aba === "perfil"
                    onClicked: if (pagina.aba !== "perfil") pagina.abrir("PerfilVendedorScreen.qml")
                }
                BotaoAntro {
                    objectName: "carrinho"
                    text: "Carrinho (" + CompradorController.tiposNaSacola + ")"
                    visible: !pagina.feirante && pagina.mostrarSacola
                    secundario: true
                    onClicked: pagina.abrir("SacolaScreen.qml")
                }
                Item { Layout.fillWidth: true }
                BotaoAntro {
                    objectName: "sair"
                    text: "Sair"
                    secundario: true
                    onClicked: {
                        var pilha = pagina.StackView.view
                        var login = Qt.resolvedUrl("LoginScreen.qml")
                        AuthController.sair()
                        pilha.clear()
                        pilha.push(login)
                    }
                }
            }
        }
    }
}
