#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <QDateTime>

#include "errorinfoexception.h"

// https://stackoverflow.com/a/3546503
bool showInGraphicalShell(QWidget *parent, const QString &pathIn)
{
    Q_UNUSED(parent);
    return QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(pathIn).absolutePath()));
}

// https://stackoverflow.com/a/17974223
bool TrashInitialized = false;
QString TrashPath;
QString TrashPathInfo;
QString TrashPathFiles;

void MoveToTrashImpl( QString file ){
    #ifdef QT_GUI_LIB
        if( !TrashInitialized ){
            QStringList paths;
            const char* xdg_data_home = getenv( "XDG_DATA_HOME" );
            if( xdg_data_home ){
                qDebug() << "XDG_DATA_HOME not yet tested";
                QString xdgTrash( xdg_data_home );
                paths.append( xdgTrash + "/Trash" );
            }
            QString home = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
            paths.append( home + "/.local/share/Trash" );
            paths.append( home + "/.trash" );
            foreach( QString path, paths ){
                if( TrashPath.isEmpty() ){
                    QDir dir( path );
                    if( dir.exists() ){
                        TrashPath = path;
                    }
                }
            }
            if( TrashPath.isEmpty() )
                throw ErrorInfoException( "Cant detect trash folder" );
            TrashPathInfo = TrashPath + "/info";
            TrashPathFiles = TrashPath + "/files";
            if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() )
                throw ErrorInfoException( "Trash doesnt looks like FreeDesktop.org Trash specification" );
            TrashInitialized = true;
        }
        QFileInfo original( file );
        if( !original.exists() )
            throw ErrorInfoException( "File doesnt exists, cant move to trash" );
        QString info;
        info += "[Trash Info]\nPath=";
        info += original.absoluteFilePath();
        info += "\nDeletionDate=";
        info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
        info += "\n";
        QString trashname = original.fileName();
        QString infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
        QString filepath = TrashPathFiles + "/" + trashname;
        int nr = 1;
        while( QFileInfo( infopath ).exists() || QFileInfo( filepath ).exists() ){
            nr++;
            trashname = original.baseName() + "." + QString::number( nr );
            if( !original.completeSuffix().isEmpty() ){
                trashname += QString( "." ) + original.completeSuffix();
            }
            infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
            filepath = TrashPathFiles + "/" + trashname;
        }
        QDir dir;
        if( !dir.rename( original.absoluteFilePath(), filepath ) ){
            throw ErrorInfoException( "move to trash failed" );
        }
        // TODO nazo
//        QFile infofile;
//        infofile.createUtf8( infopath, info );
    #else
        Q_UNUSED( file );
        throw ErrorInfoException( "Trash in server-mode not supported" );
    #endif
}

QString GetDefaultFFprobe()
{
    return "ffprobe";
}
QString GetDefaultFFmpeg()
{
    return "ffmpeg";
}
