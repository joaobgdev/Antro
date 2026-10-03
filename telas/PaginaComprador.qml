import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro


Page {
    id: pagina
    property string titulo: "Feiras do Recife"
    property bool mostrarVoltar: true
    property bool mostrarSacola: true
    background: Rectangle { color: "#F4F8EC" }
    header: Rectangle {
        implicitHeight: 72
        color: "white"
        border.color: "#EAE6D6"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 16
            Button {
                text: "Voltar"
                visible: pagina.mostrarVoltar
                onClicked: pagina.StackView.view.pop()
            }
            Image {
                source: Qt.resolvedUrl("../assets/AntroVerde.svg")
                Layout.preferredWidth: 126
                Layout.preferredHeight: 40
                fillMode: Image.PreserveAspectFit
                sourceSize.width: 252
                sourceSize.height: 80
            }
            Label {
                text: pagina.titulo
                Layout.fillWidth: true
                elide: Text.ElideRight
                color: "#3B5A3D"
                font.pixelSize: 18
            }
            Button {
                visible: pagina.mostrarSacola
                text: "Sacola (" + CompradorController.tiposNaSacola + ")"
                onClicked: pagina.StackView.view.push(Qt.resolvedUrl("SacolaScreen.qml"))
            }
        }
    }
}
