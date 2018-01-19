#include <QApplication>
#include <QSettings>

#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QObject>
#include <QDebug>

#include "consts.h"
#include "globals.h"
#include "sql.h"

#include "runguard.h"
#include "taskgetdir.h"
#include "helper.h"
#include "tableitemdata.h"
#include "settings.h"
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

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(Consts::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(Consts::APPDOMAIN);
    QCoreApplication::setApplicationName(Consts::APPNAME);



    QApplication app(argc, argv);
	
	RunGuard guard("SceneExplorer");
	if (!guard.tryToRun())
	{
		Info(nullptr, QObject::tr("Another instance is already running."));
		return 0;
	}
    
    TaskGetDir::RegisterMetaType();

	// QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    Settings settings;
    QString dbDir = settings.valueString(Consts::KEY_DBPATH);
    if(dbDir.isEmpty() || !QDir(dbDir).exists())
    {
        dbDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        QDir(dbDir).mkpath("db");
        dbDir = pathCombine(dbDir, "db");
    }
    if(!QDir(dbDir).exists())
    {
        Alert(nullptr,QString(QObject::tr("\"%1\" is not directory.")).arg(dbDir));
        return 1;
    }
    if(!QDir::setCurrent(dbDir))
    {
        Alert(nullptr, QString(QObject::tr("Failed to set \"%1\" as current directory.").arg(dbDir)));
        return 1;
    }

    qDebug() << "Current Directory =" << dbDir;

    // create and check thumbs dir
    QDir(".").mkdir(Consts::FILEPART_THUMBS);
    if(!QDir(Consts::FILEPART_THUMBS).exists())
    {
        Alert(nullptr, QString(QObject::tr("Failed to mkdir \"%1\" or it is not a directory.")).
			arg(QFileInfo(Consts::FILEPART_THUMBS).absoluteFilePath()));
        return 1;
    }

	Sql theSql;
	if (!theSql.isOK())
	{
		Alert(nullptr, QString(QObject::tr("Failed to open or create database. \"%1\"")).
			arg(QFileInfo(Sql::getDBFileName()).absoluteFilePath()));
		return 1;
	}
	gpSQL = &theSql;
#ifdef QT_DEBUG
	testSQL();
	testSQL();
#endif
    MainWindow w(nullptr, settings);
    if(!w.IsInitialized())
        return 1;
    w.show();

    return app.exec();
}
