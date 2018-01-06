#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractTableModel>
#include <QStyledItemDelegate>

class TableItemData;
class TableModel : public QAbstractTableModel
{
    Q_OBJECT
    QList<TableItemData*> items_;
    QString GetInfoText(const TableItemData& item, const bool isFilename) const;
public:
    enum TableRole {
        MovieFile = Qt::UserRole + 1,
    };
    static const int RowCountPerEntry = 3;

    TableModel(QObject *parent);
    void AppendData(TableItemData* pItemData);
    void AppendDatas(const QList<TableItemData*>&v);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override ;
};

class ImageDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ImageDelegate(QWidget *){}
//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &,
                   const QModelIndex &) const
    {
        return QSize(100,100);
    }
};



#endif // IMAGEMODEL_H
