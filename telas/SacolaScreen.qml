import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: sacolaPage
    titulo: "Minha sacola"
    mostrarSacola: false
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 18
        Label { text: "Minha sacola"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
        Label {
            text: "Seleção temporária. A confirmação de reserva e retirada será integrada à etapa de pedidos. Não há pagamento pelo aplicativo."
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            color: "#66706A"
        }
        Label { visible: CompradorController.tiposNaSacola === 0; text: "Sua sacola está vazia. Volte para escolher produtos." }
        ListView {
            id: listaSacola
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 18
            model: CompradorController.sacola
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                required property var modelData
                required property int index
                width: listaSacola.width
                height: conteudo.implicitHeight + 56
                color: "white"
                radius: 14
                border.color: "#E4E8E5"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 28
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.nome; font.bold: true; font.pixelSize: 24; color: "#22543D" }
                        Label { text: modelData.feira + " · " + modelData.vendedor; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: modelData.quantidade + " " + modelData.unidade + " · R$ " + Number(modelData.subtotal).toLocaleString(Qt.locale("pt_BR"), 'f', 2) }
                    }
                    BotaoAntro { secundario: true; text: "Remover"; onClicked: CompradorController.remover(index) }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: "Valor estimado: R$ " + CompradorController.totalEstimado.toLocaleString(Qt.locale("pt_BR"), 'f', 2); font.bold: true; font.pixelSize: 24; color: "#22543D"; Layout.fillWidth: true }
            BotaoAntro { text: "Continuar escolhendo"; onClicked: sacolaPage.StackView.view.pop() }
        }
    }
}
