import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: window

    // Resolução Janela
    width: 1280
    height: 720
    minimumWidth: 900
    minimumHeight: 600

    visible: true

    // Nome da Janela
    title: qsTr("Antro")

    // Start do Programa na LoginScreen
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: Qt.resolvedUrl("telas/LoginScreen.qml")
    }
}