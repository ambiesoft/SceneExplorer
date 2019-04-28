//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

// ImageModel.cpp
#include <QApplication>
#include <QImage>
#include <QPixmap>
#include <QFileInfo>
#include <QTableView>
#include <QColor>
#include <QSharedPointer>
#include <QMap>
#include <QDateTime>
#include <QRegExp>
#include <QDebug>
#include <QTimerEvent>

#include "../../lsMisc/stdQt/stdQt.h"

#include "consts.h"
#include "helper.h"
#include "globals.h"

#include "tableitemdata.h"
#include "tablemodel.h"

using namespace Consts;
using namespace AmbiesoftQt;

TableModel::TableModel(QTableView *parent, IMainWindow* mainWindow)
    :QAbstractTableModel(parent), parent_(parent), mainWindow_(mainWindow)
{

}
void TableModel:: AppendData(const TableItemDataPointer& pItemData, const bool enableUpdate)
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

    parent_->setSpan(newRowFilename,0,1,columnCountImage_);
    parent_->setSpan(newRowInfo,0,1,columnCountImage_);

    if(!initColumnWidth_)
    {
        initColumnWidth_=true;
        for(int i=0 ; i < columnCountImage_ ; ++i)
        {
            parent_->setColumnWidth(i, THUMB_WIDTH);
        }
    }

    if(enableUpdate)
        emit itemCountChanged();

    // parent_->setRowHeight(newRowImage, THUMB_HEIGHT);
//    setData(index(newRowImage,0), QSize(THUMB_WIDTH, THUMB_HEIGHT), Qt::SizeHintRole);
//    QHeaderView *verticalHeader = parent_->verticalHeader();
//    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
//    verticalHeader->setDefaultSectionSize(24);
}
void TableModel::ResetData(const QList<TableItemDataPointer>& all)
{
    beginResetModel();
    ClearData();

    for(auto&& item : all)
    {
        AppendData(item, false);
    }

    endResetModel();
    emit itemCountChanged();
    StartImageTimer();

    parent_->scrollToTop();
}

int TableModel::rowCount(const QModelIndex & /*parent*/) const
{
    return itemDatas_.count()*RowCountPerEntry;
}

void TableModel::ClearData()
{
    suspendImageIndexes_.clear();
    itemDatas_.clear();
    mapsFullpathToItem_.clear();
    if(imagecache_ != ImageCacheType::IC_ALWAYS)
        mapPixmaps_.clear();
    emit itemCountChanged();
}

int TableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return columnCountImage_;
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
    return QString().setNum(num,'f',2)+ "" +unit;
}

QString filetime_human(const qint64& filetime)
{
    QDateTime qdt = QDateTime::fromTime_t(filetime);
    return doublequoteIfNecessary(qdt.toString(Qt::DateFormat::DefaultLocaleShortDate));
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
static QString resolution_human(int width, int height)
{
    return QString::number(width) + "x" + QString::number(height);
}
static QString opencount_human(int count)
{
    return QString::number(count);
}
QString TableModel::GetTitleText(TableItemDataPointer item) const
{
    return ExtractInfoText(item, titleTemplate_);
    //    static const char* sep = "   ";

    //    if(GetSortColumn()==SORT_FILENAME)
    //        return item->getMovieFileName();

    //    QString ret = GetSortColumnValue(item);
    //    ret.append(sep);
    //    ret.append(item->getMovieFileName());

    //    return ret;
}

static bool checkDup(const QString& s, QSet<QString>& map)
{
    if(map.contains(s))
        return false;

    map.insert(s);
    return true;
}
QString parseYen(const QString& str)
{
    static QRegExp rx("(\\\\.)");
    QString result;
    int prevpos = 0;
    int pos = 0;

    while ((pos = rx.indexIn(str, pos)) != -1)
    {
        result += str.mid(prevpos,pos-prevpos);
        int matchedlen = rx.matchedLength();
        QString s = str.mid(pos,matchedlen);
        if(false)
        {}
        else if(s=="\\n")
        {
            result += "\n";
        }
        else if(s=="\\r")
        {
            result += "\r";
        }
        else if(s=="\\\\")
        {
            result += "\\";
        }
        else if(s=="\\t")
        {
            result += "\t";
        }
        else
        {
            result += str.mid(pos, matchedlen);
        }
        pos += matchedlen;
        prevpos = pos;
    }
    result += str.mid(prevpos);

    return result;
}
QString TableModel::ExtractInfoText(TableItemDataPointer item, const QString& str) const
{
    static QRegExp rx("(\\$\\{\\w+\\})");
    QString result;
    int prevpos = 0;
    int pos = 0;

    QSet<QString> mapExist;
    while ((pos = rx.indexIn(str, pos)) != -1)
    {
        result += str.mid(prevpos,pos-prevpos);
        int matchedlen = rx.matchedLength();
        QString s = str.mid(pos,matchedlen);// rx.cap(i++);
        if(false)
        {}
        else if(s=="${acodec}")
        {
            result += item->getAcodec();
        }
        else if(s=="${atime}")
        {
            result += filetime_human(item->getLastAccess());
        }
        else if(s=="${bitrate}")
        {
            result += bitrate_human(item->getBitrate());
        }
        else if(s=="${directory}")
        {
            result += item->getMovieDirectory();
        }
        else if(s=="${duration}")
        {
            result += duration_human(item->getDuration());
        }
        else if(s=="${format}")
        {
            result += item->getFormat();
        }
        else if(s=="${id}")
        {
            result += QString::number(item->getID());
        }
        else if(s=="${name}" && checkDup("name", mapExist))
        {
            result += item->getMovieFileName();
        }
        else if(s=="${opencount}")
        {
            result += opencount_human(item->getOpenCount());
        }
        else if(s=="${resolution}")
        {
            result += resolution_human(item->getVWidth(),item->getVHeight());
        }
        else if(s=="${size}")
        {
            result += size_human(item->getSize());
        }
        else if(s=="${sortname}")
        {
            result += GetSortColumnName(mainWindow_->GetCurrentSort());
        }
        else if(s=="${sortvalue}")
        {
            result += GetSortColumnValue(mainWindow_->GetCurrentSort(), item);
        }
        else if(s=="${tags}")
        {
            result += mainWindow_->GetTags(item->getID());
        }
        else if(s=="${vcodec}")
        {
            result += item->getVcodec();
        }
        else if(s=="${wtime}")
        {
            result += filetime_human(item->getWtime());
        }
        else
        {
            result += str.mid(pos, matchedlen);
        }
        pos += matchedlen;
        prevpos = pos;
    }
    result += str.mid(prevpos);

    return parseYen(result);
}
QString TableModel::GetInfoText(TableItemDataPointer item) const
{
    // QString str = "${size}, name=\"${name}\"";
    return ExtractInfoText(item, infoTemplate_);

    //    QString ret;
    //    static const char* sep = " ";

    //    ret.append(size_human(item->getSize()));
    //	ret.append(sep);

    //    ret.append(filetime_human(item->getWtime()));
    //	ret.append(sep);

    //    ret.append(duration_human(item->getDuration()));
    //    ret.append(sep);

    //    ret.append(dq(item->getFormat()));
    //    ret.append(sep);

    //    ret.append(bitrate_human(item->getBitrate()));
    //    ret.append(sep);

    //    ret.append(item->getVcodec());
    //    ret.append(sep);
    //    ret.append(resolution_human(item->getVWidth(),item->getVHeight()));
    //    ret.append(sep);

    //    ret.append(item->getAcodec());
    //	ret.append(sep);

    //    ret.append(dq(item->getMovieDirectory()));
    //    ret.append(sep);

    //    ret.append(opencount_human(item->getOpenCount()));
    //    // ret.append(sep);

    //    return ret;
}

QSize TableModel::calculateSize(const QModelIndex& index, const QString& str) const
{
    int colWidth = parent_->columnWidth(index.column())*columnCountImage_;
    QSize baseSize(colWidth , 10000);

    QFontMetrics metrics(this->data(index, Qt::FontRole).value<QFont>());
    QRect outRect = metrics.boundingRect(
                QRect(QPoint(0, 0), baseSize),
                Qt::AlignLeft | Qt::TextWordWrap,
                str); //this->data(index, Qt::DisplayRole).toString());

    // int multi = outRect.width() / parent_->columnWidth(index.column());
    baseSize.setHeight(outRect.height() + 5);
    return baseSize;
}
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    const int actualIndex = index.row()/RowCountPerEntry;
    int mod = index.row() % RowCountPerEntry;
    bool isFilename = mod==0;
    bool isImage = mod==1;
    bool isInfo = mod == 2;

    if(role==TableRole::MovieFileFull)
    {
        return itemDatas_[actualIndex]->getMovieFileFull();
    }
    else if(role==TableRole::ID)
    {
        return itemDatas_[actualIndex]->getID();
    }

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
            QString titleText = GetTitleText(itemDatas_[actualIndex]);
            QSize qs = calculateSize(index, titleText);
            parent_->setRowHeight(index.row(), qs.height());
            return titleText; //->getMovieFileName();
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

        case Qt::FontRole:
        {
            return fontInfo_;
        }
            break;

            // never comes
            //			case Qt::SizeHintRole:
            //			{
            //			}
            //			break;
        }
    }
    else if(isImage)
    {
        switch(role)
        {
        case Qt::DecorationRole:
        {

            TableItemDataPointer itemData = itemDatas_[actualIndex];
            if(!itemData->isDisplayed())
                qDebug() << "Image DecorationRoll First: Index=" << index.row();
            itemData->setDisplayed();

            parent_->setColumnWidth(index.row(), THUMB_WIDTH);
            // qDebug() << "DecorationRole previous rowheight = " << parent_->rowHeight(index.row());
            parent_->setRowHeight(index.row(), THUMB_HEIGHT);

            QString imageFile = pathCombine(FILEPART_THUMBS,
                                            itemData->getThumbnailFiles()[getActualColumnIndex(index.column())]);

            if(imagecache_==ImageCacheType::IC_NEVER)
            {
                QImage image(imageFile);
                return QPixmap::fromImage(image);
            }

            if(mapPixmaps_.keys().contains(imageFile))
            {
                return mapPixmaps_[imageFile];
            }
            if(suspendImageIndexes_.contains(index))
                suspendImageIndexes_.removeOne(index);

            suspendImageIndexes_.push(index);

            TableModel* pThis = const_cast<TableModel*>(this);
            pThis->StartImageTimer();
            return QVariant();
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
            QString infoText = GetInfoText(itemDatas_[actualIndex]);
            QSize qs = calculateSize(index, infoText);
            parent_->setRowHeight(index.row(), qs.height());

            // reentrant stack overflow
            // parent_->resizeRowToContents(index.row());
            // parent_->resizeColumnToContents(index.column());
            return infoText;
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

        case Qt::FontRole:
        {
            return fontDetail_;
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


//class SortFunctor
//{
//    SORTCOLUMN sc_;
//    bool rev_;

//	bool compare(const TableItemDataPointer& v1, const TableItemDataPointer& v2) const
//	{
//		switch (sc_)
//		{
//        case SORTCOLUMN::SORT_FILENAME:
//			return (v1->getMovieFileFull() < v2->getMovieFileFull());
//        case SORTCOLUMN::SORT_SIZE:
//			return (v1->getSize() < v2->getSize());
//        case SORTCOLUMN::SORT_WTIME:
//            return (v1->getWtime() < v2->getWtime());
//        case SORTCOLUMN::SORT_RESOLUTION:
//            return (v1->getResolutionMultiplied() < v2->getResolutionMultiplied());
//        case SORTCOLUMN::SORT_DURATION:
//            return (v1->getDuration() < v2->getDuration());
//        case SORTCOLUMN::SORT_BITRATE:
//            return (v1->getBitrate() < v2->getBitrate());
//        case SORTCOLUMN::SORT_OPENCOUNT:
//            return (v1->getOpenCount() < v2->getOpenCount());
//        case SORTCOLUMN::SORT_LASTACCESS:
//            return (v1->getLastAccess() < v2->getLastAccess());
//        default:
//			Q_ASSERT(false);
//		}
//		return false;
//	}
//public:
//    SortFunctor(SORTCOLUMN sc,bool rev):
//        sc_(sc), rev_(rev)
//    {}

//	bool operator()(const TableItemDataPointer& v1, const TableItemDataPointer& v2) const
//	{
//		if (rev_)
//			return compare(v2, v1);
//		else
//			return compare(v1, v2);
//	}

//};


//SORTCOLUMN TableModel::GetSortColumn() const
//{
//    return sortColumn_;
//}
//void TableModel::SetSortColumn(SORTCOLUMN sc)
//{
//    if(sortColumn_ != sc)
//    {
//        sortColumn_ = sc;
//        emit sortParameterChanged(sc,GetSortReverse());
//    }
//}




QString TableModel::GetSortColumnValue(SORTCOLUMNMY sc, TableItemDataPointer item) const
{
    switch(sc)
    {
    case SORT_FILENAME:return item->getMovieFileName();
    case SORT_SIZE:return size_human(item->getSize());
    case SORT_WTIME:return filetime_human(item->getWtime());
    case SORT_RESOLUTION: return resolution_human(item->getVWidth(),item->getVHeight());
    case SORT_DURATION:return duration_human(item->getDuration());
    case SORT_BITRATE:return bitrate_human(item->getBitrate());
    case SORT_OPENCOUNT:return opencount_human(item->getOpenCount());
    case SORT_LASTACCESS:return filetime_human(item->getLastAccess());
    default :
        Q_ASSERT(false);

    }
    return QString();
}

//bool TableModel::GetSortReverse() const
//{
//    return sortReverse_;
//}
//void TableModel::SetSortReverse(bool rev)
//{
//    if(sortReverse_ != rev)
//    {
//        sortReverse_ = rev;
//        emit sortParameterChanged(GetSortColumn(), rev);
//    }
//}

//void TableModel::Sort_obsolete(SORTCOLUMN column, bool rev)
//{
//    SetSortReverse(rev);
//    SetSortColumn(column);

//    SortFunctor func(column,rev);

//    beginResetModel();
//    std::sort(itemDatas_.begin(), itemDatas_.end(), func);
//    endResetModel();
//}
//void TableModel::Sort_obsolete(SORTCOLUMN column)
//{
//    if(column==GetSortColumn())
//        SetSortReverse(!GetSortReverse());
//    Sort_obsolete(column,GetSortReverse());
//}

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
bool TableModel::RenameEntry(const QString& oldDir,
                             const QString& oldFile,
                             const QString& newdir,
                             const QString& newfile)
{
    QString oldfull = pathCombine(oldDir,oldFile);
    TableItemDataPointer pID = mapsFullpathToItem_[oldfull];

    if(!pID)
        return false;

    VERIFY(pID->Rename(oldDir,oldFile,newdir,newfile));

    QString newfull = pathCombine(newdir,newfile);

    // reset cache
    mapsFullpathToItem_.remove(oldfull);
    mapsFullpathToItem_.insert(newfull,pID);

    int row = itemDatas_.indexOf(pID);
    Q_ASSERT(row >= 0);
    emit dataChanged(createIndex(row*3,0), createIndex((row*3)+3,0));
    return true;
}

void TableModel::UpdateOpenCountAndLastAccess(const QString& movieFile,
                                              const int* opencount,
                                              const qint64* lastaccess)
{
    TableItemDataPointer pID = mapsFullpathToItem_[normalizeFile(movieFile)];

    if (pID)
    {
        if(opencount)
            pID->setOpenCount(*opencount);

        if(lastaccess)
            pID->setLastAccess(*lastaccess);

        int row = itemDatas_.indexOf(pID);
        Q_ASSERT(row >= 0);
        emit dataChanged(createIndex(row * 3, 0), createIndex((row * 3) + 2, 0));
    }
}
QModelIndex TableModel::GetIndex(const QString& movieFile) const
{
    TableItemDataPointer pID = mapsFullpathToItem_[normalizeFile(movieFile)];
    if (pID)
    {
        int row = itemDatas_.indexOf(pID);
        Q_ASSERT(row >= 0);
        return createIndex(row*RowCountPerEntry,0);
    }
    return QModelIndex();
}
void TableModel::RemoveItem(const QString& movieFile)
{
    if(movieFile.isEmpty())
    {
        Alert(parent_, tr("File is empty."));
        return;
    }

    TableItemDataPointer pID = mapsFullpathToItem_[normalizeFile(movieFile)];

    if (pID)
    {
        VERIFY(1==mapsFullpathToItem_.remove(movieFile));

        int row = itemDatas_.indexOf(pID);
        Q_ASSERT(row >= 0);
        //beginRemoveRows(QModelIndex(), row*RowCountPerEntry, (row*RowCountPerEntry)+RowCountPerEntry);
        beginResetModel();
        itemDatas_.removeAt(row);
        //endRemoveRows();
        endResetModel();
    }
}

void TableModel::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != timerID_)
        return;

    if(suspendImageIndexes_.empty())
    {
        KillImageTimer();
        return;
    }

    static QList<qint64> lastTenTick;
    lastTenTick.append(imageElapsedTimer_.elapsed());
    if(lastTenTick.count() >= 10)
    {
        qint64 sum = 0;
        for(int i=1 ; i < 10; ++i)
            sum += (lastTenTick[i] - lastTenTick[i-1] - imageInterval_);

        sum /= 9;
        // Q_ASSERT(sum >= 0);

        if(sum < 10)
        {
            imageInterval_ = std::max(10, imageInterval_ - 10);
        }
        else
        {
            imageInterval_ += 10;
        }
        qDebug() << "Image Timer Interval" << imageInterval_;
        lastTenTick.clear();
        imageElapsedTimer_.invalidate();
        imageElapsedTimer_.start();
    }

//    if((QApplication::mouseButtons() & Qt::MouseButton::LeftButton) != 0)
//        return;

    QModelIndex index = suspendImageIndexes_.pop();
    qDebug() << QString("TableModel timer row=%1, col=%2, count=%3").arg(index.row()).arg(index.column()).
                arg(suspendImageIndexes_.count());

    const int actualIndex = index.row()/RowCountPerEntry;

    TableItemDataPointer itemData = itemDatas_[actualIndex];

    QString imageFile = pathCombine(FILEPART_THUMBS,
                                    itemData->getThumbnailFiles()[getActualColumnIndex(index.column())]);

    {
        if(!mapPixmaps_.keys().contains(imageFile))
        {
            QImage image(imageFile);
            QVariant vpix(QPixmap::fromImage(image));
            mapPixmaps_[imageFile]= vpix;
        }
        // return mapPixmaps_[imageFile];
    }
    emit dataChanged(index,index);
}
void TableModel::StartImageTimer()
{
    if(timerID_ != 0)
        return;
    Q_ASSERT(0 < imageInterval_);
    timerID_ = startTimer(imageInterval_);
    imageElapsedTimer_.start();
    qDebug() << QString("Timer %1 started").arg(timerID_);
}
void TableModel::KillImageTimer()
{
    qDebug() << QString("Timer %1 killed").arg(timerID_);
    killTimer(timerID_);
    timerID_=0;
}
