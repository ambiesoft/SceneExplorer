#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractTableModel>
#include <QStyledItemDelegate>

class ImageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ImageModel(QObject *parent);
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
