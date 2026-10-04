import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Window {
    id: window
    width: 1280
    height: 720
    minimumWidth: 900
    minimumHeight: 600

    visible: true
    title: qsTr("Antro")
    Connections {
        target: AuthController
        function onUsuarioChanged() { CompradorController.limpar() }
    }
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: "telas/LoginScreen.qml"
    }
}
