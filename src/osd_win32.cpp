#include <Windows.h>

//#include <QDir>
//#include <QMessageBox>
//#include <QProcessEnvironment>
//#include <QFileInfo>
//#include <QDesktopServices>
#include <QWidget>

#include "errorinfoexception.h"
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

// https://stackoverflow.com/a/17974223
void MoveToTrashImpl( QString file ){
    QFileInfo fileinfo( file );
    if( !fileinfo.exists() )
        throw ErrorInfoException( QObject::tr("File doesnt exists, cant move to trash" ));
    WCHAR from[ MAX_PATH ];
    memset( from, 0, sizeof( from ));
    int l = fileinfo.absoluteFilePath().toWCharArray( from );
    Q_ASSERT( 0 <= l && l < MAX_PATH );
    from[ l ] = '\0';
    SHFILEOPSTRUCT fileop;
    memset( &fileop, 0, sizeof( fileop ) );
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperation( &fileop );
    if( 0 != rv ){
        qDebug() << rv << QString::number( rv ).toInt( 0, 8 );
        throw ErrorInfoException( QObject::tr("move to trash failed" ));
    }
}
