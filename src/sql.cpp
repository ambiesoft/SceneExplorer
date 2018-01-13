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



Sql::Sql() : db_(QSqlDatabase::addDatabase("QSQLITE"))
{
     db_.setDatabaseName(DBFILENAME);
	 if (!db_.open())
		 return;

     QSqlQuery query;
     query.exec("create table FileInfo(size, ctime, wtime, directory, name, salient, thumbid)");
     query.exec("alter table FileInfo add duration");
     query.exec("alter table FileInfo add format");
     query.exec("alter table FileInfo add bitrate");
     query.exec("alter table FileInfo add vcodec");
     query.exec("alter table FileInfo add acodec");
     query.exec("alter table FileInfo add vwidth");
     query.exec("alter table FileInfo add vheight");

	 query.exec("CREATE INDEX idx_directory ON FileInfo(directory)");
	 query.exec("CREATE INDEX idx_name ON FileInfo(name)");
	 query.exec("CREATE INDEX idx_salient ON FileInfo(salient)");

#ifdef QT__DEBUG
     for (int i = 0; i < db_.tables().count(); i ++) {
         qDebug() << db_.tables().at(i);
     }
#endif
     
	 if (!query.exec("PRAGMA table_info('FileInfo')"))
		 return;

     while(query.next())
     {
         QString col=query.value("name").toString();
         allColumns_.append(col);
     }
	 qDebug() << allColumns_;
     ok_ = true;
}
Sql::~Sql()
{
    delete pQDeleteFromDirectoryName_;
    delete pQInsert_;
    delete pQGetInfo_;

    db_.close();
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
//    QSet<QString> ret;
//    for(const QString& s:allColumns_)
//    {
//       ret.insert(s);
//    }
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
//QString Sql::getAllColumnsUpdate(TableItemDataPointer tid)
//{
//    QString ret;
//    for(int i=0 ; i < allColumns_.count(); ++i)
//    {
//        ret += allColumns_[i];
//        ret += "=?";
//        if((i+1)!=allColumns_.count())
//        {
//            ret+=",";
//        }
//    }
//    return ret;
//}
QSqlQuery* Sql::getInsertQuery(TableItemDataPointer tid)
{
    static QStringList allcolumns;
    if(!pQInsert_)
    {
        pQInsert_=new QSqlQuery();
        QString preparing;
        allcolumns = getAllColumnNames();
        VERIFY(allcolumns.removeOne("directory"));
        VERIFY(allcolumns.removeOne("name"));

        preparing = "INSERT OR REPLACE INTO FileInfo (";

        for(int i=0 ; i < allcolumns.count(); ++i)
        {
            preparing += allcolumns[i];
            preparing += ",";
        }
        preparing += "directory,name ";

        preparing += ") VALUES ( ";

        for(int i=0 ; i < allcolumns.count(); ++i)
        {
            preparing += "?";
            preparing += ",";
        }
        preparing += "COALESCE((SELECT directory FROM FileInfo WHERE directory=? and name=?), ?),";
        preparing += "COALESCE((SELECT name FROM FileInfo WHERE directory=? and name=?), ?)";

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

    // first COALEASE
    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());
    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());
    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());

    // 2nd COALEASE
    pQInsert_->bindValue(bindIndex++, tid->getMovieDirectory());
    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());
    pQInsert_->bindValue(bindIndex++, tid->getMovieFileName());

    return pQInsert_;
}
int Sql::AppendData(TableItemDataPointer tid)
{


    Q_ASSERT(tid->getImageFiles().count()==5);
    if(tid->getImageFiles().isEmpty())
        return THUMBFILE_NOT_FOUND;

//    QString uuid = getUUID(tid.getImageFiles()[0]);
//    if(!isUUID(uuid))
//        return UUID_FORMAT_ERROR;

    QSqlQuery* pQInsert = getInsertQuery(tid);

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
int Sql::GetAllEntry(const QString& dir,
                     QStringList& entries,
                     QVector<qint64>& sizes,
                     QVector<qint64>& ctimes,
                     QVector<qint64>& wtimes,
                     QStringList& salients)
{
    QSqlQuery query(db_);
    if(!query.prepare("select name, size, ctime, wtime, salient from FileInfo where "
                  "directory=?"))
    {
        qDebug() << query.lastError();
        Q_ASSERT(false);
        return SQL_PREPARE_FAILED;
    }
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

//bool Sql::GetAll(QList<TableItemDataPointer>& v, const QString& dir)
//{
//    return GetAll(v, QStringList(dir));
//}
bool Sql::GetAll(QList<TableItemDataPointer>& v, const QStringList& dirs)
{
    QSqlQuery query(db_);
    if(dirs.isEmpty())
    {
        if(!query.exec("select * from FileInfo"))
        {
            Q_ASSERT(false);
            return false;
        }
    }
    else
    {
        bool ok = true;
        QString strPrepare("select * from FileInfo where ");
        for(int i=0 ; i < dirs.count(); ++i)
        {
            strPrepare += "directory like ?";
            if((i+1)!=dirs.count())
                strPrepare += " or ";
        }
        ok &= query.prepare(strPrepare);

        for(int i=0 ; i < dirs.count(); ++i)
        {
			Q_ASSERT(dirs[i].endsWith('/'));
            query.bindValue(i, dirs[i] + "%");
        }
        ok &= query.exec();
        if(!ok)
        {
            Q_ASSERT(false);
            return false;
        }
    }
    while (query.next()) {
        QString directory = query.value("directory").toString();
        if(directory.isEmpty())
            continue;
        QString name = query.value("name").toString();
        if(name.isEmpty())
            continue;
        QString movieFileFull = pathCombine(directory,name);

//        if(!QFile(movieFileFull).exists())
//             continue;

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
		TableItemDataPointer pID = TableItemData::Create(thumbs,
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
                                               vwidth,vheight);
        v.append(pID);
    }
    return true;
}

bool Sql::RenameEntry(const QString& oldDirc,
                      const QString& oldFile,
                      const QString& newDirc,
                      const QString& newFile)
{
    QString oldDir = canonicalDir(oldDirc);
    QString newDir = canonicalDir(newDirc);

    Q_ASSERT(!QFileInfo(pathCombine(oldDir,oldFile)).exists());
    Q_ASSERT(QFileInfo(pathCombine(newDir,newFile)).exists());

    if(!QFile(pathCombine(oldDir, oldFile)).exists())
    {
        QSqlQuery query;
        if(!query.prepare("update FileInfo "
                      "set directory=?,name=? "
                      "where directory=? and name=?"))
        {
            Q_ASSERT(false);
            return false;
        }
        int i=0;
        query.bindValue(i++, newDir);
        query.bindValue(i++, newFile);
        query.bindValue(i++, oldDir);
        query.bindValue(i++, oldFile);

        if(!query.exec())
        {
            Q_ASSERT(false);
            return false;
        }
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
            QSqlQuery query;
            if(!query.prepare("update FileInfo "
                          "set name=? "
                          "where directory=? and name=?"))
            {
                Q_ASSERT(false);
                return false;
            }
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
    QSqlQuery query;
    if(!query.prepare("select directory, name, size from FileInfo where "
                  "salient = ?"))
    {
        Q_ASSERT(false);
        return false;
    }
    query.bindValue(0, salient);
    if(!query.exec())
    {
        Q_ASSERT(false);
        return false;
    }

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

void showFatal(const QString& error)
{
    Alert(error);
}
#define SQC(siki) do { if(!(siki)) { Q_ASSERT(false); showFatal(query.lastError().text()); return false;}} while(false)
bool Sql::hasEntry(const QString& dir,
              const QString& file,
              const QString& sa)
{
    QSqlQuery query;

    SQC(query.prepare("select name from FileInfo where "
                  "directory=? and name=? and salient=?"));
    int i=0;
    query.bindValue(i++, dir);
    query.bindValue(i++, file);
    query.bindValue(i++, sa);

    SQC(query.exec());
    return query.next();
}
bool Sql::RemoveEntry(const QString& dir,
                 const QString& file)
{
    QSqlQuery& query = *getDeleteFromDirectoryName();

    int i = 0;

    query.bindValue(i++, dir);
    query.bindValue(i++, file);
    SQC(query.exec());
    return true;
}
