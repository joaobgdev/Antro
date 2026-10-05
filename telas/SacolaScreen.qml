import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: carrinhoPage
    titulo: "Carrinho"
    mostrarSacola: false
    property string aviso: ""

    function dinheiro(valor) { return "R$ " + Number(valor).toLocaleString(Qt.locale("pt_BR"), 'f', 2) }
    function quantidadeTexto(item) { return Number(item.quantidade).toLocaleString(Qt.locale("pt_BR"), 'f', item.passo < 1 ? 1 : 0) }

    function finalizar() {
        var resultado = CompradorController.finalizarReserva(AuthController.telefoneUsuario, AuthController.nomeUsuario)
        if (!resultado.ok) {
            carrinhoPage.aviso = resultado.erro
            return
        }
        var pilha = carrinhoPage.StackView.view
        pilha.pop(null)   // volta à home e abre o feedback por cima, sem deixar o carrinho na pilha
        pilha.push(Qt.resolvedUrl("ReservaConfirmadaScreen.qml"), {resultado: resultado})
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 18
        Label { text: "Meu carrinho"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
        Label {
            text: "Confira os itens e finalize a reserva. O valor é uma estimativa: produtos por peso podem variar na pesagem. Não há pagamento pelo aplicativo."
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            color: "#66706A"
        }
        Label { text: carrinhoPage.aviso; visible: text.length > 0; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#B3261E" }
        Label { visible: CompradorController.tiposNaSacola === 0; text: "Seu carrinho está vazio. Volte para escolher produtos." }
        ListView {
            id: listaSacola
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 18
            model: CompradorController.sacola
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                id: linha
                required property var modelData
                width: listaSacola.width
                height: conteudo.implicitHeight + 56
                color: "white"
                radius: 14
                border.color: "#E4E8E5"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 28
                    spacing: 12
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: linha.modelData.nome; font.bold: true; font.pixelSize: 24; color: "#22543D" }
                        Label { text: linha.modelData.feira + " · " + linha.modelData.vendedor; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: carrinhoPage.dinheiro(linha.modelData.preco) + " / " + linha.modelData.unidade; color: "#66706A" }
                    }
                    BotaoAntro {
                        text: "−"
                        secundario: true
                        leftPadding: 0; rightPadding: 0
                        Layout.preferredWidth: 48
                        enabled: linha.modelData.quantidade - linha.modelData.passo > 0
                        onClicked: CompradorController.alterarQuantidade(linha.modelData.indice, linha.modelData.quantidade - linha.modelData.passo)
                    }
                    Label {
                        text: carrinhoPage.quantidadeTexto(linha.modelData) + " " + linha.modelData.unidade
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 90
                    }
                    BotaoAntro {
                        text: "+"
                        secundario: true
                        leftPadding: 0; rightPadding: 0
                        Layout.preferredWidth: 48
                        enabled: linha.modelData.podeAumentar
                        onClicked: CompradorController.alterarQuantidade(linha.modelData.indice, linha.modelData.quantidade + linha.modelData.passo)
                    }
                    Label {
                        text: carrinhoPage.dinheiro(linha.modelData.subtotal)
                        font.bold: true
                        font.pixelSize: 20
                        color: "#22543D"
                        horizontalAlignment: Text.AlignRight
                        Layout.preferredWidth: 110
                    }
                    BotaoAntro { secundario: true; text: "Remover"; onClicked: CompradorController.remover(linha.modelData.indice) }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            ColumnLayout {
                Layout.fillWidth: true
                Label { text: "Total estimado"; color: "#66706A" }
                Label { text: carrinhoPage.dinheiro(CompradorController.totalEstimado); font.bold: true; font.pixelSize: 28; color: "#22543D" }
            }
            BotaoAntro { text: "Continuar escolhendo"; secundario: true; onClicked: carrinhoPage.StackView.view.pop() }
            BotaoAntro {
                text: "Finalizar reserva"
                enabled: CompradorController.tiposNaSacola > 0
                onClicked: carrinhoPage.finalizar()
            }
        }
    }
}
