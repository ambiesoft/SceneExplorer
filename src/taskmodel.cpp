#include "taskmodel.h"

void TaskModel::AddTasks(const QVector<TaskListData*>& tasks)
{
    // beginInsertRows(QModelIndex(),count,count+1);
    beginResetModel();
    for(int i=0 ; i < tasks.size(); ++i)
    {
        items_.append(tasks[i]);
        map_[tasks[i]->GetId()]=tasks[i];
    }
    endResetModel();
    //endInsertRows();


}
int TaskModel::findRow(TaskListData* p)
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
void TaskModel::RemoveTask(int id)
{
    TaskListData* p = map_[id];
    Q_ASSERT(p);

    int row = findRow(p);
    Q_ASSERT(row >= 0);

    
    // beginRemoveRows(QModelIndex(), row, row);
    // beginResetModel();

    Q_ASSERT(items_.contains(p));
    items_.removeOne(p);
    Q_ASSERT(!items_.contains(p));

    // endRemoveRows();
    // endResetModel();

    map_.remove(id);
    delete p;
}
void TaskModel::SetProgress(int id, TaskFFmpeg::Progress progress)
{
    // beginResetModel();
    Q_ASSERT(map_.contains(id));

    TaskListData* p = map_[id];
    Q_ASSERT(p);

    // int row = findRow(p);

    p->SetProgress(progress);
    // endResetModel();

    // emit dataChanged(createIndex(row,0), createIndex(row,0));
    // dataChanged(QModelIndex(),QModelIndex());// createIndex(row,0), createIndex(row,0));
}
int TaskModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return items_.count();
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

//    switch(index.column())
//    {
//    case 0:
//        return items_[index.row()]->GetProgressString();
//    case 1:
//        return items_[index.row()]->GetIdString();
//    case 2:
//        return items_[index.row()]->GetMovieFile();
//    default:
//        Q_ASSERT(false);
//    }

    Q_ASSERT(index.column()==0);
    QString ret;
    ret += items_[index.row()]->GetIdString();
    ret += items_[index.row()]->GetProgressString();
    ret += items_[index.row()]->GetMovieFile();
    return ret;
}
//bool TaskModel::canFetchMore(const QModelIndex & /* index */) const
//{
//    if (fileCount < fileList.size())
//        return true;
//    else
//        return false;
//}

//void TaskModel::fetchMore(const QModelIndex & /* index */)
//{
//    int remainder = fileList.size() - fileCount;
//    int itemsToFetch = qMin(100, remainder);

//    beginInsertRows(QModelIndex(), fileCount, fileCount+itemsToFetch-1);

//    fileCount += itemsToFetch;

//    endInsertRows();

//    emit numberPopulated(itemsToFetch);
//}
