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

#include "helper.h"

#include "settings.h"
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(Consts::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(Consts::APPDOMAIN);
    QCoreApplication::setApplicationName(Consts::APPNAME);

    QApplication app(argc, argv);

    // QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    Settings settings;
    QString dbDir = settings.value(Consts::KEY_DBPATH).toString();
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

    MainWindow w(nullptr, settings);
    if(!w.IsInitialized())
        return 1;
    w.show();

    return app.exec();
}
