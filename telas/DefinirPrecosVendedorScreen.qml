import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: definirPrecosPage

    font.family: "Segoe UI"

    background: Rectangle {
        color: "#FAFBFA"
    }

    property bool viaEdicao: false
    property var produtos: []
    property string aviso: ""

    function atualizar() {
        produtos = CompradorController.produtosDoFeirante(AuthController.telefoneUsuario)
    }

    function salvarPrecos() {
        aviso = ""
        var alterados = []
        for (var i = 0; i < linhasPreco.count; i++) {
            var linha = linhasPreco.itemAt(i)
            var texto = linha.precoTexto.trim().replace("R$", "").trim()
            if (texto.indexOf(",") >= 0) texto = texto.replace(/\./g, "").replace(",", ".")
            var valor = Number(texto)
            if (!isFinite(valor) || valor <= 0) {
                aviso = "Informe um preço maior que zero para " + linha.modelData.nome + "."
                return
            }
            if (valor !== linha.modelData.preco) alterados.push({produto: linha.modelData, preco: valor})
        }
        for (var j = 0; j < alterados.length; j++) {
            var produto = alterados[j].produto
            var erro = CompradorController.editarProdutoFeirante(
                        AuthController.telefoneUsuario, produto.id, alterados[j].preco,
                        produto.estoque, produto.estoque, produto.feiraIds)
            if (erro.length > 0) {
                aviso = "Não foi possível salvar " + produto.nome + ": " + erro
                return
            }
        }
        aviso = "Preços atualizados."
        atualizar()
    }

    Component.onCompleted: atualizar()
    StackView.onActivated: atualizar()

    function voltarParaPerfil() {
        var pilha = definirPrecosPage.StackView.view
        var indice = pilha.depth - (viaEdicao ? 3 : 2)
        if (indice >= 0) pilha.pop(pilha.get(indice))
    }

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

            BotaoAntro {
                text: "⌂  Home"

                secundario: true

                Layout.preferredWidth: 140

                onClicked: {
                    var pilha =
                        definirPrecosPage.StackView.view

                    if (pilha)
                        pilha.pop(null)
                }
            }

            BotaoAntro {
                text: "♙  Perfil"

                secundario: true
                selecionado: true

                Layout.preferredWidth: 140

                onClicked: {
                    definirPrecosPage.voltarParaPerfil()
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
                    var pilha =
                        definirPrecosPage.StackView.view

                    var login =
                        Qt.resolvedUrl(
                            "LoginScreen.qml"
                        )

                    AuthController.sair()

                    Qt.callLater(function() {
                        pilha.clear()
                        pilha.push(login)
                    })
                }
            }
        }
    }

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

            ColumnLayout {
                Layout.fillWidth: true

                Layout.leftMargin: 40
                Layout.rightMargin: 40

                spacing: 4

                Label {
                    text: "Definir preços"

                    font.pixelSize: 36
                    font.bold: true

                    color: "#17201B"
                }

                Label {
                    text:
                        "Confira como cada produto é vendido "
                        + "e informe o valor correspondente."

                    font.pixelSize: 15

                    color: "#66706A"

                    wrapMode: Text.WordWrap

                    Layout.fillWidth: true
                }
            }

            Label {
                text: definirPrecosPage.aviso
                visible: text.length > 0
                Layout.fillWidth: true
                Layout.leftMargin: 40
                Layout.rightMargin: 40
                wrapMode: Text.WordWrap
                color: "#22543D"
            }

            Rectangle {
                id: cardPrincipal

                Layout.fillWidth: true

                Layout.leftMargin: 40
                Layout.rightMargin: 40
                Layout.bottomMargin: 40

                Layout.preferredHeight:
                    conteudoCard.implicitHeight + 40

                color: "white"

                radius: 14

                border.color: "#E2E7E3"
                border.width: 1

                ColumnLayout {
                    id: conteudoCard

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top

                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.topMargin: 20

                    spacing: 18

                    RowLayout {
                        Layout.fillWidth: true

                        spacing: 20

                        Rectangle {
                            Layout.preferredWidth: 72
                            Layout.preferredHeight: 72

                            radius: 36

                            color: "#E5EEE8"

                            Label {
                                anchors.centerIn: parent

                                text: "🏷"

                                font.pixelSize: 30

                                color: "#22543D"
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true

                            spacing: 2

                            Label {
                                text: "Seus produtos"

                                font.pixelSize: 28
                                font.bold: true

                                color: "#17201B"
                            }

                            Label {
                                text:
                                    "Confira a unidade de venda "
                                    + "e o preço de cada item."

                                font.pixelSize: 15

                                color: "#66706A"
                            }
                        }

                        Rectangle {
                            Layout.preferredWidth: 500
                            Layout.preferredHeight: 72

                            radius: 10

                            color: "#FAFBFA"

                            border.color: "#E2E7E3"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 14

                                spacing: 12

                                Rectangle {
                                    Layout.preferredWidth: 34
                                    Layout.preferredHeight: 34

                                    radius: 17

                                    color: "#E5EEE8"

                                    Label {
                                        anchors.centerIn: parent

                                        text: "i"

                                        font.pixelSize: 18
                                        font.bold: true

                                        color: "#22543D"
                                    }
                                }

                                Label {
                                    Layout.fillWidth: true

                                    text:
                                        "Ex.: se o produto for vendido "
                                        + "por peso, informe o preço "
                                        + "por kg."

                                    wrapMode: Text.WordWrap

                                    font.pixelSize: 14

                                    color: "#66706A"
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        spacing: 10

                        Label {
                            visible: definirPrecosPage.produtos.length === 0
                            text: "Nenhum produto cadastrado. Adicione um produto no perfil."
                            color: "#66706A"
                        }

                        Repeater {
                            id: linhasPreco
                            model: definirPrecosPage.produtos

                            delegate: Rectangle {
                                required property var modelData
                                property alias precoTexto: campoPreco.text

                                Layout.fillWidth: true
                                Layout.preferredHeight: 74

                                radius: 10

                                color: "white"

                                border.color: "#E2E7E3"
                                border.width: 1

                                RowLayout {
                                    anchors.fill: parent

                                    anchors.leftMargin: 18
                                    anchors.rightMargin: 18
                                    anchors.topMargin: 12
                                    anchors.bottomMargin: 12

                                    spacing: 18

                                    Label {
                                        Layout.preferredWidth: 220

                                        text: modelData.nome
                                        elide: Text.ElideRight

                                        font.pixelSize: 18
                                        font.bold: true

                                        color: "#17201B"
                                    }

                                    Label {
                                        Layout.preferredWidth: 105

                                        text: "Tipo de venda"

                                        font.pixelSize: 14

                                        color: "#66706A"
                                    }

                                    Button {
                                        Layout.preferredWidth: 160
                                        Layout.preferredHeight: 46

                                        hoverEnabled: true

                                        background: Rectangle {
                                            radius: 10

                                            color:
                                                !modelData.porPeso
                                                ? "#EAF3ED"
                                                : "white"

                                            border.color:
                                                !modelData.porPeso
                                                ? "#D7E7DD"
                                                : "#D9DFDB"

                                            border.width: 1
                                        }

                                        contentItem: Row {
                                            anchors.centerIn: parent

                                            spacing: 10

                                            Rectangle {
                                                width: 20
                                                height: 20

                                                radius: 10

                                                color:
                                                    !modelData.porPeso
                                                    ? "#22543D"
                                                    : "transparent"

                                                border.color:
                                                    !modelData.porPeso
                                                    ? "#22543D"
                                                    : "#7A869A"

                                                border.width: 2

                                                Rectangle {
                                                    anchors.centerIn:
                                                        parent

                                                    width: 7
                                                    height: 7

                                                    radius: 4

                                                    color: "white"

                                                    visible:
                                                        !modelData.porPeso
                                                }
                                            }

                                            Text {
                                                text: "Por unidade"

                                                color: "#22543D"

                                                font.pixelSize: 14
                                            }
                                        }

                                        enabled: false
                                    }

                                    Button {
                                        Layout.preferredWidth: 145
                                        Layout.preferredHeight: 46

                                        hoverEnabled: true

                                        background: Rectangle {
                                            radius: 10

                                            color:
                                                modelData.porPeso
                                                ? "#EAF3ED"
                                                : "white"

                                            border.color:
                                                modelData.porPeso
                                                ? "#D7E7DD"
                                                : "#D9DFDB"

                                            border.width: 1
                                        }

                                        contentItem: Row {
                                            anchors.centerIn: parent

                                            spacing: 10

                                            Rectangle {
                                                width: 20
                                                height: 20

                                                radius: 10

                                                color:
                                                    modelData.porPeso
                                                    ? "#22543D"
                                                    : "transparent"

                                                border.color:
                                                    modelData.porPeso
                                                    ? "#22543D"
                                                    : "#7A869A"

                                                border.width: 2

                                                Rectangle {
                                                    anchors.centerIn:
                                                        parent

                                                    width: 7
                                                    height: 7

                                                    radius: 4

                                                    color: "white"

                                                    visible:
                                                        modelData.porPeso
                                                }
                                            }

                                            Text {
                                                text: "Por kg"

                                                color: "#22543D"

                                                font.pixelSize: 14
                                            }
                                        }

                                        enabled: false
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }

                                    Label {
                                        text: "Valor"

                                        font.pixelSize: 14

                                        color: "#66706A"
                                    }

                                    TextField {
                                        id: campoPreco
                                        Layout.preferredWidth: 180
                                        Layout.preferredHeight: 46

                                        text:
                                            "R$ "
                                            + Number(modelData.preco)
                                                .toLocaleString(
                                                    Qt.locale("pt_BR"),
                                                    'f',
                                                    2
                                                )

                                        leftPadding: 16

                                        font.pixelSize: 14

                                        color: "#17201B"

                                        background: Rectangle {
                                            radius: 8

                                            color: "white"

                                            border.color:
                                                "#CFD7D2"

                                            border.width: 1
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

                    RowLayout {
                        Layout.fillWidth: true

                        spacing: 12

                        Button {
                            text: "Voltar"

                            Layout.preferredWidth: 170
                            Layout.preferredHeight: 48

                            background: Rectangle {
                                radius: 10

                                color: "white"

                                border.color: "#AFC5B6"
                                border.width: 1
                            }

                            contentItem: Text {
                                text: parent.text

                                color: "#22543D"

                                font.pixelSize: 14
                                font.weight: Font.DemiBold

                                horizontalAlignment:
                                    Text.AlignHCenter

                                verticalAlignment:
                                    Text.AlignVCenter
                            }

                            onClicked: {
                                definirPrecosPage
                                    .StackView.view.pop()
                            }
                        }

                        BotaoAntro {
                            text: "Salvar"

                            Layout.preferredWidth: 170

                            onClicked: {
                                definirPrecosPage.salvarPrecos()
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    Item {
                        Layout.preferredHeight: 2
                    }
                }
            }
        }
    }
}