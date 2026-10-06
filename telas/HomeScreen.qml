import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

PaginaComprador {
    id: home
    inicio: true
    mostrarVoltar: false
    property var feiras: []
    property string aviso: ""

    function atualizar() { feiras = CompradorController.feiras() }
    Component.onCompleted: atualizar()
    StackView.onActivated: {
        if (feirante) VendedorController.carregarPerfil()
        CompradorController.recarregar()
        atualizar()
    }
    Connections {
        target: CompradorController
        function onProdutosChanged() { home.atualizar() }
    }
    Timer { interval: 60000; running: home.visible; repeat: true; onTriggered: home.atualizar() }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: home.width < 800 ? 20 : 36
        spacing: 16
        Label { text: "Feiras do Recife"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
        Label {
            text: home.feirante ? "Escolha as feiras onde você vai vender seus produtos." : "Escolha uma feira e conheça os produtores que participam dela."
            color: "#66706A"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "As feiras em andamento aparecem primeiro, conforme os horários cadastrados. Confirme eventuais mudanças com a organização."
                font.pixelSize: 13
                color: "#66706A"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            BotaoAntro { text: "Atualizar"; secundario: true; onClicked: CompradorController.recarregar() }
        }
        Label {
            text: home.aviso || (home.feirante ? VendedorController.erro : CompradorController.erro)
            visible: text.length > 0
            color: "#B3261E"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        Label { visible: lista.count === 0; text: "Nenhuma feira disponível." }
        ListView {
            id: lista
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 16
            model: home.feiras
            ScrollBar.vertical: ScrollBar {}
            delegate: Rectangle {
                id: card
                required property var modelData
                readonly property bool participa: VendedorController.feiras.indexOf(modelData.id) >= 0
                width: lista.width
                height: conteudo.implicitHeight + 44
                radius: 14
                color: "white"
                border.color: "#E2E7E3"
                GridLayout {
                    id: conteudo
                    anchors.fill: parent
                    anchors.margins: 22
                    columns: home.width < 850 ? 1 : 2
                    rowSpacing: 12
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Label { text: card.modelData.situacao; color: "#22543D"; font.bold: card.modelData.aberta }
                        Label { text: card.modelData.nome; font.pixelSize: 24; font.bold: true; color: "#17201B"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: card.modelData.local + " · " + card.modelData.horario; color: "#66706A"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        Label { text: card.modelData.vendedores + " produtores participantes"; color: "#66706A" }
                    }
                    BotaoAntro {
                        objectName: "abrirFeira"
                        text: home.feirante ? (card.participa ? "Participando · ver perfil" : "Participar da feira") : "Ver produtores"
                        secundario: home.feirante && card.participa
                        onClicked: {
                            var pilha = home.StackView.view
                            var perfil = Qt.resolvedUrl("PerfilVendedorScreen.qml")
                            var feira = Qt.resolvedUrl("FeiraScreen.qml")
                            var id = card.modelData.id
                            if (!home.feirante) {
                                pilha.push(feira, {feiraId: id})
                            } else if (card.participa || VendedorController.participarFeira(id)) {
                                Qt.callLater(function() { pilha.push(perfil) })
                            }
                        }
                    }
                }
            }
        }
    }
}
