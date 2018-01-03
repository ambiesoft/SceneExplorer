#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QFileSystemModel>
#include <QModelIndex>
#include <QSet>

class TreeModel : public QFileSystemModel
{
public:
    QSet<QPersistentModelIndex> checkedIndexes_;
    QSet<QPersistentModelIndex> partcheckedIndexes_;

    TreeModel();

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return 1;
    }
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        Q_UNUSED(section);
        Q_UNUSED(orientation);
        Q_UNUSED(role);
        return QVariant();
    }
    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        return QFileSystemModel::flags(index) | Qt::ItemIsUserCheckable;
    }

private:
    bool recursiveCheck(const QModelIndex &index, const QVariant &value);
};

#endif // TREEMODEL_H
