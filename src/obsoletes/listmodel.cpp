#include <QDebug>
#include "mainwindow.h"
#include "listitemdata.h"
#include "listmodel.h"

ListModel::ListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void ListModel:: AppendData(ListItemData* pItemData)
{
    beginResetModel();
    listItems_.append(pItemData);
    endResetModel();
}
int ListModel::rowCount(const QModelIndex &) const
{
    qDebug() << listItems_.count();
    return listItems_.count()*2;
}
//int ListModel::columnCount(const QModelIndex &parent) const
//{
//    Q_UNUSED(parent);
//    // qDebug() << win_->getItemCount();
//    return 1;
//}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    int actualIndex = index.row()/2;
    if((index.row() % 2)==0)
    {
        // text
        switch(role)
        {
        case Qt::DisplayRole:
            return listItems_[actualIndex]->getMovieFile();
        }
    }
    else
    {
        // image
        switch(role)
        {
        case Qt::DisplayRole:
        {
            for(int i=0 ; i < listItems_[actualIndex]->getImageFiles().count(); ++i)
            {

            }
        }
        break;

        }
    }
    return QVariant();
}
//bool ListModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    Q_UNUSED(role);
//    Q_UNUSED(value);
//    // m_data[index] = value;
//    emit dataChanged(index, index);
//    return true;
//}
