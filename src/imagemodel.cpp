// ImageModel.cpp
#include <QImage>
#include <QPixmap>
#include "imagemodel.h"

ImageModel::ImageModel(QObject *parent)
    :QAbstractTableModel(parent)
{
}

int ImageModel::rowCount(const QModelIndex & /*parent*/) const
{
   return 2;
}

int ImageModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

QVariant ImageModel::data(const QModelIndex &, int role) const
{
//    if (role == Qt::DisplayRole)


        QImage image("C:\\cygwin\\home\\svava\\gitdev\\SceneExplorer\\build-SceneExplorer-Desktop_Qt_5_10_0_MinGW_32bit-Debug\\49a9b297-0c15-42db-a18b-dc042fa7f4d3-1.png");

        QPixmap pix = QPixmap::fromImage(image);
//       return QString("Row%1, Column%2")
//                   .arg(index.row() + 1)
//                   .arg(index.column() +1);

        if (role == Qt::DecorationRole)
        {
            return pix;
        }
        if(role==Qt::SizeHintRole)
        {
            return pix.size();
        }
    return QVariant();
}
Qt::ItemFlags ImageModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}
