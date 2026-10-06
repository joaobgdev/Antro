import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: pagina
    aba: "reservas"
    property var pedidos: []
    property string filtro: "Todos"
    function atualizar() {
        var todos = VendedorController.pedidos()
        pedidos = todos.filter(function(p) {
            return filtro === "Todos" || (filtro === "Pendentes" && p.status === "SOLICITADA") || (filtro === "Confirmados" && p.status === "ACEITA")
        })
    }
    Component.onCompleted: atualizar()
    StackView.onActivated: atualizar()
    onFiltroChanged: atualizar()
    Connections { target: VendedorController; function onPedidosChanged() { if (AuthController.perfilUsuario === "feirante") pagina.atualizar() } }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: pagina.width < 800 ? 20 : 36
        spacing: 16
        Label { text: "Pedidos"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
        Label { text: "Confira os produtos e a retirada antes de aceitar. Recusar devolve a quantidade ao estoque."; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        RowLayout {
            Layout.fillWidth: true
            SelecaoAntro { model: ["Todos", "Pendentes", "Confirmados"]; onActivated: pagina.filtro = currentText }
            Item { Layout.fillWidth: true }
            BotaoAntro { text: "Atualizar"; secundario: true; onClicked: pagina.atualizar() }
        }
        Label { text: VendedorController.erro; visible: text.length > 0; color: "#B3261E"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { visible: lista.count === 0; text: "Nenhum pedido nesta lista." }
        ListView {
            id: lista
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16
            clip: true
            model: pagina.pedidos
            ScrollBar.vertical: ScrollBar {}
            delegate: CartaoReserva {
                required property var modelData
                width: lista.width
                reserva: modelData
                feirante: true
                onAlterar: function(id, status) { VendedorController.alterarPedido(id, status) }
            }
        }
    }
}
