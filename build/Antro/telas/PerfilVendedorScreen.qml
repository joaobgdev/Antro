import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: perfilPage

    font.family: "Segoe UI"

    background: Rectangle {
        color: "#FAFBFA"
    }

    
    // DADOS TEMPORÁRIOS
    // Depois virão do C++ / banco de dados
    

    ListModel {
        id: feirasUsuario

        ListElement {
            nomeFeira: "Feira de Casa Forte"
        }

        ListElement {
            nomeFeira: "Feira da Várzea"
        }

        ListElement {
            nomeFeira: "Feira Agro UFPE"
        }

        ListElement {
            nomeFeira: "Feira de Boa Viagem"
        }
    }

    ListModel {
        id: produtosUsuario

        ListElement {
            nomeProduto: "Alface"
            icone: "🥬"
        }

        ListElement {
            nomeProduto: "Tomate"
            icone: "🍅"
        }

        ListElement {
            nomeProduto: "Cenoura"
            icone: "🥕"
        }

        ListElement {
            nomeProduto: "Rúcula"
            icone: "🌿"
        }

        ListElement {
            nomeProduto: "Couve"
            icone: "🥬"
        }

        ListElement {
            nomeProduto: "Ovos caipiras"
            icone: "🥚"
        }
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

            // Logo
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
                selecionado: false

                Layout.preferredWidth: 140

                onClicked: {
                    perfilPage.StackView.view.pop()
                }
            }

            // PERFIL
            BotaoAntro {
                text: "♙  Perfil"

                secundario: true
                selecionado: true

                Layout.preferredWidth: 140
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
                    var pilha = perfilPage.StackView.view
                    var login =
                        Qt.resolvedUrl("LoginScreen.qml")

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

            spacing: 24

            Item {
                Layout.preferredHeight: 12
            }

            
            // TÍTULO
            

            ColumnLayout {
                Layout.fillWidth: true

                Layout.leftMargin: 40
                Layout.rightMargin: 40

                spacing: 4

                Label {
                    text: "Perfil"

                    font.pixelSize: 36
                    font.bold: true

                    color: "#17201B"
                }

                Label {
                    text:
                        "Gerencie suas informações de vendedor "
                        + "e seus produtos."

                    font.pixelSize: 15

                    color: "#66706A"
                }
            }

            
            // CARD PRINCIPAL
            

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 540

                Layout.leftMargin: 40
                Layout.rightMargin: 40
                Layout.bottomMargin: 40

                color: "white"

                radius: 14

                border.color: "#E2E7E3"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 34

                    spacing: 28

                    
                    // CONTEÚDO SUPERIOR
                    

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        spacing: 34

                        
                        // FOTO
                        

                        ColumnLayout {
                            Layout.preferredWidth: 180
                            Layout.alignment: Qt.AlignTop

                            spacing: 10

                            Rectangle {
                                Layout.preferredWidth: 150
                                Layout.preferredHeight: 150

                                Layout.alignment:
                                    Qt.AlignHCenter

                                radius: 75

                                color: "#E5EEE8"

                                Label {
                                    anchors.centerIn: parent

                                    text: "♟"

                                    font.pixelSize: 64

                                    color: "#6D897B"
                                }

                                // Botão pequeno da câmera
                                Rectangle {
                                    width: 44
                                    height: 44

                                    anchors.right:
                                        parent.right

                                    anchors.bottom:
                                        parent.bottom

                                    radius: 22

                                    color: "#F4F7F5"

                                    border.color:
                                        "#DCE5DF"

                                    Label {
                                        anchors.centerIn: parent

                                        text: "📷"

                                        font.pixelSize: 18
                                    }
                                }
                            }

                            Label {
                                text: "Alterar foto"

                                Layout.alignment:
                                    Qt.AlignHCenter

                                font.pixelSize: 14

                                color: "#66706A"
                            }
                        }

                        
                        // INFORMAÇÕES
                        

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop

                            spacing: 24

                            Label {
                                text:
                                    AuthController.nomeUsuario

                                font.pixelSize: 30
                                font.bold: true

                                color: "#17201B"
                            }

                            Label {
                                text:
                                    "Feiras que participo:"

                                font.pixelSize: 20
                                font.bold: true

                                color: "#17201B"
                            }

                            // Feiras
                            GridLayout {
                                Layout.fillWidth: true

                                columns: 2

                                rowSpacing: 12
                                columnSpacing: 12

                                Repeater {
                                    model: feirasUsuario

                                    delegate: Rectangle {
                                        required property
                                            string nomeFeira

                                        Layout.fillWidth: true
                                        Layout.preferredHeight:
                                            52

                                        radius: 10

                                        color: "#EDF4EF"

                                        RowLayout {
                                            anchors.fill:
                                                parent

                                            anchors.leftMargin:
                                                18

                                            anchors.rightMargin:
                                                14

                                            Label {
                                                Layout.fillWidth:
                                                    true

                                                text: nomeFeira

                                                font.pixelSize:
                                                    14

                                                color:
                                                    "#22543D"
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // DIVISÓRIA
                        Rectangle {
                            Layout.preferredWidth: 1
                            Layout.fillHeight: true

                            color: "#E2E7E3"
                        }

                        
                        // PRODUTOS
                        

                        ColumnLayout {
                            Layout.preferredWidth: 390
                            Layout.alignment: Qt.AlignTop

                            spacing: 20

                            Label {
                                text: "Produtos:"

                                font.pixelSize: 22
                                font.bold: true

                                color: "#17201B"
                            }

                            GridLayout {
                                Layout.fillWidth: true

                                columns: 2

                                rowSpacing: 12
                                columnSpacing: 12

                                Repeater {
                                    model: produtosUsuario

                                    delegate: Rectangle {
                                        required property
                                            string nomeProduto

                                        required property
                                            string icone

                                        Layout.fillWidth: true
                                        Layout.preferredHeight:
                                            62

                                        radius: 10

                                        color: "#EDF4EF"

                                        RowLayout {
                                            anchors.fill:
                                                parent

                                            anchors.leftMargin:
                                                18

                                            anchors.rightMargin:
                                                14

                                            spacing: 12

                                            Label {
                                                text: icone

                                                font.pixelSize:
                                                    25
                                            }

                                            Label {
                                                Layout.fillWidth:
                                                    true

                                                text:
                                                    nomeProduto

                                                font.pixelSize:
                                                    15

                                                color:
                                                    "#22543D"
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    
                    // DIVISÓRIA HORIZONTAL
                    

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1

                        color: "#E2E7E3"
                    }

                    
                    // ALTERAR
                    

                    RowLayout {
                        Layout.fillWidth: true

                        BotaoAntro {
                            text: "Alterar"

                            Layout.preferredWidth: 240

                            onClicked: {
                                perfilPage.StackView.view.push(
                                    Qt.resolvedUrl(
                                        "EditarPerfilVendedorScreen.qml"
                                    )
                                )
                            }
                        }

                        BotaoAntro {
                            text: "Definir preços"

                            Layout.preferredWidth: 240

                            onClicked: {
                                perfilPage.StackView.view.push(
                                    Qt.resolvedUrl(
                                        "DefinirPrecosVendedorScreen.qml"
                                    )
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