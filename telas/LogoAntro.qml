import QtQuick

Rectangle {
    property real escala: 1
    implicitWidth: 180 * escala
    implicitHeight: 52 * escala
    radius: 10 * escala
    color: "#22543D"
    Image {
        anchors.fill: parent
        anchors.margins: 4
        source: Qt.resolvedUrl("../assets/AntroBege.svg")
        fillMode: Image.PreserveAspectFit
    }
}
