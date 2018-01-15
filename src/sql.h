#ifndef SQL_H
#define SQL_H

#include <QSqlDatabase>

#include "tableitemdata.h"

class Sql : public QObject
{
    Q_OBJECT

	static constexpr const char* DBFILENAME = "./db.sqlite3";

    QSqlDatabase db_;
    bool ok_=false;
    QStringList allColumns_;
    int GetMovieFileInfo(const QString& movieFile,
                         bool& exist,
                         qint64& size,
                         QString& directory,
                         QString& name,
                         QString* salient,
                         qint64& ctime,
                         qint64& wtime) const;

    int removeEntry(const QString& thumbid);
    QStringList getAllColumnNames();
    QString getAllColumns(bool bBrace, bool bQ);
    // QString getAllColumnsUpdate(TableItemDataPointer tid);
public:
    enum SQL_ERROR {
        NO_ERROR,
        MOVIEFILE_NOT_FOUND,
        FILESIZE_UNDERZERO,
        ERROR_CREATE_SALIENT,
        THUMBFILE_NOT_FOUND,
        UUID_FORMAT_ERROR,
        SQL_PREPARE_FAILED,
        SQL_EXEC_FAILED,
        THUMB_EXIST,
        THUMB_NOT_EXIST,
        THUMBID_IS_NOT_UUID,
    };
    bool isOK() const {
        return ok_;
    }
	static QString getDBFileName() {
		return DBFILENAME;
	}

    static QString getErrorStrig(int thumbRet);

    QSqlQuery* pQDeleteFromDirectoryName_ = nullptr;
    QSqlQuery* getDeleteFromDirectoryName();

    QSqlQuery* pQInsert_=nullptr;
    QSqlQuery* getInsertQuery(TableItemDataPointer tid);

    QSqlQuery* pQGetInfo_=nullptr;
    QSqlQuery* getGetInfoQuery();

    Sql();
    ~Sql();
    int AppendData(TableItemDataPointer tid);
    bool IsSameFile(const QString& dir,
                    const QString& name,
                    const qint64& size,
                    const QString& salient);
    int filterWithEntry(const QString& movieDir,
                        const QStringList& movieFiles,
                        QStringList& results);
    int hasThumb(const QString& movieFile) ;

    // bool GetAll(QList<TableItemDataPointer>& v, const QString& dir = QString());
	bool GetAll(QList<TableItemDataPointer>& v,
		const QStringList& dirs = QStringList());


    int GetAllEntry(const QString& dir,
                    QStringList& entries,
                    QVector<qint64>& sizes,
                    QVector<qint64>& ctimes,
                    QVector<qint64>& wtimes,
                    QStringList& salients);
    bool RenameEntries(const QString& dir,
                       const QStringList& renameOlds,
                       const QStringList& renameNews);
    bool RenameEntry(const QString& dbDir,
                     const QString& dbFile,
                     const QString& newdir,
                     const QString& newfile);
    bool getEntryFromSalient(const QString& salient,
                             QStringList& dirsDB,
                             QStringList& filesDB,
                             QList<qint64>& sizesDB);

    bool hasEntry(const QString& dir,
                  const QString& file,
                  const QString& sa);
    bool RemoveEntry(const QString& dir,
                     const QString& file);
};

#endif // SQL_H
