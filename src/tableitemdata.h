#ifndef LISTITEMDATA_H
#define LISTITEMDATA_H

#include <QStringList>
#include <QSharedPointer>

class TableItemData;
typedef QSharedPointer<TableItemData> TableItemDataPointer;

class TableItemData
{
#ifdef QT_DEBUG
	static int dinstcount_;
#endif
    QStringList files_;
    QString movieDirectory_;
    QString movieFilename_;

    int width_;
    int height_;
    qint64 size_=-1;
    qint64 ctime_=-1;
    qint64 wtime_=-1;
    int thumbwidth_=-1;
    int thumbheight_=-1;
    double duration_=-1;
    QString format_;
    QString vcodec_;
    QString acodec_;
    int vWidth_,vHeight_;

    TableItemData(const QStringList& files,
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
                  int vWidth,int vHeight);

public:
	static TableItemDataPointer Create(
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
		const QString& vcodec,
		const QString& acodec,
		int vWidth, int vHeight)
	{
		return TableItemDataPointer(new TableItemData(
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
			vcodec,
			acodec,

			vWidth, vHeight));
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

    bool Rename(const QString& oldname,
                const QString& newname)
    {
        Q_ASSERT(movieFilename_==oldname);
        if(movieFilename_!=oldname)
            return false;

        movieFilename_=newname;
        return true;
    }
};

#endif // LISTITEMDATA_H
