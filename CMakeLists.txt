cmake_minimum_required(VERSION 3.16)

project(Antro VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)

qt_standard_project_setup(REQUIRES 6.5)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

set(SOURCES
    src/models/produto.cpp
    src/models/itempedido.cpp
    src/models/pedido.cpp
    src/models/usuario.cpp
    src/models/consumidor.cpp
    src/models/agricultor.cpp

    src/services/GerenciadorFeira.cpp
    src/services/RepositorioUsuario.cpp
    src/services/CatalogoComprador.cpp
    src/services/RepositorioCatalogo.cpp
    src/services/AgendaFeira.cpp

    src/ui/CompradorController.cpp
    src/ui/AuthController.cpp
    src/ui/VendedorController.cpp

    include/models/produto.hpp
    include/models/itempedido.hpp
    include/models/pedido.hpp
    include/models/usuario.hpp
    include/models/consumidor.hpp
    include/models/agricultor.hpp

    include/services/GerenciadorFeira.hpp
    include/services/RepositorioUsuario.hpp
    include/services/CatalogoComprador.hpp
    include/services/RepositorioCatalogo.hpp
    include/services/AgendaFeira.hpp

    include/ui/CompradorController.hpp
    include/ui/AuthController.hpp
    include/ui/VendedorController.hpp
)

qt_add_executable(appAntro main.cpp ${SOURCES})

target_include_directories(
    appAntro
    PRIVATE
        include
        include/ui
)

if (Qt6_VERSION VERSION_GREATER_EQUAL 6.8)
    qt_policy(SET QTP0004 NEW)
endif()

function(adicionar_telas alvo)
    qt_add_qml_module(${alvo}
        URI Antro
        VERSION 1.0
        OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml/${alvo}/Antro"

        QML_FILES
            Main.qml

            telas/LoginScreen.qml
            telas/HomeScreen.qml
            telas/CampoAntro.qml
            telas/SelecaoAntro.qml
            telas/BotaoAntro.qml
            telas/LogoAntro.qml
            telas/PaginaComprador.qml
            telas/FeiraScreen.qml
            telas/SacolaScreen.qml
            telas/ReservaConfirmadaScreen.qml
            telas/MinhasReservasScreen.qml
            telas/CartaoReserva.qml
            telas/CatalogoScreen.qml

            telas/PedidosVendedorScreen.qml
            telas/PerfilVendedorScreen.qml
            telas/EditarPerfilVendedorScreen.qml
            telas/DefinirPrecosVendedorScreen.qml

        RESOURCES
            assets/FolhaIcone.svg
            assets/FolhaMarca.svg
            assets/AntroBege.svg
            assets/AntroFolhinha.svg
            assets/AntroVerde.svg
    )

endfunction()

adicionar_telas(appAntro)

set_target_properties(appAntro PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(
    appAntro
    PRIVATE
        Qt6::Quick
        Qt6::Sql
)

install(
    TARGETS appAntro

    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(CTest)
if (BUILD_TESTING)
    add_executable(test_catalogo tests/test_catalogo_comprador.cpp
        src/services/CatalogoComprador.cpp src/models/produto.cpp src/models/itempedido.cpp)
    target_include_directories(test_catalogo PRIVATE include)
    add_test(NAME catalogo COMMAND test_catalogo)

    add_executable(test_repositorio tests/test_repositorio_catalogo.cpp
        src/services/RepositorioCatalogo.cpp src/services/AgendaFeira.cpp
        src/services/CatalogoComprador.cpp src/models/produto.cpp)
    target_include_directories(test_repositorio PRIVATE include)
    target_link_libraries(test_repositorio PRIVATE Qt6::Core Qt6::Sql)
    add_test(NAME repositorio COMMAND test_repositorio)

    qt_add_executable(test_telas tests/test_telas.cpp ${SOURCES})
    target_include_directories(test_telas PRIVATE include include/ui)
    target_link_libraries(test_telas PRIVATE Qt6::Quick Qt6::Sql)
    adicionar_telas(test_telas)
    add_test(NAME telas COMMAND test_telas)
    set_tests_properties(telas PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software" TIMEOUT 45)

    foreach(alvo test_catalogo test_repositorio test_telas)
        if (MSVC)
            target_compile_options(${alvo} PRIVATE /UNDEBUG)
        else()
            target_compile_options(${alvo} PRIVATE -UNDEBUG)
        endif()
    endforeach()
endif()
