#include "taskmodel.h"

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
