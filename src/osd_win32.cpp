//#include <QDir>
//#include <QMessageBox>
//#include <QProcessEnvironment>
//#include <QFileInfo>
//#include <QDesktopServices>
#include <QWidget>

#include "helper.h"

// https://stackoverflow.com/a/3546503
bool showInGraphicalShell(QWidget *parent, const QString &pathIn)
{
    Q_UNUSED(parent);

	QString explorer;
	QString path = QProcessEnvironment::systemEnvironment().value("PATH");
	QStringList paths = path.split(';');
	for (int i = 0; i < paths.length(); ++i)
	{
		QString s = pathCombine(paths[i], "explorer.exe");

		if (QFile(s).exists())
		{
			explorer = s;
			break;
		}
	}
	if (explorer.isEmpty()) {
        return false;
	}
	QString param;
	if (!QFileInfo(pathIn).isDir())
	{
		QStringList args;
		args << "/select,";
		args << QDir::toNativeSeparators(pathIn);
		args << ",/n";
		QProcess::startDetached(explorer, args);
	}
	else
	{
		QStringList args;
		args << QDir::toNativeSeparators(pathIn);
		QProcess::startDetached(explorer, args);
	}

    return true;
}
