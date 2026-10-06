import QtQuick
import QtQuick.Controls

TextField {
    id: campo
    implicitHeight: 48
    font.pixelSize: 16
    leftPadding: 14
    rightPadding: 14
    verticalAlignment: Text.AlignVCenter
    color: "#17201B"
    placeholderTextColor: "#8B9590"
    background: Rectangle {
        color: "white"
        radius: 10
        border.color: campo.activeFocus ? "#22543D" : "#D5DDD7"
    }
}
