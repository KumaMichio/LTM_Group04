cmake_minimum_required(VERSION 3.16)
project(MillionaireClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

set(SOURCES
    main.cpp
    LoginWindow.cpp
    NetworkClient.cpp
    GameModeSelectionWindow.cpp
    QuickModeWindow.cpp
    WinWindow.cpp
    LoseWindow.cpp
)

set(HEADERS
    LoginWindow.h
    NetworkClient.h
    GameModeSelectionWindow.h
    QuickModeWindow.h
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

