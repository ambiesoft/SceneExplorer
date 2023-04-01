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


//#include <QApplication>
//#include <QImage>
//#include <QPixmap>
//#include <QFileInfo>
#include <QTableView>
//#include <QColor>
//#include <QSharedPointer>
//#include <QMap>
//#include <QDateTime>
//#include <QRegExp>
#include <QDebug>
//#include <QTimerEvent>
#include <QFileInfo>

#include "../../lsMisc/stdQt/stdQt.h"

#include "consts.h"

//#include "helper.h"
//#include "globals.h"

//#include "tableitemdata.h"
#include "tablemodel.h"

using namespace Consts;
using namespace AmbiesoftQt;

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    const int actualIndex = index.row()/RowCountPerEntry;
    int mod = index.row() % RowCountPerEntry;
    bool isFilename = mod==0;
    bool isImage = mod==1;
    bool isInfo = mod == 2;

    switch(role)
    {
    case TableRole::MovieFileFull:
        return itemDatas_[actualIndex]->getMovieFileFull();

    case TableRole::ID:
        return itemDatas_[actualIndex]->getID();

    case TableRole::GetUrl:
        return itemDatas_[actualIndex]->getUrl();

    case TableRole::GetMemo:
        return itemDatas_[actualIndex]->getMemo();
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

            // workaround for showing entire title.
            // sometimes title not shown properly
            QSize qs = calculateSize(index, titleText + " ");
            parent_->setRowHeight(index.row(), qs.height());
            return titleText;
        }
            break;

        case Qt::TextColorRole:
        {
            // if(bShowMissing_)
            {
                if(!QFileInfo(itemDatas_[actualIndex]->getMovieFileFull()).exists())
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
                qDebug() << "Image DecorationRoll First: Index=" << index.row() << __FUNCTION__;
            itemData->setDisplayed();

            parent_->setColumnWidth(index.row(), mainWindow_->GetThumbWidth());
            // qDebug() << "DecorationRole previous rowheight = " << parent_->rowHeight(index.row());
            parent_->setRowHeight(index.row(), mainWindow_->GetThumbHeight());

            QString imageFile = pathCombine(FILEPART_THUMBS,
                                            itemData->getThumbnailFiles()[getActualColumnIndex(index.column())]);

            if(imagecache_==ImageCacheType::IC_NEVER)
            {
                QImage image(imageFile);
                return QPixmap::fromImage(image);
            }

            if(!scrolled_)
            {
                if(!mapPixmaps_.keys().contains(imageFile))
                {
                    QImage image(imageFile);
                    QVariant vpix(QPixmap::fromImage(image));
                    mapPixmaps_[imageFile]= vpix;
                }
            }
            if(mapPixmaps_.keys().contains(imageFile))
            {
                return mapPixmaps_[imageFile];
            }

            std::deque<QModelIndex>::iterator found =
                    std::find(suspendImageIndexes_.begin(), suspendImageIndexes_.end(), index);
            if(found != suspendImageIndexes_.end())
            {
                suspendImageIndexes_.erase(found);
            }
            suspendImageIndexes_.push_back(index);


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
            // if (bShowMissing_)
            {
                if (!QFileInfo(itemDatas_[actualIndex]->getMovieFileFull()).exists())
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
