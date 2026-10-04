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

    
    // DADOS TEMPORÁRIOS
    // Depois substituiremos por um VendedorController em C++
    

    ListModel {
        id: pedidosPendentes

        ListElement {
            pedidoId: 1024
            cliente: "Ana Silva"
            quantidadeItens: 3
            total: 27.50
            horario: "Hoje, 09:12"
        }

        ListElement {
            pedidoId: 1025
            cliente: "Carlos Mendes"
            quantidadeItens: 2
            total: 42.00
            horario: "Hoje, 10:03"
        }

        ListElement {
            pedidoId: 1026
            cliente: "Mariana Oliveira"
            quantidadeItens: 3
            total: 18.90
            horario: "Hoje, 11:20"
        }

        ListElement {
            pedidoId: 1027
            cliente: "João Pereira"
            quantidadeItens: 3
            total: 36.00
            horario: "Hoje, 13:45"
        }
    }

    ListModel {
        id: pedidosConfirmados

        ListElement {
            pedidoId: 1018
            cliente: "Fernanda Costa"
            horario: "08:21"
        }

        ListElement {
            pedidoId: 1017
            cliente: "Ricardo Almeida"
            horario: "08:50"
        }

        ListElement {
            pedidoId: 1016
            cliente: "Juliana Santos"
            horario: "09:15"
        }

        ListElement {
            pedidoId: 1015
            cliente: "Pedro Henrique"
            horario: "09:42"
        }

        ListElement {
            pedidoId: 1014
            cliente: "Camila Rocha"
            horario: "10:18"
        }

        ListElement {
            pedidoId: 1013
            cliente: "Lucas Martins"
            horario: "10:37"
        }
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
                    anchors.margins: 12

                    source: Qt.resolvedUrl("../assets/AntroBege.svg")

                    fillMode: Image.PreserveAspectFit
                }
            }

            // Home
            BotaoAntro {
                text: "⌂  Home"

                secundario: true
                selecionado: true

                Layout.preferredWidth: 140
            }

            // Perfil
            BotaoAntro {
                text: "♙  Perfil"

                secundario: true
                selecionado: false

                Layout.preferredWidth: 140

                onClicked: {
                    // Depois:
                    // pedidosPage.StackView.view.push(
                    //     Qt.resolvedUrl("PerfilVendedorScreen.qml")
                    // )
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

    
    // FUNÇÕES TEMPORÁRIAS
    

    function aceitarPedido(indice) {

        var pedido = pedidosPendentes.get(indice)

        pedidosConfirmados.append({
            "pedidoId": pedido.pedidoId,
            "cliente": pedido.cliente,
            "horario": pedido.horario.replace("Hoje, ", "")
        })

        pedidosPendentes.remove(indice)
    }

    function recusarPedido(indice) {

        // Por enquanto apenas removemos da lista.
        // Depois vamos chamar o C++ e alterar o status do Pedido.

        pedidosPendentes.remove(indice)
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
                    text: "Pedidos"

                    font.pixelSize: 36
                    font.bold: true

                    color: "#17201B"
                }

                Label {
                    text: "Acompanhe e confirme os pedidos recebidos pelo site."

                    font.pixelSize: 15

                    color: "#66706A"
                }
            }

            // 
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
                            text: pedidosPendentes.count + " pedidos"

                            font.pixelSize: 13

                            color: "#77817C"
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    Repeater {
                        model: pedidosPendentes

                        delegate: Rectangle {
                            required property int index
                            required property int pedidoId
                            required property string cliente
                            required property int quantidadeItens
                            required property double total
                            required property string horario

                            Layout.fillWidth: true
                            Layout.preferredHeight: 126

                            color: "white"

                            radius: 12

                            border.color: "#E2E7E3"
                            border.width: 1

                            RowLayout {
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

                                        font.pixelSize: 12

                                        color: "#9AA39E"
                                    }
                                }

                                // Recusar
                                Button {
                                    text: "Recusar"

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
                                        pedidosPage.recusarPedido(index)
                                    }
                                }

                                // Aceitar
                                BotaoAntro {
                                    text: "Aceitar"

                                    implicitWidth: 105

                                    onClicked: {
                                        pedidosPage.aceitarPedido(index)
                                    }
                                }
                            }
                        }
                    }

                    // Caso não haja pedidos
                    Rectangle {
                        visible: pedidosPendentes.count === 0

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

                    Layout.minimumHeight: 560
                    Layout.alignment: Qt.AlignTop

                    radius: 14

                    color: "#F1F6F2"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 22

                        spacing: 12

                        Label {
                            text: "Confirmados"

                            font.pixelSize: 22
                            font.bold: true

                            color: "#17201B"
                        }

                        Label {
                            text: pedidosConfirmados.count + " pedidos"

                            font.pixelSize: 13

                            color: "#77817C"
                        }

                        Item {
                            Layout.preferredHeight: 3
                        }

                        Repeater {
                            model: pedidosConfirmados

                            delegate: Rectangle {
                                required property int pedidoId
                                required property string cliente
                                required property string horario

                                Layout.fillWidth: true
                                Layout.preferredHeight: 72

                                radius: 10

                                color: "white"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 14

                                    spacing: 12

                                    // Ícone verde
                                    Rectangle {
                                        Layout.preferredWidth: 34
                                        Layout.preferredHeight: 34

                                        radius: 17

                                        color: "#E2F0E7"

                                        Label {
                                            anchors.centerIn: parent

                                            text: "✓"

                                            color: "#22543D"

                                            font.pixelSize: 17
                                            font.bold: true
                                        }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true

                                        spacing: 0

                                        // Número maior
                                        Label {
                                            text: "#" + pedidoId

                                            font.pixelSize: 20
                                            font.bold: true

                                            color: "#17201B"
                                        }

                                        Label {
                                            text: cliente

                                            font.pixelSize: 13

                                            color: "#66706A"
                                        }
                                    }

                                    Label {
                                        text: horario

                                        font.pixelSize: 12

                                        color: "#9AA39E"
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }

                        Button {
                            text: "Ver todos os confirmados"

                            Layout.fillWidth: true

                            implicitHeight: 44

                            background: Rectangle {
                                radius: 8

                                color: "transparent"

                                border.color: "#AFC5B6"
                            }

                            contentItem: Text {
                                text: parent.text

                                color: "#22543D"

                                font.pixelSize: 13

                                horizontalAlignment:
                                    Text.AlignHCenter

                                verticalAlignment:
                                    Text.AlignVCenter
                            }
                        }
                    }
                }
            }
        }
    }
}