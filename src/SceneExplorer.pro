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
    ffmpeg.cpp \
    aboutdialog.cpp \
    tableitemdelegate.cpp \
    commandoption.cpp \
    docinfodialog.cpp \
    documentsql.cpp \
    taginputdialog.cpp \
    tagitem.cpp \
    mycontextmenu.cpp \
    renamedialog.cpp \
    mainwindow_ctor.cpp \
    mainwindow_oncopy.cpp \
    mainwindow_onpaste.cpp \
    mainwindow_onedit.cpp \
    mainwindow_onselectall.cpp

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
    ffmpeg.h \
    aboutdialog.h \
    imainwindow.h \
    tableitemdelegate.h \
    commandoption.h \
    docinfodialog.h \
    documentsql.h \
    taginputdialog.h \
    tagitem.h \
    sqlcommon.h \
    mycontextmenu.h \
    renamedialog.h \
    osd.h

FORMS += \
        mainwindow.ui \
    option.ui \
    optionfontdialog.ui \
    optionexternaltoolsdialog.ui \
    optionextension.ui \
    aboutdialog.ui \
    docinfodialog.ui \
    taginputdialog.ui \
    renamedialog.ui

RC_FILE = app.rc

DISTFILES += \
    ../History.txt

PRECOMPILED_HEADER = stable.h

message($$QMAKESPEC)

win32 {
   SOURCES += \
            osd_win32.cpp





#    win32-g++ {
#        LIBS += -lOle32
#    }
#    win32-msvc* {
#        LIBS += Ole32.lib
#    }
}
linux-g++ {
    SOURCES += \
            osd_linux.cpp
}


TRANSLATIONS += translations/i18n_Japanese.ts

RESOURCES += \
    resource.qrc


# I18N https://wiki.qt.io/Automating_generation_of_qm_files starts ---------------
LANGUAGES += Japanese

# parameters: var, prepend, append
defineReplace(prependAll) {
 for(a,$$1):result += $$2$${a}$$3
 return($$result)
}

TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/translations/i18n_, .ts)

# TRANSLATIONS_FILES =

qtPrepareTool(LRELEASE, lrelease)
for(tsfile, TRANSLATIONS) {
#   qmfile = $$shadowed($$tsfile)
#   qmfile ~= s,.ts$,.qm,
#   qmdir = $$dirname(qmfile)
#   !exists($$qmdir) {
#   mkpath($$qmdir)|error("Aborting.")
# }
# command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile
  command = $$LRELEASE -removeidentical $$tsfile

  system($$command)|error("Failed to run: $$command")
#  TRANSLATIONS_FILES += $$qmfile
}
# I18n ends ----------------------------------------------------------------------



## Copy misc files starts ---------------------------------
#OTHER_FILES += \
#    dist/History.txt \
#    dist/License.txt

#win32 {
#    DESTDIR_WIN = $${DESTDIR}
#    DESTDIR_WIN ~= s,/,\\,g
#    PWD_WIN = $${PWD}
#    PWD_WIN ~= s,/,\\,g
#    OTHER_FILES ~= s,/,\\,g
#    for(FILE, OTHER_FILES){
#        QMAKE_POST_LINK += $$quote(cmd /c copy /y $${PWD_WIN}\\$${FILE} $${DESTDIR_WIN}$$escape_expand(\\n\\t))
#    }
#}
## Copy misc files enss ------------------------------------
