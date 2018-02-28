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

#include <QString>
#include "tasklistdata.h"

#ifdef QT_DEBUG
int TaskListData::dinstcount_ = 0;
#endif

TaskListDataPointer TaskListData::Create(int id, const QString& movieFile)
{
    return TaskListDataPointer(new TaskListData(id, movieFile));
}

QString TaskListData::GetIdString() const
{
    return QString::number(id_);
}
QString TaskListData::GetProgressString() const
{
    switch(progress_)
    {
    case TaskFFmpeg::Progress::Uninitialized:
        return " ";
    case TaskFFmpeg::Progress::Progressing:
        return ">";
    case TaskFFmpeg::Progress::Finished:
        return "O";
    default:
        Q_ASSERT(false);
    }
    return "";
}
