cmake_minimum_required(VERSION 3.16)
project(StellarPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/resources)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Svg SvgWidgets OpenGLWidgets)

# Find libmpv
find_package(PkgConfig REQUIRED)
pkg_check_modules(MPV REQUIRED mpv)

# Include directories
include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${MPV_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MpvWidget.cpp
    src/PlayerController.cpp
    src/PlaylistModel.cpp
    src/SubtitleSettingsDialog.cpp
    src/SettingsManager.cpp
    src/ThemeManager.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/MpvWidget.h
    include/PlayerController.h
    include/PlaylistModel.h
    include/SubtitleSettingsDialog.h
    include/SettingsManager.h
    include/ThemeManager.h
)

# UI files
set(UI_FILES
    resources/MainWindow.ui
    resources/SubtitleSettingsDialog.ui
)

# Wrap UI files
qt6_wrap_ui(UI_HEADERS ${UI_FILES})

# Resource files
set(RESOURCE_FILES resources/resources.qrc)

# Wrap resource files
qt6_add_resources(QRC_SOURCES ${RESOURCE_FILES})

# Executable
add_executable(StellarPlayer ${SOURCES} ${HEADERS} ${UI_HEADERS} ${QRC_SOURCES})

# Link libraries
target_link_libraries(StellarPlayer Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Svg Qt6::SvgWidgets Qt6::OpenGLWidgets ${MPV_LIBRARIES})

# Compiler flags
target_compile_options(StellarPlayer PRIVATE -Wall -Wextra)

# Install
install(TARGETS StellarPlayer DESTINATION bin)