cmake_minimum_required(VERSION 3.16)

project(RastreadorSalud VERSION 1.0 LANGUAGES CXX)

# --- Activar módulos automáticos de Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencias de Qt necesarias
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

# --- Archivos fuente y de cabecera del proyecto
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    user.cpp
    user.h
    healthrecord.cpp
    healthrecord.h
    databasemanager.cpp
    databasemanager.h
    healthanalyzer.cpp
    healthanalyzer.h
    csvexporter.cpp
    csvexporter.h
    addrecorddialog.cpp
    addrecorddialog.h
    addrecorddialog.ui
    logindialog.cpp
    logindialog.h
    logindialog.ui
)

# --- Crear ejecutable principal
qt_add_executable(RastreadorSalud
    ${PROJECT_SOURCES}
    logindialog.h logindialog.cpp logindialog.ui
)

# --- Vincular las librerías de Qt necesarias
target_link_libraries(RastreadorSalud
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Sql
)

# --- Propiedades específicas para Windows y macOS
set_target_properties(RastreadorSalud PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
