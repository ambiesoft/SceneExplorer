#include "tableitemdata.h"

#include <QStringList>

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
}
