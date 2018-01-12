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
	static TaskListDataPointer Create(int id, const QString& movieFile)
	{
		return TaskListDataPointer(new TaskListData(id, movieFile));
	}
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
