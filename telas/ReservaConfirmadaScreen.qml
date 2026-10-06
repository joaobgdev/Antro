import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: pagina
    property var resultado: ({})
    mostrarSacola: false
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: pagina.width < 800 ? 20 : 36
        spacing: 16
        Label { text: "Solicitação enviada"; font.pixelSize: 32; font.bold: true; color: "#22543D" }
        Label { text: "Cada produtor vai confirmar seu pedido. Acompanhe as respostas em Minhas reservas."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        ListView {
            id: lista
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 16
            model: pagina.resultado.reservas || []
            ScrollBar.vertical: ScrollBar {}
            delegate: CartaoReserva {
                required property var modelData
                width: lista.width
                reserva: modelData
                onAlterar: function(id, status) {
                    if (CompradorController.cancelarReserva(id)) pagina.StackView.view.replace(Qt.resolvedUrl("MinhasReservasScreen.qml"))
                }
            }
        }
        BotaoAntro { text: "Ver minhas reservas"; onClicked: pagina.StackView.view.replace(Qt.resolvedUrl("MinhasReservasScreen.qml")) }
        Label { text: CompradorController.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
    }
}
