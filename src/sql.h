#ifndef SQL_H
#define SQL_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class TableItemData;
class Sql : public QObject
{
    Q_OBJECT

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
    QString getAllColumns(bool bBrace, bool bQ);
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
    static QString getErrorStrig(int thumbRet);

    QSqlQuery* pQDeleteFromDirectoryName_ = nullptr;
    QSqlQuery* getDeleteFromDirectoryName();

    QSqlQuery* pQInsert_=nullptr;
    QSqlQuery* getInsertQuery();

    QSqlQuery* pQGetInfo_=nullptr;
    QSqlQuery* getGetInfoQuery();

    Sql();
    ~Sql();
    int AppendData(const TableItemData& tid);
    bool IsSameFile(const QString& dir,
                    const QString& name,
                    const qint64& size,
                    const QString& salient);
    int filterWithEntry(const QString& movieDir,
                        const QStringList& movieFiles,
                        QStringList& results);
    int hasThumb(const QString& movieFile) ;

    bool GetAll(QList<TableItemData*>& v);
    int GetAllEntry(const QString& dir,
                    QStringList& entries,
                    QVector<qint64>& sizes,
                    QVector<qint64>& ctimes,
                    QVector<qint64>& wtimes,
                    QStringList& salients);
    bool RenameEntries(const QString& dir,
                       const QStringList& renameOlds,
                       const QStringList& renameNews);
};

#endif // SQL_H
