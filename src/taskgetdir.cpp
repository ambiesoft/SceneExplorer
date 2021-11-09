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

#include <QObject>
#include <QDirIterator>
#include <QThread>
#include <QDateTime>

#include "helper.h"
#include "globals.h"
#include "extension.h"
#include "taskgetdir.h"

void TaskGetDir::RegisterMetaType()
{
    qRegisterMetaType< QList<qint64> >( "QList<qint64>" );
    qRegisterMetaType< QList<MovieFileInfo> >( "QList<MovieFileInfo>" );
}

TaskGetDir::TaskGetDir(int loopId,
                       int id,
                       const QString& dir,
                       SORTCOLUMNMY sortby, bool ascending,
                       const IFFTask2Main* pFF2M):
    loopId_(loopId),
    id_(id),
    dir_(dir),
    sortby_(sortby), ascending_(ascending)
{
    Q_ASSERT(!dir.isEmpty());
    pFF2M_ = pFF2M;
}

TaskGetDir::~TaskGetDir()
{
}

void TaskGetDir::run()
{
    QThread::Priority* priority = pFF2M_->GetTaskPriority();
    if(priority)
        QThread::currentThread()->setPriority(*priority);
    runStuff(dir_);
    emit finished_GetDir(loopId_, id_, dir_);
}

void TaskGetDir::runStuff(const QString& dir)
{
    if(gStop)
        return;
    while(gPaused)
        QThread::sleep(5);
    if(gStop)
        return;



    {
        QList<MovieFileInfo> mfis;
        QDirIterator itFile(dir, QDir::NoDotAndDotDot|QDir::Files); // ,QDirIterator::Subdirectories);
        while(itFile.hasNext())
        {
            if(gStop)
                return;
            itFile.next();
            Q_ASSERT(itFile.fileInfo().isFile());
            if (Extension::IsMovieExtension(itFile.fileName()))
            {
                MovieFileInfo mfi(itFile.fileName(),
                                  itFile.fileInfo().size(),
                                  itFile.fileInfo().birthTime().toSecsSinceEpoch(),
                                  itFile.fileInfo().lastModified().toSecsSinceEpoch(),
                                  createSalient(itFile.filePath(), QFile(itFile.filePath()).size()));
                mfis.push_back(mfi);
            }
        }

        // ascending           descending
        // 3                   55
        // 9                   12
        // 12                   9
        // 55                   3

#define CASESORT(sorttype, func)                                    \
    case sorttype:                                                  \
        if(ascending_)                                              \
        {                                                           \
            std::sort(mfis.begin(), mfis.end(),                     \
                 [](MovieFileInfo& a, MovieFileInfo& b) -> bool     \
            {                                                       \
                return a.func() < b.func();                         \
            });                                                     \
        }                                                           \
        else                                                        \
        {                                                           \
            std::sort(mfis.begin(), mfis.end(),                     \
                 [](MovieFileInfo& a, MovieFileInfo& b) -> bool     \
            {                                                       \
                return b.func() < a.func();                         \
            });                                                     \
        }                                                           \
        break;


        switch(sortby_)
        {
            CASESORT(SORT_FILENAME,name)
            CASESORT(SORT_SIZE,size)
            CASESORT(SORT_CTIME,ctime)
            CASESORT(SORT_WTIME,wtime)
            default:
                break;
        }
#undef CASESORT
        emit afterGetDir(loopId_, id_,
                         QDir(dir).absolutePath(),
                         mfis);
    }

    {
        QDirIterator itDir(dir, QDir::NoDotAndDotDot|QDir::Dirs);
        while(itDir.hasNext())
        {
            runStuff(itDir.next());
        }
    }
}
