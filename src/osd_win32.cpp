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

#include <Windows.h>
#include <Shlobj.h>

#include <QApplication>
#include <QDir>
//#include <QMessageBox>
//#include <QProcessEnvironment>
//#include <QFileInfo>
//#include <QDesktopServices>
#include <QWidget>

#include "../../profile/cpp/Profile/include/ambiesoft.profile.h"
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


QString GetDefaultFFprobe()
{
    return pathCombine(GetAppDir(), "ffmpeg/bin/ffprobe.exe");
}
QString GetDefaultFFmpeg()
{
    return pathCombine(GetAppDir(), "ffmpeg/bin/ffmpeg.exe");
}

static QString getSpecialFolder(int id)
{
    wchar_t path[MAX_PATH] = {0};
    if(S_OK != SHGetFolderPath(
                NULL,
                id,
                NULL,
                SHGFP_TYPE_CURRENT,
                path))

    {
        Alert(nullptr, QObject::tr("Failed to get folder location."));
        return QString();
    }


//    PWSTR pOut=NULL;
//    if(S_OK != SHGetKnownFolderPath(id,0,NULL,&pOut))
//    {
//        Alert(nullptr, QObject::tr("Failed to get Roaming folder. Default folder will be used."));
//        return QString();
//    }

    QString dir = QString::fromUtf16((const ushort*)path);
//    CoTaskMemFree(pOut);

    dir = QDir(dir).canonicalPath();

    QDir(dir).mkdir("Ambiesoft");
    dir = pathCombine(dir, "Ambiesoft");

    QDir(dir).mkdir("SceneExplorer");
    dir = pathCombine(dir, "SceneExplorer");

    return dir;
}
QString getInifile(bool& bExit)
{
    QFileInfo trfi(QCoreApplication::applicationFilePath());
    std::string folini = pathCombine(trfi.absolutePath(), "folder.ini").toStdString();
    int intval = -1;
    Ambiesoft::Profile::GetInt("Main", "PathType", -1, intval, folini);

    QString dir;
    switch (intval)
    {
    case 0:
        dir = trfi.absolutePath();
        break;

    case 1:
        dir = getSpecialFolder(CSIDL_LOCAL_APPDATA);
        break;

    default:
    case 2:
        dir = getSpecialFolder(CSIDL_APPDATA);
        break;

    case 3:
    {
        std::string t;
        Ambiesoft::Profile::GetString("Main", "folder", "", t, folini);
        dir = t.c_str();
        if(dir.isEmpty())
        {
            Alert(nullptr, QObject::tr("Folder settings does not have sufficient data. Lanuch FolderConfig.exe and reconfigure."));
            bExit=true;
            return QString();
        }
        else
        {
            if(!QDir(dir).exists())
            {
                if(!YesNo(nullptr, QObject::tr("\"%1\" does not exist. Do you want to create it?").arg(dir)))
                {
                    bExit=true;
                    return QString();
                }
                QDir(dir).mkpath(".");
                if(!QDir(dir).exists())
                {
                    Alert(nullptr, QObject::tr("Failed to create directory."));
                    bExit=true;
                    return QString();
                }
            }
        }
    }
    break;
    } // switch

    if (dir.isEmpty())
        return QString();

    return pathCombine(dir, "SceneExplorer.ini");
}
