#include "globals.h"

#include "taskmodel.h"

void TaskModel::AddTasks(const QVector<TaskListDataPointer>& tasks)
{
    if(tasks.isEmpty())
        return;

    int count = items_.count();
    beginInsertRows(QModelIndex(),
                    count,
                    count+tasks.count()-1);
    for(int i=0 ; i < tasks.size(); ++i)
    {
        items_.append(tasks[i]);
        map_[tasks[i]->GetId()]=tasks[i];
    }
    endInsertRows();
}
int TaskModel::findRow(TaskListDataPointer p)
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
    TaskListDataPointer p = map_[id];
    Q_ASSERT(p);

    int row = findRow(p);
    Q_ASSERT(row >= 0);
    
    // beginRemoveRows(QModelIndex(), row, row);
    Q_ASSERT(items_.contains(p));
    items_.removeOne(p);
    Q_ASSERT(!items_.contains(p));
    // endRemoveRows();

    map_.remove(id);

    // myupdate();
    if(rowCount()==0)
    {
        beginResetModel();
        endResetModel();
    }
}
void TaskModel::ClearAllTasks()
{
	beginResetModel();
	map_.clear();
	//for (int i = 0; i < items_.count(); ++i)
	//{
	//	delete items_[i];
	//}
	items_.clear();
	endResetModel();
}
void TaskModel::myupdate()
{
    if(gStop)
        return;

//    QRegion r(parent_->rect());
//    parent_->setDirtyRegion(r);
//    return;
    ((QWidget*)parent_)->update();
    parent_->repaint();
    parent_->updateGeometry();
    for(int i=0; i < qMin(rowCount(),10) ; ++i)
        parent_->update(createIndex(i,0));
    ((QWidget*)parent_)->update();
    parent_->repaint();

}
void TaskModel::SetProgress(int id, TaskFFmpeg::Progress progress)
{

    Q_ASSERT(map_.contains(id));

	TaskListDataPointer p = map_[id];
    Q_ASSERT(p);

    p->SetProgress(progress);

    //myupdate();
//    int row = findRow(p);
//    emit dataChanged(createIndex(row,0), createIndex(row,0));
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
//    return QVariant();

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
