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
}
void TaskGetDir::run()
{
    if(priority_)
        QThread::currentThread()->setPriority(*priority_);
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
        QStringList files;
        QList<qint64> sizes,wtimes;
		QStringList salients;
        QDirIterator itFile(dir, QDir::NoDotAndDotDot|QDir::Files); // ,QDirIterator::Subdirectories);
        while(itFile.hasNext())
        {
            if(gStop)
                return;
            itFile.next();
            Q_ASSERT(itFile.fileInfo().isFile());
            if (Extension::IsMovieExtension(itFile.fileName()))
			{
				files.append(itFile.fileName());
                sizes.append(itFile.fileInfo().size());
                wtimes.append(itFile.fileInfo().lastModified().toSecsSinceEpoch());
				salients.append(createSalient(itFile.filePath(), QFile(itFile.filePath()).size()));
			}
        }
        emit afterGetDir(loopId_, id_,
                         QDir(dir).absolutePath(),
                         files,
                         sizes,
                         wtimes,
                         salients);
    }

    {
        QDirIterator itDir(dir, QDir::NoDotAndDotDot|QDir::Dirs);
        while(itDir.hasNext())
        {
            runStuff(itDir.next());
        }
    }
}
