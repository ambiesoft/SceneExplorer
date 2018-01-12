#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>
#include <QListView>
#include "taskffmpeg.h"
#include "tasklistdata.h"


class TaskModel : public QAbstractTableModel
{
    Q_OBJECT
    //QList<TaskListData*> items_;
    QVector<TaskListDataPointer> items_;
    QMap<int, TaskListDataPointer> map_;

    QListView* parent_;
    void myupdate();
public:
    TaskModel(QListView* parent) : QAbstractTableModel(parent)
    {
        parent_=parent;
    }
    void AddTasks(const QVector<TaskListDataPointer>& tasks);
    int findRow(TaskListDataPointer p);
    void RemoveTask(int id);
	void ClearAllTasks();
    void SetProgress(int id, TaskFFmpeg::Progress progress);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return 1;
    }
//	void UpdateList()
//	{
//		emit dataChanged(QModelIndex(), QModelIndex());
//	}
//signals:
//    void numberPopulated(int number);

//protected:
//    bool canFetchMore(const QModelIndex &parent) const override;
//    void fetchMore(const QModelIndex &parent) override;
};

#endif // TASKMODEL_H
