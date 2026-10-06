// Imports
import QtQuick
import QtQuick.Controls
import Antro

Window {
    // Resolução
    width: 1280
    height: 720
    minimumWidth: 640
    minimumHeight: 560

    visible: true
    title: "Antro"

    // Stackview (Camadas de interface)
    StackView {
        id: stackView
        objectName: "navegacao"
        anchors.fill: parent // Acompanha resolução
        initialItem: "telas/LoginScreen.qml" // Começa na tela de login
    }
}
