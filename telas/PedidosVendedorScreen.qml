import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: pedidosPage

    font.family: "Segoe UI"

    background: Rectangle {
        color: "#FAFBFA"
    }

    
    readonly property bool vendedor: AuthController.perfilUsuario === "feirante"
    property var pedidosPendentes: []
    property var pedidosConfirmados: []
    property string aviso: ""

    function atualizar() {
        if (!CompradorController.bancoPronto) {
            aviso = "O banco de dados não está disponível."
            return
        }
        var reservas = CompradorController.reservas(AuthController.telefoneUsuario, vendedor)
        var erro = CompradorController.ultimoErro()
        if (erro.length > 0) { aviso = erro; return }
        pedidosPendentes = reservas.filter(function(reserva) { return reserva.status === "SOLICITADA" })
        pedidosConfirmados = reservas.filter(function(reserva) { return reserva.status !== "SOLICITADA" })
    }

    function alterarPedido(id, status) {
        aviso = CompradorController.alterarReserva(AuthController.telefoneUsuario, vendedor, id, status)
        atualizar()
    }

    function statusTexto(status) {
        switch (status) {
        case "SOLICITADA": return "Aguardando aceite do vendedor"
        case "ACEITA": return "Reserva confirmada"
        case "RECUSADA": return "Recusada"
        case "CANCELADA": return "Cancelada"
        case "RETIRADA": return "Retirada concluída"
        default: return "Confira o status dos itens"
        }
    }

    function horarioRetirada(reserva) {
        return reserva.data ? "Retirada: " + reserva.data + " às " + reserva.hora : "Retirada não agendada"
    }

    function descricaoItens(itens) {
        return itens.map(function(item) {
            return item.nome + " · " + item.quantidade + " " + item.unidade
                    + " · " + item.feira + (!pedidosPage.vendedor ? " · " + item.vendedor : "")
                    + " · " + pedidosPage.statusTexto(item.status)
        }).join("\n")
    }

    function podeCancelar(reserva) {
        return reserva.itens.some(function(item) {
            return item.status === "SOLICITADA" || item.status === "ACEITA"
        })
    }

    Component.onCompleted: atualizar()
    StackView.onActivated: atualizar()
    Connections {
        target: CompradorController
        function onReservasChanged() { pedidosPage.atualizar() }
    }
    Timer {
        interval: 5000
        running: pedidosPage.StackView.status === StackView.Active
        repeat: true
        onTriggered: pedidosPage.atualizar()
    }

    // CABEÇALHO
    

    header: Rectangle {
        implicitHeight: 92

        color: "white"

        border.color: "#E2E7E3"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 36
            anchors.rightMargin: 36

            spacing: 18

            // Logo
            Rectangle {
                Layout.preferredWidth: 185
                Layout.preferredHeight: 56

                radius: 10
                color: "#173E2D"

                Image {
                    anchors.fill: parent
                    anchors.margins: 4

                    source: Qt.resolvedUrl("../assets/AntroBege.svg")

                    fillMode: Image.PreserveAspectFit
                }
            }

            // Home
            BotaoAntro {
                text: "⌂  Home"

                secundario: true
                selecionado: false

                Layout.preferredWidth: 140

                onClicked: pedidosPage.StackView.view.pop(null)
            }

            // Perfil
            BotaoAntro {
                text: "♙  Perfil"
                visible: pedidosPage.vendedor

                secundario: true
                selecionado: false

                Layout.preferredWidth: 140

                onClicked: {
                    pedidosPage.StackView.view.push(
                        Qt.resolvedUrl(
                            "PerfilVendedorScreen.qml"
                        )
                    )
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: AuthController.nomeUsuario

                color: "#59635E"
                font.pixelSize: 15
            }

            BotaoAntro {
                text: "Sair"
                secundario: true

                onClicked: {
                    var pilha = pedidosPage.StackView.view
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

    
    // CONTEÚDO DA PÁGINA
    

    ScrollView {
        anchors.fill: parent

        clip: true

        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width

            spacing: 24

            // Margem superior
            Item {
                Layout.preferredHeight: 12
            }

            // 
            // TÍTULO
            // 

            ColumnLayout {
                Layout.fillWidth: true

                Layout.leftMargin: 40
                Layout.rightMargin: 40

                spacing: 4

                Label {
                    text: pedidosPage.vendedor ? "Pedidos" : "Minhas reservas"

                    font.pixelSize: 36
                    font.bold: true

                    color: "#17201B"
                }

                Label {
                    text: pedidosPage.vendedor
                          ? "Acompanhe e confirme os pedidos recebidos."
                          : "Acompanhe as solicitações e o aceite de cada vendedor."

                    font.pixelSize: 15

                    color: "#66706A"
                }
            }

            // 
            Label {
                text: pedidosPage.aviso
                visible: text.length > 0
                Layout.fillWidth: true
                Layout.leftMargin: 40
                Layout.rightMargin: 40
                wrapMode: Text.WordWrap
                color: "#B3261E"
            }

            // DUAS COLUNAS
            // 

            RowLayout {
                Layout.fillWidth: true

                Layout.leftMargin: 40
                Layout.rightMargin: 40
                Layout.bottomMargin: 40

                spacing: 26

                
                // PEDIDOS PENDENTES
                

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop

                    spacing: 14

                    RowLayout {
                        Layout.fillWidth: true

                        Label {
                            text: "Pendentes"

                            font.pixelSize: 20
                            font.bold: true

                            color: "#17201B"
                        }

                        Label {
                            text: pedidosPage.pedidosPendentes.length + " pedidos"

                            font.pixelSize: 18
                            font.bold: true

                            color: "#22543D"
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    Repeater {
                        model: pedidosPage.pedidosPendentes

                        delegate: Rectangle {
                            required property var modelData
                            readonly property int pedidoId: modelData.id
                            readonly property string cliente: pedidosPage.vendedor ? modelData.comprador : "Solicitação enviada"
                            readonly property int quantidadeItens: modelData.itens.length
                            readonly property double total: modelData.total
                            readonly property string horario: pedidosPage.horarioRetirada(modelData)

                            Layout.fillWidth: true
                            Layout.preferredHeight: dadosPedido.implicitHeight + 44

                            color: "white"

                            radius: 12

                            border.color: "#E2E7E3"
                            border.width: 1

                            RowLayout {
                                id: dadosPedido
                                anchors.fill: parent
                                anchors.margins: 22

                                spacing: 20

                                // Dados do pedido
                                ColumnLayout {
                                    Layout.fillWidth: true

                                    spacing: 5

                                    RowLayout {
                                        spacing: 10

                                        Label {
                                            text: "#" + pedidoId

                                            font.pixelSize: 15
                                            font.bold: true

                                            color: "#22543D"
                                        }

                                        Label {
                                            text: cliente

                                            font.pixelSize: 17
                                            font.bold: true

                                            color: "#17201B"
                                        }
                                    }

                                    Label {
                                        text:
                                            quantidadeItens
                                            + (quantidadeItens === 1
                                               ? " item"
                                               : " itens")
                                            + "   •   R$ "
                                            + Number(total).toLocaleString(
                                                Qt.locale("pt_BR"),
                                                'f',
                                                2
                                            )

                                        font.pixelSize: 14

                                        color: "#66706A"
                                    }

                                    Label {
                                        text: horario
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap

                                        font.pixelSize: 12

                                        color: "#9AA39E"
                                    }
                                    Label {
                                        text: pedidosPage.descricaoItens(modelData.itens)
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap
                                        font.pixelSize: 13
                                        color: "#66706A"
                                    }
                                }

                                // Recusar
                                Button {
                                    text: pedidosPage.vendedor ? "Recusar" : "Cancelar"

                                    implicitWidth: 105
                                    implicitHeight: 44

                                    hoverEnabled: true

                                    background: Rectangle {
                                        radius: 8

                                        color:
                                            parent.hovered
                                            ? "#FFF5F5"
                                            : "white"

                                        border.color: "#EF6A6A"
                                        border.width: 1
                                    }

                                    contentItem: Text {
                                        text: parent.text

                                        color: "#D83F3F"

                                        font.pixelSize: 14
                                        font.weight: Font.DemiBold

                                        horizontalAlignment:
                                            Text.AlignHCenter

                                        verticalAlignment:
                                            Text.AlignVCenter
                                    }

                                    onClicked: {
                                        pedidosPage.alterarPedido(pedidoId, pedidosPage.vendedor ? "RECUSADA" : "CANCELADA")
                                    }
                                }

                                // Aceitar
                                BotaoAntro {
                                    text: "Aceitar"
                                    visible: pedidosPage.vendedor

                                    implicitWidth: 105

                                    onClicked: {
                                        pedidosPage.alterarPedido(pedidoId, "ACEITA")
                                    }
                                }
                            }
                        }
                    }

                    // Caso não haja pedidos
                    Rectangle {
                        visible: pedidosPage.pedidosPendentes.length === 0

                        Layout.fillWidth: true
                        Layout.preferredHeight: 120

                        color: "white"

                        radius: 12

                        border.color: "#E2E7E3"

                        Label {
                            anchors.centerIn: parent

                            text: "Nenhum pedido aguardando confirmação."

                            color: "#77817C"

                            font.pixelSize: 15
                        }
                    }
                }

                
                // CONFIRMADOS
                
                Rectangle {
                    Layout.preferredWidth: 380
                    Layout.preferredHeight: 600
                    Layout.alignment: Qt.AlignTop

                    radius: 14
                    color: "#F1F6F2"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 22
                        spacing: 12

                        Label {
                            text: "Confirmados e histórico"
                            font.pixelSize: 22
                            font.bold: true
                            color: "#17201B"
                        }

                        // quantidade maior
                        Label {
                            text: pedidosPage.pedidosConfirmados.length + (pedidosPage.pedidosConfirmados.length === 1 ? " pedido" : " pedidos")
                            font.pixelSize: 18
                            font.bold: true
                            color: "#22543D"
                        }

                        Item {
                            Layout.preferredHeight: 3
                        }

                        ListView {
                            id: listaConfirmados

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            clip: true
                            spacing: 12
                            model: pedidosPage.pedidosConfirmados

                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AsNeeded
                            }

                            delegate: Rectangle {
                                id: pedidoHistorico
                                required property var modelData

                                width: listaConfirmados.width
                                height: resumoHistorico.implicitHeight + 28
                                radius: 10
                                color: "white"

                                ColumnLayout {
                                    id: resumoHistorico
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.top: parent.top
                                    anchors.margins: 14
                                    spacing: 10

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 12
                                        Rectangle {
                                            Layout.preferredWidth: 34
                                            Layout.preferredHeight: 34
                                            radius: 17
                                            color: "#E2F0E7"
                                            Label {
                                                anchors.centerIn: parent
                                                text: pedidoHistorico.modelData.status === "ACEITA" || pedidoHistorico.modelData.status === "RETIRADA" ? "✓" : "•"
                                                color: "#22543D"
                                                font.pixelSize: 17
                                                font.bold: true
                                            }
                                        }
                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 0
                                            Label {
                                                text: "#" + pedidoHistorico.modelData.id
                                                font.pixelSize: 20
                                                font.bold: true
                                                color: "#17201B"
                                            }
                                            Label {
                                                text: pedidosPage.vendedor ? pedidoHistorico.modelData.comprador : pedidosPage.statusTexto(pedidoHistorico.modelData.status)
                                                Layout.fillWidth: true
                                                wrapMode: Text.WordWrap
                                                font.pixelSize: 13
                                                color: "#66706A"
                                            }
                                        }
                                    }
                                    Label {
                                        text: pedidosPage.horarioRetirada(pedidoHistorico.modelData)
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap
                                        font.pixelSize: 12
                                        color: "#9AA39E"
                                    }
                                    Label {
                                        text: pedidosPage.descricaoItens(pedidoHistorico.modelData.itens)
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap
                                        font.pixelSize: 13
                                        color: "#66706A"
                                    }
                                    Label {
                                        text: "Total estimado: R$ " + Number(pedidoHistorico.modelData.total).toLocaleString(Qt.locale("pt_BR"), 'f', 2)
                                        font.pixelSize: 14
                                        color: "#22543D"
                                    }
                                    BotaoAntro {
                                        text: "Marcar retirada"
                                        visible: pedidosPage.vendedor && pedidoHistorico.modelData.status === "ACEITA"
                                        Layout.fillWidth: true
                                        onClicked: pedidosPage.alterarPedido(pedidoHistorico.modelData.id, "RETIRADA")
                                    }
                                    BotaoAntro {
                                        text: "Cancelar itens pendentes"
                                        secundario: true
                                        visible: !pedidosPage.vendedor && pedidosPage.podeCancelar(pedidoHistorico.modelData)
                                        Layout.fillWidth: true
                                        onClicked: pedidosPage.alterarPedido(pedidoHistorico.modelData.id, "CANCELADA")
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
