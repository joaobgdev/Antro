import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Antro

Page {
    id: homePage

    
    // Estado
    
    readonly property bool isFarmer: AuthController.perfilUsuario === "feirante"

    property string consulta: ""
    property string filtroFeira: ""
    property string categoria: "Todos"
    property int totalItensSacola: 0
    property real valorTotalSacola: 0.0

    // Responsividade (mesmos breakpoints do protótipo)
    readonly property bool mid: width <= 1150
    readonly property bool narrow: width <= 850
    readonly property bool phone: width <= 650
    readonly property int sidePad: phone ? 17 : (narrow ? 22 : 34)

    // Ganchos para a navegação (conecte no main.qml quando as telas existirem)
    signal navegar(string destino)        // "comprar" | "feiras" | "produtores" | "pedidos" | "area"
    signal abrirFeira(string feiraId)
    signal abrirProdutor(int produtorId)
    signal abrirSacola()

    
    // Dados de exemplo (iguais aos do protótipo)
    
    readonly property var categorias: ["Todos", "Hortaliças", "Frutas", "Legumes", "Raízes", "Temperos"]

    readonly property var feiras: [
        { id: "varzea",    nome: "Feira Agroecológica da Várzea",                  area: "Várzea",      local: "Praça da Várzea",                                    dia: "Sábados",        ini: 7,  fim: 10 },
        { id: "gracas",    nome: "Espaço Agroecológico das Graças",                area: "Graças",      local: "Rua Andrade de Souza, atrás do Colégio São Luís",    dia: "Sábados",        ini: 4,  fim: 11 },
        { id: "casaforte", nome: "Feira de Produtos Orgânicos de Casa Forte",      area: "Casa Forte",  local: "Praça da Vitória Régia",                             dia: "Sábados",        ini: 5,  fim: 11 },
        { id: "trindade",  nome: "Espaço Agroecológico do Sítio da Trindade",      area: "Casa Amarela",local: "Estrada do Arraial, Sítio da Trindade",              dia: "Sábados",        ini: 5,  fim: 11 },
        { id: "cordeiro",  nome: "Feira Agroecológica da Juventude do Cordeiro",   area: "Cordeiro",    local: "Avenida Caxangá, Parque de Exposições do Cordeiro",  dia: "Sextas-feiras",  ini: 5,  fim: 11 },
        { id: "aurora",    nome: "Feira Agroecológica da Aurora",                  area: "Boa Vista",   local: "Rua da Aurora, em frente ao Ed. Alfredo Bandeira",   dia: "Quartas-feiras", ini: 14, fim: 20 }
    ]

    readonly property var produtores: [
        { id: 1, nome: "Ana Oliveira",  feiras: ["varzea", "gracas"] },
        { id: 2, nome: "João Batista",  feiras: ["varzea", "cordeiro"] },
        { id: 3, nome: "Rosa Ferreira", feiras: ["gracas", "casaforte", "trindade"] },
        { id: 4, nome: "Pedro Santos",  feiras: ["aurora", "trindade", "casaforte"] }
    ]

    readonly property var produtos: [
        { id: 1, nome: "Alface crespa", categoria: "Hortaliças", emoji: "🥬", preco: 4, unidade: "unidade", produtor: 1, disponivel: true },
        { id: 2, nome: "Tomate",        categoria: "Legumes",    emoji: "🍅", preco: 8, unidade: "500 g",   produtor: 1, disponivel: true },
        { id: 3, nome: "Banana-prata",  categoria: "Frutas",     emoji: "🍌", preco: 6, unidade: "dúzia",   produtor: 2, disponivel: true },
        { id: 4, nome: "Macaxeira",     categoria: "Raízes",     emoji: "🌱", preco: 7, unidade: "1 kg",    produtor: 2, disponivel: true },
        { id: 5, nome: "Cenoura",       categoria: "Raízes",     emoji: "🥕", preco: 5, unidade: "500 g",   produtor: 1, disponivel: true },
        { id: 6, nome: "Coentro",       categoria: "Temperos",   emoji: "🌿", preco: 3, unidade: "maço",    produtor: 3, disponivel: true },
        { id: 7, nome: "Manga",         categoria: "Frutas",     emoji: "🥭", preco: 7, unidade: "1 kg",    produtor: 4, disponivel: true },
        { id: 8, nome: "Abóbora",       categoria: "Legumes",    emoji: "🎃", preco: 6, unidade: "1 kg",    produtor: 4, disponivel: true }
    ]

    
    // Funções auxiliares
    
    function feiraPorId(id) {
        for (var i = 0; i < feiras.length; ++i)
            if (feiras[i].id === id) return feiras[i]
        return null
    }
    function produtorPorId(id) {
        for (var i = 0; i < produtores.length; ++i)
            if (produtores[i].id === id) return produtores[i]
        return null
    }
    function normalizar(s) {
        return s.toLocaleLowerCase().normalize("NFD").replace(/[\u0300-\u036f]/g, "")
    }
    function areasRetirada(produtorId) {
        return produtorPorId(produtorId).feiras.map(function (id) {
            return feiraPorId(id).area
        }).join(" · ")
    }
    function horario(f) { return f.dia + " · " + f.ini + "h às " + f.fim + "h" }
    function moeda(n) { return "R$ " + n.toFixed(2).replace(".", ",") }

    function filtrarProdutos() {
        var q = normalizar(consulta)
        return produtos.filter(function (p) {
            var prod = produtorPorId(p.produtor)
            return (categoria === "Todos" || p.categoria === categoria)
                && (filtroFeira === "" || prod.feiras.indexOf(filtroFeira) !== -1)
                && normalizar(p.nome + " " + prod.nome).indexOf(q) !== -1
        })
    }
    readonly property var produtosFiltrados: filtrarProdutos()

    function adicionar(preco) {
        totalItensSacola += 1
        valorTotalSacola += preco
    }
    function limparFiltros() {
        consulta = ""
        filtroFeira = ""
        categoria = "Todos"
        txtBusca.text = ""
        comboFeira.currentIndex = 0
    }

    
    // Componentes reutilizáveis
    
    component Botao: Button {
        id: b
        property bool contorno: false
        implicitHeight: 44
        leftPadding: 17
        rightPadding: 17
        hoverEnabled: true
        background: Rectangle {
            radius: 10
            color: b.contorno ? (b.hovered ? "#F4F8EC" : "white")
                              : (b.hovered ? "#29462D" : "#3B5A3D")
            border.color: b.contorno ? "#BFCBB8" : "#3B5A3D"
        }
        contentItem: Text {
            text: b.text
            color: b.contorno ? "#3B5A3D" : "white"
            font.pixelSize: 14
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        HoverHandler { cursorShape: Qt.PointingHandCursor }
    }

    component LinkTexto: Text {
        signal clicado()
        color: "#3B5A3D"
        font.pixelSize: 14
        font.weight: Font.DemiBold
        font.underline: true
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicado()
        }
    }

    component Chip: Rectangle {
        id: chip
        property string rotulo
        property bool ativo: false
        signal clicado()
        implicitWidth: chipText.implicitWidth + 34
        implicitHeight: 38
        radius: height / 2
        color: ativo ? "#3B5A3D" : "transparent"
        border.color: ativo ? "#3B5A3D" : "#D9DFD1"
        Text {
            id: chipText
            anchors.centerIn: parent
            text: chip.rotulo
            font.pixelSize: 14
            color: chip.ativo ? "white" : "#3B5A3D"
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: chip.clicado()
        }
    }

    // Cantos arredondados "por fora" (usado na foto do card lateral)
    component CornerMask: Canvas {
        property bool ladoDireito: false
        property color corFora: "#F4F8EC"
        width: 17
        height: 17
        onCorForaChanged: requestPaint()
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.fillStyle = corFora
            ctx.beginPath()
            if (!ladoDireito) {
                ctx.moveTo(0, 0); ctx.lineTo(0, 17); ctx.lineTo(17, 17)
                ctx.arc(17, 0, 17, Math.PI / 2, Math.PI, false)
            } else {
                ctx.moveTo(17, 0); ctx.lineTo(17, 17); ctx.lineTo(0, 17)
                ctx.arc(0, 0, 17, Math.PI / 2, 0, true)
            }
            ctx.closePath()
            ctx.fill()
        }
    }

    component NavLinks: RowLayout {
        id: nav
        property int fonte: 15
        signal escolhido(string destino)
        spacing: 26
        Repeater {
            model: [
                { k: "comprar",    t: "Comprar" },
                { k: "feiras",     t: "Feiras do Recife" },
                { k: "produtores", t: "Produtores" },
                { k: "pedidos",    t: "Meus pedidos" }
            ]
            delegate: Item {
                id: navItem
                required property var modelData
                implicitWidth: navLabel.implicitWidth
                implicitHeight: navLabel.implicitHeight + 20
                Text {
                    id: navLabel
                    anchors.verticalCenter: parent.verticalCenter
                    text: navItem.modelData.t
                    font.pixelSize: nav.fonte
                    color: navItem.modelData.k === "comprar" ? "white" : "#DAE6D5"
                }
                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 2
                    color: navItem.modelData.k === "comprar" ? "#84C6A8" : "transparent"
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: nav.escolhido(navItem.modelData.k)
                }
            }
        }
    }

    component FeiraCard: Rectangle {
        id: fc
        property var feira
        property string horarioTexto
        property color headColor: "#EAE6D6"
        signal explorar()
        signal comoChegar()
        color: "white"
        radius: 16
        border.color: "#D9DFD1"
        implicitHeight: fcCol.implicitHeight + 2

        ColumnLayout {
            id: fcCol
            anchors.fill: parent
            anchors.margins: 1
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 56
                radius: 15
                color: fc.headColor
                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 15
                    color: parent.color
                }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 22
                    anchors.rightMargin: 22
                    Text {
                        text: fc.feira.area.toUpperCase()
                        font.pixelSize: 12
                        font.weight: Font.DemiBold
                        font.letterSpacing: 1
                        color: "#26392C"
                    }
                    Item { Layout.fillWidth: true }
                    Text {
                        text: "SEMANAL"
                        font.pixelSize: 12
                        font.weight: Font.DemiBold
                        font.letterSpacing: 1
                        color: "#26392C"
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 22
                spacing: 0

                Text {
                    Layout.fillWidth: true
                    Layout.bottomMargin: 10
                    text: fc.feira.nome
                    wrapMode: Text.WordWrap
                    font.pixelSize: 20
                    font.bold: true
                    color: "#26392C"
                }
                Text {
                    Layout.fillWidth: true
                    Layout.bottomMargin: 10
                    text: fc.feira.local
                    wrapMode: Text.WordWrap
                    font.pixelSize: 14
                    color: "#5C6A5E"
                }
                Text {
                    Layout.fillWidth: true
                    Layout.bottomMargin: 4
                    text: fc.horarioTexto
                    font.pixelSize: 14
                    font.bold: true
                    color: "#3B5A3D"
                }
                Text {
                    Layout.fillWidth: true
                    text: "Horário de referência. Confirme antes de ir."
                    wrapMode: Text.WordWrap
                    font.pixelSize: 12
                    color: "#5C6A5E"
                }
                Item { Layout.fillHeight: true; Layout.minimumHeight: 14 }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Botao {
                        Layout.fillWidth: true
                        text: "Explorar feira"
                        onClicked: fc.explorar()
                    }
                    LinkTexto {
                        text: "Como chegar ↗"
                        onClicado: fc.comoChegar()
                    }
                }
            }
        }
    }

    
    // Fundo e cabeçalho
    
    background: Rectangle { color: "#F4F8EC" }

    header: Rectangle {
        color: "#3B5A3D"
        implicitHeight: headerColumn.implicitHeight + (homePage.phone ? 25 : 42)

        ColumnLayout {
            id: headerColumn
            width: Math.min(parent.width - 2 * homePage.sidePad, 1332)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            spacing: 15

            RowLayout {
                Layout.fillWidth: true
                spacing: homePage.narrow ? 15 : 45

                // Marca
                RowLayout {
                    spacing: 11

                    Canvas {
                        id: antCanvas
                        Layout.preferredWidth: homePage.phone ? 30 : (homePage.narrow ? 35 : 42)
                        Layout.preferredHeight: Layout.preferredWidth
                        onWidthChanged: requestPaint()
                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.reset()
                            var k = width / 64
                            ctx.scale(k, k)

                            // patas
                            ctx.strokeStyle = "#84C6A8"
                            ctx.lineWidth = 3.5
                            ctx.lineCap = "round"
                            var legs = [[14,25,6,17],[18,41,7,50],[29,23,25,10],
                                        [30,42,25,55],[45,23,50,11],[47,40,55,51]]
                            ctx.beginPath()
                            for (var i = 0; i < legs.length; ++i) {
                                ctx.moveTo(legs[i][0], legs[i][1])
                                ctx.lineTo(legs[i][2], legs[i][3])
                            }
                            ctx.stroke()

                            // corpo
                            ctx.fillStyle = "#F4F8EC"
                            var body = [[16,33,9],[33,32,11],[50,31,8]]
                            for (var j = 0; j < body.length; ++j) {
                                ctx.beginPath()
                                ctx.arc(body[j][0], body[j][1], body[j][2], 0, 2 * Math.PI)
                                ctx.fill()
                            }

                            // olho
                            ctx.fillStyle = "#3B5A3D"
                            ctx.beginPath()
                            ctx.arc(53, 28, 1.5, 0, 2 * Math.PI)
                            ctx.fill()
                        }
                    }

                    ColumnLayout {
                        spacing: 3
                        Text {
                            text: "formiga"
                            color: "white"
                            font.pixelSize: homePage.phone ? 24 : (homePage.narrow ? 27 : 31)
                            font.weight: Font.ExtraBold
                            font.letterSpacing: -1.8
                        }
                        Text {
                            text: "RECIFE · PE"
                            color: "#DCE9D3"
                            font.pixelSize: homePage.phone ? 9 : 10
                            font.weight: Font.DemiBold
                            font.letterSpacing: 1.8
                        }
                    }
                }

                // Navegação (desktop)
                NavLinks {
                    visible: !homePage.narrow
                    Layout.fillWidth: true
                    onEscolhido: destino => homePage.navegar(destino)
                }
                Item { Layout.fillWidth: true; visible: homePage.narrow }

                // Ações
                RowLayout {
                    spacing: homePage.phone ? 12 : 24

                    LinkTexto {
                        text: "Sou produtor"
                        color: "white"
                        font.bold: true
                        font.pixelSize: homePage.phone ? 12 : 14
                        onClicado: homePage.navegar("area")
                    }

                    LinkTexto {
                        text: "Sair"
                        color: "#DAE6D5"
                        font.weight: Font.Normal
                        font.pixelSize: homePage.phone ? 12 : 14
                        onClicado: {
                            AuthController.sair()
                            if (homePage.StackView.view)
                                homePage.StackView.view.replace(Qt.resolvedUrl("LoginScreen.qml"))
                        }
                    }

                    Rectangle {
                        implicitWidth: bagRow.implicitWidth + 28
                        implicitHeight: 44
                        radius: 10
                        color: "#84C6A8"

                        RowLayout {
                            id: bagRow
                            anchors.centerIn: parent
                            spacing: 9
                            Text {
                                text: "♧ Sacola"
                                color: "#183725"
                                font.pixelSize: homePage.phone ? 12 : 14
                                font.weight: Font.Bold
                            }
                            Rectangle {
                                implicitWidth: Math.max(22, badgeText.implicitWidth + 10)
                                implicitHeight: 22
                                radius: 11
                                color: "#3B5A3D"
                                Text {
                                    id: badgeText
                                    anchors.centerIn: parent
                                    text: homePage.totalItensSacola
                                    color: "white"
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: homePage.abrirSacola()
                        }
                    }
                }
            }

            // Navegação (telas estreitas: linha própria)
            NavLinks {
                visible: homePage.narrow
                Layout.fillWidth: true
                spacing: 17
                fonte: homePage.phone ? 13 : 14
                onEscolhido: destino => homePage.navegar(destino)
            }
        }
    }

    
    // Conteúdo rolável
    
    Flickable {
        id: scroller
        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: pageColumn.implicitHeight
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: pageColumn
            width: scroller.width
            spacing: 0

            // ------------------------------------------------ Principal
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: mainCol.implicitHeight + (homePage.phone ? 64 : 94)

                ColumnLayout {
                    id: mainCol
                    y: homePage.phone ? 24 : 36
                    width: Math.min(parent.width - 2 * homePage.sidePad, 1292)
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 0

                    // ---------- Visão do feirante (placeholder)
                    ColumnLayout {
                        visible: homePage.isFarmer
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: "Seus produtos"
                            font.pixelSize: 25
                            font.bold: true
                            color: "#3B5A3D"
                        }
                        Text {
                            text: "Você ainda não cadastrou nenhum produto."
                            color: "#859B74"
                            font.pixelSize: 14
                        }
                        Botao {
                            text: "Adicionar produto"
                            // TODO: abrir tela de cadastro de produto
                        }
                    }

                    // ---------- Visão do comprador (protótipo "Comprar")
                    ColumnLayout {
                        visible: !homePage.isFarmer
                        Layout.fillWidth: true
                        spacing: 0

                        // Introdução
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.bottomMargin: 27
                            spacing: 20

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 0

                                Text {
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                    text: "PEQUENOS PRODUTORES · FEIRAS DO RECIFE"
                                    font.pixelSize: 12
                                    font.bold: true
                                    font.letterSpacing: 1.56
                                    color: "#3B5A3D"
                                    Layout.bottomMargin: 11
                                }
                                Text {
                                    Layout.fillWidth: true
                                    Layout.bottomMargin: 12
                                    text: "O que vai ter na sua feira?"
                                    wrapMode: Text.WordWrap
                                    font.pixelSize: Math.max(30, Math.min(43, homePage.width * 0.03))
                                    font.weight: Font.ExtraBold
                                    font.letterSpacing: -font.pixelSize * 0.045
                                    color: "#26392C"
                                }
                                Text {
                                    Layout.fillWidth: true
                                    Layout.maximumWidth: 670
                                    text: "Escolha seus alimentos, converse com quem produz e combine a retirada."
                                    wrapMode: Text.WordWrap
                                    font.pixelSize: 16
                                    color: "#5C6A5E"
                                }
                            }

                            Rectangle {
                                visible: !homePage.narrow
                                Layout.alignment: Qt.AlignBottom
                                implicitWidth: locText.implicitWidth + 32
                                implicitHeight: 42
                                radius: 21
                                color: "white"
                                border.color: "#D9DFD1"
                                Text {
                                    id: locText
                                    anchors.centerIn: parent
                                    text: "Recife, Pernambuco"
                                    font.pixelSize: 14
                                    color: "#26392C"
                                }
                            }
                        }

                        // Busca
                        GridLayout {
                            Layout.fillWidth: true
                            Layout.bottomMargin: 20
                            columns: homePage.phone ? 1 : (homePage.narrow ? 2 : 3)
                            columnSpacing: 10
                            rowSpacing: 10

                            TextField {
                                id: txtBusca
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                implicitHeight: 46
                                leftPadding: 14
                                rightPadding: 14
                                verticalAlignment: Text.AlignVCenter
                                placeholderText: "Busque um alimento ou produtor"
                                placeholderTextColor: "#8A9686"
                                color: "#26392C"
                                font.pixelSize: 16
                                onTextChanged: homePage.consulta = text
                                background: Rectangle {
                                    radius: 10
                                    color: "white"
                                    border.color: txtBusca.activeFocus ? "#33805B" : "#BFCBB8"
                                    border.width: txtBusca.activeFocus ? 2 : 1
                                }
                            }

                            ComboBox {
                                id: comboFeira
                                Layout.fillWidth: homePage.phone
                                Layout.preferredWidth: homePage.narrow ? 210 : 250
                                implicitHeight: 46
                                model: ["Todas as feiras"].concat(homePage.feiras.map(function (f) { return f.nome }))
                                onActivated: homePage.filtroFeira = currentIndex === 0
                                             ? "" : homePage.feiras[currentIndex - 1].id
                                background: Rectangle {
                                    radius: 10
                                    color: "white"
                                    border.color: "#BFCBB8"
                                }
                                contentItem: Text {
                                    leftPadding: 14
                                    rightPadding: 30
                                    text: comboFeira.displayText
                                    font.pixelSize: 16
                                    color: "#26392C"
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                            }

                            Botao {
                                visible: !homePage.narrow
                                text: "Buscar"
                                onClicked: homePage.consulta = txtBusca.text
                            }
                        }

                        // Categorias
                        Flow {
                            Layout.fillWidth: true
                            Layout.bottomMargin: 24
                            spacing: 8

                            Repeater {
                                model: homePage.categorias
                                delegate: Chip {
                                    required property string modelData
                                    rotulo: modelData
                                    ativo: homePage.categoria === modelData
                                    onClicado: homePage.categoria = modelData
                                }
                            }
                        }

                        // Produtos + lateral
                        GridLayout {
                            Layout.fillWidth: true
                            columns: homePage.phone ? 1 : 2
                            columnSpacing: homePage.narrow ? 20 : 28
                            rowSpacing: 22

                            // ---- Produtos
                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                Layout.alignment: Qt.AlignTop
                                spacing: 0

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.bottomMargin: 16
                                    spacing: 10
                                    Text {
                                        text: "Direto da colheita"
                                        font.pixelSize: homePage.phone ? 22 : 25
                                        font.bold: true
                                        font.letterSpacing: -0.75
                                        color: "#26392C"
                                    }
                                    Item { Layout.fillWidth: true }
                                    Text {
                                        text: homePage.produtosFiltrados.length
                                              + (homePage.produtosFiltrados.length === 1 ? " produto" : " produtos")
                                        font.pixelSize: homePage.phone ? 12 : 14
                                        color: "#5C6A5E"
                                    }
                                }

                                GridLayout {
                                    id: productGrid
                                    Layout.fillWidth: true
                                    columns: homePage.mid ? 2 : 3
                                    columnSpacing: homePage.phone ? 12 : 17
                                    rowSpacing: homePage.phone ? 12 : 17

                                    Repeater {
                                        model: homePage.produtosFiltrados
                                        delegate: Rectangle {
                                            id: pcard
                                            required property var modelData
                                            required property int index
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: 1
                                            Layout.fillHeight: true
                                            implicitHeight: pcol.implicitHeight + 2
                                            radius: 16
                                            color: "white"
                                            border.color: "#D9DFD1"

                                            ColumnLayout {
                                                id: pcol
                                                anchors.fill: parent
                                                anchors.margins: 1
                                                spacing: 0

                                                // Topo colorido com emoji
                                                Rectangle {
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: homePage.phone ? 105 : 124
                                                    radius: 15
                                                    color: pcard.index % 3 === 0 ? "#E8EDDE"
                                                         : (pcard.index % 3 === 1 ? "#EFE6D6" : "#DFEFE5")

                                                    Rectangle {
                                                        anchors.left: parent.left
                                                        anchors.right: parent.right
                                                        anchors.bottom: parent.bottom
                                                        height: 15
                                                        color: parent.color
                                                    }
                                                    Text {
                                                        anchors.centerIn: parent
                                                        text: pcard.modelData.emoji
                                                        font.pixelSize: homePage.phone ? 52 : 61
                                                    }
                                                    Rectangle {
                                                        anchors.left: parent.left
                                                        anchors.top: parent.top
                                                        anchors.leftMargin: 10
                                                        anchors.topMargin: 10
                                                        implicitWidth: catText.implicitWidth + 16
                                                        implicitHeight: catText.implicitHeight + 6
                                                        radius: height / 2
                                                        color: "#F0FFFFFF"
                                                        Text {
                                                            id: catText
                                                            anchors.centerIn: parent
                                                            text: pcard.modelData.categoria
                                                            font.pixelSize: 11
                                                            font.weight: Font.DemiBold
                                                            font.letterSpacing: 0.22
                                                            color: "#26392C"
                                                        }
                                                    }
                                                }

                                                // Corpo
                                                ColumnLayout {
                                                    Layout.fillWidth: true
                                                    Layout.fillHeight: true
                                                    Layout.margins: homePage.phone ? 13 : 17
                                                    spacing: 0

                                                    Text {
                                                        Layout.fillWidth: true
                                                        Layout.bottomMargin: 4
                                                        text: pcard.modelData.nome
                                                        wrapMode: Text.WordWrap
                                                        font.pixelSize: homePage.phone ? 17 : 18
                                                        font.bold: true
                                                        color: "#26392C"
                                                    }
                                                    Text {
                                                        text: homePage.produtorPorId(pcard.modelData.produtor).nome
                                                        font.pixelSize: homePage.phone ? 12 : 14
                                                        font.underline: true
                                                        color: "#5C6A5E"
                                                        MouseArea {
                                                            anchors.fill: parent
                                                            cursorShape: Qt.PointingHandCursor
                                                            onClicked: homePage.abrirProdutor(pcard.modelData.produtor)
                                                        }
                                                    }
                                                    Text {
                                                        Layout.fillWidth: true
                                                        Layout.topMargin: 12
                                                        Layout.bottomMargin: 17
                                                        text: "Retirada: " + homePage.areasRetirada(pcard.modelData.produtor)
                                                        wrapMode: Text.WordWrap
                                                        font.pixelSize: 12
                                                        color: "#65715C"
                                                    }
                                                    Item { Layout.fillHeight: true }
                                                    RowLayout {
                                                        Layout.fillWidth: true
                                                        spacing: 8

                                                        ColumnLayout {
                                                            spacing: 0
                                                            Text {
                                                                text: homePage.moeda(pcard.modelData.preco)
                                                                font.pixelSize: homePage.phone ? 19 : 20
                                                                font.weight: Font.ExtraBold
                                                                color: "#26392C"
                                                            }
                                                            Text {
                                                                text: "por " + pcard.modelData.unidade
                                                                font.pixelSize: 12
                                                                color: "#5C6A5E"
                                                            }
                                                        }
                                                        Item { Layout.fillWidth: true }
                                                        Rectangle {
                                                            implicitWidth: 37
                                                            implicitHeight: 37
                                                            radius: 18.5
                                                            color: addArea.containsMouse ? "#203D28" : "#3B5A3D"
                                                            opacity: pcard.modelData.disponivel ? 1 : 0.55
                                                            Text {
                                                                anchors.centerIn: parent
                                                                anchors.verticalCenterOffset: -1
                                                                text: "+"
                                                                color: "white"
                                                                font.pixelSize: 25
                                                            }
                                                            MouseArea {
                                                                id: addArea
                                                                anchors.fill: parent
                                                                hoverEnabled: true
                                                                enabled: pcard.modelData.disponivel
                                                                cursorShape: Qt.PointingHandCursor
                                                                onClicked: homePage.adicionar(pcard.modelData.preco)
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // Nenhum resultado
                                    Rectangle {
                                        visible: homePage.produtosFiltrados.length === 0
                                        Layout.columnSpan: productGrid.columns
                                        Layout.fillWidth: true
                                        implicitHeight: emptyCol.implicitHeight + 100
                                        radius: 16
                                        color: "white"
                                        border.color: "#BCC8B2"

                                        ColumnLayout {
                                            id: emptyCol
                                            anchors.centerIn: parent
                                            spacing: 10
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: "Nenhum alimento encontrado"
                                                font.pixelSize: 25
                                                font.bold: true
                                                color: "#26392C"
                                            }
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: "Tente outro nome, categoria ou feira."
                                                color: "#5C6A5E"
                                            }
                                            Botao {
                                                Layout.alignment: Qt.AlignHCenter
                                                contorno: true
                                                text: "Limpar filtros"
                                                onClicked: homePage.limparFiltros()
                                            }
                                        }
                                    }
                                }
                            }

                            // ---- Lateral
                            ColumnLayout {
                                Layout.preferredWidth: homePage.phone ? -1 : (homePage.narrow ? 230 : 282)
                                Layout.fillWidth: homePage.phone
                                Layout.alignment: Qt.AlignTop
                                spacing: 19

                                Rectangle {
                                    Layout.fillWidth: true
                                    implicitHeight: sideCol.implicitHeight + 2
                                    radius: 17
                                    color: "#EAE6D6"
                                    border.color: "#D9DFD1"

                                    ColumnLayout {
                                        id: sideCol
                                        anchors.fill: parent
                                        anchors.margins: 1
                                        spacing: 0

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            Layout.margins: 20
                                            spacing: 0

                                            Text {
                                                text: "ENCONTRE NA VÁRZEA"
                                                font.pixelSize: 12
                                                font.bold: true
                                                font.letterSpacing: 1.56
                                                color: "#3B5A3D"
                                                Layout.bottomMargin: 10
                                            }
                                            Text {
                                                Layout.fillWidth: true
                                                Layout.bottomMargin: 10
                                                text: "Uma feira que já faz parte do bairro."
                                                wrapMode: Text.WordWrap
                                                font.pixelSize: 22
                                                font.bold: true
                                                font.letterSpacing: -0.66
                                                color: "#26392C"
                                            }
                                            Text {
                                                Layout.fillWidth: true
                                                Layout.bottomMargin: 17
                                                text: "Praça da Várzea\nSábados · 7h às 10h*"
                                                font.pixelSize: 14
                                                color: "#26392C"
                                            }
                                            Botao {
                                                Layout.fillWidth: true
                                                text: "Conhecer a feira →"
                                                onClicked: homePage.abrirFeira("varzea")
                                            }
                                            Text {
                                                Layout.topMargin: 6
                                                text: "* Horário de referência."
                                                font.pixelSize: 11
                                                color: "#5C6859"
                                            }
                                        }

                                        Image {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 159
                                            source: "cesta.jpg"
                                            fillMode: Image.PreserveAspectCrop
                                            asynchronous: true
                                        }
                                    }

                                    CornerMask {
                                        anchors.left: parent.left
                                        anchors.bottom: parent.bottom
                                    }
                                    CornerMask {
                                        ladoDireito: true
                                        anchors.right: parent.right
                                        anchors.bottom: parent.bottom
                                    }
                                }

                                ColumnLayout {
                                    visible: !homePage.phone
                                    Layout.fillWidth: true
                                    Layout.leftMargin: 5
                                    Layout.rightMargin: 5
                                    spacing: 5

                                    Text {
                                        text: "Combine direto com o produtor"
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "#3B5A3D"
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        text: "A reserva só se confirma após o aceite. A retirada e o pagamento são combinados entre vocês."
                                        wrapMode: Text.WordWrap
                                        font.pixelSize: 14
                                        color: "#5C6A5E"
                                    }
                                }
                            }
                        }

                        // Feiras perto da sua rotina
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.topMargin: 29
                            implicitHeight: 1
                            color: "#D9DFD1"
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.topMargin: 23
                            Layout.bottomMargin: 16
                            spacing: 18
                            Text {
                                text: "Feiras perto da sua rotina"
                                font.pixelSize: homePage.phone ? 22 : 25
                                font.bold: true
                                font.letterSpacing: -0.75
                                color: "#26392C"
                            }
                            Item { Layout.fillWidth: true }
                            LinkTexto {
                                text: "Ver todas →"
                                onClicado: homePage.navegar("feiras")
                            }
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: homePage.phone ? 1 : (homePage.mid ? 2 : 3)
                            columnSpacing: 18
                            rowSpacing: 18

                            Repeater {
                                model: homePage.feiras.slice(0, 3)
                                delegate: FeiraCard {
                                    required property var modelData
                                    required property int index
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: 1
                                    Layout.fillHeight: true
                                    feira: modelData
                                    horarioTexto: homePage.horario(modelData)
                                    headColor: index === 0 ? "#84C6A8" : "#EAE6D6"
                                    onExplorar: homePage.abrirFeira(modelData.id)
                                    onComoChegar: Qt.openUrlExternally(
                                        "https://www.google.com/maps/search/?api=1&query="
                                        + encodeURIComponent(modelData.local + ", Recife, PE"))
                                }
                            }
                        }
                    }
                }
            }

            // ------------------------------------------------ Rodapé
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 1
                color: "#D9DFD1"
            }
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: footerRow.implicitHeight + 54

                RowLayout {
                    id: footerRow
                    y: 27
                    width: Math.min(parent.width - 2 * homePage.sidePad, 1292)
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 32

                    ColumnLayout {
                        Layout.alignment: Qt.AlignTop
                        spacing: 0
                        Text {
                            text: "formiga"
                            font.pixelSize: 20
                            font.bold: true
                            color: "#3B5A3D"
                        }
                        Text {
                            text: "Uma rede que aproxima."
                            font.pixelSize: 12
                            color: "#5C6A5E"
                        }
                    }
                    Text {
                        Layout.fillWidth: true
                        Layout.maximumWidth: 430
                        Layout.alignment: Qt.AlignTop
                        text: "Feiras pré-cadastradas do Recife. Horários de referência sujeitos a confirmação com a organização."
                        wrapMode: Text.WordWrap
                        font.pixelSize: 13
                        color: "#5C6A5E"
                    }
                    Item { Layout.fillWidth: true }
                }
            }
        }
    }
}
