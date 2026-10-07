import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: carrinho
    mostrarSacola: false
    property string aviso: ""

    function dinheiro(valor) { return "R$ " + Number(valor).toLocaleString(Qt.locale("pt_BR"), 'f', 2) }
    // Solicita a reserva e abre a confirmação somente se der certo.
    function finalizar() {
        var resultado = CompradorController.finalizarReserva()
        if (!resultado.ok) { aviso = resultado.erro; return }
        var pilha = carrinho.StackView.view
        pilha.pop(null)
        pilha.push(Qt.resolvedUrl("ReservaConfirmadaScreen.qml"), {resultado: resultado})
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: carrinho.width < 800 ? 20 : 36
        contentWidth: availableWidth
        clip: true
        ColumnLayout {
            width: parent.width
            spacing: 16
            Label { text: "Meu carrinho"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
            Label {
                text: "Escolha a retirada em cada feira. O valor é estimado e pode variar na pesagem. O pagamento é combinado diretamente com o produtor."
                color: "#66706A"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            Label { text: carrinho.aviso || CompradorController.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label { visible: CompradorController.tiposNaSacola === 0; text: "Seu carrinho está vazio. Volte para escolher produtos." }
            Repeater {
                model: CompradorController.sacola
                delegate: Rectangle {
                    id: item
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: conteudo.implicitHeight + 44
                    color: "white"
                    radius: 14
                    border.color: "#E2E7E3"
                    GridLayout {
                        id: conteudo
                        anchors.fill: parent
                        anchors.margins: 22
                        columns: carrinho.width < 1000 ? 1 : 2
                        rowSpacing: 12
                        ColumnLayout {
                            Layout.fillWidth: true
                            Label { text: item.modelData.nome; font.pixelSize: 24; font.bold: true; color: "#22543D" }
                            Label { text: item.modelData.vendedor + " · " + item.modelData.feira; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
                            Label { text: carrinho.dinheiro(item.modelData.preco) + " / " + item.modelData.unidade + " · subtotal " + carrinho.dinheiro(item.modelData.subtotal) }
                        }
                        RowLayout {
                            BotaoAntro { text: "−"; secundario: true; enabled: item.modelData.quantidade > item.modelData.passo + 0.000001; onClicked: CompradorController.alterarQuantidade(item.modelData.indice, item.modelData.quantidade - item.modelData.passo) }
                            Label { text: Number(item.modelData.quantidade).toLocaleString(Qt.locale("pt_BR"), 'f', item.modelData.passo < 1 ? 1 : 0) + " " + item.modelData.unidade; Layout.minimumWidth: 72; horizontalAlignment: Text.AlignHCenter }
                            BotaoAntro { text: "+"; secundario: true; enabled: item.modelData.podeAumentar; onClicked: CompradorController.alterarQuantidade(item.modelData.indice, item.modelData.quantidade + item.modelData.passo) }
                            BotaoAntro { text: "Remover"; secundario: true; onClicked: CompradorController.remover(item.modelData.indice) }
                        }
                    }
                }
            }
            // Pede uma data e um horário para cada feira da sacola.
            Repeater {
                model: CompradorController.retiradas
                delegate: Rectangle {
                    id: retirada
                    required property var modelData
                    property string diaSelecionado: modelData.data
                    property var datas: CompradorController.datasRetirada(modelData.feiraId)
                    property var janelas: CompradorController.janelasRetirada(modelData.feiraId, diaSelecionado)
                    Layout.fillWidth: true
                    implicitHeight: horarios.implicitHeight + 44
                    color: "#E5EEE8"
                    radius: 14
                    ColumnLayout {
                        id: horarios
                        anchors.fill: parent
                        anchors.margins: 22
                        spacing: 12
                        Label { text: "Retirada · " + retirada.modelData.feira; font.pixelSize: 20; font.bold: true; color: "#22543D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        GridLayout {
                            Layout.fillWidth: true
                            columns: carrinho.width < 800 ? 1 : 2
                            ColumnLayout {
                                Layout.fillWidth: true
                                Label { text: "Data" }
                                SelecaoAntro {
                                    id: dataEscolhida
                                    objectName: "dataRetirada"
                                    Layout.fillWidth: true
                                    model: retirada.datas
                                    textRole: "texto"
                                    currentIndex: {
                                        for (var i = 0; i < retirada.datas.length; i++)
                                            if (retirada.datas[i].valor === retirada.modelData.data) return i
                                        return -1
                                    }
                                    displayText: currentIndex < 0 ? "Escolha uma data" : currentText
                                    onActivated: retirada.diaSelecionado = retirada.datas[currentIndex].valor
                                }
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                Label { text: "Horário" }
                                SelecaoAntro {
                                    id: horarioEscolhido
                                    objectName: "horarioRetirada"
                                    Layout.fillWidth: true
                                    enabled: retirada.diaSelecionado.length > 0
                                    model: retirada.janelas
                                    textRole: "texto"
                                    currentIndex: {
                                        if (retirada.diaSelecionado !== retirada.modelData.data) return -1
                                        for (var i = 0; i < retirada.janelas.length; i++)
                                            if (retirada.janelas[i].inicio === retirada.modelData.inicio) return i
                                        return -1
                                    }
                                    displayText: currentIndex < 0 ? "Escolha um horário" : currentText
                                    onActivated: {
                                        var janela = retirada.janelas[currentIndex]
                                        // Guarda a janela de retirada escolhida para esta feira.
                                        CompradorController.agendar(retirada.modelData.feiraId, retirada.diaSelecionado, janela.inicio, janela.fim)
                                    }
                                }
                            }
                        }
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                visible: CompradorController.tiposNaSacola > 0
                Label { text: "Total estimado: " + carrinho.dinheiro(CompradorController.totalEstimado); font.pixelSize: 22; font.bold: true; color: "#22543D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                BotaoAntro { objectName: "solicitarReserva"; text: "Solicitar reserva"; onClicked: carrinho.finalizar() }
            }
            Item { Layout.preferredHeight: 16 }
        }
    }
}
