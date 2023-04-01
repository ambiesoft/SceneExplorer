#-------------------------------------------------
#
# Project created by QtCreator 2018-01-01T22:05:57
#
#-------------------------------------------------

QT += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32 {
    # for QtWin::fromHICON
    QT += winextras
}

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
    ../../lsMisc/stdQt/ambapp.cpp \
    directorycheckthread.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_contextmenu.cpp \
    mainwindow_debugtest.cpp \
    showlastscanerrorsdialog.cpp \
    tablemodel.cpp \
    tablemodel_data.cpp \
    taskcheckthumbs.cpp \
    taskffmpeg.cpp \
    taskgetdir.cpp \
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
    directoryentry.cpp \
    directoryitem.cpp \
    findcombobox.cpp \
    errorinfoexception.cpp \
    extension.cpp \
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
    mainwindow_onselectall.cpp \
    mainwindow_working.cpp \
    ../../lsMisc/stdQt/stdQt.cpp \
    ../../lsMisc/stdQt/runguard.cpp \
    tagidsinfo.cpp \
    ../../lsMisc/stdQt/inisettings.cpp \
    ../../lsMisc/stdQt/waitingcursor.cpp \
    itempropertydialog.cpp \
    ../../lsMisc/stdosd/blockedbool.cpp \
    historylist.cpp \
    mainwindow_history.cpp \
    mainwindow_document.cpp \
    tagentry.cpp \
    directoryentrydialog.cpp

HEADERS += \
    ../../lsMisc/stdQt/ambapp.h \
    ../../lsMisc/stdosd/PathString.h \
    ../../lsMisc/stdosd/stdosd.h \
    ../../lsMisc/stdosd/stdosd_literal.h \
    IFFTask2Main.h \
    debugflag.h \
    directorycheckthread.h \
    gitrev.h \
    mainwindow.h \
    showlastscanerrorsdialog.h \
    tablemodel.h \
    consts.h \
    taskcheckthumbs.h \
    taskffmpeg.h \
    taskgetdir.h \
    optiondialog.h \
    globals.h \
    sql.h \
    taskmodel.h \
    tasklistdata.h \
    tableitemdata.h \
    helper.h \
    stable.h \
    directoryentry.h \
    directoryitem.h \
    findcombobox.h \
    errorinfoexception.h \
    extension.h \
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
    osd.h \
	../../profile/cpp/Profile/include/ambiesoft.profile.h \
	../../lsMisc/stdosd/SetPrority.h \
    stdafx.h \
    ../../lsMisc/stdQt/stdQt.h \
    ../../lsMisc/stdQt/runguard.h \
    tagidsinfo.h \
    ../../lsMisc/stdQt/inisettings.h \
    ../../lsMisc/stdQt/waitingcursor.h \
    itempropertydialog.h \
    ../../lsMisc/stdosd/blockedbool.h \
    historylist.h \
    tagentry.h \
    directoryentrydialog.h

FORMS += \
        mainwindow.ui \
    option.ui \
    optionexternaltoolsdialog.ui \
    optionextension.ui \
    aboutdialog.ui \
    docinfodialog.ui \
    showlastscanerrorsdialog.ui \
    taginputdialog.ui \
    renamedialog.ui \
    itempropertydialog.ui \
    directoryentrydialog.ui

# RC_FILE = app.rc

DISTFILES += \
    ../History.txt

PRECOMPILED_HEADER = stable.h

message($$QMAKESPEC)

win32 {
    message("win32")
    HEADERS += ../../lsMisc/OpenCommon.h \
            ../../lsMisc/GetLastErrorString.h

    SOURCES += ../../lsMisc/OpenCommon.cpp \
            ../../lsMisc/GetLastErrorString.cpp \
            osd_win32.cpp \
            ../../lsMisc/stdosd/SetPrority_win.cpp \
            ../../lsMisc/stdQt/stdQt_win32.cpp

    win32-g++ {
        message("win32-g++")
        LIBS += -lshlwapi -lPsapi
    }
    win32-msvc* {
        message("win32-msvc*")
        LIBS += User32.lib
        # Ole32.lib
    }
    RC_ICONS += icon.ico
}
linux {
    message("linux-g++")
    DEFINES += "AMBIESOFT_FILENAME_CASESENSITIVE=\"1\""

    SOURCES += \
        osd_linux.cpp \
        ../../lsMisc/stdosd/SetPrority_linux.cpp \
        ../../lsMisc/stdQt/stdQt_linux.cpp

    RC_ICONS += icon.ico
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
message("Fix me, why this message appears 3 times in Windows.")
# message($$TRANSLATIONS)

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
