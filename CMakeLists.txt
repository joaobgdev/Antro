# Configs Básicas
cmake_minimum_required(VERSION 3.16)[cite: 1]

project(Antro VERSION 0.1 LANGUAGES CXX)[cite: 1]

set(CMAKE_CXX_STANDARD 17)[cite: 1]
set(CMAKE_CXX_STANDARD_REQUIRED ON)[cite: 1]

# Bibliotecas
find_package(Qt6 REQUIRED COMPONENTS Quick Sql)[cite: 1]

qt_standard_project_setup(REQUIRES 6.5)[cite: 1]

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")[cite: 1]
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)[cite: 1]
endif()[cite: 1]

# Arquivos
set(SOURCES[cite: 1]
    # SRC Models
    src/models/produto.cpp[cite: 1]
    src/models/itempedido.cpp[cite: 1]
    src/models/pedido.cpp[cite: 1]
    src/models/usuario.cpp[cite: 1]
    src/models/consumidor.cpp[cite: 1]
    src/models/agricultor.cpp[cite: 1]

    # SRC Services
    src/services/GerenciadorFeira.cpp[cite: 1]
    src/services/RepositorioUsuario.cpp[cite: 1]
    src/services/CatalogoComprador.cpp[cite: 1]
    src/services/RepositorioCatalogo.cpp[cite: 1]
    src/services/AgendaFeira.cpp[cite: 1]

    # SRC UI
    src/ui/CompradorController.cpp[cite: 1]
    src/ui/AuthController.cpp[cite: 1]
    src/ui/VendedorController.cpp[cite: 1]

    # Include Models
    include/models/produto.hpp[cite: 1]
    include/models/itempedido.hpp[cite: 1]
    include/models/pedido.hpp[cite: 1]
    include/models/usuario.hpp[cite: 1]
    include/models/consumidor.hpp[cite: 1]
    include/models/agricultor.hpp[cite: 1]

    # Include Services
    include/services/GerenciadorFeira.hpp[cite: 1]
    include/services/RepositorioUsuario.hpp[cite: 1]
    include/services/CatalogoComprador.hpp[cite: 1]
    include/services/RepositorioCatalogo.hpp[cite: 1]
    include/services/AgendaFeira.hpp[cite: 1]

    # Include UI
    include/ui/CompradorController.hpp[cite: 1]
    include/ui/AuthController.hpp[cite: 1]
    include/ui/VendedorController.hpp[cite: 1]
)[cite: 1]

qt_add_executable(appAntro main.cpp ${SOURCES})[cite: 1]

target_include_directories([cite: 1]
    appAntro[cite: 1]
    PRIVATE[cite: 1]
        include[cite: 1]
        include/ui[cite: 1]
)[cite: 1]

if (Qt6_VERSION VERSION_GREATER_EQUAL 6.8)[cite: 1]
    qt_policy(SET QTP0004 NEW)[cite: 1]
endif()[cite: 1]

function(adicionar_telas alvo)[cite: 1]
    qt_add_qml_module(${alvo}[cite: 1]
        URI Antro[cite: 1]
        VERSION 1.0[cite: 1]
        OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml/${alvo}/Antro"[cite: 1]

        QML_FILES[cite: 1]
            Main.qml[cite: 1]

            telas/LoginScreen.qml[cite: 1]
            telas/HomeScreen.qml[cite: 1]
            telas/CampoAntro.qml[cite: 1]
            telas/SelecaoAntro.qml[cite: 1]
            telas/BotaoAntro.qml[cite: 1]
            telas/LogoAntro.qml[cite: 1]
            telas/PaginaComprador.qml[cite: 1]
            telas/FeiraScreen.qml[cite: 1]
            telas/SacolaScreen.qml[cite: 1]
            telas/ReservaConfirmadaScreen.qml[cite: 1]
            telas/MinhasReservasScreen.qml[cite: 1]
            telas/CartaoReserva.qml[cite: 1]
            telas/CatalogoScreen.qml[cite: 1]

            telas/PedidosVendedorScreen.qml[cite: 1]
            telas/PerfilVendedorScreen.qml[cite: 1]
            telas/EditarPerfilVendedorScreen.qml[cite: 1]
            telas/DefinirPrecosVendedorScreen.qml[cite: 1]

        RESOURCES[cite: 1]
            assets/FolhaIcone.svg[cite: 1]
            assets/FolhaMarca.svg[cite: 1]
            assets/AntroBege.svg[cite: 1]
            assets/AntroFolhinha.svg[cite: 1]
            assets/AntroVerde.svg[cite: 1]
    )[cite: 1]

endfunction()[cite: 1]

adicionar_telas(appAntro)[cite: 1]

set_target_properties(appAntro PROPERTIES[cite: 1]
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}[cite: 1]
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}[cite: 1]
    MACOSX_BUNDLE TRUE[cite: 1]
    WIN32_EXECUTABLE TRUE[cite: 1]
)[cite: 1]

target_link_libraries([cite: 1]
    appAntro[cite: 1]
    PRIVATE[cite: 1]
        Qt6::Quick[cite: 1]
        Qt6::Sql[cite: 1]
)[cite: 1]

install([cite: 1]
    TARGETS appAntro[cite: 1]

    BUNDLE DESTINATION .[cite: 1]
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}[cite: 1]
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}[cite: 1]
)[cite: 1]

include(CTest)[cite: 1]
if (BUILD_TESTING)[cite: 1]
    add_executable(test_catalogo tests/test_catalogo_comprador.cpp[cite: 1]
        src/services/CatalogoComprador.cpp src/models/produto.cpp src/models/itempedido.cpp)[cite: 1]
    target_include_directories(test_catalogo PRIVATE include)[cite: 1]
    add_test(NAME catalogo COMMAND test_catalogo)[cite: 1]

    add_executable(test_repositorio tests/test_repositorio_catalogo.cpp[cite: 1]
        src/services/RepositorioCatalogo.cpp src/services/AgendaFeira.cpp[cite: 1]
        src/services/CatalogoComprador.cpp src/models/produto.cpp)[cite: 1]
    target_include_directories(test_repositorio PRIVATE include)[cite: 1]
    target_link_libraries(test_repositorio PRIVATE Qt6::Core Qt6::Sql)[cite: 1]
    add_test(NAME repositorio COMMAND test_repositorio)[cite: 1]

    qt_add_executable(test_telas tests/test_telas.cpp ${SOURCES})[cite: 1]
    target_include_directories(test_telas PRIVATE include include/ui)[cite: 1]
    target_link_libraries(test_telas PRIVATE Qt6::Quick Qt6::Sql)[cite: 1]
    adicionar_telas(test_telas)[cite: 1]
    add_test(NAME telas COMMAND test_telas)[cite: 1]
    set_tests_properties(telas PROPERTIES[cite: 1]
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software" TIMEOUT 45)[cite: 1]

    foreach(alvo test_catalogo test_repositorio test_telas)[cite: 1]
        if (MSVC)[cite: 1]
            target_compile_options(${alvo} PRIVATE /UNDEBUG)[cite: 1]
        else()
