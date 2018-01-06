#ifndef TASKLISTDATA_H
#define TASKLISTDATA_H

#include "taskffmpeg.h"

class TaskListData
{
private:
    int id_;
    QString movieFile_;
    TaskFFmpeg::Progress progress_ = TaskFFmpeg::Progress::Uninitialized;

public:
    TaskListData(int id, const QString& movieFile):id_(id), movieFile_(movieFile)
    {}
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
