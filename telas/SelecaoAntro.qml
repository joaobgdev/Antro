import QtQuick
import QtQuick.Controls

ComboBox {
    id: selecao
    implicitWidth: 180
    implicitHeight: 48
    font.pixelSize: 16
    leftPadding: 14
    rightPadding: 32
    palette.buttonText: "#17201B"
    background: Rectangle {
        color: selecao.enabled ? "white" : "#ECEFED"
        radius: 10
        border.color: selecao.activeFocus ? "#22543D" : "#D5DDD7"
    }
}
