// Imports
import QtQuick
import QtQuick.Controls
import Antro

Window {
<<<<<<< HEAD
=======

    // Resolução
>>>>>>> 59b3808 (Adição de Comentários Main.cpp, Main.qml, CMakeLists)
    width: 1280
    height: 720
    minimumWidth: 640
    minimumHeight: 560
<<<<<<< HEAD
    visible: true
    title: "Antro"
    StackView {
        id: stackView
        objectName: "navegacao"
        anchors.fill: parent
        initialItem: "telas/LoginScreen.qml"
=======

    visible: true
    title: "Antro"

    // Stackview (Camadas de interface)
    StackView {
        id: stackView
        objectName: "navegacao"
        anchors.fill: parent // Acompanha resolução
        initialItem: "telas/LoginScreen.qml" // Começa na tela de login
>>>>>>> 59b3808 (Adição de Comentários Main.cpp, Main.qml, CMakeLists)
    }

}
