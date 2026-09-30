cmake_minimum_required(VERSION 3.16)

project(Antro VERSION 0.1 LANGUAGES CXX)

# Configuração do padrão C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)

qt_standard_project_setup()

# Conteúdo importado do Figma/Qt (se existir)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

# Ficheiros de código C++ do projeto
set(SOURCES
    src/models/produto.cpp
    src/models/itempedido.cpp
    src/models/pedido.cpp
    src/models/consumidor.cpp
    src/models/agricultor.cpp
    src/models/usuario.cpp
    src/services/GerenciadorFeira.cpp
    src/services/RepositorioUsuario.cpp
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
    include/ui/AuthController.hpp
)

# Criação do executável com suporte nativo ao Qt 6
qt_add_executable(appAntro ${SOURCES})

# Mapeamento da pasta de cabeçalhos (.hpp)
target_include_directories(appAntro PRIVATE include)

# Módulo de interface QML
qt_add_qml_module(appAntro
    URI Antro
    VERSION 1.0
    SOURCES
        include/ui/AuthController.hpp
        src/ui/AuthController.cpp
    QML_FILES
        Main.qml
        telas/LoginScreen.qml
        telas/CatalogoScreen.qml
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