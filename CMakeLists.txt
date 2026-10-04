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
    src/ui/CompradorController.cpp
    src/ui/AuthController.cpp
    main.cpp

    include/models/produto.hpp
    include/models/itempedido.hpp
    include/models/pedido.hpp
    include/models/usuario.hpp
    include/models/consumidor.hpp
    include/models/agricultor.hpp
    include/services/GerenciadorFeira.hpp
    include/services/RepositorioUsuario.hpp
    include/services/CatalogoComprador.hpp
    include/ui/CompradorController.hpp
    include/ui/AuthController.hpp
)
qt_add_executable(appAntro ${SOURCES})
target_include_directories(appAntro PRIVATE include include/ui)
qt_policy(SET QTP0004 NEW)
qt_add_qml_module(appAntro
    URI Antro
    VERSION 1.0
    QML_FILES
        Main.qml
        telas/LoginScreen.qml
        telas/HomeScreen.qml
        telas/BotaoAntro.qml
        telas/PaginaComprador.qml
        telas/FeiraScreen.qml
        telas/SacolaScreen.qml
        telas/CatalogoScreen.qml
        telas/PedidosVendedorScreen.qml
    RESOURCES
        assets/FolhaIcone.svg
        assets/FolhaMarca.svg
        assets/AntroBege.svg
        assets/AntroFolhinha.svg
        assets/AntroVerde.svg
)

set_target_properties(appAntro PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appAntro
    PRIVATE Qt6::Quick Qt6::Sql
)

install(TARGETS appAntro
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
