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



    int newRowFilename = rowCount()-TableModel::RowCountPerEntry;
    int newRowImage = newRowFilename+1;
    int newRowInfo = newRowImage+1;

    parent_->setSpan(newRowFilename,0,1,5);
    parent_->setSpan(newRowInfo,0,1,5);

    static bool initColumnWidth=false;
    if(!initColumnWidth)
    {
        initColumnWidth=true;
        for(int i=0 ; i < 5 ; ++i)
        {
            parent_->setColumnWidth(i, Consts::THUMB_WIDTH);
        }
    }

    // parent_->setRowHeight(newRowImage, Consts::THUMB_HEIGHT);

    emit itemCountChanged();
}
void TableModel::ResetData(const QList<TableItemDataPointer>& all)
{
    beginResetModel();
    ClearData();


    parent_->scrollToTop();


    for(int i=0;i < all.count(); ++i)
    {
        AppendData(all[i], false);
    }

//    beginInsertRows(QModelIndex(),
//                    0,
//                    (itemDatas_.count()*RowCountPerEntry)+RowCountPerEntry-1);

//    endInsertRows();
    Sort(GetSortColumn(), GetSortReverse());
    endResetModel();

    emit itemCountChanged();
}

int TableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return itemDatas_.count()*RowCountPerEntry;
}

void TableModel::ClearData()
{
    itemDatas_.clear();
    mapsFullpathToItem_.clear();

    emit itemCountChanged();
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
    return QString().setNum(num,'f',2)+ " " +unit;
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
static QString bitrate_human(int bitrate)
{
    float num = bitrate;
    QStringList list;
    list << "kb/s" << "mb/s" << "gb/s" << "tb/s";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+ " " +unit;
}
QString TableModel::GetInfoText(TableItemData& item) const
{
    QString ret;
    static const char* sep = "   ";

    ret.append(size_human(item.getSize()));
    ret.append(sep);

    ret.append(duration_human(item.getDuration()));
    ret.append(sep);

    ret.append(dq(item.getFormat()));
    ret.append(sep);

    ret.append(bitrate_human(item.getBitrate()));
    ret.append(sep);

    ret.append(item.getVcodec());
    ret.append(sep);
    ret.append(QString::number(item.getVWidth()) + "x" + QString::number(item.getVHeight()));
    ret.append(sep);

    ret.append(item.getAcodec());
	ret.append(sep);

    ret.append(dq(item.getMovieDirectory()));
	// ret.append(sep);

    return ret;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    int actualIndex = index.row()/RowCountPerEntry;
    int mod = index.row() % RowCountPerEntry;
    bool isFilename = mod==0;
    bool isImage = mod==1;
	bool isInfo = mod == 2;

    if(role==TableRole::MovieFile)
    {
        return itemDatas_[actualIndex]->getMovieFileFull();
    }

// never comes
//    if(role==Qt::SizeHintRole)
//    {
//        if(isFilename)
//        {
//            parent_->setSpan(index.row(),0,1,5);
//        }
//    }

    if(isFilename)
    {
        if(index.column() != 0)
            return QVariant();

        switch(role)
        {
            case Qt::DisplayRole:
            {
            // infinite loop
//                parent_->setSpan(index.row(),0,1,5);
//                parent_->setSpan(index.row()+3,0,1,5);
//                parent_->setSpan(index.row()+6,0,1,5);
                return itemDatas_[actualIndex]->getMovieFileName();
            }
            break;

            case Qt::TextColorRole:
            {
                if(bShowMissing_)
                {
                    if(!QFile(itemDatas_[actualIndex]->getMovieFileFull()).exists())
                    {
                        return QColor(Qt::red);
                    }
                }
            }
            break;

            case Qt::TextAlignmentRole:
            {
                return Qt::AlignBottom;
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
                parent_->setColumnWidth(index.row(), Consts::THUMB_WIDTH);
                parent_->setRowHeight(index.row(), Consts::THUMB_HEIGHT);

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
	else if (isInfo)
	{
		switch (role)
		{
		case Qt::DisplayRole:
		{
//            parent_->setSpan(index.row(),0,1,5);
//            parent_->setSpan(index.row()+3,0,1,5);
//            parent_->setSpan(index.row()+6,0,1,5);
            return GetInfoText(*(itemDatas_[actualIndex]));
		}
		break;

        case Qt::TextColorRole:
        {
			if (bShowMissing_)
			{
				if (!QFile(itemDatas_[actualIndex]->getMovieFileFull()).exists())
				{
					return QColor(Qt::red);
				}
			}
        }
        break;

        case Qt::TextAlignmentRole:
        {
            return Qt::AlignTop;
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


class SortFunctor
{
    TableModel::SORTCOLUMN sc_;
    bool rev_;

	bool compare(const TableItemDataPointer& v1, const TableItemDataPointer& v2) const
	{
		switch (sc_)
		{
        case TableModel::SORTCOLUMN::SORT_FILENAME:
			return (v1->getMovieFileFull() < v2->getMovieFileFull());
			break;
        case TableModel::SORTCOLUMN::SORT_SIZE:
			return (v1->getSize() < v2->getSize());
			break;
        case TableModel::SORTCOLUMN::SORT_WTIME:
            return (v1->getWtime() < v2->getWtime());
            break;
        case TableModel::SORTCOLUMN::SORT_RESOLUTION:
            return (v1->getResolutionMultiplied() < v2->getResolutionMultiplied());
            break;
        case TableModel::SORTCOLUMN::SORT_DURATION:
            return (v1->getDuration() < v2->getDuration());
            break;
        case TableModel::SORTCOLUMN::SORT_BITRATE:
            return (v1->getBitrate() < v2->getBitrate());
            break;
        default:
			Q_ASSERT(false);
		}
		return false;
	}
public:
    SortFunctor(TableModel::SORTCOLUMN sc,bool rev):
        sc_(sc), rev_(rev)
    {}

	bool operator()(const TableItemDataPointer& v1, const TableItemDataPointer& v2) const
	{
		if (rev_)
			return compare(v2, v1);
		else
			return compare(v1, v2);
	}

};


TableModel::SORTCOLUMN TableModel::GetSortColumn() const
{
    return sortColumn_;
}
void TableModel::SetSortColumn(SORTCOLUMN sc)
{
    if(sortColumn_ != sc)
    {
        sortColumn_ = sc;
        emit sortParameterChanged(sc,GetSortReverse());
    }
}


QString TableModel::GetSortColumnName(SORTCOLUMN sc)
{
    switch(sc)
    {

    case SORT_FILENAME:return tr("Filename");
    case SORT_SIZE:return tr("Size");
    case SORT_WTIME:return tr("Wtime");
    case SORT_RESOLUTION: return tr("Resolution");
    case SORT_DURATION:return tr("Duration");
    case SORT_BITRATE:return tr("Bitrate");
    default :
        Q_ASSERT(false);

    }
    return QString();
}



bool TableModel::GetSortReverse() const
{
    return sortReverse_;
}
void TableModel::SetSortReverse(bool rev)
{
    if(sortReverse_ != rev)
    {
        sortReverse_ = rev;
        emit sortParameterChanged(GetSortColumn(), rev);
    }
}

void TableModel::Sort(SORTCOLUMN column, bool rev)
{
    SetSortReverse(rev);
    SetSortColumn(column);

    SortFunctor func(column,rev);

    beginResetModel();
    std::sort(itemDatas_.begin(), itemDatas_.end(), func);
    endResetModel();
}
void TableModel::Sort(SORTCOLUMN column)
{
    if(column==GetSortColumn())
        SetSortReverse(!GetSortReverse());
    Sort(column,GetSortReverse());
}

//bool TableModel::RenameEntries(const QString& dir,
//                   const QStringList& renameOlds,
//                   const QStringList& renameNews)
//{
//    bool ret = true;
//    for(int i=0 ; i < renameOlds.count(); ++i)
//    {
//		TableItemDataPointer pID = mapsFullpathToItem_[pathCombine(dir,renameOlds[i])];
//        Q_ASSERT(pID);
//        if(pID)
//        {
//            ret &= pID->Rename(renameOlds[i], renameNews[i]);
//        }
//    }
//    return ret;
//}
bool TableModel::RenameEntry(const QString& dbDir,
                 const QString& dbFile,
                 const QString& newdir,
                 const QString& newfile)
{
    bool ret = true;
    TableItemDataPointer pID = mapsFullpathToItem_[pathCombine(dbDir,dbFile)];

    if(pID)
    {
        ret &= pID->Rename(dbDir,dbFile,newdir,newfile);

        int row = itemDatas_.indexOf(pID);
        Q_ASSERT(row >= 0);
        emit dataChanged(createIndex(row*3,0), createIndex((row*3)+2,0));
    }
    return ret;
}

//void TableModel::SetShowMissing(bool bToggle)
//{
//    if(bShowMissing_==bToggle)
//        return;
//
//    bShowMissing_ = bToggle;
//}
