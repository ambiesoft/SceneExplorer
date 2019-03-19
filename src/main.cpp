//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Visual Leak Detector
// #include <vld.h>

#include <QApplication>
#include <QSettings>

#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QObject>
#include <QDebug>
#include <QFileDialog>
#include <QProcess>
#include <QTranslator>
#include <QLibraryInfo>
#include <QCommandLineParser>

#include "../../lsMisc/stdQt/stdQt.h"
#include "../../lsMisc/stdQt/runguard.h"
#include "../../lsMisc/stdQt/settings.h"

#include "consts.h"
#include "globals.h"
#include "sql.h"
#include "osd.h"
#include "taskgetdir.h"
#include "helper.h"
#include "tableitemdata.h"
#include "ffmpeg.h"
#include "commandoption.h"
#include "mainwindow.h"

using namespace Consts;
using namespace AmbiesoftQt;

#ifdef QT_DEBUG
static void testSQL()
{
    Q_ASSERT(isThumbFileName("58c4d22e-8b8b-4773-9fac-80a69a8fa880-5.jpg"));
    Q_ASSERT(isThumbFileName("58c4d22e-AAAA-4773-9fac-80a69a8fa880-5.jpg"));
    Q_ASSERT(!isThumbFileName("58c4d22e8b8b47739fac80a69a8fa8805.jpg"));
    Q_ASSERT(!isThumbFileName("58c4d22e-8b8b-4773-9fac-80a69a8fa880-5"));
    Q_ASSERT(!isThumbFileName("5b8b-4773-9fac-80a69a8fa880-5"));

    Q_ASSERT(!isThumbFileName(""));
    Q_ASSERT(!isThumbFileName("a58c4d22e-8b8b-4773-9fac-80a69a8fa880-5.jpg"));
#ifdef Q_WS_WIN
    Q_ASSERT(isThumbFileName("thumb\\58c4d22e-8b8b-4773-9fac-80a69a8fa880-5.jpg"));
    Q_ASSERT(isThumbFileName("T:\\thumb\\58c4d22e-8b8b-4773-9fac-80a69a8fa880-5.jpg"));
#endif
    Q_ASSERT(isThumbFileName("/thumb/58c4d22e-8b8b-4773-9fac-80a69a8fa880-5.jpg"));
    Q_ASSERT(isThumbFileName("thumb/58c4d22e-8b8b-4773-9fac-80a69a8fa880-5.jpg"));


    QString dir = normalizeDir(GetAppDir());
    QString file = "moviefile.mp3";
    QString file2 =  "file2.mp4";

    // first clean all items of
    QFile(pathCombine(dir,file)).remove();
    QFile(pathCombine(dir,file2)).remove();

    Q_ASSERT(gpSQL->RemoveAllMissingEntries(dir));

    // prepare actual file for not causing assert.
    qDebug() << "Test file is " << pathCombine(dir,file);
    {
        QFile f(pathCombine(dir,file));
        Q_ASSERT(f.open(QFile::WriteOnly));
        f.write("data");
    }


    QStringList thumbfiles;
    thumbfiles << "00c75732-e92b-4e3c-90a1-914bd137797f-1.png";
    thumbfiles << "00c75732-e92b-4e3c-90a1-914bd137797f-2.png";
    thumbfiles << "00c75732-e92b-4e3c-90a1-914bd137797f-3.png";
    thumbfiles << "00c75732-e92b-4e3c-90a1-914bd137797f-4.png";
    thumbfiles << "00c75732-e92b-4e3c-90a1-914bd137797f-5.png";

    static int la=QTime::currentTime().second();
    static int oc=100;

    TableItemDataPointer tid = TableItemData::Create(0,
                                                     thumbfiles,
                                                     dir,
                                                     file,
                                                     33333,
                                                     22344,
                                                     33333,
                                                     240,
                                                     160,
                                                     33.33,
                                                     "frm",
                                                     33,
                                                     "vc",
                                                     "ac",
                                                     640,
                                                     480,
                                                     oc++,
                                                     ++la);
    tid->setThumbExt("jpg");

    // insert same tid and check same id
    gpSQL->AppendData(tid);
    qint64 id1 = tid->getID();
    qDebug() << "InsertedID=" << id1;

    gpSQL->AppendData(tid);
    qint64 id2 = tid->getID();
    qDebug() << "InsertedID=" << id2;

    Q_ASSERT(id1==id2);


    // check rename does not change id
    QFile(pathCombine(dir,file)).rename(pathCombine(dir,file2));
    Q_ASSERT(gpSQL->RenameEntry(dir, file, dir,file2));
    qint64 id3;
    Q_ASSERT(gpSQL->GetID(dir, file, id3));
    Q_ASSERT(id3==-1);
    Q_ASSERT(gpSQL->GetID(dir,file2,id3));
    Q_ASSERT(id1==id3);


}
#endif


bool OpenDatabaseDirectory(const QString& dbDir)
{
    if(Q_UNLIKELY(!QDir(dbDir).exists()))
    {
        Alert(nullptr,QObject::tr("\"%1\" is not directory.").arg(dbDir));
        return false;
    }
    if(Q_UNLIKELY(!QDir::setCurrent(dbDir)))
    {
        Alert(nullptr, QObject::tr("Failed to set \"%1\" as current directory.").arg(dbDir));
        return false;
    }

    qDebug() << "Current Directory =" << dbDir;

    // create and check thumbs dir
    QDir(".").mkdir(FILEPART_THUMBS);
    if(Q_UNLIKELY(!QDir(FILEPART_THUMBS).exists()))
    {
        Alert(nullptr, QObject::tr("Failed to mkdir \"%1\" or it is not a directory.").
              arg(QFileInfo(FILEPART_THUMBS).absoluteFilePath()));
        return false;
    }


    return true;
}

bool GetDatabaseDirectory(IniSettings& settings, QString& dbDirToSet, bool& bQuit)
{
    bQuit = false;
    if(settings.valueBool(KEY_USE_CUSTOMDATABASEDIR))
    {
        dbDirToSet = ExpandEnv(settings.valueString(KEY_DATABASE_PATH));
    }
    else
        dbDirToSet.clear();

    if(!dbDirToSet.isEmpty() && !QDir(dbDirToSet).exists())
    {
        QDir().mkpath(dbDirToSet);
        if(!QDir(dbDirToSet).exists())
        {
            Alert(nullptr,QObject::tr("Failed to create directory \"%1\"").
                  arg(dbDirToSet));
        }
    }


    if (dbDirToSet.isEmpty())
    {
        dbDirToSet = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        QDir().mkpath(dbDirToSet);
    }

    if(dbDirToSet.isEmpty() || !QDir(dbDirToSet).exists())
    {
        if(settings.valueBool(KEY_USE_CUSTOMDATABASEDIR))
        {
            dbDirToSet = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
            QFileDialog dlg;
            dlg.setFileMode(QFileDialog::FileMode::DirectoryOnly);
            dlg.setDirectory(dbDirToSet);
            dlg.setWindowTitle(QObject::tr("Choose Database directory"));
            if(!dlg.exec())
            {
                bQuit = true;
                return true;
            }

            if(dlg.selectedFiles().isEmpty())
                return false;

            dbDirToSet = dlg.selectedFiles()[0];
            settings.setValue(KEY_DATABASE_PATH, dbDirToSet);
        }
        else
        {
            // default dir
            return false;
        }
    }
    return true;
}

enum PROGRAM_RETURN {
    PR_OK = 0,
    PR_GETDIRECTORYFAILED,
    PR_OPENSCENEDIRECTORYFAILED,
    PR_SQLOPENFAILED,
    PR_WINDOWINITFAILED,
    PR_LAUNCHTHISAPPFAILED,
};

std::unique_ptr<CommandOption> gpCommandOption;

static bool IsAvailableLanguage(const QString& lang)
{
    QString i18nFile = ":/translations/i18n_" + lang + ".qm";
    return QFile(i18nFile).exists();
}

int main2(int argc, char *argv[], QApplication& theApp)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    bool bExit = false;
    QString inifile = getInifile(bExit);// "N:\\Ambiesoft\\SceneExplorer\\SceneExplorer.ini";
    if(bExit)
        return 1;
    QScopedPointer<IniSettings> settings(inifile.isEmpty() ?
                                          new IniSettings(QApplication::organizationName(), QApplication::applicationName()) :
                                          new IniSettings(inifile));
    if(!settings->isAccessible())
    {
        Alert(nullptr,
              QObject::tr("\"%1\" is not accessible.").arg(settings->fileName()));
        return 1;
    }

    const QString lang = settings->valueString(KEY_LANGUAGE);

    QTranslator qtTranslator;
    do {
        QString qti18nFile = "qt_";
        if(lang.isEmpty())
        {
            // default
            QString syslang = GetSystemDefaultLang();
            if(syslang=="English" || !IsAvailableLanguage(syslang))
                break;

            qti18nFile += QLocale::system().name();
        }
        else if(lang=="English")
        {
            qti18nFile += "en_US";
            break;
        }
        else if(lang=="Japanese")
        {
            qti18nFile += "ja_JP";
        }
        else
        {
            Q_ASSERT(false);
            break;
        }
        qDebug() << "Qt language" << qti18nFile;
        if(qtTranslator.load(qti18nFile,
                             QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        {
            theApp.installTranslator(&qtTranslator);
        }
        else
        {
            Alert(nullptr, QObject::tr("Failed to set Qt language to '%1'.").arg(lang));
        }
    } while(false);

    QTranslator myappTranslator;
    QString i18nFile = ":/translations/i18n_";

    do {
        if(lang.isEmpty())
        {
            // default
            QString syslang = GetSystemDefaultLang();
            if(syslang=="English" || !IsAvailableLanguage(syslang))
                break;

            i18nFile += syslang;
        }
        else if(lang=="English")
        {
            i18nFile.clear();
            break;
        }
        else if(lang=="Japanese")
        {
            i18nFile += lang;
        }
        else
        {
            Q_ASSERT(false);
            break;
        }
        if(myappTranslator.load(i18nFile)) //, trdir);
        {
            theApp.installTranslator(&myappTranslator);
        }
        else
        {
            Alert(nullptr, QObject::tr("Failed to set language to '%1'.").arg(lang));
        }
    } while(false);

    RunGuard guard("SceneExplorer");
    if (!guard.tryToRun())
    {
        Info(nullptr, QObject::tr("Another instance is already running."));
        return 0;
    }

    qDebug () << "CurrentStyle: " << QApplication::style()->objectName();

    // style:  "windows", "windowsvista", "fusion", or "macintosh".
    QString style = settings->valueString(KEY_STYLE);
    if(!style.isEmpty())
    {
        if(!QApplication::setStyle(style))
        {
            Alert(nullptr, QObject::tr("Failed to set style") + ": " + style);
        }
    }


    TaskGetDir::RegisterMetaType();


    QString dbdir;
    dbdir = gpCommandOption->DBDir();
    if(dbdir.isEmpty())
    {
        bool bQuit = false;
        if(!GetDatabaseDirectory(*settings,dbdir,bQuit))
        {
            Alert(nullptr, QObject::tr("Failed to get default database directory."));
            return PR_GETDIRECTORYFAILED;
        }
        if(bQuit)
            return 0;
    }
    if(!OpenDatabaseDirectory(dbdir))
    {
        QString message = QObject::tr("Failed to open database directory \"%1\".").
                arg(dbdir);
        if(settings->valueBool(KEY_USE_CUSTOMDATABASEDIR))
        {
            message += "\n\n";
            message += QObject::tr("Do you want to set database directory to default?");
            if(YesNo(nullptr, message))
            {
                settings->setValue(KEY_USE_CUSTOMDATABASEDIR, false);
                gReboot = true;
            }
        }
        else
        {
            Alert(nullptr, message);
        }
        return PR_OPENSCENEDIRECTORYFAILED;
    }

    Sql theSql;
    if (Q_UNLIKELY(!theSql.isOK()))
    {
        QString errMessage =
                QObject::tr("Failed to open or create database. \"%1\"").
                arg(QFileInfo(Sql::getDBFileName()).absoluteFilePath());
        errMessage += "\n\n";
        errMessage += theSql.getLastError();
        Alert(nullptr, errMessage);
        return PR_SQLOPENFAILED;
    }
    gpSQL = &theSql;
#ifdef QT_DEBUG
    testSQL();
    testSQL();
#endif


    MainWindow w(nullptr, *settings, gpCommandOption->doc());
    if(Q_UNLIKELY(!w.IsInitialized()))
    {
        Alert(nullptr, QObject::tr("Window initialization failed."));
        return PR_WINDOWINITFAILED;
    }
    w.show();

    return theApp.exec();
}

void noMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    Q_UNUSED(msg);
    //    QByteArray localMsg = msg.toLocal8Bit();
    //    switch (type) {
    //    case QtDebugMsg:
    //        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    //        break;
    //    case QtInfoMsg:
    //        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    //        break;
    //    case QtWarningMsg:
    //        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    //        break;
    //    case QtCriticalMsg:
    //        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    //        break;
    //    case QtFatalMsg:
    //        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    //        abort();
    //    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(ORGANIZATION);
    QCoreApplication::setOrganizationDomain(APPDOMAIN);
    QCoreApplication::setApplicationName(APPNAME);
    QCoreApplication::setApplicationVersion(APPVERSION);

    QApplication app(argc, argv);

#ifdef QT_NO_DEBUG
    if ( !app.arguments().contains(QLatin1String("--with-debug") ))
    {
        qInstallMessageHandler(noMessageOutput);
    }
#endif

    if(!processCommandLine())
        return 0;

    int ret = main2(argc, argv, app);
    if(gReboot)
    {
        QString thisapp = QCoreApplication::applicationFilePath();
        QStringList args = QCoreApplication::arguments();
        if(args.length() != 0)
            args.removeFirst();;

        if(Q_UNLIKELY(!QProcess::startDetached(thisapp, args)))
        {
            Alert(nullptr, QObject::tr("Failed to launch \"%1\"").
                  arg(thisapp));
            return PR_LAUNCHTHISAPPFAILED;
        }
    }
    return ret;
}
