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

#include <QObject>
#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QFileInfo>
#include <QFileDevice>
#include <QDateTime>
#include <QSqlError>
#include <QSet>
#include <QMessageBox>
#include <QUuid>

#include "tableitemdata.h"
#include "helper.h"
#include "globals.h"
#include "sql.h"
#include "sqlcommon.h"


static void showFatal(const QString& error)
{
    Alert(nullptr, error);
}

#define SQC_BASE(Q,siki,RET) do { if(!( Q.siki )) { showFatal(lastError_=(Q).lastError().text()); Q_ASSERT(false); RET;}} while(false)
#define SQC(Q,siki) SQC_BASE(Q,siki,return false)
#define SQCN(Q,siki) SQC_BASE(Q,siki,return)
#define SQCI(Q,siki) SQC_BASE(Q,siki,return -1)

bool Sql::CreateDBInfoTable()
{
    QSqlQuery query;

    // Create DbInfo, insert database id
    query.exec("CREATE TABLE DbInfo("
               "id INTEGER PRIMARY KEY, "
               "dbid TEXT NOT NULL)");
    query.exec("ALTER TABLE DbInfo ADD COLUMN version INTEGER");

    if (!query.exec("PRAGMA table_info('DbInfo')"))
    {
        lastError_ = query.lastError().text();
        return false;
    }

    QStringList allColumnsMust;
    allColumnsMust << "id" << "dbid" << "version";

    QStringList allColumns;
    while(query.next())
    {
        QString col=query.value("name").toString();
        allColumns.append(col);
    }

    if(allColumnsMust.count() != allColumns.count())
    {
        return false;
    }

//    SQC(query,exec("SELECT * FROM DbInfo WHERE id=1"));
//    if(!query.next())
//    {
//        SQC(query,exec("INSERT INTO DbInfo (id) VALUES (1)"));
//    }
    return true;
}
int Sql::GetDBVersion()
{
    QSqlQuery query;
    SQCI(query,exec("SELECT version FROM DbInfo WHERE id=1"));
    if(!query.next())
        return -1;
    return query.value("version").toInt();
}
Sql::Sql() : db_(QSqlDatabase::addDatabase("QSQLITE"))
{
    db_.setDatabaseName(DBFILENAME);
    if (!db_.open())
    {
        lastError_ = db_.lastError().text();
        return;
    }

    if(!CreateDBInfoTable())
        return;
    int version = GetDBVersion();

    QSqlQuery query;

    query.exec("SELECT dbid FROM DbInfo WHERE id=1");
    query.next();
    dbid_ = query.value("dbid").toString();

    if(!isUUID(dbid_))
    {
        if(!dbid_.isEmpty())
        {
            if(!YesNo(nullptr,
                      tr("dbid is not null but incorrect. Do you want to override it with new id?"),
                      QMessageBox::Warning))
            {
                return;
            }
        }
        dbid_ = QUuid::createUuid().toString();
        dbid_ = dbid_.mid(1,dbid_.length()-2);
        if(!isUUID(dbid_))
            return;
        QString sql ="INSERT OR REPLACE INTO DbInfo (id, dbid) VALUES (1, '" + dbid_ + "')";
        if(!query.exec(sql))
        {
            qDebug() << query.lastError().text();
            lastError_ = query.lastError().text();
            return;
        }
    }

    query.exec("CREATE TABLE FileInfo( "
               "id INTEGER PRIMARY KEY,"
               "directory TEXT,"
               "name TEXT,"
               "size INT NOT NULL DEFAULT '0',"
               "ctime INT NOT NULL DEFAULT '0',"
               "wtime INT NOT NULL DEFAULT '0',"
               "salient TEXT,"
               "thumbid TEXT,"
               "duration INT NOT NULL DEFAULT '0',"
               "format TEXT,"
               "bitrate INT NOT NULL DEFAULT '0',"
               "vcodec TEXT,"
               "acodec TEXT,"
               "vwidth INT NOT NULL DEFAULT '0',"
               "vheight INT NOT NULL DEFAULT '0')"
               );
    qDebug() << query.lastError().text();
    //query.exec("ALTER TABLE FileInfo Add lastaccess");

    query.exec("CREATE INDEX idx_directory ON FileInfo(directory)");
    query.exec("CREATE INDEX idx_name ON FileInfo(name)");
    // make "INSERT OR REPLACE" to work
    query.exec("CREATE UNIQUE INDEX idx_directoryname ON FileInfo(directory,name)");
    query.exec("CREATE INDEX idx_salient ON FileInfo(salient)");
    // query.exec("CREATE INDEX idx_lastaccess ON FileInfo(lastaccess)");
//    query.exec("ALTER TABLE FileInfo Add opencount_tmp INT");
//    query.exec("ALTER TABLE FileInfo Add lastaccess_tmp INT");
    qDebug() << query.lastError().text();

#ifdef QT__DEBUG
    for (int i = 0; i < db_.tables().count(); i ++) {
        qDebug() << db_.tables().at(i);
    }
#endif

    if (!query.exec("PRAGMA table_info('FileInfo')"))
    {
        lastError_ = query.lastError().text();
        return;
    }

    while(query.next())
    {
        QString col=query.value("name").toString();
        if(!col.endsWith("_tmp"))
            allColumns_.append(col);
    }
    qDebug() << allColumns_;


    if(version != DBVERSION)
    {
        SQCN(query,prepare("UPDATE DbInfo SET version=?"));
        query.bindValue(0, DBVERSION);
        SQCN(query,exec());
    }
    ok_ = true;
}
Sql::~Sql()
{
    delete pQDeleteFromDirectoryName_;
    delete pQInsert_;
    delete pQGetInfo_;

    db_.close();
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

    ctime = fi.birthTime().toSecsSinceEpoch();
    wtime = fi.lastModified().toSecsSinceEpoch();

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
QStringList Sql::getAllColumnNames()
{
    return allColumns_;
}
QString Sql::getAllColumns(bool bBrace, bool bQ)
{
    QString ret;
    ret += " "; //safe space
    if(bBrace)
        ret.append("(");

    for(int i=0 ; i < allColumns_.count(); ++i)
    {
        if(bQ)
        {
            ret.append("?");
        }
        else
        {
            ret.append(allColumns_[i]);
        }
        if( (i+1) != allColumns_.count())
            ret.append(",");
    }

    if(bBrace)
        ret.append(")");

    ret += " "; //safe space
    return ret;
}

QSqlQuery* Sql::getInsertQuery(TableItemDataPointer tid)
{
    static QStringList allcolumns;
    if(!pQInsert_)
    {
        pQInsert_=new QSqlQuery();
        QString preparing;
        allcolumns = getAllColumnNames();

        VERIFY(allcolumns.removeOne("id"));
        VERIFY(allcolumns.removeOne("directory"));
        VERIFY(allcolumns.removeOne("name"));
        // VERIFY(allcolumns.removeOne("opencount"));

        preparing = "INSERT OR REPLACE INTO FileInfo (";

        for(int i=0 ; i < allcolumns.count(); ++i)
        {
            preparing += allcolumns[i];
            preparing += ",";
        }
        preparing += "id,directory,name ";

        preparing += ") VALUES ( ";

        for(int i=0 ; i < allcolumns.count(); ++i)
        {
            preparing += "?";
            preparing += ",";
        }

        preparing += "COALESCE((SELECT id FROM FileInfo WHERE directory=? and name=?),?),";
        preparing += "COALESCE((SELECT directory FROM FileInfo WHERE directory=? and name=?), ?),";
        preparing += "COALESCE((SELECT name FROM FileInfo WHERE directory=? and name=?), ?)";
        // preparing += "COALESCE((SELECT opencount FROM FileInfo WHERE directory=? and name=?), ?)";

        preparing+=")";



        //    QString preparing =
        //        QString("insert into FileInfo ") +
        //        getAllColumns(true,false) +
        //        "values "+
        //        getAllColumns(true,true);
        qDebug() << preparing;


        if(!pQInsert_->prepare(preparing))
        {
            qDebug() << pQInsert_->lastError() << preparing;
            Q_ASSERT(false);
            return nullptr;
        }
    }

    int bindIndex=0;

    QMap<QString,QVariant> allmap = tid->getColumnValues();
#ifdef QT_DEBUG
    for(const QString& s:allcolumns)
    {
        Q_ASSERT(allmap.contains(s));
    }
#endif

    for (const QString& c : allcolumns)
    {
        pQInsert_->bindValue(bindIndex++, allmap[c]);
    }

    // 1st COALEASE
    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());
    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());
    pQInsert_->bindValue(bindIndex++, tid->getIDVariant());

    // 2nd COALEASE
    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());
    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());
    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());

    // 3rd COALEASE
    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());
    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());
    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());

//    // 3rd COALEASE
//    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());
//    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());
//    pQInsert_->bindValue(bindIndex++, tid->getOpenCount());

    return pQInsert_;
}
qint64 Sql::AppendData(TableItemDataPointer tid)
{
    Q_ASSERT(tid->getImageFiles().count()==5);
    if(tid->getImageFiles().isEmpty())
        return THUMBFILE_NOT_FOUND;

    QSqlQuery* pQInsert = getInsertQuery(tid);

    if(!pQInsert->exec())
    {
        qDebug() << pQInsert->lastError();
        return SQL_EXEC_FAILED;
    }
    tid->setID(pQInsert->lastInsertId().toLongLong());
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
int Sql::GetAllEntry(const QString& dir,
                     QStringList& entries,
                     QVector<qint64>& sizes,
                     QVector<qint64>& ctimes,
                     QVector<qint64>& wtimes,
                     QStringList& salients)
{
    MYQMODIFIER QSqlQuery query("select name, size, ctime, wtime, salient from FileInfo where "
                           "directory=?",
                           db_);
    query.bindValue(0, dir);

    if(!query.exec())
    {
        return SQL_EXEC_FAILED;
    }

    while(query.next())
    {
        QString name = query.value("name").toString();
        entries.append(name);
        sizes.append(query.value("size").toLongLong());
        ctimes.append(query.value("ctime").toLongLong());
        wtimes.append(query.value("wtime").toLongLong());
        salients.append(query.value("salient").toString());
    }

    return 0;
}

QStringList GetThumbFilesFromThumbID(const QString& thumbID)
{
    if(!isUUID(thumbID))
        return QStringList();

    QStringList ret;
    for(int i=1 ; i <= 5 ; ++i)
    {
        QString t=thumbID;
        t+="-";
        t+=QString::number(i);
        t+=".";
        t+=GetThumbExt();

        t = pathCombine("thumbs", t);
        ret.append(t);
    }
    return ret;
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
            t+=".";
            t+=GetThumbExt();

            t = pathCombine("thumbs", t);
            if(!QFile(t).exists())
            {
                RemoveEntryFromThumbID(thumbid);
                return THUMBFILE_NOT_FOUND;
            }
        }

        Q_ASSERT(!pGetInfo->next());
        return THUMB_EXIST;
    }
    return THUMB_NOT_EXIST;
}
bool Sql::RemoveEntryThumb(const QString& dir,
                           const QString& name,
                           QString& removedThumbID)
{
    if(dir.isEmpty() || name.isEmpty())
    {
        Q_ASSERT(false);
        return false;
    }
    QSqlQuery query = myPrepare("SELECT thumbid FROM FileInfo WHERE "
                   "directory=? and name=?");

    int i=0;
    query.bindValue(i++, dir);
    query.bindValue(i++, name);

    SQC(query, exec());

    if(!query.next())
        return false;

    removedThumbID = query.value("thumbid").toString();
    if(!isUUID(removedThumbID))
    {
        Q_ASSERT(false);
        return false;
    }
    QStringList thumbfiles = GetThumbFilesFromThumbID(removedThumbID);
    Q_ASSERT(thumbfiles.count()==5);
    for(const QString& f : thumbfiles)
    {
        QFile(f).remove();
        Q_ASSERT(!QFile(f).exists());
    }
    return true;
}

int Sql::RemoveEntryFromThumbID(const QString& thumbid)
{
    if(!isUUID(thumbid))
        return THUMBID_IS_NOT_UUID;

    MYQMODIFIER QSqlQuery query("delete from FileInfo where "
                    "thumbid=?",
                    db_);

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

void AppendLitmiArg(QString& sql, const LimitArg& limit)
{
    if(!limit)
        return;

    sql += QString(" LIMIT %1, %2").arg(limit.GetOffset()).arg(limit.GetCount());
}
void AppendSortArg(QString& sql, const QStringList& sortbys, bool sortrev)
{
    if(sortbys.isEmpty())
        return;

    sql += " ORDER BY ";
    for(int i=0 ; i < sortbys.count(); ++i)
    {
        sql += sortbys[i];
        sql += " ";
        sql += sortrev ? "ASC" : "DESC";
        if((i+1) != sortbys.count())
            sql += ",";
    }
}

bool Sql::GetAllSqlString(
        QSqlQuery& query,
        const QStringList& selects,
        const QStringList& dirs,
        const QString& find,
        SORTCOLUMNMY sortcolumn,
        bool sortrev,
        const LimitArg& limit,
        const QSet<qint64>* tagids)
{
    QString sql = "SELECT ";
    for (int i = 0; i < selects.count(); ++i)
    {
        sql.append(selects[i]);

        if((i+1) != selects.count())
            sql.append(", ");
    }

    QStringList sortby;
    switch(sortcolumn)
    {
    case SORT_NONE:
        break;
    case SORT_FILENAME:
        sortby << "name";
        break;
    case SORT_FULLNAME:
        sortby << "directory" << "name";
        break;
    case SORT_SIZE:
        sortby << "size";
        break;
    case SORT_WTIME:
        sortby << "wtime";
        break;
    case SORT_RESOLUTION:
        sql += ", vwidth * vheight AS resolution ";
        sortby << "resolution";
        break;
    case SORT_DURATION:
        sortby << "duration";
        break;
    case SORT_BITRATE:
        sortby << "bitrate";
        break;
    case SORT_OPENCOUNT:
        sortby << "opencount";
        break;
    case SORT_LASTACCESS:
        sortby << "lastaccess";
        break;

    default:
        Q_ASSERT(false);
        break;
    }

    Q_ASSERT(!docdb_.isEmpty() && docdb_ != "nodb");
    sql += " FROM FileInfo LEFT JOIN " + docdb_ + ".Access ON FileInfo.id = " + docdb_ + ".Access.id " +
            "AND " + docdb_ + ".Access.dbid='" + gpSQL->getDbID() + "'";

    QVector<QVariant> binds;
    if(false)
    {
        // check dbid is same or NULL(joined)
        sql += " WHERE (" + docdb_ + ".Access.dbid='" + gpSQL->getDbID() + "' OR " + docdb_ +".Access.dbid IS NULL) ";
        if(!find.isEmpty())
        {
            sql += " AND name LIKE ?";
            binds.append("%"+find+"%");
        }

        if(tagids)
        {
            if(tagids->isEmpty())
            {
                sql += " AND 1=0 ";
            }
            else
            {
                sql += " AND (";
                for(int i=0 ; i < tagids->count(); ++i)
                {
                    sql += "FileInfo.id=?";
                    if( (i+1) != tagids->count())
                        sql += " or ";
                }
                sql += ")";
            }
        }
        sql += " AND 1=1";


        AppendSortArg(sql, sortby, sortrev);
        AppendLitmiArg(sql, limit);

        qDebug() << sql;
        SQC(query, prepare(sql));
        int bindIndex=0;
        for(int i=0 ; i < binds.count(); ++i)
        {
            query.bindValue(bindIndex++, binds[i]);
        }

        if(tagids && !tagids->isEmpty())
        {
            for( const qint64& tagid : *tagids)
            {
                query.bindValue(bindIndex++, tagid);
            }
        }
    }
    else
    {
        // check dbid is same or NULL(joined)
        // sql += " WHERE (" + docdb_ + ".Access.dbid='" + gpSQL->getDbID() + "' OR " + docdb_ +".Access.dbid IS NULL) AND (";
//        sql += QString(" WHERE (%1.Access.dbid='%2' OR %3.Access.dbid IS NULL) AND (").
//                arg(docdb_).arg(gpSQL->getDbID()).arg(docdb_);
        sql += " WHERE (";

        const int dircount = dirs.count();
        if(dircount != 0)
        {
            for(int i=0 ; i < dircount; ++i)
            {
                sql += "directory LIKE ?";
                if((i+1)!=dircount)
                    sql += " or ";
            }
        }
        else
        {
            sql += "1==1";
        }
        sql += ") ";

        if(!find.isEmpty())
            sql += " and name LIKE ?";

        if(tagids)
        {
            if(tagids->isEmpty())
            {
                sql += " AND 0=1 ";
            }
            else
            {
                sql += " AND (";
                for(int i=0 ; i < tagids->count(); ++i)
                {
                    sql += "FileInfo.id=?";
                    if( (i+1) != tagids->count())
                        sql += " or ";
                }
                sql += ")";
            }
        }

        AppendSortArg(sql, sortby, sortrev);
        AppendLitmiArg(sql, limit);
        qDebug() << sql;
        SQC(query, prepare(sql));

        int bindIndex;
        for(bindIndex=0 ; bindIndex < dirs.count(); ++bindIndex)
        {
            Q_ASSERT(dirs[bindIndex].endsWith('/'));
            query.bindValue(bindIndex, dirs[bindIndex] + "%");
        }
        if(!find.isEmpty())
            query.bindValue(bindIndex++, "%"+find+"%");

        if(tagids && !tagids->isEmpty())
        {
            for( const qint64& tagid : *tagids)
            {
                query.bindValue(bindIndex++, tagid);
            }
        }
    }

    return true;
}
qlonglong Sql::GetAllCount(const QStringList& dirs)
{
    QSqlQuery query(db_);
    GetAllSqlString(
                query,
                QStringList() << "Count(*) AS C",
                dirs,
                QString(),
                SORT_NONE,
                false,
                LimitArg(),
                nullptr);
    SQC(query, exec());
    while (query.next())
    {
        return query.value("C").toLongLong();
    }
    return 0;
}
bool Sql::GetAll(QList<TableItemDataPointer>& v,
                 const QStringList& dirs,
                 const QString& find,
                 bool bOnlyMissing,
                 SORTCOLUMNMY sortcolumn,
                 bool sortrev,
                 const LimitArg& limit,
                 const QSet<qint64>* tagids)
{
    QSqlQuery query(db_);
    GetAllSqlString(query,
                    QStringList() << "*",
                    dirs,
                    find,
                    sortcolumn,
                    sortrev,
                    limit,
                    tagids);

    SQC(query,exec());

    while (query.next())
    {
        qint64 id = query.value("id").toLongLong();

        QString directory = query.value("directory").toString();
        if(directory.isEmpty())
            continue;
        QString name = query.value("name").toString();
        if(name.isEmpty())
            continue;

        if(bOnlyMissing)
        {
            QString movieFileFull = pathCombine(directory,name);
            if(QFile(movieFileFull).exists())
                continue;
        }

        QString thumbid = query.value("thumbid").toString();
        QStringList thumbs;
        for(int i=1 ; i <= 5 ; ++i)
        {
            QString t=thumbid;
            t+="-";
            t+=QString::number(i);
            t+=".";
            t+=GetThumbExt();
            thumbs.append(t);
        }

        qint64 size = query.value("size").toLongLong();
        qint64 ctime = query.value("ctime").toLongLong();
        qint64 wtime = query.value("wtime").toLongLong();
        QString salitnet = query.value("salient").toString();
        double duration  = query.value("duration").toDouble();
        QString format = query.value("format").toString();
        int bitrate = query.value("bitrate").toInt();
        QString vcodec = query.value("vcodec").toString();
        QString acodec = query.value("acodec").toString();

        int vwidth = query.value("vwidth").toInt();
        int vheight = query.value("vheight").toInt();

        int opencount = query.value("opencount").toInt();
        qint64 lastaccess = query.value("lastaccess").toLongLong();
        TableItemDataPointer pID = TableItemData::Create(id,
                                                         thumbs,
                                                         directory,
                                                         name,

                                                         size,
                                                         ctime,
                                                         wtime,

                                                         0,0,
                                                         duration,
                                                         format,
                                                         bitrate,

                                                         vcodec,acodec,
                                                         vwidth,vheight,

                                                         opencount,
                                                         lastaccess);
        v.append(pID);
    }
    return true;
}

bool Sql::RenameEntry(const QString& oldDirc,
                      const QString& oldFile,
                      const QString& newDirc,
                      const QString& newFile)
{
    QString oldDir = normalizeDir(oldDirc);
    QString newDir = normalizeDir(newDirc);

    Q_ASSERT(!QFileInfo(pathCombine(oldDir,oldFile)).exists());
    Q_ASSERT(QFileInfo(pathCombine(newDir,newFile)).exists());

    if(!QFile(pathCombine(oldDir, oldFile)).exists())
    {
        MYQMODIFIER QSqlQuery query = myPrepare("update FileInfo "
                        "set directory=?,name=? "
                        "where directory=? and name=?");

        int i=0;
        query.bindValue(i++, newDir);
        query.bindValue(i++, newFile);
        query.bindValue(i++, oldDir);
        query.bindValue(i++, oldFile);

        SQC(query, exec());
    }
    return true;
}
bool Sql::RenameEntries(const QString& dir,
                        const QStringList& renameOlds,
                        const QStringList& renameNews)
{
    for(int i=0 ; i < renameOlds.count(); ++i)
    {
        // check old file not exists
        QString oldfile = renameOlds[i];
        QString newfile = renameNews[i];
        Q_ASSERT(!oldfile.isEmpty());
        Q_ASSERT(!newfile.isEmpty());
        Q_ASSERT(oldfile != newfile);
        Q_ASSERT(!QFile(pathCombine(dir, oldfile)).exists());
        if(!QFile(pathCombine(dir, oldfile)).exists())
        {
            MYQMODIFIER QSqlQuery query = myPrepare("update FileInfo "
                            "set name=? "
                            "where directory=? and name=?");
            int i=0;
            query.bindValue(i++, newfile);
            query.bindValue(i++, dir);
            query.bindValue(i++, oldfile);

            if(!query.exec())
            {
                Q_ASSERT(false);
                return false;
            }
        }
    }

    return true;
}

bool Sql::getEntryFromSalient(const QString& salient,
                              QStringList& dirsDB,
                              QStringList& filesDB,
                              QList<qint64>& sizesDB)
{
    MYQMODIFIER QSqlQuery query("select directory, name, size from FileInfo where "
                           "salient = ?");

    query.bindValue(0, salient);
    SQC(query, exec());

    while(query.next())
    {
        QString dir = query.value("directory").toString();
        Q_ASSERT(!dir.isEmpty());
        QString name = query.value("name").toString();
        Q_ASSERT(!name.isEmpty());
        qint64 size = query.value("size").toLongLong();

        dirsDB.append(dir);
        filesDB.append(name);
        sizesDB.append(size);
    }
    return true;
}


bool Sql::hasEntry(const QString& dir,
                   const QString& file,
                   const qint64& size,
                   const qint64& wtime,
                   const QString& sa)
{
    MYQMODIFIER QSqlQuery query = myPrepare("select name from FileInfo where "
                           "directory=? and name=? and size=? and wtime=? and salient=?");

    int i=0;
    query.bindValue(i++, dir);
    query.bindValue(i++, file);
    query.bindValue(i++, size);
    query.bindValue(i++, wtime);
    query.bindValue(i++, sa);

    SQC(query,exec());
    return query.next();
}
bool Sql::GetID(const QString& dir,
           const QString& file,
           qint64& id)
{
    MYQMODIFIER QSqlQuery query = myPrepare("SELECT id FROM FileInfo WHERE directory=? AND name=?");
    int i=0;
    query.bindValue(i++,dir);
    query.bindValue(i++,file);

    SQC(query,exec());
    if(!query.next())
    {
        id=-1;
        return true;
    }

    id = query.value("id").toLongLong();
    return true;
}

bool Sql::RemoveEntry(const QString& dir,
                      const QString& file,
                      QString* error)
{
    QSqlQuery& query = *getDeleteFromDirectoryName();

    int i = 0;

    query.bindValue(i++, dir);
    query.bindValue(i++, file);
    SQC(query,exec());
    if(1 != query.numRowsAffected())
    {
        if(error)
        {
            *error = QString(tr("numRowAffected is not 1 but %1.")).
                    arg(query.numRowsAffected());
        }
        return false;
    }
    return true;
}

bool Sql::RemoveAllMissingEntries(const QString& dirc)
{
    QSqlQuery query;

    if (dirc.isEmpty())
    {
        SQC(query,prepare("SELECT directory,name FROM FileInfo"));
    }
    else
    {
        QString dir = normalizeDir(dirc);
        SQC(query,prepare("SELECT directory,name FROM FileInfo WHERE directory LIKE ?"));

        query.bindValue(0, dir+"%");
    }

    SQC(query,exec());

    QSet<QPair<QString, QString> > dels;
    while (query.next())
    {
        QString d = query.value("directory").toString();
        QString n = query.value("name").toString();
        if (d.isEmpty() || n.isEmpty())
        {
            Q_ASSERT(false);
            continue;
        }
        QString fullpath = pathCombine(d, n);
        if (!QFileInfo(fullpath).exists())
        {
            dels.insert(qMakePair(d, n));
        }
    }


    for (QSet<QPair<QString, QString> >::iterator it = dels.begin(); it != dels.end(); ++it)
    {
        QString removed;
        VERIFY(RemoveEntryThumb(it->first, it->second, removed));
    }

    SQC(query, prepare("DELETE FROM FileInfo WHERE directory=? AND name=?"));
    for (QSet<QPair<QString, QString> >::iterator it = dels.begin(); it != dels.end(); ++it)
    {
        int i = 0;
        query.bindValue(i++, it->first);
        query.bindValue(i++, it->second);

        SQC(query, exec());
    }

    return true;
}
//bool Sql::ApplyOpenCount(const QMap<qint64,int>& opencounts)
//{
//    {
//        MYQMODIFIER QSqlQuery query("UPDATE FileInfo SET opencount_tmp=0");

//        // clear
//        query.exec();
//    }

//    for(const qint64& id : opencounts.keys())
//    {
//        MYQMODIFIER QSqlQuery query("UPDATE FileInfo SET opencount_tmp=? WHERE id=?");

//        int i=0;
//        query.bindValue(i++, opencounts[id]);
//        query.bindValue(i++, id);
//        SQC(query,exec());
//        Q_ASSERT(query.numRowsAffected()==1 || query.numRowsAffected()==0);
//    }
//    return true;
//}
//bool Sql::ApplyLastAccesses(const QMap<qint64,qint64>& lastaccesses)
//{
//    {
//        MYQMODIFIER QSqlQuery query("UPDATE FileInfo SET lastaccess_tmp=0");

//        // clear
//        query.exec();
//    }

//    for(const qint64& id : lastaccesses.keys())
//    {
//        MYQMODIFIER QSqlQuery query("UPDATE FileInfo SET lastaccess_tmp=? WHERE id=?");

//        int i=0;
//        query.bindValue(i++, lastaccesses[id]);
//        query.bindValue(i++, id);
//        SQC(query,exec());
//        Q_ASSERT(query.numRowsAffected()==1 || query.numRowsAffected()==0);
//    }
//    return true;
//}


bool Sql::AttachDocument(const QString& docFile)
{
    if(!docFile_.isEmpty())
    {
        DetachDocument();
    }
    Q_ASSERT(docFile_.isEmpty());
    Q_ASSERT(docdb_ == "nodb");

    static int docdbid = 0;
    ++docdbid;
    docdb_ = "docdb" + QString::number(docdbid);

    QSqlQuery query;
    SQC(query,prepare("ATTACH ? AS " + docdb_));

    query.bindValue(0, docFile);
    SQC(query,exec());
    docFile_=docFile;
    return true;
}
bool Sql::DetachDocument()
{
    QSqlQuery q;

    Q_ASSERT(!docdb_.isEmpty() && docdb_ != "nodb");
    if(!q.exec("DETACH " + docdb_))
    {
        Q_ASSERT(false);
    }
    docFile_ = QString();
    docdb_ = "nodb";
    return true;
}
