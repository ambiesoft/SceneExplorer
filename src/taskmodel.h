#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>

#include "taskffmpeg.h"
#include "tasklistdata.h"

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    //QList<TaskListData*> items_;
    QVector<TaskListData*> items_;
    QMap<int, TaskListData*> map_;

public:
    TaskModel(){}
    void AddTasks(const QVector<TaskListData*>& tasks);
    int findRow(TaskListData* p);
    void RemoveTask(int id);
    void SetProgress(int id, TaskFFMpeg::Progress progress);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//signals:
//    void numberPopulated(int number);

//protected:
//    bool canFetchMore(const QModelIndex &parent) const override;
//    void fetchMore(const QModelIndex &parent) override;
};

#endif // TASKMODEL_H
