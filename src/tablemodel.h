#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H



#include "tableitemdata.h"

class QTableView;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum SORTCOLUMN{
        SORT_FILENAME,
        SORT_SIZE,
        SORT_WTIME,
        SORT_RESOLUTION,
        SORT_DURATION,
        SORT_BITRATE,
    } ;
private:
    QList<TableItemDataPointer> itemDatas_;
    QMap<QString, TableItemDataPointer> mapsFullpathToItem_;

	bool bShowMissing_ = false;

    QTableView* parent_;
    QString GetInfoText(TableItemData& item) const;

    // static bool itemDataLessThan(const TableItemDataPointer v1, const TableItemDataPointer v2);

    void ClearData();

public:
    enum TableRole {
        MovieFile = Qt::UserRole + 1,
        // SelectedMovieFile = Qt::UserRole + 1,
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
    void Sort(SORTCOLUMN column);
    void Sort(SORTCOLUMN column, bool rev);
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

	void SetShowMissing(bool bToggle)
	{
		bShowMissing_ = bToggle;
	}
	bool IsShowMissing() const {
		return bShowMissing_;
	}

signals:
    void itemCountChanged();

};

class FileMissingFilterProxyModel : public QSortFilterProxyModel
{

public:
	FileMissingFilterProxyModel(QWidget*parent) : QSortFilterProxyModel(parent){}

	bool filterAcceptsRow(
		int sourceRow,
		const QModelIndex &sourceParent) const
	{
		if (((TableModel*) sourceModel())->IsShowMissing())
			return true;

		QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
		QVariant v = sourceModel()->data(index, TableModel::MovieFile);
		QString s = v.toString();
		return QFile(s).exists();
	}

};
//#include <QItemDelegate>
//class ImageSizeDelegate : public QItemDelegate 
//{
//    Q_OBJECT
//public:
//	ImageSizeDelegate(QWidget * parent) : QItemDelegate(parent){}
////    void paint(QPainter *painter, const QStyleOptionViewItem &option,
////               const QModelIndex &index) const;
//	QSize sizeHint(
//		const QStyleOptionViewItem &option,
//		const QModelIndex &index) const Q_DECL_OVERRIDE
//    {
//        return QSize(100,100);
//    }
//};



#endif // IMAGEMODEL_H
