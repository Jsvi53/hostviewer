cmake_minimum_required(VERSION 3.5)

project(18_hostviewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

message(STATUS "CMAKE_AUTOMOC: ${CMAKE_AUTOMOC}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools OpenGLWidgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets LinguistTools OpenGLWidgets SerialPort)

set(TS_FILES 18_hostviewer_zh_CN.ts)

aux_source_directory("src" SRC_LISTS)
file(GLOB INC_LISTS "include/*.h")


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

message(STATUS "PROJECT_SOURCES: ${PROJECT_SOURCES}")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(18_hostviewer MANUAL_FINALIZATION ${PROJECT_SOURCES})
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET 18_hostviewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})     # macro for language translation

else()
    if(ANDROID)
        add_library(18_hostviewer SHARED ${PROJECT_SOURCES})
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(18_hostviewer ${PROJECT_SOURCES})
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
target_sources(18_hostviewer PRIVATE ${SRC_LISTS} ${INC_LISTS})
target_include_directories(18_hostviewer PRIVATE "include"
                                                $Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS
                                                $Qt${QT_VERSION_MAJOR}OpenGLWidgets_INCLUDE_DIRS
                                                $Qt${QT_VERSION_MAJOR}SerialPort_INCLUDE_DIRS
                                                $Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS
                                                )
target_link_libraries(18_hostviewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                            Qt${QT_VERSION_MAJOR}::OpenGLWidgets
                                            Qt${QT_VERSION_MAJOR}::SerialPort
                                            Qt${QT_VERSION_MAJOR}::Core
                                            )


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.18_hostviewer)
endif()
set_target_properties(18_hostviewer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS 18_hostviewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(18_hostviewer)
endif()
