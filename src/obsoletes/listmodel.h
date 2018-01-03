#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>

class MainWindow;

class ListModel : public QAbstractListModel
{
    QList<ListItemData*> listItems_;

public:
    ListModel(QObject* parent);
    void AppendData(ListItemData* pItemData);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // LISTMODEL_H
