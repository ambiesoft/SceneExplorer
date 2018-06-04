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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif

#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

#include <sys/time.h>
#include <sys/resource.h>

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

static inline int ioprio_set(int which, int who, int ioprio)
{
    return syscall(SYS_ioprio_set, which, who, ioprio);
}



enum {
    IOPRIO_CLASS_NONE,
    IOPRIO_CLASS_RT,
    IOPRIO_CLASS_BE,
    IOPRIO_CLASS_IDLE,
};

enum {
    IOPRIO_WHO_PROCESS = 1,
    IOPRIO_WHO_PGRP,
    IOPRIO_WHO_USER,
};

#define IOPRIO_CLASS_SHIFT	(13)
#define IOPRIO_PRIO_MASK	((1UL << IOPRIO_CLASS_SHIFT) - 1)

#define IOPRIO_PRIO_CLASS(mask)	((mask) >> IOPRIO_CLASS_SHIFT)
#define IOPRIO_PRIO_DATA(mask)	((mask) & IOPRIO_PRIO_MASK)
#define IOPRIO_PRIO_VALUE(class, data)	(((class) << IOPRIO_CLASS_SHIFT) | data)


bool setProcessPriority(const qint64& pid, QThread::Priority priority, QStringList& errors)
{
   int prio = -1;
   switch(priority)
   {
   case QThread::HighestPriority:
       prio = -19;
       break;
   case QThread::HighPriority:
       prio = -5;
       break;
   case QThread::NormalPriority:
       prio=0;
       break;
   case QThread::LowPriority:
       prio=5;
       break;
   case QThread::LowestPriority:
       prio=19;
       break;
   case QThread::IdlePriority:
       prio=19;
       break;
   default:
       Q_ASSERT(false);
       return false;
   }

   bool failed = false;
   int err = setpriority(PRIO_PROCESS, pid, prio);
   if(err != 0)
   {
       errors << QObject::tr("setpriority(%1) failed with %2.").arg(prio).arg(err);
       failed = true;
   }

   if(priority==QThread::IdlePriority)
   {
       // 7 is from https://github.com/karelzak/util-linux/blob/master/schedutils/ionice.c
       int ioprio = IOPRIO_PRIO_VALUE(IOPRIO_CLASS_IDLE,7);
       err = ioprio_set(IOPRIO_WHO_PROCESS, pid, ioprio);
       if(err != 0)
       {
           failed = true;
           errors << QObject::tr("ioprio_set(%1) failed with %2.").arg(ioprio).arg(err);
       }
   }
   return !failed;
}
