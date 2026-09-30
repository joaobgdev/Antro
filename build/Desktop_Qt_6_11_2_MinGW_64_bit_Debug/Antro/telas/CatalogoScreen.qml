import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: catalogoScreen

    // Propriedades temporárias para a sacola
    property int totalItensSacola: 0
    property real valorTotalSacola: 0.00

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Cabeçalho / header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: "#2E7D32"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16

                Button {
                    text: "‹ Voltar"
                    onClicked: catalogoScreen.StackView.view.pop()
                }

                Text {
                    text: "🌾 Feira Agroecológica - Antro"
                    color: "white"
                    font.pixelSize: 20
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: "📍 Recife"
                    color: "#C8E6C9"
                    font.pixelSize: 14
                }
            }
        }

        // Catálogo de produtos
        ListView {
            id: listViewProdutos
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 12
            topMargin: 16
            bottomMargin: 16

            model: ListModel {
                ListElement { prodId: 101; nome: "Alface Crespa Orgânica"; preco: 3.50; unidadepeso: "unidade"; imagem: "🥬" }
                ListElement { prodId: 102; nome: "Tomate Cereja"; preco: 12.00; unidadepeso: "kg"; imagem: "🍅" }
                ListElement { prodId: 103; nome: "Abóbora Cabotiá"; preco: 6.00; unidadepeso: "kg"; imagem: "🎃" }
                ListElement { prodId: 104; nome: "Banana Prata"; preco: 8.50; unidadepeso: "kg"; imagem: "🍌" }
            }

            delegate: Rectangle {
                width: listViewProdutos.width - 32
                height: 80
                radius: 10
                color: "#FFFFFF"
                border.color: "#E0E0E0"
                anchors.horizontalCenter: parent.horizontalCenter

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12

                    Text {
                        text: model.imagem
                        font.pixelSize: 32
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: model.nome
                            font.pixelSize: 16
                            font.bold: true
                        }

                        Text {
                            text: "R$ " + model.preco.toFixed(2) + " / " + model.unidadepeso
                            color: "#2E7D32"
                            font.bold: true
                        }
                    }

                    Button {
                        text: "+ Adicionar"
                        onClicked: {
                            totalItensSacola += 1
                            valorTotalSacola += model.preco
                        }
                    }
                }
            }
        }

        // Barra inferior/ Sacola
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: "#F5F5F5"
            border.color: "#E0E0E0"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16

                ColumnLayout {
                    Text {
                        text: "Total: R$ " + valorTotalSacola.toFixed(2)
                        font.pixelSize: 18
                        font.bold: true
                        color: "#2E7D32"
                    }
                    Text {
                        text: totalItensSacola + " itens selecionados"
                        font.pixelSize: 12
                        color: "#666666"
                    }
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: "Ver Sacola 🛒"
                    enabled: totalItensSacola > 0
                }
            }
        }
    }
}