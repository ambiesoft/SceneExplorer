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

// #define _WIN32_WINNT 0x0501 // XP and above
#include <Windows.h>
#include <ShlObj.h>

#include <QApplication>
#include <QDir>
//#include <QMessageBox>
//#include <QProcessEnvironment>
//#include <QFileInfo>
//#include <QDesktopServices>
#include <QWidget>
#include <QThread>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QProcess>
#include <QDebug>
#include <QtWin>

#include "../../lsMisc/stdQt/stdQt.h"

#include "../../lsMisc/GetLastErrorString.h"
#include "../../profile/cpp/Profile/include/ambiesoft.profile.h"
#include "../../lsMisc/stdosd/SetPrority.h"
#include "../../lsMisc/OpenCommon.h"

#include "errorinfoexception.h"
#include "helper.h"
#include "osd.h"

using namespace Ambiesoft::stdosd::Process;
using namespace Ambiesoft;
using namespace AmbiesoftQt;


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
        qDebug() << rv << QString::number( rv ).toInt( nullptr, 8 ) << __FUNCTION__;
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







bool setProcessPriority(const qint64& pid, QThread::Priority priority, QStringList& errors)
{
    CPUPRIORITY cpuPriority = CPU_NONE;
    IOPRIORITY ioPriority = IO_NONE;
    MEMORYPRIORITY memPriority = MEMORY_NONE;

    switch (priority)
    {
    case QThread::HighestPriority:
        cpuPriority = CPU_HIGH;
        ioPriority = IO_HIGH;
        memPriority = MEMORY_HIGH;
        break;
    case QThread::HighPriority:
        cpuPriority = CPU_ABOVENORMAL;
        ioPriority = IO_ABOVENORMAL;
        memPriority = MEMORY_ABOVENORMAL;
        break;
    case QThread::NormalPriority:
        cpuPriority = CPU_NORMAL;
        ioPriority = IO_NORMAL;
        memPriority = MEMORY_NORMAL;
        break;
    case QThread::LowPriority:
        cpuPriority = CPU_BELOWNORMAL;
        ioPriority = IO_BELOWNORMAL;
        memPriority = MEMORY_BELOWNORMAL;
        break;
    case QThread::LowestPriority:
    case QThread::IdlePriority:
        cpuPriority = CPU_IDLE;
        ioPriority = IO_IDLE;
        memPriority = MEMORY_IDLE;
        break;
    default:
        Q_ASSERT(false);
        return false;
    }

    int ret = SetProirity(pid,
                          cpuPriority,
                          ioPriority,
                          memPriority);

    if (ret != 0)
    {
        std::wstring lastError = GetLastErrorString(static_cast<DWORD>(ret));
        errors << QString::fromStdWString(lastError);
    }
    return ret==0;
}


bool myRename(const QString& oldfull, const QString& newfull, QString& error)
{
    if(!MoveFileW(oldfull.toStdWString().c_str(), newfull.toStdWString().c_str()))
    {
        error = QString::fromStdWString(GetLastErrorStringW(GetLastError()));
        return false;
    }
    return true;
}



bool StartProcessDetached(const QString& exe, const QString& arg)
{
    return !!OpenCommon(nullptr,
               exe.toStdWString().c_str(),
               arg.toStdWString().c_str());
}

QIcon GetIconFromExecutable(const QString& exe)
{
    SHFILEINFO info = {};
    std::unique_ptr<SHFILEINFO,std::function<void(SHFILEINFO*p)>> freer(
                                                                      &info,
                                                                      [](SHFILEINFO*p)
    {
        if(p && p->hIcon)
            DestroyIcon(p->hIcon);
    });

    UINT flags =
            SHGFI_USEFILEATTRIBUTES |
            SHGFI_ICON |
            SHGFI_SMALLICON;

    if(!SHGetFileInfo(QDir::toNativeSeparators(exe).toStdWString().c_str(),
                      0,
                      &info,
                      sizeof(info),
                      flags))
    {
        qDebug() << "SHGetFileInfo failed" << __FUNCTION__;
        return QIcon();
    }
    if(!info.hIcon)
    {
        qDebug() << "SHGetFileInfo returns null icon" << __FUNCTION__;
        return QIcon();
    }

    return QIcon(QtWin::fromHICON(info.hIcon));
}
