TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# check submodule
!exists( googletest/googletest/src/gtest-all.cc ) {
    error( "Submodule not found: run 'git submodule update -i'" )
}

SOURCES += \
        ../../lsMisc/stdQt/stdQt.cpp \
        googletest/googletest/src/gtest-all.cc \
        googletest/googletest/src/gtest.cc \
        googletest/googletest/src/gtest_main.cc \

INCLUDEPATH += googletest/googletest/include \
        googletest/googletest/

HEADERS += \
    ../../lsMisc/stdQt/stdQt.h \ \
    main.h \
    stdafx.h


win32 {
    message("win32")
    HEADERS += \
        ../../lsMisc/GetLastErrorString.h

    SOURCES += \
        ../../lsMisc/stdQt/stdQt_win32.cpp \
        ../../lsMisc/GetLastErrorString.cpp \
        main_win32.cpp

    win32-g++ {
        message("win32-g++")
        LIBS += -lshlwapi -lPsapi
    }
    win32-msvc* {
        message("win32-msvc*")
        LIBS += User32.lib shlwapi.lib shell32.lib
    }
}
linux {
    message("linux-g++")
    SOURCES += \
        ../../lsMisc/stdQt/stdQt_linux.cpp \
        main_linux.cpp
}


