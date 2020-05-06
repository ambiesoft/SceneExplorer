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

#ifndef LISTITEMDATA_H
#define LISTITEMDATA_H

#include <QSharedPointer>
#include <QVariant>

class TableItemData;
typedef QSharedPointer<TableItemData> TableItemDataPointer;
// typedef QSharedPointer<QPixmap> PixmapPointer;

class TableItemData
{
#ifdef QT_DEBUG
    static int dinstcount_;
#endif

public:
    class TableItemDataArgs
    {
    public:
         qint64 id = 0;
         QStringList thumbFiles;
         QString movieDirectory;
         QString movieFileName;

         qint64 size = 0;
         qint64 ctime = 0;
         qint64 wtime = 0;

        int thumbwidth = 0, thumbheight = 0;
        double duration = 0;
        QString format;
        int bitrate = 0;
        QString vcodec;
        QString acodec;
        int vWidth = 0, vHeight = 0;

        double fps = 0;
        QString url;
        QString memo;

        int opencount = 0;
        qint64 lastaccess = 0;
    public:
        TableItemDataArgs() = delete;
        TableItemDataArgs(const qint64& id,
                          const QStringList& thumbFiles,
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
                          const QString& url,
                          const QString& memo,

                          int opencount,
                          const qint64& lastaccess):
            id(id),
            thumbFiles(thumbFiles),
            movieDirectory(movieDirectory),
            movieFileName(movieFileName),

            size(size),
            ctime(ctime),
            wtime(wtime),

            thumbwidth(thumbwidth),
            thumbheight(thumbheight),
            duration(duration),
            format(format),
            bitrate(bitrate),
            vcodec(vcodec),
            acodec(acodec),
            vWidth(vWidth),
            vHeight(vHeight),

            fps(fps),
            url(url),
            memo(memo),

            opencount(opencount),
            lastaccess(lastaccess){}
    };
private:
    TableItemDataArgs values_;
    QString thumbExt_;


    TableItemData(const TableItemDataArgs& args);

public:
    static TableItemDataPointer Create(const TableItemDataArgs& args)
    {
        Q_ASSERT(!args.movieDirectory.isEmpty());
        Q_ASSERT(!args.movieFileName.isEmpty());
        if(args.movieDirectory.isEmpty() || args.movieFileName.isEmpty())
        {
            return nullptr;
        }
        return TableItemDataPointer(new TableItemData(args));
    }
    ~TableItemData()
    {
#ifdef QT_DEBUG
        --dinstcount_;
#endif
    }
#ifdef QT_DEBUG
    static bool isAllClear()
    {
        return dinstcount_ == 0;
    }
#endif
    qint64 getID() const {
        return values_.id;
    }
    QStringList getThumbnailFiles() const {
        return values_.thumbFiles;
    }
    QVariant getIDVariant() const {
        return values_.id==0 ? QVariant() : values_.id;
    }
    void setID(const qint64& id) {
        Q_ASSERT(values_.id==0 || values_.id==id);
        values_.id=id;
    }
    int getThumbWidth() const {
        return values_.thumbwidth;
    }
    int getThumbHeight() const {
        return values_.thumbheight;
    }
    QString getMovieDirectory() const
    {
        Q_ASSERT(values_.movieDirectory.endsWith('/'));
        return values_.movieDirectory;
    }
    QString getMovieFileName() const
    {
        return values_.movieFileName;
    }
    QString getMovieFileFull() const ;
    QString getFormat() const {
        return values_.format;
    }
    void setFormat(const QString& s) {
        values_.format = s;
    }
    int getBitrate() const {
        return values_.bitrate;
    }
    void setBitrate(int i) {
        values_.bitrate=i;
    }
    double getDuration() const {
        return values_.duration;
    }
    void setDuration(const double& d) {
        values_.duration=d;
    }
    QString getVcodec() const {
        return values_.vcodec;
    }
    void setVcodec(const QString& s) {
        values_.vcodec=s;
    }
    QString getAcodec() const {
        return values_.acodec;
    }
    void setAcodec(const QString& s) {
        values_.acodec=s;
    }
    int getVWidth() const {
        return values_.vWidth;
    }
    void setVWidth(int i) {
        values_.vWidth=i;
    }
    int getVHeight() const {
        return values_.vHeight;
    }
    void setVHeight(int i) {
        values_.vHeight=i;
    }
    qint64 getSize() const;
    qint64 getCtime() const;
    qint64 getWtime() const;

    int getOpenCount() const {
        return values_.opencount;
    }
    void setOpenCount(int v) {
        values_.opencount = v;
    }
    qint64 getLastAccess() const {
        return values_.lastaccess;
    }
    void setLastAccess(const qint64& v) {
        values_.lastaccess = v;
    }


    QString getThumbExt() const {
        Q_ASSERT(!thumbExt_.isEmpty());
        return thumbExt_;
    }
    void setThumbExt(const QString& ext) {
        thumbExt_=ext;
    }

    double getFps() const {
        return values_.fps;
    }
    void setFps(const double& d) {
        values_.fps=d;
    }

    QString getUrl() const {
        return values_.url;
    }
    void setUrl(const QString& url) {
        values_.url = url;
    }

    QString getMemo() const {
        return values_.memo;
    }
    void setMemo(const QString& memo) {
        values_.memo=memo;
    }

//    int recordversion_=0;
//    int getRecodVersion() const {
//        return recordversion_;
//    }
//    void setRecordVersion(const int& i) {
//        recordversion_=i;
//    }

    int getResolutionMultiplied() const {
        return values_.vWidth * values_.vHeight;
    }
    bool Rename(const QString& olddir,
                const QString& oldname,
                const QString& newdir,
                const QString& newname)
    {
        Q_ASSERT(values_.movieDirectory==olddir);
        Q_ASSERT(values_.movieFileName==oldname);
        if(values_.movieDirectory!=olddir)
            return false;

        if(values_.movieFileName==oldname)
            return false;

        values_.movieDirectory=newdir;
        values_.movieFileName=newname;

        return true;
    }
    QMap<QString,QVariant> getColumnValues() const;

    // int opencountTmp_ = 0;
    //	void IncrementOpenCount() {
    //        ++opencount_;
    //	}

    bool displayed_=false;
    bool isDisplayed() const {
        return displayed_;
    }
    void setDisplayed() {
        displayed_=true;
    }

};

#endif // LISTITEMDATA_H
