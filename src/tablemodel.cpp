// ImageModel.cpp
#include <QImage>
#include <QPixmap>
#include <QFileInfo>
#include <QTableView>

#include "consts.h"
#include "helper.h"

#include "tableitemdata.h"
#include "tablemodel.h"

TableModel::TableModel(QTableView *parent)
    :QAbstractTableModel(parent)
{
    parent_=parent;
}
void TableModel:: AppendData(TableItemData* pItemData)
{
    beginInsertRows(QModelIndex(),
                    items_.count()*RowCountPerEntry,
                    (items_.count()*RowCountPerEntry)+RowCountPerEntry-1);
    // beginResetModel();
    items_.append(pItemData);
    // endResetModel();
    endInsertRows();


    int newRowFilename = rowCount()-TableModel::RowCountPerEntry;
    int newRowInfo = newRowFilename+1;
    int newRowImage = newRowFilename+2;

    parent_->setSpan(newRowFilename,0,1,5);
    parent_->setSpan(newRowInfo,0,1,5);
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
void TableModel::AppendDatas(const QList<TableItemData*>&v)
{
    beginResetModel();
    items_.append(v);
    endResetModel();
}
int TableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return items_.count()*RowCountPerEntry;
}

int TableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

// http://comments.gmane.org/gmane.comp.lib.qt.general/34914
QString size_human(QFileInfo& fi)
{
    float num = fi.size();
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

QString TableModel::GetInfoText(const TableItemData& item, bool isFilename) const
{
    QString ret;
    if(isFilename)
    {
        ret.append(tr("File:"));
        ret.append(" ");
        ret.append(item.getMovieFile());
        return ret;
    }
    else
    {
        ret.append(tr("Size:"));
        ret.append(" ");
        QFileInfo fi(item.getMovieFile());
        ret.append(size_human(fi));
        ret.append(", ");

        ret.append(tr("Format:"));
        ret.append(" ");
        ret.append(item.getFormat());

        return ret;
    }
}
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    int actualIndex = index.row()/RowCountPerEntry;
    bool isFilename = (index.row()%RowCountPerEntry)==0;
    bool isInfo = (index.row()%RowCountPerEntry)==1;

    if(role==TableRole::MovieFile)
    {
        return items_[actualIndex]->getMovieFile();
    }
    if(isFilename)
    {
        if(index.column() != 0)
            return QVariant();

        switch(role)
        {
            case Qt::DisplayRole:
            {
                return GetInfoText(*(items_[actualIndex]),true);
            }
            break;
        }
    }
    else if(isInfo)
    {
        if(index.column() != 0)
            return QVariant();

        switch(role)
        {
            case Qt::DisplayRole:
            {
                return GetInfoText(*(items_[actualIndex]),false);
            }
            break;
        }
    }
    else
    {
        switch(role)
        {
            case Qt::DecorationRole:
            {
                QString imageFile = items_[actualIndex]->getImageFiles()[index.column()];
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
