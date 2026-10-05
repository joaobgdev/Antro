import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

// Formulário do feirante para cadastrar um produto (fica guardado no SQLite).
PaginaComprador {
    id: formPage
    titulo: produto.id ? "Editar produto" : "Novo produto"
    property var produto: ({})
    Component.onCompleted: {
        nome.text = produto.nome || ""
        if (produto.id) {
            nome.text = produto.nome
            preco.text = String(produto.preco)
            estoque.text = String(produto.estoque)
            porPeso.checked = produto.porPeso
            feirasEscolhidas = produto.feiraIds.slice()
        }
    }
    mostrarSacola: false
    property string erro: ""
    property var feirasEscolhidas: []

    function numero(texto) { return Number(String(texto).trim().replace(",", ".")) }

    function salvar() {
        var msg = produto.id ? CompradorController.editarProdutoFeirante(
                    AuthController.telefoneUsuario, produto.id, numero(preco.text), numero(estoque.text), produto.estoque, feirasEscolhidas)
                    : CompradorController.adicionarProdutoFeirante(
                    AuthController.telefoneUsuario, AuthController.nomeUsuario, AuthController.subtituloUsuario,
                    nome.text, numero(preco.text), porPeso.checked, numero(estoque.text), feirasEscolhidas)
        if (msg.length > 0) { erro = msg; return }
        formPage.StackView.view.pop()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 14
        Label { text: formPage.produto.id ? "Editar produto" : "Adicionar produto"; font.pixelSize: 36; font.bold: true; color: "#17201B" }
        Label { text: formPage.erro; visible: text.length > 0; wrapMode: Text.WordWrap; Layout.fillWidth: true; color: "#B3261E" }

        TextField { id: nome; enabled: !formPage.produto.id; placeholderText: "Nome do produto (ex.: Alface crespa)"; Layout.fillWidth: true }
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            TextField { id: preco; placeholderText: porPeso.checked ? "Preço por kg (R$)" : "Preço por unidade (R$)"; Layout.fillWidth: true; inputMethodHints: Qt.ImhFormattedNumbersOnly }
            TextField { id: estoque; placeholderText: porPeso.checked ? "Estoque (kg, passos de 0,5)" : "Estoque (unidades)"; Layout.fillWidth: true; inputMethodHints: Qt.ImhFormattedNumbersOnly }
        }
        Label { visible: !!formPage.produto.id; text: "Estoque disponível para novas solicitações, sem os itens já reservados."; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        CheckBox { id: porPeso; enabled: !formPage.produto.id; text: "Vendido por peso (kg)" }

        Label { text: "Em quais feiras você vende este produto?"; font.pixelSize: 18; color: "#22543D" }
        Repeater {
            model: CompradorController.feiras()
            delegate: CheckBox {
                required property var modelData
                text: modelData.nome + " (" + modelData.bairro + ")"
                checked: formPage.feirasEscolhidas.indexOf(modelData.id) >= 0
                onToggled: {
                    var lista = formPage.feirasEscolhidas.filter(function(id) { return id !== modelData.id })
                    if (checked) lista.push(modelData.id)
                    formPage.feirasEscolhidas = lista
                }
            }
        }
        Item { Layout.fillHeight: true }
        RowLayout {
            spacing: 12
            BotaoAntro { text: "Salvar produto"; onClicked: formPage.salvar() }
            BotaoAntro { text: "Cancelar"; secundario: true; onClicked: formPage.StackView.view.pop() }
        }
    }
}
