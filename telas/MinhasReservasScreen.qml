import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: pagina
    aba: "reservas"
    property var reservas: []
    function atualizar() { reservas = CompradorController.reservas() }
    Component.onCompleted: atualizar()
    StackView.onActivated: atualizar()
    Connections { target: CompradorController; function onReservasChanged() { if (AuthController.perfilUsuario === "comprador") pagina.atualizar() } }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: pagina.width < 800 ? 20 : 36
        spacing: 16
        RowLayout {
            Layout.fillWidth: true
            Label { text: "Minhas reservas"; font.pixelSize: 32; font.bold: true; color: "#17201B"; Layout.fillWidth: true }
            BotaoAntro { text: "Atualizar"; secundario: true; onClicked: pagina.atualizar() }
        }
        Label { text: "Acompanhe a resposta de cada produtor. O pagamento acontece diretamente na retirada."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: CompradorController.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { visible: lista.count === 0; text: "Você ainda não fez reservas." }
        ListView {
            id: lista
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16
            clip: true
            model: pagina.reservas
            ScrollBar.vertical: ScrollBar {}
            delegate: CartaoReserva {
                required property var modelData
                width: lista.width
                reserva: modelData
                onAlterar: function(id, status) { CompradorController.cancelarReserva(id) }
            }
        }
    }
}
