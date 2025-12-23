cmake_minimum_required(VERSION 3.16)
project(MillionaireClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable Qt6 deployment tools that may cause appman-controller issues
set(QT_NO_APP_MAN_CONTROLLER TRUE CACHE BOOL "Disable appman-controller" FORCE)
set(QT_NO_DEPLOY_TOOL TRUE CACHE BOOL "Disable Qt deployment tool" FORCE)

# Enable automatic MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Disable Qt6 deployment
set(QT_DEPLOY_TOOL_EXECUTABLE "" CACHE STRING "Qt deployment tool" FORCE)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

set(SOURCES
    main.cpp
    LoginWindow.cpp
    NetworkClient.cpp
    GameModeSelectionWindow.cpp
    WelcomeWindow.cpp
    QuickModeWindow.cpp
    OneVNWindow.cpp
    WinWindow.cpp
    LoseWindow.cpp
)

set(HEADERS
    LoginWindow.h
    NetworkClient.h
    GameModeSelectionWindow.h
    WelcomeWindow.h
    QuickModeWindow.h
    OneVNWindow.h
    WinWindow.h
    LoseWindow.h
)

set(UI_FORMS
    LoginWindow.ui
)

# Use add_executable instead of qt6_add_executable to avoid conflicts
add_executable(MillionaireClient ${SOURCES} ${HEADERS} ${UI_FORMS})

# Link Qt6 libraries
target_link_libraries(MillionaireClient
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

