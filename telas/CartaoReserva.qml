import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Rectangle {
    id: card
    property var reserva: ({})
    property bool feirante: false
    signal alterar(int id, string status)
    implicitHeight: conteudo.implicitHeight + 44
    radius: 14
    color: "white"
    border.color: "#E2E7E3"
    ColumnLayout {
        id: conteudo
        anchors.fill: parent
        anchors.margins: 22
        spacing: 10
        Label { text: "#" + card.reserva.id + " · " + (card.feirante ? card.reserva.comprador : card.reserva.banca); font.pixelSize: 24; font.bold: true; color: "#22543D"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: card.reserva.situacao; font.bold: true; color: "#22543D" }
        Label { text: card.reserva.feira + " · " + card.reserva.local; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#66706A" }
        Label { text: "Retirada: " + card.reserva.retirada; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Repeater {
            model: card.reserva.itens || []
            delegate: Label {
                required property var modelData
                text: modelData.nome + " · " + Number(modelData.quantidade).toLocaleString(Qt.locale("pt_BR"), 'f', modelData.unidade === "kg" ? 1 : 0) + " " + modelData.unidade + " · R$ " + Number(modelData.subtotal).toLocaleString(Qt.locale("pt_BR"), 'f', 2)
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
        Label { text: "Total estimado: R$ " + Number(card.reserva.total).toLocaleString(Qt.locale("pt_BR"), 'f', 2); font.bold: true }
        Flow {
            Layout.fillWidth: true
            spacing: 10
            BotaoAntro { objectName: "aceitarPedido"; text: "Aceitar"; visible: card.feirante && card.reserva.podeAceitar; onClicked: card.alterar(card.reserva.id, "ACEITA") }
            BotaoAntro { objectName: "recusarPedido"; text: "Recusar"; secundario: true; visible: card.feirante && card.reserva.podeRecusar; onClicked: card.alterar(card.reserva.id, "RECUSADA") }
            BotaoAntro { objectName: "retirarPedido"; text: "Marcar como retirado"; visible: card.feirante && card.reserva.podeRetirar; onClicked: card.alterar(card.reserva.id, "RETIRADA") }
            BotaoAntro { objectName: "cancelarReserva"; text: "Cancelar reserva"; secundario: true; visible: !card.feirante && card.reserva.podeCancelar; onClicked: card.alterar(card.reserva.id, "CANCELADA") }
        }
    }
}
