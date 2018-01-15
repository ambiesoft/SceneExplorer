#include <QStringList>
#include <QFileInfo>
#include <QDateTime>
#include <QMap>
#include <QVariant>

#include "helper.h"
#include "tableitemdata.h"

#ifdef QT_DEBUG
int TableItemData::dinstcount_ = 0;
#endif

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
                             int bitrate,
                             const QString& vcodec,
                             const QString& acodec,
                             int vWidth,int vHeight)
{
#ifdef QT_DEBUG
	++dinstcount_;
#endif
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
    bitrate_=bitrate;
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

QMap<QString,QVariant> TableItemData::getColumnValues() const
{
    QMap<QString,QVariant> ret;
    ret["size"]=getSize();
    ret["ctime"]=getCtime();
    ret["wtime"]=getWtime();
    ret["directory"]=getMovieDirectory();
    ret["name"]=getMovieFileName();
    ret["salient"] = createSalient(getMovieFileFull(), getSize());
    ret["thumbid"] = getUUIDFromThumbfile(getImageFiles()[0]);
    ret["duration"]=getDuration();
    ret["format"]=getFormat();
    ret["bitrate"]=getBitrate();
    ret["vcodec"]=getVcodec();
    ret["acodec"]=getAcodec();
    ret["vwidth"]=getVWidth();
    ret["vheight"]=getVHeight();

    return ret;
}
