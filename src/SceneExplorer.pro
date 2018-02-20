#-------------------------------------------------
#
# Project created by QtCreator 2018-01-01T22:05:57
#
#-------------------------------------------------

QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SceneExplorer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    tablemodel.cpp \
    settings.cpp \
    taskffmpeg.cpp \
    taskgetdir.cpp \
    waitcursor.cpp \
    optiondialog.cpp \
    globals.cpp \
    sql.cpp \
    taskmodel.cpp \
    tasklistdata.cpp \
    mainwindow_slot.cpp \
    tableitemdata.cpp \
    mainwindow_action.cpp \
    mainwindow_event.cpp \
    helper.cpp \
    foldermodel.cpp \
    directoryentry.cpp \
    directoryitem.cpp \
    findcombobox.cpp \
    runguard.cpp \
    errorinfoexception.cpp \
    blockedbool.cpp \
    extension.cpp \
    optionfontdialog.cpp \
    optionexternaltoolsdialog.cpp \
    externaltoolitem.cpp \
    optionextension.cpp \
    document.cpp \
    ffmpeg.cpp

HEADERS += \
        mainwindow.h \
    tablemodel.h \
    settings.h \
    consts.h \
    taskffmpeg.h \
    taskgetdir.h \
    waitcursor.h \
    optiondialog.h \
    globals.h \
    sql.h \
    taskmodel.h \
    tasklistdata.h \
    tableitemdata.h \
    helper.h \
    foldermodel.h \
    stable.h \
    directoryentry.h \
    directoryitem.h \
    findcombobox.h \
    runguard.h \
    errorinfoexception.h \
    blockedbool.h \
    extension.h \
    optionfontdialog.h \
    optionexternaltoolsdialog.h \
    externaltoolitem.h \
    optionextension.h \
    document.h \
    ffmpeg.h

FORMS += \
        mainwindow.ui \
    option.ui \
    optionfontdialog.ui \
    optionexternaltoolsdialog.ui \
    optionextension.ui

RC_FILE = app.rc

DISTFILES += \
    info/info-ffprobe-json.txt \
    info/db-update.txt \
    info/Scenario-rename.txt

PRECOMPILED_HEADER = stable.h

message($$QMAKESPEC)

win32 {
    SOURCES += \
            osd_win32.cpp
}
linux-g++ {
    SOURCES += \
            osd_linux.cpp
}


TRANSLATIONS += translations/i18n_jp.ts

RESOURCES += \
    resource.qrc
