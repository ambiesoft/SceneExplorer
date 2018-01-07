#include <QStringList>
#include <QFileInfo>

#include "tableitemdata.h"

TableItemData::TableItemData(const QStringList& files,
                   int width,
                   int height,
                   const QString& movieFile,
                   const QString& format)
{
    files_ = files;
    width_ = width;
    height_ = height;
    movieFile_ = movieFile;
    format_=format;
    size_ = -1;
}
qint64 TableItemData::getSize() const
{
    if(size_==-1)
    {
        QFileInfo fi(movieFile_);
        size_=fi.size();
    }
    return size_;
}
