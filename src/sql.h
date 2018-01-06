#ifndef SQL_H
#define SQL_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class Sql : public QObject
{
    Q_OBJECT

    QSqlDatabase db_;

public:
    enum SQL_ERROR {
        NO_ERROR,
        MOVIEFILE_NOT_FOUND,
        FILESIZE_UNDERZERO,
        ERROR_CREATE_SALIENT,
        THUMBFILE_NOT_FOUND,
        UUID_FORMAT_ERROR,
        SQL_EXEC_FAILED
    };
    Sql();
    ~Sql();
    int AppendData(const QStringList& files,
             int width, int height,
             const QString& movieFile,
             const QString& format);

    bool hasThumb(const QString& file) const
    {
        Q_UNUSED(file);
        return false;
    }
};

#endif // SQL_H
