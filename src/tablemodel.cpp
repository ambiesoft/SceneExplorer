// ImageModel.cpp
#include <QImage>
#include <QPixmap>
#include <QFileInfo>
#include <QTableView>
#include <QColor>

#include "consts.h"
#include "helper.h"

#include "tableitemdata.h"
#include "tablemodel.h"

TableModel::TableModel(QTableView *parent)
    :QAbstractTableModel(parent)
{
    parent_=parent;
}
void TableModel:: AppendData(TableItemDataPointer pItemData, const bool enableUpdate)
{
    if(enableUpdate)
    {
        beginInsertRows(QModelIndex(),
                        itemDatas_.count()*RowCountPerEntry,
                        (itemDatas_.count()*RowCountPerEntry)+RowCountPerEntry-1);
        itemDatas_.append(pItemData);
        endInsertRows();
    }
    else
    {
        itemDatas_.append(pItemData);
    }
    mapsFullpathToItem_[pItemData->getMovieFileFull()] = pItemData;

    int newRowInfo = rowCount()-TableModel::RowCountPerEntry;
    int newRowImage = newRowInfo+1;
    int newRowSeparator = newRowImage+1;

    parent_->setSpan(newRowInfo,0,1,5);
    parent_->setSpan(newRowSeparator,0,1,5);
    // ui->tableView->resizeRowToContents(newRowFilename);
    // ui->tableView->resizeRowToContents(newRowInfo);

    static bool initColumnWidth=false;
    if(!initColumnWidth)
    {
        initColumnWidth=true;
        for(int i=0 ; i < 5 ; ++i)
        {
            parent_->setColumnWidth(i, Consts::THUMB_WIDTH);
        }
    }

    parent_->setRowHeight(newRowImage, Consts::THUMB_HEIGHT);
}
void TableModel::ResetData(const QList<TableItemDataPointer>& all)
{
    beginResetModel();
    ClearData();
    for(int i=0;i < all.count(); ++i)
    {
        AppendData(all[i],!false);
    }
    endResetModel();
}
//void TableModel::AppendDatas(const QList<TableItemData*>&v)
//{
//    beginResetModel();
//    items_.append(v);
//    endResetModel();
//}
int TableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return itemDatas_.count()*RowCountPerEntry;
}
void TableModel::ClearData()
{
    itemDatas_.clear();
    mapsFullpathToItem_.clear();
}

int TableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

// http://comments.gmane.org/gmane.comp.lib.qt.general/34914
QString size_human(const qint64& size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+unit;
}
QString duration_human(double duration)
{
    int iduration = (int)(duration);

    int hour = iduration/3600;
    int minutes = (iduration%3600)/60;
    int seconds = iduration%60;

    return
                QString("%1").arg((int)hour, 2, 10, QChar('0')) + ":"+
                QString("%1").arg((int)minutes, 2, 10, QChar('0'))+":"+
                QString("%1").arg((int)seconds, 2, 10, QChar('0'));

}
static QString dq(const QString& s)
{
	if (s.isEmpty())
		return "\"\"";

	if (s[0] == '\\')
		return s;

    if (!s.contains(" ") && !s.contains(","))
		return s;

	return "\"" + s + "\"";
}
QString TableModel::GetInfoText(TableItemData& item) const
{
    QString ret;
    static const char* sep = ", ";
    ret.append(dq(item.getMovieFileFull()));
    ret.append(sep);

    ret.append(size_human(item.getSize()));
    ret.append(sep);

    ret.append(duration_human(item.getDuration()));
    ret.append(sep);

    ret.append(dq(item.getFormat()));
    ret.append(sep);

    ret.append(item.getVcodec());
    ret.append(sep);
    ret.append(QString::number(item.getVWidth()) + "x" + QString::number(item.getVHeight()));
    ret.append(sep);

    ret.append(item.getAcodec());

    return ret;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    int actualIndex = index.row()/RowCountPerEntry;
    int mod = index.row() % RowCountPerEntry;
    bool isInfo = mod==0;
    bool isImage = mod==1;

    if(role==TableRole::MovieFile)
    {
        return itemDatas_[actualIndex]->getMovieFileFull();
    }

    if(isInfo)
    {
        if(index.column() != 0)
            return QVariant();

        switch(role)
        {
            case Qt::DisplayRole:
            {
                return GetInfoText(*(itemDatas_[actualIndex]));
            }
            break;

            case Qt::TextColorRole:
            {
                if(!QFile(itemDatas_[actualIndex]->getMovieFileFull()).exists())
                {
                    return QColor(Qt::red);
                }
            }
            break;

        }
    }
    else if(isImage)
    {
        switch(role)
        {
            case Qt::DecorationRole:
            {
                QString imageFile = itemDatas_[actualIndex]->getImageFiles()[index.column()];
                imageFile = pathCombine(Consts::FILEPART_THUMBS, imageFile);

                // QString imageFile("C:\\Cygwin\\home\\fjUnc\\gitdev\\SceneExplorer\\build-SceneExplorer-Desktop_Qt_5_10_0_MSVC2013_64bit-Debug\\0b76916f-3fb8-49be-a7da-d110ed476952-2.png");
                QImage image(imageFile);
                QPixmap pix = QPixmap::fromImage(image);
                return pix;
            }
            break;
        }
    }
    return QVariant();
}
Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}



bool TableModel::itemDataLessThan(const TableItemDataPointer v1, const TableItemDataPointer v2)
{
    bool ret;
    switch(sSortColumn_)
    {
    case SORTCOLUMN::FILENAME:
        ret = (v1->getMovieFileFull() < v2->getMovieFileFull());
        break;
    case SORTCOLUMN::SIZE:
        ret = (v1->getSize() < v2->getSize());
        break;
    case SORTCOLUMN::WTIME:
        ret = (v1->getWtime() < v2->getWtime());
        break;
    default:
        Q_ASSERT(false);
    }
    return sSortReverse_ ? !ret : ret;
}
TableModel::SORTCOLUMN TableModel::sSortColumn_;
bool TableModel::sSortReverse_;

void TableModel::SortCommon(SORTCOLUMN column)
{
    sSortReverse_ = !sSortReverse_;
    beginResetModel();
    sSortColumn_ = column;
    qSort(itemDatas_.begin(), itemDatas_.end(), &TableModel::itemDataLessThan);
    // std::sort(itemDatas_.begin(), itemDatas_.end(), &TableModel::itemDataLessThan);
    endResetModel();
}
void TableModel::SortByFileName()
{
    SortCommon(SORTCOLUMN::FILENAME);
}

void TableModel::SortBySize()
{
    SortCommon(SORTCOLUMN::SIZE);
}
void TableModel::SortByWtime()
{
    SortCommon(SORTCOLUMN::WTIME);
}

bool TableModel::RenameEntries(const QString& dir,
                   const QStringList& renameOlds,
                   const QStringList& renameNews)
{
    bool ret = true;
    for(int i=0 ; i < renameOlds.count(); ++i)
    {
		TableItemDataPointer pID = mapsFullpathToItem_[pathCombine(dir,renameOlds[i])];
        Q_ASSERT(pID);
        if(pID)
        {
            ret &= pID->Rename(renameOlds[i], renameNews[i]);
        }
    }
    return ret;
}
