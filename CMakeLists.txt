cmake_minimum_required(VERSION 3.16)
project(SimpleMp3Player LANGUAGES CXX)

# Helps IDEs (VS Code, clangd) find Qt headers via compile database.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt_add_executable(SimpleMp3Player WIN32
    main.cpp
    MainWindow.h
    MainWindow.cpp
)

target_link_libraries(SimpleMp3Player PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)
