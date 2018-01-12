#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H



#include "tableitemdata.h"

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
    QList<TableItemDataPointer> itemDatas_;
    QMap<QString, TableItemDataPointer> mapsFullpathToItem_;

    QTableView* parent_;
    QString GetInfoText(TableItemData& item) const;

    static bool itemDataLessThan(const TableItemDataPointer v1, const TableItemDataPointer v2);
    void SortCommon(SORTCOLUMN column);
    void ClearData();
public:
    enum TableRole {
        MovieFile = Qt::UserRole + 1,
        SelectedMovieFile = Qt::UserRole + 1,
    };
    static const int RowCountPerEntry = 3;

    TableModel(QTableView *parent);
    void AppendData(TableItemDataPointer pItemData, const bool enableUpdate = true);
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

//    bool RenameEntries(const QString& dir,
//                       const QStringList& renameOlds,
//                       const QStringList& renameNews);
    bool RenameEntry(const QString& dbDir,
                     const QString& dbFile,
                     const QString& newdir,
                     const QString& newfile);
    void ResetData(const QList<TableItemDataPointer>& all);
    int GetItemCount() const {
        return itemDatas_.count();
    }

signals:
    void itemCountChanged();

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
