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

#include "consts.h"
#include "globals.h"
#include "sql.h"

#include "runguard.h"
#include "taskgetdir.h"
#include "helper.h"
#include "tableitemdata.h"
#include "settings.h"
#include "ffmpeg.h"

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
        soc++);
	gpSQL->AppendData(tid);
}
#endif


bool OpenSceneDirectory(Settings& settings,const QString& dbDir)
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

    settings.setValue(Consts::KEY_DATABASE_PATH, dbDir);
    return true;
}

bool GetDefaultSceneDirectory(Settings& settings, QString& dbdir)
{
    if(settings.valueBool(Consts::KEY_USE_CUSTOMDATABASEDIR))
        dbdir = settings.valueString(Consts::KEY_DATABASE_PATH);
    else
        dbdir.clear();

    if(!dbdir.isEmpty() && !QDir(dbdir).exists())
    {
        QDir().mkpath(dbdir);
        if(!QDir(dbdir).exists())
        {
            Alert(nullptr,QString(QObject::tr("Failed to create directory \"%1\"")).
                  arg(dbdir));
        }
    }


	if (dbdir.isEmpty())
	{
		dbdir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        QDir().mkpath(dbdir);
	}

    if(dbdir.isEmpty() || !QDir(dbdir).exists())
    {
        dbdir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        QFileDialog dlg;
        dlg.setFileMode(QFileDialog::FileMode::DirectoryOnly);
        dlg.setDirectory(dbdir);
        dlg.setWindowTitle(QObject::tr("Choose Database directory"));
        if(!dlg.exec())
            return false;

        if(dlg.selectedFiles().isEmpty())
            return false;

        dbdir = dlg.selectedFiles()[0];
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


int main2(int argc, char *argv[], QApplication& theApp)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QTranslator qtTranslator;
    QString qti18nFile = "qt_" + QLocale::system().name();
    qtTranslator.load(qti18nFile,
         QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    theApp.installTranslator(&qtTranslator);


    QTranslator myappTranslator;
    // QString i18nFile = "i18n_ja_JP";// + QLocale::system().name();
    QString i18nFile = "i18n_ja";// + QLocale::system().name();

	QFileInfo trfi(QCoreApplication::applicationFilePath());
	QString trdir = pathCombine(trfi.absolutePath(), "translations");
    myappTranslator.load(i18nFile, trdir);
    theApp.installTranslator(&myappTranslator);


	RunGuard guard("SceneExplorer");
	if (!guard.tryToRun())
	{
		Info(nullptr, QObject::tr("Another instance is already running."));
		return 0;
	}
    
    TaskGetDir::RegisterMetaType();


	QString inifile = getInifile();// "N:\\Ambiesoft\\SceneExplorer\\SceneExplorer.ini";
    QScopedPointer<Settings> settings(inifile.isEmpty() ? new Settings : new Settings(inifile));
    QString dbdir;
    if(!GetDefaultSceneDirectory(*settings,dbdir))
        return PR_GETDIRECTORYFAILED;

    if(!OpenSceneDirectory(*settings,dbdir))
        return PR_OPENSCENEDIRECTORYFAILED;

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
        return PR_WINDOWINITFAILED;
    w.show();

    return theApp.exec();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(Consts::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(Consts::APPDOMAIN);
    QCoreApplication::setApplicationName(Consts::APPNAME);

    QApplication app(argc, argv);

    int ret = main2(argc, argv, app);
    if(gReboot)
    {
        QString thisapp = QCoreApplication::applicationFilePath();
        if(!QProcess::startDetached(thisapp))
        {
            Alert(nullptr, QString(QObject::tr("Failed to launch \"%1\"")).
                  arg(thisapp));
            return PR_LAUNCHTHISAPPFAILED;
        }
    }
    return ret;
}
