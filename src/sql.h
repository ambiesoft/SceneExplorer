#ifndef SQL_H
#define SQL_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class TableItemData;
class Sql : public QObject
{
    Q_OBJECT

    QSqlDatabase db_;

    int GetMovieFileInfo(const QString& movieFile,
                         bool& exist,
                         qint64& size,
                         QString& directory,
                         QString& name,
                         QString* salient,
                         qint64& ctime,
                         qint64& wtime) const;

    int removeEntry(const QString& thumbid);

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
    static QString getErrorStrig(int thumbRet);

    QSqlQuery* pQDeleteFromDirectoryName_ = nullptr;
    QSqlQuery* getDeleteFromDirectoryName();

    QSqlQuery* pQInsert_=nullptr;
    QSqlQuery* getInsertQuery();

    QSqlQuery* pQGetInfo_=nullptr;
    QSqlQuery* getGetInfoQuery();

    Sql();
    ~Sql();
    int AppendData(const QStringList& files,
             int width, int height,
             const QString& movieFile,
             const QString& format);
    bool IsSameFile(const QString& dir,
                    const QString& name,
                    const qint64& size,
                    const QString& salient);
    int filterWithEntry(const QString& movieDir,
                        const QStringList& movieFiles,
                        QStringList& results);
    int hasThumb(const QString& movieFile) ;

    bool GetAll(QList<TableItemData*>& v);
};

#endif // SQL_H
