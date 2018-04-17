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

#include "consts.h"
#include "globals.h"
#include "sql.h"

#include "runguard.h"
#include "taskgetdir.h"
#include "helper.h"
#include "tableitemdata.h"
#include "settings.h"
#include "ffmpeg.h"
#include "commandoption.h"
#include "mainwindow.h"

#ifdef QT_DEBUG
static void testSQL()
{
	QStringList files;
	files << "00c75732-e92b-4e3c-90a1-914bd137797f-1.png";
	files << "00c75732-e92b-4e3c-90a1-914bd137797f-2.png";
	files << "00c75732-e92b-4e3c-90a1-914bd137797f-3.png";
	files << "00c75732-e92b-4e3c-90a1-914bd137797f-4.png";
	files << "00c75732-e92b-4e3c-90a1-914bd137797f-5.png";

    static int soc=11;
	TableItemDataPointer tid = TableItemData::Create(files,
		"X:/aaa/bbb/",
		"moviefile.mp3",
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
        soc++,
        0);
	gpSQL->AppendData(tid);
}
#endif


bool OpenDatabaseDirectory(const QString& dbDir)
{
    if(!QDir(dbDir).exists())
    {
        Alert(nullptr,QString(QObject::tr("\"%1\" is not directory.")).arg(dbDir));
        return false;
    }
    if(!QDir::setCurrent(dbDir))
    {
        Alert(nullptr, QString(QObject::tr("Failed to set \"%1\" as current directory.").arg(dbDir)));
        return false;
    }

    qDebug() << "Current Directory =" << dbDir;

    // create and check thumbs dir
    QDir(".").mkdir(Consts::FILEPART_THUMBS);
    if(!QDir(Consts::FILEPART_THUMBS).exists())
    {
        Alert(nullptr, QString(QObject::tr("Failed to mkdir \"%1\" or it is not a directory.")).
            arg(QFileInfo(Consts::FILEPART_THUMBS).absoluteFilePath()));
        return false;
    }


    return true;
}

bool GetDefaultDatabaseDirectory(Settings& settings, QString& dbDir, bool& bQuit)
{
    bQuit = false;
    if(settings.valueBool(Consts::KEY_USE_CUSTOMDATABASEDIR))
        dbDir = settings.valueString(Consts::KEY_DATABASE_PATH);
    else
        dbDir.clear();

    if(!dbDir.isEmpty() && !QDir(dbDir).exists())
    {
        QDir().mkpath(dbDir);
        if(!QDir(dbDir).exists())
        {
            Alert(nullptr,QString(QObject::tr("Failed to create directory \"%1\"")).
                  arg(dbDir));
        }
    }


    if (dbDir.isEmpty())
	{
        dbDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        QDir().mkpath(dbDir);
	}

    if(dbDir.isEmpty() || !QDir(dbDir).exists())
    {
        if(settings.valueBool(Consts::KEY_USE_CUSTOMDATABASEDIR))
        {
            dbDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
            QFileDialog dlg;
            dlg.setFileMode(QFileDialog::FileMode::DirectoryOnly);
            dlg.setDirectory(dbDir);
            dlg.setWindowTitle(QObject::tr("Choose Database directory"));
            if(!dlg.exec())
            {
                bQuit = true;
                return true;
            }

            if(dlg.selectedFiles().isEmpty())
                return false;

            dbDir = dlg.selectedFiles()[0];
            settings.setValue(Consts::KEY_DATABASE_PATH, dbDir);
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

CommandOption* gpCommandOption;

int main2(int argc, char *argv[], QApplication& theApp)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QString inifile = getInifile();// "N:\\Ambiesoft\\SceneExplorer\\SceneExplorer.ini";
    QScopedPointer<Settings> settings(inifile.isEmpty() ? new Settings : new Settings(inifile));


    QTranslator qtTranslator;
    QString qti18nFile = "qt_" + QLocale::system().name();
    qtTranslator.load(qti18nFile,
         QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    theApp.installTranslator(&qtTranslator);


    QTranslator myappTranslator;
    QString i18nFile = ":/translations/i18n_";
    QString lang = settings->valueString(Consts::KEY_LANGUAGE);
    if(lang.isEmpty())
    {
        // default
        i18nFile += GetSystemDefaultLang();
    }
    else if(lang=="English")
    {
        i18nFile.clear();
    }
    else
    {
        i18nFile += lang;
    }
//	QFileInfo trfi(QCoreApplication::applicationFilePath());
//	QString trdir = pathCombine(trfi.absolutePath(), "translations");
    myappTranslator.load(i18nFile);//, trdir);
    theApp.installTranslator(&myappTranslator);


	RunGuard guard("SceneExplorer");
	if (!guard.tryToRun())
	{
		Info(nullptr, QObject::tr("Another instance is already running."));
		return 0;
	}
    
    // style:  "windows", "windowsvista", "fusion", or "macintosh".
    QApplication::setStyle("fusion");


    TaskGetDir::RegisterMetaType();


    QString dbdir;
    dbdir = gpCommandOption->DBDir();
    if(dbdir.isEmpty())
    {
        bool bQuit = false;
        if(!GetDefaultDatabaseDirectory(*settings,dbdir,bQuit))
        {
            Alert(nullptr, QString(QObject::tr("Failed to get default database directory.")));
            return PR_GETDIRECTORYFAILED;
        }
        if(bQuit)
            return 0;
    }
    if(!OpenDatabaseDirectory(dbdir))
    {
        QString message = QString(QObject::tr("Failed to open database directory \"%1\".")).
                        arg(dbdir);
        if(settings->valueBool(Consts::KEY_USE_CUSTOMDATABASEDIR))
        {
            message += "\n\n";
            message += QObject::tr("Do you want to set database directory to default?");
            if(YesNo(nullptr, message))
            {
                settings->setValue(Consts::KEY_USE_CUSTOMDATABASEDIR, false);
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
	if (!theSql.isOK())
	{
		Alert(nullptr, QString(QObject::tr("Failed to open or create database. \"%1\"")).
			arg(QFileInfo(Sql::getDBFileName()).absoluteFilePath()));
        return PR_SQLOPENFAILED;
	}
	gpSQL = &theSql;
#ifdef QT_DEBUG
	testSQL();
	testSQL();
#endif


    MainWindow w(nullptr, *settings);
    if(!w.IsInitialized())
    {
        Alert(nullptr, QString(QObject::tr("Window initialization failed.")));
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
    QCoreApplication::setOrganizationName(Consts::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(Consts::APPDOMAIN);
    QCoreApplication::setApplicationName(Consts::APPNAME);
    QCoreApplication::setApplicationVersion(Consts::APPVERSION);

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
        if(!QProcess::startDetached(thisapp, QCoreApplication::arguments()))
        {
            Alert(nullptr, QString(QObject::tr("Failed to launch \"%1\"")).
                  arg(thisapp));
            return PR_LAUNCHTHISAPPFAILED;
        }
    }
    return ret;
}
