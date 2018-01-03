#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QFileSystemModel>
#include <QModelIndex>

class TreeModel : public QFileSystemModel
{
public:
    TreeModel();

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return 1;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        Q_UNUSED(section);
        Q_UNUSED(orientation);
        Q_UNUSED(role);
        return QVariant();
    }
};

#endif // TREEMODEL_H
