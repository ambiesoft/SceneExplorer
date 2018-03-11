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

#ifndef TASKLISTDATA_H
#define TASKLISTDATA_H

#include <QSharedPointer>

#include "taskffmpeg.h"

class TaskListData;
typedef QSharedPointer<TaskListData> TaskListDataPointer;



class TaskListData
{

private:
#ifdef QT_DEBUG
	static int dinstcount_;
#endif
    int id_;
    QString movieFile_;
    TaskFFmpeg::Progress progress_ = TaskFFmpeg::Progress::Uninitialized;


    TaskListData(int id, const QString& movieFile):id_(id), movieFile_(movieFile)
    {
#ifdef QT_DEBUG
		++dinstcount_;
#endif
	}
public:
    static TaskListDataPointer Create(int id, const QString& movieFile);
	~TaskListData()
	{
#ifdef QT_DEBUG
		--dinstcount_;
#endif
	}
#ifdef QT_DEBUG
	static bool isAllClear()
	{
		return dinstcount_ == 0;
	}
#endif

    int GetId() const
    {
        return id_;
    }
    static int GetColumnCount()
    {
        // id and moveFile
        return 2;
    }

    QString GetIdString() const;
    QString GetProgressString() const;
    QString GetMovieFile() const
    {
        return movieFile_;
    }

    void SetProgress(TaskFFmpeg::Progress progress)
    {
        progress_=progress;
    }
};


#endif // TASKLISTDATA_H
