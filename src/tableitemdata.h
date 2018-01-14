#ifndef LISTITEMDATA_H
#define LISTITEMDATA_H



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
    int bitrate_=0;
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
                  int bitrate,
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
        int bitrate,
		const QString& vcodec,
		const QString& acodec,
		int vWidth, int vHeight)
	{
        Q_ASSERT(!movieDirectory.isEmpty());
        Q_ASSERT(!movieFileName.isEmpty());
        if(movieDirectory.isEmpty() || movieFileName.isEmpty())
        {
            return nullptr;
        }
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
            bitrate,
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
};

#endif // LISTITEMDATA_H
