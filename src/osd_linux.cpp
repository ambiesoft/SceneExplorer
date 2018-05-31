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

#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <QDateTime>

#include "errorinfoexception.h"

#include "osd.h"

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

QString getInifile(bool& bExit)
{
    Q_UNUSED(bExit);
    return QString();
}
bool isLegalFilePath(QString filename, QString* pError)
{
    for( QChar& c : filename)
    {
        if(c=='/')
        {
            if(pError)
            {
                *pError = QObject::tr("Filename could not have '/'.");
                return false;
            }
        }
        if(c==0)
        {
            if(pError)
            {
                *pError = QObject::tr("Filename could not have Nul character.");
                return false;
            }
        }
    }
    return true;
}
QString GetIllegalFilenameCharacters()
{
    return "/";
}
