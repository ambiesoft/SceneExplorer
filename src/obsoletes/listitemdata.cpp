#include "listitemdata.h"

#include <QStringList>

ListItemData::ListItemData(const QStringList& files, int width, int height, const QString& movieFile)
{
    files_ = files;
    width_ = width;
    height_ = height;
    movieFile_ = movieFile;
}
