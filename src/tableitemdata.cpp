#include <QStringList>
#include <QFileInfo>
#include <QDateTime>

#include "helper.h"
#include "tableitemdata.h"

TableItemData::TableItemData(const QStringList& files,
                             const QString& movieDirectory,
                             const QString& movieFileName,

                             const qint64& size,
                             const qint64& ctime,
                             const qint64& wtime,

                             int thumbwidth,
                             int thumbheight,
                             const double& duration,
                             const QString& format,
                             const QString& vcodec,
                             const QString& acodec,
                             int vWidth,int vHeight)
{
    files_ = files;
    movieDirectory_=movieDirectory;
    movieFilename_=movieFileName;

    size_=size;
    ctime_=ctime;
    wtime_=wtime;

    thumbwidth_=thumbwidth;
    thumbheight_=thumbheight;

    duration_=duration;
    format_=format;

    vcodec_=vcodec;
    acodec_=acodec;

    vWidth_=vWidth;
    vHeight_=vHeight;
}
QString TableItemData::getMovieFileFull() const {
    Q_ASSERT(!movieDirectory_.isEmpty());
    Q_ASSERT(!movieFilename_.isEmpty());

    return pathCombine(movieDirectory_,movieFilename_);
}

qint64 TableItemData::getSize() const
{
    return size_;
}
qint64 TableItemData::getCtime() const
{
    return ctime_;
}
qint64 TableItemData::getWtime() const
{
    return wtime_;
}
