#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>

#include "taskffmpeg.h"
#include "tasklistdata.h"

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    QList<TaskListData*> items_;
    QMap<int, TaskListData*> map_;

public:
    TaskModel(){}
    void AddTask(TaskListData* p)
    {
        // beginInsertRows(QModelIndex(),count,count+1);
        beginResetModel();
        items_.append(p);
        endResetModel();
        //endInsertRows();

        map_[p->GetId()]=p;
    }
    int findRow(TaskListData* p)
    {
        for(int i=0 ; i < items_.count();++i)
        {
            if(p->GetId()==items_[i]->GetId())
            {
                return i;
            }
        }
        return -1;
    }
    void RemoveTask(int id)
    {
        TaskListData* p = map_[id];
        Q_ASSERT(p);

        int row = findRow(p);
        Q_ASSERT(row >= 0);

        beginRemoveRows(QModelIndex(), row, row);

        Q_ASSERT(items_.contains(p));
        items_.removeOne(p);
        Q_ASSERT(!items_.contains(p));

        endRemoveRows();

        map_.remove(id);
        delete p;
    }
    void SetProgress(int id, TaskFFMpeg::Progress progress)
    {
        beginResetModel();
        Q_ASSERT(map_.contains(id));
        map_[id]->SetProgress(progress);
        endResetModel();
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return items_.count();
    }
//    int columnCount(const QModelIndex &parent = QModelIndex()) const override
//    {
//        Q_UNUSED(parent);
//        // +1 for progress indicator
//        return 1+2;
//    }
//    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
//    {
//        return createIndex(row,column);
//    }
//    QModelIndex parent(const QModelIndex &child) const override
//    {
//        return QModelIndex();
//    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // TASKMODEL_H
