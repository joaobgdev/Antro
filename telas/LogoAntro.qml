import QtQuick

// Logo do Antro (a mesma do cabeçalho das telas internas). "escala" aumenta ou diminui o conjunto.
Rectangle {
    id: logo
    property real escala: 1.0
    implicitWidth: 180 * escala
    implicitHeight: 52 * escala
    radius: 10 * escala
    color: "#22543D"
    Row {
        anchors.centerIn: parent
        spacing: 2 * logo.escala
        Text { text: "Antro"; font.family: "Georgia"; font.pixelSize: 36 * logo.escala; font.bold: true; color: "#EAE6D6" }
        Image {
            width: 24 * logo.escala
            height: 40 * logo.escala
            source: Qt.resolvedUrl("../assets/FolhaMarca.svg")
            fillMode: Image.PreserveAspectFit
        }
    }
}
