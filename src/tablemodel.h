#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractTableModel>
#include <QStyledItemDelegate>

class TableItemData;
class QTableView;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum SORTCOLUMN{
        FILENAME,
        SIZE,
        WTIME,
    } ;
private:
    QList<TableItemData*> items_;
    QMap<QString, TableItemData*> mapsFullpathToItem_;

    QTableView* parent_;
    QString GetInfoText(TableItemData& item) const;

    static bool itemDataLessThan(const TableItemData* v1, const TableItemData* v2);
    void SortCommon(SORTCOLUMN column);
public:
    enum TableRole {
        MovieFile = Qt::UserRole + 1,
        SelectedMovieFile = Qt::UserRole + 1,
    };
    static const int RowCountPerEntry = 3;

    TableModel(QTableView *parent);
    void AppendData(TableItemData* pItemData);
    // void AppendDatas(const QList<TableItemData*>&v);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override ;
    static SORTCOLUMN sSortColumn_;
    static bool sSortReverse_;

    void SortByFileName();
    void SortBySize();
    void SortByWtime();

    bool RenameEntries(const QString& dir,
                       const QStringList& renameOlds,
                       const QStringList& renameNews);
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
