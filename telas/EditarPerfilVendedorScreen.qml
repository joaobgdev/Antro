import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: editarPerfilPage

    property var produtos: []
    property var feiras: []
    property var feirasEscolhidas: []
    property var produtoSelecionado: produtos[seletorProduto.currentIndex] || ({})
    property string aviso: ""

    function atualizar() {
        var id = produtoSelecionado.id
        produtos = CompradorController.produtosDoFeirante(AuthController.telefoneUsuario)
        feiras = CompradorController.feiras()
        for (var i = 0; i < produtos.length; i++) {
            if (produtos[i].id === id) seletorProduto.currentIndex = i
        }
        selecionarProduto()
    }

    function selecionarProduto() {
        var produto = produtos[seletorProduto.currentIndex] || ({})
        feirasEscolhidas = produto.feiraIds ? produto.feiraIds.slice() : []
    }

    function salvarFeiras() {
        if (!produtoSelecionado.id) { aviso = "Selecione um produto."; return }
        aviso = CompradorController.editarProdutoFeirante(
                    AuthController.telefoneUsuario, produtoSelecionado.id,
                    produtoSelecionado.preco, produtoSelecionado.estoque,
                    produtoSelecionado.estoque, feirasEscolhidas)
        if (aviso.length === 0) aviso = "Feiras do produto atualizadas."
    }

    function adicionarFeira() {
        aviso = CompradorController.adicionarFeira(campoNovaFeira.text)
        if (aviso.length === 0) campoNovaFeira.clear()
    }

    function adicionarProduto() {
        editarPerfilPage.StackView.view.push(Qt.resolvedUrl("ProdutoFeiranteScreen.qml"), {
            produto: {nome: campoProduto.text.trimmed()}
        })
    }

    function removerProduto(id) {
        aviso = CompradorController.removerProdutoFeirante(AuthController.telefoneUsuario, id)
                ? "" : CompradorController.ultimoErro()
    }

    Component.onCompleted: atualizar()
    StackView.onActivated: atualizar()
    Connections {
        target: CompradorController
        function onProdutosChanged() { editarPerfilPage.atualizar() }
    }

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

            
            Label {
                text: editarPerfilPage.aviso
                visible: text.length > 0
                Layout.fillWidth: true
                Layout.leftMargin: 40
                Layout.rightMargin: 40
                wrapMode: Text.WordWrap
                color: "#22543D"
            }

            // CARD DAS FEIRAS
            

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: conteudoFeiras.implicitHeight + 60

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
                        id: conteudoFeiras
                        Layout.fillWidth: true

                        spacing: 14

                        Label {
                            text: "Em quais feiras você vende?"

                            font.pixelSize: 28
                            font.bold: true

                            color: "#17201B"
                        }

                        Label {
                            text:
                                "Escolha o produto e selecione as feiras onde ele é vendido."

                            font.pixelSize: 15

                            color: "#66706A"
                        }

                        
                        // OPÇÕES DE FEIRA
                        

                        ComboBox {
                            id: seletorProduto
                            Layout.fillWidth: true
                            model: editarPerfilPage.produtos
                            textRole: "nome"
                            enabled: count > 0
                            onCurrentIndexChanged: editarPerfilPage.selecionarProduto()
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 2
                            rowSpacing: 12
                            columnSpacing: 12

                            Repeater {
                                model: editarPerfilPage.feiras
                                delegate: CheckBox {
                                    required property var modelData
                                    Layout.fillWidth: true
                                    text: modelData.nome
                                    enabled: !!editarPerfilPage.produtoSelecionado.id
                                    checked: editarPerfilPage.feirasEscolhidas.indexOf(modelData.id) >= 0
                                    onToggled: {
                                        var ids = editarPerfilPage.feirasEscolhidas.filter(function(id) {
                                            return id !== modelData.id
                                        })
                                        if (checked) ids.push(modelData.id)
                                        editarPerfilPage.feirasEscolhidas = ids
                                    }
                                    contentItem: Text {
                                        text: parent.text
                                        leftPadding: parent.indicator.width + parent.spacing
                                        verticalAlignment: Text.AlignVCenter
                                        wrapMode: Text.WordWrap
                                        color: "#22543D"
                                    }
                                }
                            }
                        }

                        BotaoAntro {
                            text: "Salvar feiras do produto"
                            enabled: !!editarPerfilPage.produtoSelecionado.id
                            onClicked: editarPerfilPage.salvarFeiras()
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
                                    editarPerfilPage.adicionarFeira()
                                }
                            }

                            BotaoAntro {
                                text: "Adicionar"

                                Layout.preferredWidth: 150

                                onClicked: {
                                    editarPerfilPage.adicionarFeira()
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

                Layout.preferredHeight: conteudoProdutos.implicitHeight + 60

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
                        id: conteudoProdutos
                        Layout.fillWidth: true

                        spacing: 14

                        Label {
                            text: "Produtos com que você trabalha"

                            font.pixelSize: 28
                            font.bold: true

                            color: "#17201B"
                        }

                        Label {
                            text:
                                "Adicione os produtos que você vende. "
                                + "Clique em “+” para cadastrar ou no nome de um produto para editar."

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
                                    editarPerfilPage.adicionarProduto()
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
                                    editarPerfilPage.adicionarProduto()
                                }
                            }
                        }

                        
                        // PRODUTOS ADICIONADOS
                        

                        Flow {
                            Layout.fillWidth: true

                            spacing: 10

                            Repeater {
                                model: editarPerfilPage.produtos

                                delegate: Rectangle {
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
                                            MouseArea {
                                                anchors.fill: parent
                                                cursorShape: Qt.PointingHandCursor
                                                onClicked: editarPerfilPage.StackView.view.push(
                                                               Qt.resolvedUrl("ProdutoFeiranteScreen.qml"), {produto: modelData})
                                            }

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
                                                editarPerfilPage.removerProduto(modelData.id)
                                            }
                                        }
                                    }
                                }
                            }
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
                                        Qt.resolvedUrl("DefinirPrecosVendedorScreen.qml"), {viaEdicao: true}
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