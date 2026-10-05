import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: editarPerfilPage

    font.family: "Segoe UI"

    background: Rectangle {
        color: "#FAFBFA"
    }
    
    // HEADER
    

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

            // LOGO
            Rectangle {
                Layout.preferredWidth: 220
                Layout.preferredHeight: 64

                radius: 10
                color: "#173E2D"

                Image {
                    anchors.centerIn: parent

                    width: 190
                    height: 54

                    source: Qt.resolvedUrl(
                        "../assets/AntroBege.svg"
                    )

                    fillMode: Image.PreserveAspectFit
                }
            }

            // HOME
            BotaoAntro {
                text: "⌂  Home"

                secundario: true

                Layout.preferredWidth: 140

                onClicked: {
                    var pilha = editarPerfilPage.StackView.view

                    pilha.pop(null)
                }
            }

            // PERFIL
            BotaoAntro {
                text: "♙  Perfil"

                secundario: true
                selecionado: true

                Layout.preferredWidth: 140

                onClicked: {
                    editarPerfilPage.StackView.view.pop()
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
                    var pilha = editarPerfilPage.StackView.view
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

    
    // CONTEÚDO
    

    ScrollView {
        anchors.fill: parent

        clip: true

        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width

            spacing: 22

            Item {
                Layout.preferredHeight: 12
            }

            
            // TÍTULO DA PÁGINA
            

            ColumnLayout {
                Layout.fillWidth: true

                Layout.leftMargin: 40
                Layout.rightMargin: 40

                spacing: 4

                Label {
                    text: "Alterar perfil"

                    font.pixelSize: 36
                    font.bold: true

                    color: "#17201B"
                }

                Label {
                    text:
                        "Atualize as feiras que você participa "
                        + "e os produtos que você vende."

                    font.pixelSize: 15

                    color: "#66706A"
                }
            }

            
            // CARD DAS FEIRAS
            

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 390

                Layout.leftMargin: 40
                Layout.rightMargin: 40

                color: "white"

                radius: 14

                border.color: "#E2E7E3"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 30

                    spacing: 28

                    // ícone
                    Rectangle {
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 80

                        Layout.alignment: Qt.AlignTop

                        radius: 40

                        color: "#E5EEE8"

                        Label {
                            anchors.centerIn: parent

                            text: "⌂"

                            font.pixelSize: 36

                            color: "#22543D"
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        spacing: 14

                        Label {
                            text: "Quais feiras você participa?"

                            font.pixelSize: 28
                            font.bold: true

                            color: "#17201B"
                        }

                        Label {
                            text:
                                "Selecione as feiras onde você vende seus produtos."

                            font.pixelSize: 15

                            color: "#66706A"
                        }

                        
                        // OPÇÕES DE FEIRA
                        

                        GridLayout {
                            Layout.fillWidth: true

                            columns: 4

                            rowSpacing: 12
                            columnSpacing: 12

                            // CASA FORTE
                            CheckBox {
                                id: feiraCasaForte

                                text: "Feira de Casa Forte"

                                checked:
                                    VendedorController.feiraSelecionada(
                                        "Feira de Casa Forte"
                                    )

                                Layout.fillWidth: true

                                onClicked: {
                                    VendedorController.alternarFeira(
                                        "Feira de Casa Forte"
                                    )
                                }
                            }

                            // VÁRZEA
                            CheckBox {
                                text: "Feira da Várzea"

                                checked:
                                    VendedorController.feiraSelecionada(
                                        "Feira da Várzea"
                                    )

                                Layout.fillWidth: true

                                onClicked: {
                                    VendedorController.alternarFeira(
                                        "Feira da Várzea"
                                    )
                                }
                            }

                            // UFPE
                            CheckBox {
                                text: "Feira Agro UFPE"

                                checked:
                                    VendedorController.feiraSelecionada(
                                        "Feira Agro UFPE"
                                    )

                                Layout.fillWidth: true

                                onClicked: {
                                    VendedorController.alternarFeira(
                                        "Feira Agro UFPE"
                                    )
                                }
                            }

                            // BOA VIAGEM
                            CheckBox {
                                text: "Feira de Boa Viagem"

                                checked:
                                    VendedorController.feiraSelecionada(
                                        "Feira de Boa Viagem"
                                    )

                                Layout.fillWidth: true

                                onClicked: {
                                    VendedorController.alternarFeira(
                                        "Feira de Boa Viagem"
                                    )
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1

                            color: "#E2E7E3"
                        }

                        Label {
                            text: "Não encontrou sua feira?"

                            font.pixelSize: 16
                            font.bold: true

                            color: "#17201B"
                        }

                        Label {
                            text:
                                "Adicione o nome da feira que você participa."

                            font.pixelSize: 13

                            color: "#66706A"
                        }

                        // INPUT DA NOVA FEIRA
                        RowLayout {
                            Layout.fillWidth: true

                            spacing: 14

                            TextField {
                                id: campoNovaFeira

                                Layout.fillWidth: true

                                implicitHeight: 50

                                placeholderText:
                                    "Digite o nome da feira"

                                leftPadding: 16

                                color: "#17201B"

                                placeholderTextColor:
                                    "#9AA39E"

                                background: Rectangle {
                                    radius: 8

                                    color: "white"

                                    border.color:
                                        "#CFD7D2"
                                }

                                onAccepted: {
                                    if (
                                        VendedorController.adicionarFeira(
                                            campoNovaFeira.text
                                        )
                                    ) {
                                        campoNovaFeira.clear()
                                    }
                                }
                            }

                            BotaoAntro {
                                text: "Adicionar"

                                Layout.preferredWidth: 150

                                onClicked: {
                                    if (
                                        VendedorController.adicionarFeira(
                                            campoNovaFeira.text
                                        )
                                    ) {
                                        campoNovaFeira.clear()
                                    }
                                }
                            }
                        }
                    }
                }
            }

            
            // CARD DOS PRODUTOS
            

            Rectangle {
                Layout.fillWidth: true

                Layout.leftMargin: 40
                Layout.rightMargin: 40
                Layout.bottomMargin: 40

                Layout.preferredHeight: 330

                color: "white"

                radius: 14

                border.color: "#E2E7E3"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 30

                    spacing: 28

                    // ÍCONE
                    Rectangle {
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 80

                        Layout.alignment: Qt.AlignTop

                        radius: 40

                        color: "#E5EEE8"

                        Label {
                            anchors.centerIn: parent

                            text: "●"

                            font.pixelSize: 34

                            color: "#22543D"
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        spacing: 14

                        Label {
                            text: "Frutas com que você trabalha"

                            font.pixelSize: 28
                            font.bold: true

                            color: "#17201B"
                        }

                        Label {
                            text:
                                "Adicione os produtos que você vende. "
                                + "Digite um item por vez e clique em “+”."

                            font.pixelSize: 15

                            color: "#66706A"
                        }

                        
                        // INPUT PRODUTO
                        

                        RowLayout {
                            Layout.fillWidth: true

                            spacing: 14

                            TextField {
                                id: campoProduto

                                Layout.fillWidth: true

                                implicitHeight: 50

                                placeholderText:
                                    "Digite o nome da fruta ou produto"

                                leftPadding: 16

                                color: "#17201B"

                                placeholderTextColor:
                                    "#9AA39E"

                                background: Rectangle {
                                    radius: 8

                                    color: "white"

                                    border.color:
                                        "#CFD7D2"
                                }

                                onAccepted: {
                                    if (
                                        VendedorController.adicionarProduto(
                                            campoProduto.text
                                        )
                                    ) {
                                        campoProduto.clear()
                                    }
                                }
                            }

                            Button {
                                implicitWidth: 54
                                implicitHeight: 54

                                hoverEnabled: true

                                background: Rectangle {
                                    radius: 27

                                    color:
                                        parent.hovered
                                        ? "#286149"
                                        : "#22543D"
                                }

                                contentItem: Text {
                                    text: "+"

                                    color: "white"

                                    font.pixelSize: 28

                                    horizontalAlignment:
                                        Text.AlignHCenter

                                    verticalAlignment:
                                        Text.AlignVCenter
                                }

                                onClicked: {
                                    if (
                                        VendedorController.adicionarProduto(
                                            campoProduto.text
                                        )
                                    ) {
                                        campoProduto.clear()
                                    }
                                }
                            }
                        }

                        
                        // PRODUTOS ADICIONADOS
                        

                        Flow {
                            Layout.fillWidth: true

                            spacing: 10

                            Repeater {
                                model: VendedorController.produtos

                                delegate: Rectangle {
                                    required property int index
                                    required property var modelData

                                    width:
                                        textoProduto.implicitWidth + 48

                                    height: 42

                                    radius: 10

                                    color: "#EDF4EF"

                                    Row {
                                        anchors.centerIn: parent

                                        spacing: 10

                                        Label {
                                            id: textoProduto

                                            text: modelData.nome

                                            font.pixelSize: 14

                                            color: "#22543D"
                                        }

                                        Button {
                                            width: 22
                                            height: 22

                                            flat: true

                                            background:
                                                Rectangle {
                                                    color:
                                                        "transparent"
                                                }

                                            contentItem: Text {
                                                text: "×"

                                                color:
                                                    "#738078"

                                                font.pixelSize:
                                                    18

                                                horizontalAlignment:
                                                    Text.AlignHCenter

                                                verticalAlignment:
                                                    Text.AlignVCenter
                                            }

                                            onClicked: {
                                                VendedorController.removerProduto(
                                                    index
                                                )
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1

                            color: "#E2E7E3"
                        }

                        
                        // AVANÇAR
                        

                        RowLayout {
                            Layout.fillWidth: true

                            BotaoAntro {
                                text: "Avançar"

                                Layout.preferredWidth: 240

                                onClicked: {
                                    editarPerfilPage.StackView.view.push(
                                        Qt.resolvedUrl("DefinirPrecosVendedorScreen.qml")
                                    )
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }
}