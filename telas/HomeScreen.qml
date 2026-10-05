import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: homePage
    readonly property bool isFarmer: AuthController.perfilUsuario === "feirante"
    inicio: true
    mostrarVoltar: false
    mostrarSacola: !isFarmer
    property var meusProdutos: []

    function atualizarProdutos() {
        meusProdutos = isFarmer ? CompradorController.produtosDoFeirante(AuthController.telefoneUsuario) : []
    }
    Component.onCompleted: { CompradorController.atualizar(); atualizarProdutos() }
    StackView.onActivated: CompradorController.atualizar()
    property string erro: ""
    Connections {
        target: CompradorController
        function onProdutosChanged() { homePage.atualizarProdutos() }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 18
        Label {
            text: homePage.isFarmer ? "Seus produtos" : "Feiras do Recife"
            font.pixelSize: 36
            font.bold: true
            color: "#17201B"
        }
        Label { text: homePage.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        ColumnLayout {
            visible: homePage.isFarmer
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18
            RowLayout {
                Layout.fillWidth: true
                Label {
                    Layout.fillWidth: true
                    text: homePage.meusProdutos.length === 0 ? "Você ainda não cadastrou nenhum produto." : "Produtos cadastrados: " + homePage.meusProdutos.length
                    color: "#66706A"
                }
                BotaoAntro { text: "Adicionar produto"; onClicked: homePage.StackView.view.push(Qt.resolvedUrl("ProdutoFeiranteScreen.qml")) }
            }
            ListView {
                id: listaMeusProdutos
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                spacing: 18
                model: homePage.meusProdutos
                ScrollBar.vertical: ScrollBar {}
                delegate: Rectangle {
                    id: produtoCard
                    required property var modelData
                    width: listaMeusProdutos.width
                    height: produtoLinha.implicitHeight + 56
                    color: "white"
                    radius: 14
                    border.color: "#E4E8E5"
                    RowLayout {
                        id: produtoLinha
                        anchors.fill: parent
                        anchors.margins: 28
                        ColumnLayout {
                            Layout.fillWidth: true
                            Label { text: produtoCard.modelData.nome; font.bold: true; font.pixelSize: 24; color: "#22543D" }
                            Label { text: "R$ " + Number(produtoCard.modelData.preco).toLocaleString(Qt.locale("pt_BR"), 'f', 2) + " / " + produtoCard.modelData.unidade + " · estoque: " + produtoCard.modelData.estoque + " " + produtoCard.modelData.unidade }
                            Label { text: "Feiras: " + produtoCard.modelData.feiras; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        }
                        BotaoAntro { text: "Editar"; secundario: true; onClicked: homePage.StackView.view.push(Qt.resolvedUrl("ProdutoFeiranteScreen.qml"), {produto: produtoCard.modelData}) }
                        BotaoAntro { secundario: true; text: "Remover"; onClicked: {
                            homePage.erro = CompradorController.removerProdutoFeirante(AuthController.telefoneUsuario, produtoCard.modelData.id) ? "" : CompradorController.ultimoErro()
                        } }
                    }
                }
            }
        }
        Label {
            visible: !homePage.isFarmer
            text: "Escolha uma feira para conhecer os vendedores e seus produtos."
            color: "#66706A"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        ListView {
            id: listaFeiras
            visible: !homePage.isFarmer
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 18
            model: CompradorController.feiras()
            Connections { target: CompradorController; function onProdutosChanged() { listaFeiras.model = CompradorController.feiras() } }
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                required property var modelData
                width: listaFeiras.width
                height: conteudo.implicitHeight + 56
                color: "white"
                radius: 14
                border.color: "#E4E8E5"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 28
                    Rectangle {
                        Layout.preferredWidth: 72
                        Layout.preferredHeight: 72
                        radius: 36
                        color: "#E5EEE8"
                        Label { anchors.centerIn: parent; text: "⌂"; font.pixelSize: 36; color: "#22543D" }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.bairro; color: "#66706A"; font.pixelSize: 13 }
                        Label { text: modelData.nome; font.bold: true; font.pixelSize: 24; color: "#22543D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: modelData.local + " · " + modelData.horario; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    }
                    BotaoAntro {
                        text: "Ver vendedores"
                        onClicked: homePage.StackView.view.push(Qt.resolvedUrl("FeiraScreen.qml"), {feiraId: modelData.id})
                    }
                }
            }
        }
    }
}
