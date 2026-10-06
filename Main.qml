import QtQuick
import QtQuick.Controls
import Antro

Window {
    width: 1280
    height: 720
    minimumWidth: 640
    minimumHeight: 560
    visible: true
    title: "Antro"
    StackView {
        id: stackView
        objectName: "navegacao"
        anchors.fill: parent
        initialItem: "telas/LoginScreen.qml"
    }
}
