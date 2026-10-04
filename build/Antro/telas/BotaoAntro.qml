import QtQuick
import QtQuick.Controls

Button {
    id: botao
    property bool secundario: false
    property bool selecionado: false
    implicitHeight: 48
    leftPadding: 22
    rightPadding: 22
    hoverEnabled: true
    background: Rectangle {
        radius: 10
        color: !botao.enabled ? "#ECEFED" : botao.secundario ? (botao.selecionado || botao.hovered ? "#E5EEE8" : "white") : (botao.down ? "#183C2C" : botao.hovered ? "#286149" : "#22543D")
        border.color: botao.secundario ? "#E2E7E3" : "transparent"
    }
    contentItem: Text {
        text: botao.text
        color: !botao.enabled ? "#8B9590" : botao.secundario ? "#22543D" : "white"
        font.pixelSize: 16
        font.weight: Font.DemiBold
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    HoverHandler { cursorShape: botao.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor }
}
