#include <QObject>
#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QFileInfo>
#include <QFileDevice>
#include <QDateTime>
#include <QSqlError>

#include "tableitemdata.h"
#include "helper.h"
#include "sql.h"

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

     // データを書き込む

     // データを出力する
     qDebug() << "records";
     query.exec("select * from test1");
     while (query.next()) {
         int id = query.value(0).toInt();
         QString name = query.value(1).toString();
         QString memo = query.value(2).toString();
         qDebug() << QString("id(%1),name(%2),memo(%3)").arg(id).arg(name).arg(memo);
     }
}
Sql::~Sql()
{
     db_.close();
}

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

    return true;
}

int Sql::AppendData(const QStringList& files,
         int width, int height,
         const QString& movieFile,
         const QString& format)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(format);
    QFileInfo mF(movieFile);
    if(!mF.exists())
        return MOVIEFILE_NOT_FOUND;

    qint64 size = mF.size();
    if(size <= 0)
        return FILESIZE_UNDERZERO;

    QString directory = mF.absolutePath();
    QString name = mF.fileName();

    QString salient = createSalient(movieFile, size);
    if(salient.isEmpty())
        return ERROR_CREATE_SALIENT;


    qint64 ctime = mF.fileTime(QFileDevice::FileBirthTime).toSecsSinceEpoch();
    qint64 wtime = mF.fileTime(QFileDevice::FileModificationTime).toSecsSinceEpoch();


    if(files.isEmpty())
        return THUMBFILE_NOT_FOUND;

    QString uuid = getUUID(files[0]);
    if(!isUUID(uuid))
        return UUID_FORMAT_ERROR;

    QSqlQuery query(db_);
    query.prepare("insert into FileInfo (size, ctime, wtime, directory, name, salient, thumbid) "
                  "values (?, ?, ?, ?, ?, ?, ?)");

    int i = 0;
    query.bindValue(i++, size);
    query.bindValue(i++, ctime);
    query.bindValue(i++, wtime);
    query.bindValue(i++, directory);
    query.bindValue(i++, name);
    query.bindValue(i++, salient);
    query.bindValue(i++, uuid);

    if(!query.exec())
    {
        qDebug() << query.lastError();
        return SQL_EXEC_FAILED;
    }
    return 0;
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


        TableItemData* pID = new TableItemData(thumbs,0,0,movieFile,"format");
        v.append(pID);
    }
    return true;
}
