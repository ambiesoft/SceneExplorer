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

#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractTableModel>
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
