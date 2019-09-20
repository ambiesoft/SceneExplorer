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

#include <QStringList>
#include <QFileInfo>
#include <QDateTime>
#include <QMap>
#include <QVariant>

#include "../../lsMisc/stdQt/stdQt.h"

#include "helper.h"
#include "tableitemdata.h"

using namespace AmbiesoftQt;

#ifdef QT_DEBUG
int TableItemData::dinstcount_ = 0;
#endif

TableItemData::TableItemData(const qint64& id,
                             const QStringList& files,
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
                             int vWidth,int vHeight,
                             const double& fps,
                             int opencount,
                             const qint64& lastaccess)
{
#ifdef QT_DEBUG
    ++dinstcount_;
#endif
    id_=id;
    thumbFiles_ = files;
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

    fps_=fps;
    opencount_=opencount;
    lastaccess_ = lastaccess;
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
    ret["thumbid"] = getUUIDFromThumbfile(getThumbnailFiles()[0]);
    ret["duration"]=getDuration();
    ret["format"]=getFormat();
    ret["bitrate"]=getBitrate();
    ret["vcodec"]=getVcodec();
    ret["acodec"]=getAcodec();
    ret["vwidth"]=getVWidth();
    ret["vheight"]=getVHeight();
    //	ret["opencount"] = getOpenCount();
    //	ret["lastaccess"] = getLastAccess();
    ret["thumbext"] = getThumbExt();

    // version 3
    ret["fps"] = getFps();


    return ret;
}
