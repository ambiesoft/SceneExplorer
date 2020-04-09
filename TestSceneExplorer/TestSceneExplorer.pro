TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
        ../../lsMisc/stdQt/stdQt.cpp \
        googletest/googletest/src/gtest-all.cc \
        googletest/googletest/src/gtest.cc \
        googletest/googletest/src/gtest_main.cc \
        main.cpp

INCLUDEPATH += googletest/googletest/include \
        googletest/googletest/

HEADERS += \
    ../../lsMisc/stdQt/stdQt.h \
    ../../lsMisc/stdafx.h


win32 {
    message("win32")
    HEADERS += \
        ../../lsMisc/GetLastErrorString.h

    SOURCES += \
        ../../lsMisc/stdQt/stdQt_win32.cpp \
        ../../lsMisc/GetLastErrorString.cpp

    win32-g++ {
        message("win32-g++")
        LIBS += -lshlwapi -lPsapi
    }
}
linux {
    message("linux-g++")
    SOURCES += \
        ../../lsMisc/stdQt/stdQt_linux.cpp
}


