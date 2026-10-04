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
        anchors.margins: 24
        spacing: 12
        Label { text: "Minha sacola"; font.pixelSize: 26; font.bold: true; color: "#3B5A3D" }
        Label {
            text: "Seleção temporária. A confirmação de reserva e retirada será integrada à etapa de pedidos. Não há pagamento pelo aplicativo."
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            color: "#61705F"
        }
        Label { visible: CompradorController.tiposNaSacola === 0; text: "Sua sacola está vazia. Volte para escolher produtos." }
        ListView {
            id: listaSacola
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 12
            model: CompradorController.sacola
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                required property var modelData
                required property int index
                width: listaSacola.width
                height: conteudo.implicitHeight + 32
                color: "white"
                radius: 10
                border.color: "#EAE6D6"
                RowLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 16
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.nome; font.bold: true; font.pixelSize: 20; color: "#3B5A3D" }
                        Label { text: modelData.feira + " · " + modelData.vendedor; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: modelData.quantidade + " " + modelData.unidade + " · R$ " + Number(modelData.subtotal).toLocaleString(Qt.locale("pt_BR"), 'f', 2) }
                    }
                    Button { text: "Remover"; onClicked: CompradorController.remover(index) }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: "Valor estimado: R$ " + CompradorController.totalEstimado.toLocaleString(Qt.locale("pt_BR"), 'f', 2); font.bold: true; font.pixelSize: 20; color: "#3B5A3D"; Layout.fillWidth: true }
            Button { text: "Continuar escolhendo"; onClicked: sacolaPage.StackView.view.pop() }
        }
    }
}
