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

class TableItemData;
typedef QSharedPointer<TableItemData> TableItemDataPointer;
// typedef QSharedPointer<QPixmap> PixmapPointer;

class TableItemData
{
#ifdef QT_DEBUG
	static int dinstcount_;
#endif
    QStringList files_;
    QString movieDirectory_;
    QString movieFilename_;

    qint64 id_ = 0;
    int width_;
    int height_;
    qint64 size_=-1;
    qint64 ctime_=-1;
    qint64 wtime_=-1;
    int thumbwidth_=-1;
    int thumbheight_=-1;
    double duration_=-1;
    QString format_;
    int bitrate_=0;
    QString vcodec_;
    QString acodec_;
    int vWidth_,vHeight_;

    qint64 lastaccess_ = 0;
    TableItemData(const qint64& id,
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

                  const qint64& lastaccess);

public:
	static TableItemDataPointer Create(
            const qint64& id,
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
        int vWidth, int vHeight,
        const qint64& lastaccess)
	{
        Q_ASSERT(!movieDirectory.isEmpty());
        Q_ASSERT(!movieFileName.isEmpty());
        if(movieDirectory.isEmpty() || movieFileName.isEmpty())
        {
            return nullptr;
        }
        return TableItemDataPointer(new TableItemData(
            id,
			files,
			movieDirectory,
			movieFileName,

			size,
			ctime,
			wtime,

			thumbwidth,
			thumbheight,
			duration,
			format,
            bitrate,
			vcodec,
			acodec,

            vWidth, vHeight,

            lastaccess));
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
        return id_;
    }
    QVariant getIDVariant() const {
        return id_==0 ? QVariant() : id_;
    }
    void setID(const qint64& id) {
        Q_ASSERT(id_==0 || id_==id);
        id_=id;
    }
    QStringList getImageFiles() const {
        return files_;
    }
    int getWidth() const {
        return width_;
    }
    int getHeight() const {
        return height_;
    }
    QString getMovieDirectory() const
    {
        Q_ASSERT(movieDirectory_.endsWith('/'));
        return movieDirectory_;
    }
    QString getMovieFileName() const
    {
        return movieFilename_;
    }
    QString getMovieFileFull() const ;
    QString getFormat() const {
        return format_;
    }
    int getBitrate() const {
        return bitrate_;
    }
    double getDuration() const {
        return duration_;
    }
    QString getVcodec() const {
        return vcodec_;
    }
    QString getAcodec() const {
        return acodec_;
    }
    int getVWidth() const {
        return vWidth_;
    }
    int getVHeight() const {
        return vHeight_;
    }
    qint64 getSize() const;
    qint64 getCtime() const;
    qint64 getWtime() const;

    int getOpenCount() const {
        return opencountTmp_;
    }
    void setOpenCount(int v) {
        opencountTmp_ = v;
    }
    qint64 getLastAccess() const {
        return lastaccess_;
    }
    void setLastAccess(const qint64& v) {
        lastaccess_ = v;
    }
    int getResolutionMultiplied() const {
        return vWidth_*vHeight_;
    }
    bool Rename(const QString& olddir,
                const QString& oldname,
                const QString& newdir,
                const QString& newname)
    {
        Q_ASSERT(movieDirectory_==olddir);
        Q_ASSERT(movieFilename_==oldname);
        if(movieDirectory_!=olddir)
            return false;

        if(movieFilename_!=oldname)
            return false;

        movieDirectory_=newdir;
        movieFilename_=newname;

        return true;
    }
    QMap<QString,QVariant> getColumnValues() const;

    int opencountTmp_ = 0;
	void IncrementOpenCount() {
        ++opencountTmp_;
	}
};

#endif // LISTITEMDATA_H
