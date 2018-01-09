#ifndef MYDIRMODEL_H
#define MYDIRMODEL_H

#include <QDirModel>
#include <QObject>

class MyDirModel : public QDirModel
{
public:
    MyDirModel(QObject* parent) : QDirModel(parent){}

//    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE
//    {
//        return QDirModel::index(row+1,column,parent);
//    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        if(index.row() == (QDirModel::rowCount(QModelIndex()) + 1))
            {
                if(role == Qt::DisplayRole)
                 return QString(tr("bla"));

                if(role == Qt::DecorationRole)
                 return iconProvider()->icon(QFileIconProvider::Desktop);

            return QVariant();
            }
        return QDirModel::data(index, role);
        }
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE
    {
        if(parent == QModelIndex())
         return QDirModel::rowCount(parent) + 1;

        return QDirModel::rowCount(parent);

    }
};

#endif // MYDIRMODEL_H
