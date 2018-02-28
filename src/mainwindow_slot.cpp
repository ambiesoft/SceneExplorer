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
#include <QDateTime>

#include "taskmodel.h"
#include "tableitemdata.h"

#include "consts.h"
#include "globals.h"
#include "sql.h"
#include "helper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::sayHello(int loopId, int id,
               const QString& movieFile)
{
    if(loopId != gLoopId)
        return;

    taskModel_->SetProgress(id, TaskFFmpeg::Progressing);
    insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Started"), movieFile));
}
void MainWindow::sayNo(int loopId,
                       int id,
                       const QString& movieFile,
                       const QString& errorReason)
{
    if(loopId != gLoopId)
        return;

    insertLog(TaskKind::FFMpeg, id, QString("%1 (%2) \"%3\"").arg(tr("Not a video"), errorReason, movieFile));
}
void MainWindow::sayGoodby(int loopId,  int id,
                           const QStringList& files,
                           const QString& movieFile,
                           int thumbwidth,
                           int thumbheight,
                           const double& duration,
                           const QString& format,
                           int bitrate,
                           const QString& vcodec,
                           const QString& acodec,
                           int vWidth,int vHeight)
{
	if (loopId != gLoopId)
	{
        for(const QString& thumbfile:files)
        {
            if(thumbfile.count() == Consts::THUMB_FILENAME_LENGTH)
            {
                QFile fi(pathCombine(Consts::FILEPART_THUMBS, thumbfile));
                if(fi.exists())
                {
                    VERIFY(fi.remove());
                }
            }
        }
		return;
	}

    QFileInfo fi(movieFile);

    QString dir,name;
    canonicalDirAndName(fi.canonicalFilePath(), dir, name);
    TableItemDataPointer pTID = TableItemData::Create(
                            files,
                            dir,
                            name,
                            fi.size(),
                            fi.birthTime().toSecsSinceEpoch(),
                            fi.lastModified().toSecsSinceEpoch(),
                            thumbwidth,thumbheight,
                            duration,
                            format,
                            bitrate,
                            vcodec,acodec,
                            vWidth,vHeight,

                            0
                );
    if(IsDirSelected(canonicalDir(fi.canonicalPath())))
    {
        tableModel_->RemoveItem(fi.canonicalFilePath());
        tableModel_->AppendData(pTID);
    }

    QString removedThumbID;
    if(gpSQL->RemoveEntryThumb(dir,name, removedThumbID))
    {
        insertLog(TaskKind::SQL, id, QString(tr("Thumbnail %1 has been removed from database").
                                             arg(removedThumbID)));
    }

    int sqlError = gpSQL->AppendData(pTID);
    if(sqlError==0)
    {
        insertLog(TaskKind::SQL, id, QString("%1 \"%2\"").arg(tr("Written in Database"), movieFile));
    }
    else
    {
        insertLog(TaskKind::SQL, id, QString("%1 \"%2\"").arg(tr("Failed to write on Database"), movieFile));
    }

    insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Done"), movieFile));

}
void MainWindow::sayDead(int loopId, int id)
{
    if(loopId != gLoopId)
        return;

    taskModel_->RemoveTask(id);
}
void MainWindow::finished_FFMpeg(int loopId, int id)
{
    if(loopId != gLoopId)
        return;

    Q_UNUSED(id);

    idManager_->IncrementDone(IDKIND_FFmpeg);
    Q_ASSERT(idManager_->Get(IDKIND_FFmpeg) >= idManager_->GetDone(IDKIND_FFmpeg));


    if(idManager_->isAllTaskFinished())
    {
        onTaskEnded();
        insertLog(TaskKind::App, 0, tr(Consts::ALL_TASK_FINISHED));
    }
}
