import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

// Tela de feedback: aparece depois de "Finalizar reserva" no carrinho.
PaginaComprador {
    id: confirmacao
    property var resultado: ({})
    readonly property var itens: resultado.itens || []
    titulo: "Reserva realizada"
    mostrarVoltar: false
    mostrarSacola: false

    function dinheiro(valor) { return "R$ " + Number(valor).toLocaleString(Qt.locale("pt_BR"), 'f', 2) }
    function quantidadeTexto(item) { return Number(item.quantidade).toLocaleString(Qt.locale("pt_BR"), 'f', item.passo < 1 ? 1 : 0) }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 18

        RowLayout {
            spacing: 20
            Rectangle {
                Layout.preferredWidth: 72
                Layout.preferredHeight: 72
                radius: 36
                color: "#22543D"
                Label { anchors.centerIn: parent; text: "✓"; color: "white"; font.pixelSize: 40; font.bold: true }
            }
            ColumnLayout {
                Label { text: "Seus produtos foram reservados!"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
                Label { text: "Reserva nº " + (confirmacao.resultado.codigo || ""); font.pixelSize: 16; color: "#66706A" }
            }
        }
        Label {
            text: "Retire seus produtos " + ((confirmacao.resultado.feiras || []).length > 1 ? "nas feiras " : "na feira ")
                  + (confirmacao.resultado.feiras || []).join(", ") + ". O pagamento é feito direto com o vendedor, na retirada. Produtos vendidos por peso podem ter o valor ajustado na pesagem."
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            color: "#59635E"
        }
        Label { text: "Resumo da reserva"; font.pixelSize: 24; color: "#22543D" }
        ListView {
            id: listaResumo
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 12
            model: confirmacao.itens
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                id: linha
                required property var modelData
                width: listaResumo.width
                height: resumoLinha.implicitHeight + 36
                color: "white"
                radius: 14
                border.color: "#E4E8E5"
                RowLayout {
                    id: resumoLinha
                    anchors.fill: parent
                    anchors.margins: 18
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: linha.modelData.nome; font.bold: true; font.pixelSize: 20; color: "#22543D" }
                        Label { text: linha.modelData.feira + " · " + linha.modelData.vendedor; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
                    }
                    Label { text: confirmacao.quantidadeTexto(linha.modelData) + " " + linha.modelData.unidade }
                    Label { text: confirmacao.dinheiro(linha.modelData.subtotal); font.bold: true; color: "#22543D"; Layout.preferredWidth: 100; horizontalAlignment: Text.AlignRight }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            ColumnLayout {
                Layout.fillWidth: true
                Label { text: "Total estimado"; color: "#66706A" }
                Label { text: confirmacao.dinheiro(confirmacao.resultado.total || 0); font.bold: true; font.pixelSize: 28; color: "#22543D" }
            }
            BotaoAntro {
                text: "Voltar para as feiras"
                onClicked: confirmacao.StackView.view.pop(null)
            }
        }
    }
}
