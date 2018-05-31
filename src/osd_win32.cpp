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
#include <Shlobj.h>

#include <QApplication>
#include <QDir>
//#include <QMessageBox>
//#include <QProcessEnvironment>
//#include <QFileInfo>
//#include <QDesktopServices>
#include <QWidget>
#include <QThread>

#include "../../profile/cpp/Profile/include/ambiesoft.profile.h"
#include "errorinfoexception.h"
#include "helper.h"
#include "osd.h"

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

    dir = QDir(dir).absolutePath();

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

// https://stackoverflow.com/a/45282192
bool isLegalFilePath(QString filename, QString* pError)
{
    // Windows filenames are not case sensitive.
    filename = filename.toUpper();

    QString illegal=GetIllegalFilenameCharacters();

    foreach (const QChar& c, filename)
    {
        // Check for control characters
         if (c.toLatin1() > 0 && c.toLatin1() < 32)
         {
             if(pError)
             {
                 *pError = QObject::tr("Filename could not have control characters.");
             }
             return false;
        }
        // Check for illegal characters
        if (illegal.contains(c))
        {
            if(pError)
            {
                *pError = QObject::tr("Filename could not have these characters.");
                *pError += "\n\n";
                *pError += GetIllegalFilenameCharacters();
            }
            return false;
        }
    }

    // Check for device names in filenames
    static QStringList devices;

    if (!devices.count())
        devices << "CON" << "PRN" << "AUX" << "NUL" << "COM0" << "COM1" << "COM2"
                << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8" << "COM9" << "LPT0"
                << "LPT1" << "LPT2" << "LPT3" << "LPT4" << "LPT5" << "LPT6" << "LPT7" << "LPT8"
                << "LPT9";

    const QFileInfo fi(filename);
    const QString basename = fi.baseName();

    foreach (const QString& s, devices)
    {
        if (basename == s)
        {
            if(pError)
            {
                *pError = QObject::tr("Filename must not be device names.");
            }
            return false;
        }
    }
    // Check for trailing periods or spaces
    if (filename.right(1)==".")
    {
        if(pError)
        {
            *pError = QObject::tr("Filename should not end with period.");
        }
        return false;
    }
    if( filename.right(1)==" ")
    {
        if(pError)
        {
            *pError = QObject::tr("Filename must not end with space character.");
        }
        return false;
    }

    if (filename.right(1)=="\\")
    {
        if(pError)
        {
            *pError = QObject::tr("Filename must not end with backslash character.");
        }
        return false;
    }
    if (filename.right(1)=="/")
    {
        if(pError)
        {
            *pError = QObject::tr("Filename must not end with slash character.");
        }
        return false;
    }
    // Check for pathnames that are too long (disregarding raw pathnames)
    if (filename.length()>260)
    {
        if(pError)
        {
            *pError = QObject::tr("Filename is too long.");
        }
        return false;
    }

//    // Exclude raw device names
//    if (filename.left(4)=="\\\\.\\")
//        return false;


    return true;
}
QString GetIllegalFilenameCharacters()
{
    return "<>:\"|?*";
}

struct NativeHandle
{
	HANDLE h_;
	NativeHandle(HANDLE h) : h_(h)
	{}
	~NativeHandle()
	{
		if(h_)
			CloseHandle(h_);
	}
	operator bool() const {
		return h_ != NULL;
	}
    operator HANDLE() const {
		return h_;
	}
};

// http://blog.misterfoo.com/2010/07/process-priority-utility.html
// these values determined by poking around in the debugger - use at your own risk!
const DWORD ProcessInformationMemoryPriority = 0x27;
const DWORD ProcessInformationIoPriority = 0x21;
const DWORD DefaultMemoryPriority = 5;
const DWORD LowMemoryPriority = 3;
const DWORD DefaultIoPriority = 2;
const DWORD LowIoPriority = 1;
const DWORD VeryLowIoPriority = 0;
typedef NTSTATUS(NTAPI *FNNtSetInformationProcess)(
	HANDLE process,
	ULONG infoClass,
	void* data,
	ULONG dataSize);
struct NTFuncs
{
	FNNtSetInformationProcess fnNtSetInformationProcess = NULL;
	HMODULE hDLL_ = NULL;
	NTFuncs()
	{
		hDLL_ = LoadLibrary(L"ntdll.dll");
		if (hDLL_)
		{
			fnNtSetInformationProcess = (FNNtSetInformationProcess)GetProcAddress(
				hDLL_,
				"NtSetInformationProcess");
		}
	}
	~NTFuncs()
	{
		if (hDLL_)
			FreeLibrary(hDLL_);
	}
	BOOL SetPriority(HANDLE hProcess, QThread::Priority priority)
	{
		DWORD dwProcessPriority = -1;
		switch (priority)
		{
		case QThread::HighestPriority:
			dwProcessPriority = HIGH_PRIORITY_CLASS;
			break;
		case QThread::HighPriority:
			dwProcessPriority = ABOVE_NORMAL_PRIORITY_CLASS;
			break;
		case QThread::NormalPriority:
			dwProcessPriority = NORMAL_PRIORITY_CLASS;
			break;
		case QThread::LowPriority:
			dwProcessPriority = BELOW_NORMAL_PRIORITY_CLASS;
			break;
		case QThread::LowestPriority:
			dwProcessPriority = IDLE_PRIORITY_CLASS;
			break;
		case QThread::IdlePriority:
			dwProcessPriority = IDLE_PRIORITY_CLASS;
			break;
		default:
			Q_ASSERT(false);
			break;
		}
        Q_ASSERT(dwProcessPriority != (DWORD)-1);
		BOOL bFailed = FALSE;
		bFailed |= !SetPriorityClass(hProcess, dwProcessPriority);
		if (priority == QThread::IdlePriority && fnNtSetInformationProcess)
		{
			ULONG memory = LowMemoryPriority;
			bFailed |= !(0==fnNtSetInformationProcess(
				hProcess, 
				ProcessInformationMemoryPriority,
				&memory,
				sizeof(memory)));

			ULONG io = VeryLowIoPriority;
			bFailed = !(0==fnNtSetInformationProcess(
				hProcess,
				ProcessInformationIoPriority,
				&io,
				sizeof(io)));
		}
		return !bFailed;
	}
} gNTFuncs;
bool setProcessPriority(const qint64& pid, QThread::Priority priority)
{
	NativeHandle handle(OpenProcess(PROCESS_SET_INFORMATION, FALSE, (DWORD)pid));
	if (!handle)
		return false;

	//return SetPriorityClass(handle, IDLE_PRIORITY_CLASS);
	// return SetPriorityClass(handle, PROCESS_MODE_BACKGROUND_BEGIN);
	return gNTFuncs.SetPriority(handle, priority);
}
