#include <QObject>
#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QFileInfo>
#include <QFileDevice>
#include <QDateTime>
#include <QSqlError>
#include <QSet>

#include "tableitemdata.h"
#include "helper.h"
#include "sql.h"

static QString getBytecode1(char c)
{
    QString ret;
    switch( (c & 0xF0)>>4 )
    {
    case 0: ret+="0";break;
    case 1: ret+="1";break;
    case 2: ret+="2";break;
    case 3: ret+="3";break;
    case 4: ret+="4";break;
    case 5: ret+="5";break;
    case 6: ret+="6";break;
    case 7: ret+="7";break;
    case 8: ret+="8";break;
    case 9: ret+="9";break;
    case 0xa: ret+="a";break;
    case 0xb: ret+="b";break;
    case 0xc: ret+="c";break;
    case 0xd: ret+="d";break;
    case 0xe: ret+="e";break;
    case 0xf: ret+="f";break;
default:
        Q_ASSERT(false);
    }
    switch(c & 0x0F)
    {
    case 0: ret+="0";break;
    case 1: ret+="1";break;
    case 2: ret+="2";break;
    case 3: ret+="3";break;
    case 4: ret+="4";break;
    case 5: ret+="5";break;
    case 6: ret+="6";break;
    case 7: ret+="7";break;
    case 8: ret+="8";break;
    case 9: ret+="9";break;
    case 0xa: ret+="a";break;
    case 0xb: ret+="b";break;
    case 0xc: ret+="c";break;
    case 0xd: ret+="d";break;
    case 0xe: ret+="e";break;
    case 0xf: ret+="f";break;
default:
        Q_ASSERT(false);
    }

    Q_ASSERT(ret.size()==2);
    return ret;
}
static QString getBytecode(const char* pByte, int size)
{
    QString ret;
    for(int i=0; i< size; ++i)
    {
        ret += getBytecode1(pByte[i]);
    }
    return ret;
}


Sql::Sql() : db_(QSqlDatabase::addDatabase("QSQLITE"))
{
    // データベースを開く。無ければ作る

     db_.setDatabaseName("./db.sqlite3");
     db_.open();
     qDebug() << "tables 1";
     for (int i = 0; i < db_.tables().count(); i ++) {
         qDebug() << db_.tables().at(i);
     }

     // テーブルを作る。すでに有れば何もしない
     QSqlQuery query;
     query.exec("create table FileInfo(size, ctime, wtime, directory, name, salient, thumbid)");
     qDebug() << "tables 2";
     for (int i = 0; i < db_.tables().count(); i ++) {
         qDebug() << db_.tables().at(i);
     }
}
Sql::~Sql()
{
    delete pQDeleteFromDirectoryName_;
    delete pQInsert_;
    delete pQGetInfo_;

    db_.close();
}

static QString createSalient(const QString& file, const qint64& size)
{
    QFile f(file);
    if(!f.open(QIODevice::ReadOnly))
        return QString();

    qint64 readed=0;
    char buff[20]={0};
    readed=f.read(buff, 20);
    if(readed <= 0)
        return QString();

    QString ret = getBytecode(buff, readed);

    if(size <= 20)
        return ret;

    if(!f.seek(size-20))
        return QString();
    readed=f.read(buff, 20);
    if(readed != 20)
        return QString();
    ret += "-";
    ret += getBytecode(buff, 20);

    if(size <= 40)
        return ret;

    if(!f.seek(size/3))
        return QString();
    readed=f.read(buff, 20);
    if(readed != 20)
        return QString();
    ret += "-";
    ret += getBytecode(buff, 20);

    if(!f.seek( size/3*2))
        return QString();
    readed=f.read(buff, 20);
    ret += "-";
    ret += getBytecode(buff, 20);

    return ret;
}
static QString getUUID(const QString& file)
{
    int i = file.lastIndexOf('.');
    if(i < 0)
        return QString();
    QString ret = file.left(i);
    if(ret.length() < 2)
        return QString();
    ret = ret.left(ret.length()-2);
    return ret;
}
static bool isUUID(const QString& s)
{
    if(s.isEmpty())
        return false;
    if(s.length()!=36)
        return false;
    return true;
}

int Sql::GetMovieFileInfo(const QString& movieFile,
                     bool& exist,
                     qint64& size,
                     QString& directory,
                     QString& name,
                     QString* salient,
                     qint64& ctime,
                     qint64& wtime) const
{
    QFileInfo fi(movieFile);
    exist = fi.exists();
    if(!exist)
        return MOVIEFILE_NOT_FOUND;

    size = fi.size();
    if(size <= 0)
        return FILESIZE_UNDERZERO;

    directory = fi.absolutePath();
    name = fi.fileName();

    ctime = fi.fileTime(QFileDevice::FileBirthTime).toSecsSinceEpoch();
    wtime = fi.fileTime(QFileDevice::FileModificationTime).toSecsSinceEpoch();

    if(salient)
    {
        *salient = createSalient(movieFile, size);
        if(salient->isEmpty())
            return ERROR_CREATE_SALIENT;
    }
    return 0;
}

QSqlQuery* Sql::getDeleteFromDirectoryName()
{
    if(pQDeleteFromDirectoryName_)
        return pQDeleteFromDirectoryName_;

    pQDeleteFromDirectoryName_ = new QSqlQuery(db_);
    if(!pQDeleteFromDirectoryName_->prepare("delete from FileInfo where "
                  "directory=? and name=?"))
    {
        qDebug() << pQDeleteFromDirectoryName_->lastError();
        Q_ASSERT(false);
        return nullptr;
    }
    return pQDeleteFromDirectoryName_;
}
QSqlQuery* Sql::getInsertQuery()
{
    if(pQInsert_)
        return pQInsert_;
    pQInsert_=new QSqlQuery(db_);

    if(!pQInsert_->prepare("insert into FileInfo (size, ctime, wtime, directory, name, salient, thumbid) "
                  "values (?, ?, ?, ?, ?, ?, ?)"))
    {
        qDebug() << pQInsert_->lastError();
        Q_ASSERT(false);
        return nullptr;
    }
    return pQInsert_;
}
int Sql::AppendData(const QStringList& files,
         int width, int height,
         const QString& movieFile,
         const QString& format)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(format);

    bool exist;
    qint64 size;
    QString directory;
    QString name;
    QString salient;
    qint64 ctime;
    qint64 wtime;
    int ret = GetMovieFileInfo(
                     movieFile,
                     exist,
                     size,
                     directory,
                     name,
                     &salient,
                     ctime,
                     wtime);
    if(ret != 0)
        return ret;

    if(files.isEmpty())
        return THUMBFILE_NOT_FOUND;

    QString uuid = getUUID(files[0]);
    if(!isUUID(uuid))
        return UUID_FORMAT_ERROR;

    {
        QSqlQuery* pQuery = getDeleteFromDirectoryName();

        int i = 0;
        pQuery->bindValue(i++, directory);
        pQuery->bindValue(i++, name);
        if(!pQuery->exec())
        {
            qDebug() << pQuery->lastError();
            return SQL_EXEC_FAILED;
        }
    }

    QSqlQuery* pQInsert = getInsertQuery();
    int i = 0;
    pQInsert->bindValue(i++, size);
    pQInsert->bindValue(i++, ctime);
    pQInsert->bindValue(i++, wtime);
    pQInsert->bindValue(i++, directory);
    pQInsert->bindValue(i++, name);
    pQInsert->bindValue(i++, salient);
    pQInsert->bindValue(i++, uuid);

    if(!pQInsert->exec())
    {
        qDebug() << pQInsert->lastError();
        return SQL_EXEC_FAILED;
    }
    return 0;
}
QSqlQuery* Sql::getGetInfoQuery()
{
    if(pQGetInfo_)
        return pQGetInfo_;
    pQGetInfo_=new QSqlQuery(db_);

    if(!pQGetInfo_->prepare("select * from FileInfo where "
                   "size=? and directory=? and name=? and salient=? and ctime=? and wtime=?"))
    {
        qDebug() << pQGetInfo_->lastError();
        Q_ASSERT(false);
        return nullptr;
    }
    return pQGetInfo_;
}

bool Sql::IsSameFile(const QString& dir,
                const QString& name,
                const qint64& size,
                const QString& salient)
{
    bool exist2;
    qint64 size2;
    QString directory2;
    QString name2;
    QString salient2;
    qint64 ctime2;
    qint64 wtime2;
    int ret = GetMovieFileInfo(
                     pathCombine(dir,name),
                     exist2,
                     size2,
                     directory2,
                     name2,
                     nullptr,
                     ctime2,
                     wtime2);
    if(ret != 0)
        return false;

    if(size != size2)
        return false;

    // need to check salient
    GetMovieFileInfo(
                     pathCombine(dir,name),
                     exist2,
                     size2,
                     directory2,
                     name2,
                     &salient2,
                     ctime2,
                     wtime2);
    if(salient != salient2)
        return false;

    return true;
}
int Sql::filterWithEntry(const QString& movieDir,
                         const QStringList& movieFiles,
                         QStringList& results)
{
    if(movieFiles.isEmpty())
        return 0;

    QSet<QString> sets;
    for(int i=0 ; i < movieFiles.count(); ++i)
        sets.insert(movieFiles[i]);

    QSqlQuery query(db_);
    if(!query.prepare("select size,name,salient from FileInfo where "
                  "directory=?"))
    {
        qDebug() << pQGetInfo_->lastError();
        Q_ASSERT(false);
        return SQL_PREPARE_FAILED;
    }
    query.bindValue(0, movieDir);

    if(!query.exec())
    {
        return SQL_EXEC_FAILED;
    }

    while(query.next())
    {
        qint64 size = query.value("size").toLongLong();
        QString name = query.value("name").toString();
        QString salient = query.value("salient").toString();

        if(sets.contains(name) && IsSameFile(movieDir,name,size,salient))
        {
            sets.remove(name);
        }
    }

    QSet<QString>::iterator it;
    for (it = sets.begin(); it != sets.end(); ++it)
        results.append(*it);
    return 0;
}
int Sql::hasThumb(const QString& movieFile)
{
    bool exist;
    qint64 size;
    QString directory;
    QString name;
    QString salient;
    qint64 ctime;
    qint64 wtime;
    int ret = GetMovieFileInfo(
                     movieFile,
                     exist,
                     size,
                     directory,
                     name,
                     &salient,
                     ctime,
                     wtime);
    if(ret != 0)
        return ret;

    QSqlQuery* pGetInfo = getGetInfoQuery();
    int i = 0;
    pGetInfo->bindValue(i++, size);
    pGetInfo->bindValue(i++, directory);
    pGetInfo->bindValue(i++, name);
    pGetInfo->bindValue(i++, salient);
    pGetInfo->bindValue(i++, ctime);
    pGetInfo->bindValue(i++, wtime);
    if(!pGetInfo->exec())
    {
        qDebug() << pGetInfo->lastError();
        return SQL_EXEC_FAILED;
    }
    while (pGetInfo->next())
    {
        QString thumbid = pGetInfo->value("thumbid").toString();
        QStringList thumbs;
        for(int i=1 ; i <= 5 ; ++i)
        {
            QString t=thumbid;
            t+="-";
            t+=QString::number(i);
            t+=".png";

            t = pathCombine("thumbs", t);
            if(!QFile(t).exists())
            {
                removeEntry(thumbid);
                return THUMBFILE_NOT_FOUND;
            }
        }

        Q_ASSERT(!pGetInfo->next());
        return THUMB_EXIST;
    }
    return THUMB_NOT_EXIST;
}
int Sql::removeEntry(const QString& thumbid)
{
    if(!isUUID(thumbid))
        return THUMBID_IS_NOT_UUID;

    QSqlQuery query(db_);

    if(!query.prepare("delete from FileInfo where "
                   "thumbid=?"))
    {
        qDebug() << query.lastError();
        return SQL_EXEC_FAILED;
    }
    int i = 0;
    query.bindValue(i++, thumbid);
    if(!query.exec())
    {
        qDebug() << query.lastError();
        return SQL_EXEC_FAILED;
    }
    return 0;
}
QString Sql::getErrorStrig(int thumbRet)
{
    switch(thumbRet)
    {
    case NO_ERROR: return tr("No Error");
    case MOVIEFILE_NOT_FOUND: return tr("Video file not found.");
    case FILESIZE_UNDERZERO: return tr("File size is under 0.");
    case ERROR_CREATE_SALIENT: return tr("Failed to create Salient.");
    case THUMBFILE_NOT_FOUND: return tr("Thumbnail file(s) not found.");
    case UUID_FORMAT_ERROR: return tr("UUID format error.");
    case SQL_EXEC_FAILED: return tr("Sql failed.");
    case THUMB_EXIST: return tr("Thumb exists.");
    case THUMB_NOT_EXIST: return tr("Thumb not exists.");
    }
    Q_ASSERT(false);
    return QString();
}
bool Sql::GetAll(QList<TableItemData*>& v)
{
    QSqlQuery query(db_);
    if(!query.exec("select * from FileInfo"))
        return false;
    while (query.next()) {
        QString thumbid = query.value("thumbid").toString();
        QStringList thumbs;
        for(int i=1 ; i <= 5 ; ++i)
        {
            QString t=thumbid;
            t+="-";
            t+=QString::number(i);
            t+=".png";
            thumbs.append(t);
        }
        QString movieFile = pathCombine(query.value("directory").toString(),
                                        query.value("name").toString());

        if(QFile(movieFile).exists())
        {
            TableItemData* pID = new TableItemData(thumbs,0,0,movieFile,"format");
            v.append(pID);
        }
    }
    return true;
}
