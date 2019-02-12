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


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

#include "../../lsMisc/stdQt/stdQt.h"

#include "sqlcommon.h"
#include "tableitemdata.h"
#include "helper.h"
#include "sql.h"
#include "directoryitem.h"

#include "documentsql.h"

using namespace AmbiesoftQt;

static void showFatal(const QString& error)
{
    Alert(nullptr, error);
}

#define SQC_BASE(Q,siki,RET) do { if(!( Q.siki )) { showFatal(lastError_=(Q).lastError().text()); Q_ASSERT(false); RET;}} while(false)
#define SQC(Q,siki) SQC_BASE(Q,siki,return false)
#define SQCN(Q,siki) SQC_BASE(Q,siki,return)
#define SQCI(Q,siki) SQC_BASE(Q,siki,return -1)

static QString getDBConnection()
{
    static int i=0;
    ++i;
    return "DOC" + QString::number(i);
}
static QString docdb(const QString& s)
{
    return gpSQL->docdb() + "." + s;
}
bool DocumentSql::CreateDBInfoTableDoc(QSqlDatabase& db)
{
    QSqlQuery query(db);

    // Create DbInfo, insert database id
    query.exec("CREATE TABLE DbInfoDoc( "
               "id INTEGER PRIMARY KEY)");
    query.exec("ALTER TABLE DbInfoDoc ADD COLUMN version INTEGER");

    if (!query.exec("PRAGMA table_info('DbInfoDoc')"))
    {
        lastError_ = query.lastError().text();
        return false;
    }

    QStringList allColumnsMust;
    allColumnsMust << "id" << "version";

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

    SQC(query,exec("SELECT * FROM DbInfoDoc WHERE id=1"));
    if(!query.next())
    {
        SQC(query,exec("INSERT INTO DbInfoDoc (id) VALUES (1)"));
    }
    return true;
}
int DocumentSql::GetDBVersionDoc(QSqlDatabase& db)
{
    QSqlQuery query(db);
    SQCI(query,exec("SELECT version FROM DbInfoDoc WHERE id=1"));
    if(!query.next())
        return -1;
    return query.value("version").toInt();
}
DocumentSql::DocumentSql(const QString& file) :
    ok_(false)
{
    {
        QSqlDatabase db(QSqlDatabase::addDatabase("QSQLITE",getDBConnection()));


        qDebug() << db.databaseName();

        db.setDatabaseName(file);
        if (!db.open() || !db.isOpen() || !db.isValid())
        {
            lastError_ = db.lastError().text();
            return;
        }

        if(!CreateDBInfoTableDoc(db))
            return;
        int version = GetDBVersionDoc(db);

        QSqlQuery q(db);
        q.exec("CREATE TABLE Settings ( "
               "id INTEGER NOT NULL PRIMARY KEY,"
               "allselected INT NOT NULL DEFAULT '0',"
               "allchecked INT NOT NULL DEFAULT '0',"
               "lastrow INT NOT NULL DEFAULT '0',"
               "lastcolumn INT NOT NULL DEFAULT '0'"
               ")"
               );
        qDebug() << q.lastError().text();
        q.exec("ALTER TABLE Settings ADD COLUMN alltagselected INT NOT NULL DEFAULT '0'");
        qDebug() << q.lastError().text();
        q.exec("ALTER TABLE Settings ADD COLUMN notagstagselected INT NOT NULL DEFAULT '0'");
        qDebug() << q.lastError().text();
        {
            // Create record 1
            q.exec("SELECT id FROM Settings WHERE id=1");
            q.exec();
            qDebug() << q.lastError().text();
            if(!q.next())
            {
                q.exec("INSERT INTO Settings (id) VALUES (1)");
                qDebug() << q.lastError().text();
            }
        }

        q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Settings';");
        q.next();
        if("Settings" != q.value("name").toString())
        {
            lastError_ = tr("Table Settings could not be created.");
            return;
        }



        q.exec("CREATE TABLE Directories ( "
               "id INTEGER NOT NULL PRIMARY KEY,"
               "directory TEXT,"
               "selected INT,"
               "checked INT)"
               );
        qDebug() << q.lastError().text();

        q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Directories';");
        q.next();
        if("Directories" != q.value("name").toString())
        {
            lastError_ = tr("Table Directories could not be created.");
            return;
        }



        q.exec("CREATE TABLE Access ( "
               "id INTEGER NOT NULL,"
               "opencount INT NOT NULL DEFAULT 0,"
               "lastaccess INT,"
               "dbid TEXT NOT NULL)"
               );
        qDebug() << q.lastError().text();
        q.exec("CREATE UNIQUE INDEX idx_Access_id_dbid ON Access(id,dbid)");
        qDebug() << q.lastError().text();
        q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Access';");
        q.next();
        if("Access" != q.value("name").toString())
        {
            lastError_ = tr("Table Access could not be created.");
            return;
        }


        q.exec("CREATE TABLE Tag ( "
               "tagid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "tag,"
               "yomi,"
               "dbid TEXT NOT NULL)"
               );
        qDebug() << q.lastError().text();
        q.exec("CREATE UNIQUE INDEX idx_Tag_tagid_dbid ON Tag(tagid,dbid)");
        qDebug() << q.lastError().text();
        q.exec("ALTER TABLE Tag ADD COLUMN selected INT NOT NULL DEFAULT '0'");
        q.exec("ALTER TABLE Tag ADD COLUMN checked INT NOT NULL DEFAULT '0'");

        q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Tag';");
        q.next();
        if("Tag" != q.value("name").toString())
        {
            lastError_ = tr("Table Tag could not be created.");
            return;
        }

        q.exec("CREATE TABLE Tagged ( "
               "id INTEGER NOT NULL,"
               "tagid INTEGER NOT NULL,"
               "dbid TEXT NOT NULL)"
               );
        qDebug() << q.lastError().text();
        q.exec("CREATE UNIQUE INDEX idx_Tagged_id_tagid_dbid ON Tagged(id,tagid,dbid)");
        qDebug() << q.lastError().text();
        q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Tagged';");
        q.next();
        if("Tagged" != q.value("name").toString())
        {
            lastError_ = tr("Table Tagged could not be created.");
            return;
        }

        if(version != DBVERSIONDOC)
        {
            SQCN(q,prepare("UPDATE DbInfoDoc SET version=?"));
            q.bindValue(0, DBVERSIONDOC);
            SQCN(q,exec());
        }
    }
    if(!gpSQL->AttachDocument(file))
    {
        lastError_ = gpSQL->lastError();
        return;
    }

    ok_ = true;
}
DocumentSql::~DocumentSql()
{
    if(!ok_)
        return;
    gpSQL->DetachDocument();
}

bool DocumentSql::isAllSelectedCommon(const QString& col) const
{
    QSqlQuery query("SELECT " + col + " FROM " + docdb("Settings") + " WHERE id=1");

    SQC(query,exec());
    if(!query.next())
        return false;
    bool ok;
    bool ret = query.value(col).toInt(&ok) != 0;
    if(!ok)
        return false;
    return ret;
}

bool DocumentSql::isDirAllSelected() const
{
    return isAllSelectedCommon("allselected");
}
bool DocumentSql::isTagAllSelected() const
{
    return isAllSelectedCommon("alltagselected");
}
bool DocumentSql::isTagNotagsSelected() const
{
    return isAllSelectedCommon("notagstagselected");
}

bool DocumentSql::setAllSelectedCommon(const QString& col, bool b)
{
    QSqlQuery query("UPDATE " + docdb("Settings") + " SET " + col + "=? WHERE id=1");

    query.bindValue(0, b ? 1 : 0);
    SQC(query,exec());
    return true;
}
bool DocumentSql::setDirAllSelected(bool b)
{
    return setAllSelectedCommon("allselected",b);
}
bool DocumentSql::setTagAllSelected(bool b)
{
    return setAllSelectedCommon("alltagselected",b);
}
bool DocumentSql::setTagNotagsSelected(bool b)
{
    return setAllSelectedCommon("notagstagselected",b);
}



bool DocumentSql::setDirNormalItemState(const DirectoryItem* item)
{
    QSqlQuery query = myPrepare("UPDATE " + docdb("Directories") + " SET selected=?, checked=? WHERE id=?");
    int i=0;
    query.bindValue(i++, item->IsSelectedInt());
    query.bindValue(i++, item->IsCheckedInt());
    query.bindValue(i++, item->dirid());
    SQC(query,exec());
    qDebug() << query.numRowsAffected();
    return true;
}
bool DocumentSql::isDirAllChecked() const
{
    MYQMODIFIER QSqlQuery query("SELECT allchecked FROM " + docdb("Settings") + " WHERE id=1");
    SQC(query,exec());
    if(!query.next())
        return false;
    bool ok;
    bool ret = query.value("allchecked").toInt(&ok) != 0;
    if(!ok)
        return false;
    return ret;
}
bool DocumentSql::setDirAllChecked(bool b)
{
    MYQMODIFIER QSqlQuery query("UPDATE " + docdb("Settings") + " SET allchecked=? WHERE id=1");

    query.bindValue(0, b ? 1 : 0);
    SQC(query, exec());
    return true;
}


int DocumentSql::dirCount() const
{
    MYQMODIFIER QSqlQuery query("SELECT count(*) FROM " + docdb("Directories"));
    SQC(query,exec());
    if(!query.next())
        return 0;
    return query.value(0).toInt();
}
QString DocumentSql::getDirText(int index) const
{
    MYQMODIFIER QSqlQuery query("SELECT directory FROM " + docdb("Directories") + " WHERE id=?");

    query.bindValue(0, index);
    if(!query.exec())
        return QString();
    if(!query.next())
        return QString();
    return query.value(0).toString();
}
bool DocumentSql::setDirectory(int index, DirectoryItem* di)
{
    MYQMODIFIER QSqlQuery query = myPrepare("INSERT OR REPLACE INTO " + docdb("Directories") + " (id, directory, selected, checked) VALUES (?,?,?,?)");

    int i = 0;
    query.bindValue(i++, index);
    query.bindValue(i++, di->text());
    query.bindValue(i++, di->isSelected() ? 1 : 0);
    query.bindValue(i++, di->IsChecked() ? 1 : 0);
    SQC(query, exec());
    return true;
}

bool DocumentSql::isDirSelected(int index) const
{
    MYQMODIFIER QSqlQuery query("SELECT selected FROM " + docdb("Directories") + " WHERE id=?");

    query.bindValue(0, index);
    SQC(query, exec());
    if (!query.next())
        return false;
    return query.value(0).toInt() != 0;
}

bool DocumentSql::isDirChecked(int index) const
{
    MYQMODIFIER QSqlQuery query("SELECT checked FROM " + docdb("Directories") + " WHERE id=?");

    query.bindValue(0, index);
    SQC(query, exec());
    if(!query.next())
        return false;
    return query.value(0).toInt() != 0;
}

bool DocumentSql::removeDirectoryOver(int index)
{
    MYQMODIFIER QSqlQuery query("DELETE FROM " + docdb("Directories") + " WHERE id > ?");

    query.bindValue(0, index);
    SQC(query, exec());
    return true;
}

bool DocumentSql::SetLastPos(int row, int column)
{
    // this caused sigsegv in linux
    // QSqlQuery query("UPDATE Settings SET lastrow=?, lastcolumn=? WHERE id=1");
    QSqlQuery query;
    SQC(query,prepare("UPDATE " + docdb("Settings") + " SET lastrow=?, lastcolumn=? WHERE id=1"));

    int i=0;
    query.bindValue(i++, row);
    query.bindValue(i++, column);
    SQC(query,exec());

    return true;
}
bool DocumentSql::GetLastPos(int& row, int& column) const
{
    MYQMODIFIER QSqlQuery query("SELECT * FROM " + docdb("Settings") + " WHERE id=1");
    SQC(query,exec());
    if(!query.next())
        return false;
    bool ok;
    row = query.value("lastrow").toInt(&ok);
    if(!ok)
        return false;
    column = query.value("lastcolumn").toInt(&ok);
    if(!ok)
        return false;

    return true;
}

bool DocumentSql::IncrementOpenCountAndLastAccess(const qint64& id)
{
    MYQMODIFIER QString state =
            "REPLACE into " + docdb("Access") + " (id,opencount,lastaccess,dbid) VALUES "
                                                "(?,"
                                                "COALESCE((SELECT opencount FROM " + docdb("Access") + " WHERE id=? AND dbid=?),0)+1,"
                                                                                                       "?,"
                                                                                                       "?)";

    MYQMODIFIER QSqlQuery query = myPrepare(state);

    int i=0;
    query.bindValue(i++, id);
    query.bindValue(i++, id);
    query.bindValue(i++, gpSQL->getDbID());
    query.bindValue(i++, QDateTime::currentSecsSinceEpoch());
    query.bindValue(i++, gpSQL->getDbID());

    SQC(query,exec());
    Q_ASSERT(query.numRowsAffected() == 1);

    return true;
}

bool DocumentSql::setOpenCountAndLascAccess_obsolete(const QList<TableItemDataPointer>& all)
{
    MYQMODIFIER QSqlQuery query("SELECT id,opencount,lastaccess,dbid FROM " + docdb("Access") + " WHERE dbid=?");

    query.bindValue(0, gpSQL->getDbID());
    SQC(query,exec());
    QMap<qint64, int> mapOpenCount;
    QMap<qint64, qint64> mapLastAccess;
    while(query.next())
    {
        mapOpenCount.insert(query.value("id").toLongLong(), query.value("opencount").toInt());
        mapLastAccess.insert(query.value("id").toLongLong(), query.value("lastaccess").toLongLong());
    }

    for(TableItemDataPointer td : all)
    {
        if(mapOpenCount.contains(td->getID()))
        {
            td->setOpenCount(mapOpenCount[td->getID()]);
        }

        if(mapLastAccess.contains(td->getID()))
        {
            td->setLastAccess(mapLastAccess[td->getID()]);
        }
    }

    return true;
}

#include <tagitem.h>
bool DocumentSql::GetAllTags(QList<TagItem*>& tags,bool bHasParent) const
{
    MYQMODIFIER QSqlQuery query("SELECT * FROM " + docdb("Tag") + " WHERE dbid=? ORDER BY yomi");

    int i=0;
    query.bindValue(i++, gpSQL->getDbID());
    SQC(query,exec());
    while(query.next())
    {
        TagItem* ti = new TagItem(bHasParent,
                                  query.value("tagid").toLongLong(),
                                  TagItem::TI_NORMAL,
                                  query.value("tag").toString(),
                                  query.value("yomi").toString());

        tags.append(ti);
    }
    return true;
}
bool DocumentSql::IsTagExist(const QString& tag) const
{
    MYQMODIFIER QSqlQuery query = myPrepare("SELECT tagid FROM " + docdb("Tag") + " WHERE tag=? AND dbid=?");

    int i=0;
    query.bindValue(i++, tag);
    query.bindValue(i++, gpSQL->getDbID());
    SQC(query,exec());
    return query.next();
}
bool DocumentSql::Insert(const QString& tag, const QString& yomi, qint64& insertedTag) const
{
    MYQMODIFIER QSqlQuery query = myPrepare("REPLACE INTO " + docdb("Tag") + " (tag,yomi,dbid) VALUES (?,?,?)");

    insertedTag = -1;
    int i=0;
    // query.bindValue(i++, id);
    query.bindValue(i++,tag);
    query.bindValue(i++,yomi);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());
    insertedTag = query.lastInsertId().toLongLong();
    return true;
}

bool DocumentSql::GetTaggedIDs(const QList<qint64>& tagids, QSet<qint64>& taggedids) const
{
    if(tagids.isEmpty())
    {
        taggedids.clear();
        return true;
    }
    QString sql = "SELECT id FROM " + docdb("Tagged") + " WHERE 1==1 AND ";
    for(int i=0 ; i < tagids.count(); ++i)
    {
        if(i==0)
            sql += "(";
        sql += "tagid=? ";
        if((i+1)!=tagids.count())
            sql += " OR ";
        else
            sql += ") ";
    }
    sql += " AND dbid=?";

    QSqlQuery query;
    SQC(query,prepare(sql));

    int bindIndex=0;
    for(int i=0; i < tagids.count(); ++i)
    {
        query.bindValue(bindIndex++, tagids[i]);
    }
    query.bindValue(bindIndex++,gpSQL->getDbID());

    SQC(query,exec());
    while(query.next())
    {
        taggedids.insert(query.value("id").toLongLong());
    }
    return true;
}
bool DocumentSql::SetTagged(const qint64& id, const qint64& tagid, const bool bSet) const
{
    if(tagid <= 0 || id <= 0)
    {
        Alert(nullptr,tr("Tagid or ID is below 0."));
        return false;
    }


    if (bSet)
    {
        MYQMODIFIER QSqlQuery query = myPrepare("REPLACE INTO " + docdb("Tagged") + " (id,tagid,dbid) VALUES (?,?,?)");

        int i = 0;
        query.bindValue(i++, id);
        query.bindValue(i++, tagid);
        query.bindValue(i++, gpSQL->getDbID());

        SQC(query, exec());
    }
    else
    {
        QSqlQuery query = myPrepare("DELETE FROM " + docdb("Tagged") + " WHERE id=? AND tagid=? AND dbid=?");

        int i = 0;
        query.bindValue(i++, id);
        query.bindValue(i++, tagid);
        query.bindValue(i++, gpSQL->getDbID());
        SQC(query, exec());
    }
    return true;
}
bool DocumentSql::GetTag(const qint64& tagid, QString& tag, QString& yomi) const
{
    MYQMODIFIER QSqlQuery query = myPrepare("SELECT tag,yomi FROM " + docdb("Tag") + " WHERE tagid=? AND dbid=?");
    
    int i=0;
    query.bindValue(i++, tagid);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());
    if(!query.next())
    {
        tag=yomi=QString();
        return true;
    }
    tag = query.value("tag").toString();
    yomi = query.value("yomi").toString();

    return true;
}
bool DocumentSql::ReplaceTag(const qint64& tagid, const QString& tag, const QString& yomi)
{
    MYQMODIFIER QSqlQuery query = myPrepare("UPDATE " + docdb("Tag") + " SET tag=?,yomi=? WHERE tagid=? AND dbid=?");

    int i=0;
    query.bindValue(i++,tag);
    query.bindValue(i++,yomi);
    query.bindValue(i++,tagid);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());

    return true;
}
bool DocumentSql::DeleteTag(const qint64& tagid)
{
    MYQMODIFIER QSqlQuery query = myPrepare("DELETE FROM " + docdb("Tag") + " WHERE tagid=? AND dbid=?");

    int i=0;
    query.bindValue(i++,tagid);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());

    return true;
}
bool DocumentSql::GetTagsFromID(const qint64& id, QSet<qint64>& tagids)
{
    if( id <= 0)
        return true;

    MYQMODIFIER QSqlQuery query = myPrepare("SELECT tagid FROM " + docdb("Tagged") + " WHERE id=? AND dbid=?");

    int i=0;
    query.bindValue(i++, id);
    query.bindValue(i++, gpSQL->getDbID());
    SQC(query,exec());
    while(query.next())
    {
        tagids.insert(query.value("tagid").toLongLong());
    }

    return true;
}
//bool DocumentSql::GetOpenCounts(QMap<qint64,int>& opencounts)
//{
//    MYQMODIFIER QSqlQuery query = myq("SELECT id,opencount FROM Access WHERE dbid=?");
//    query.bindValue(0,gpSQL->getDbID());
//    SQC(query,exec());
//    while(query.next())
//    {
//        opencounts[query.value("id").toLongLong()]=query.value("opencount").toInt();
//    }
//    return true;
//}
//bool DocumentSql::GetLastAccesses(QMap<qint64,qint64>& lastaccesses)
//{
//    MYQMODIFIER QSqlQuery query = myq("SELECT id,lastaccess FROM Access");
//    SQC(query,exec());
//    while(query.next())
//    {
//        lastaccesses[query.value("id").toLongLong()]=query.value("lastaccess").toLongLong();
//    }
//    return true;
//}
bool DocumentSql::GetOpenCountAndLastAccess(const qint64& id, int& opencount, qint64& lastaccess)
{
    MYQMODIFIER QSqlQuery query = myPrepare("SELECT opencount,lastaccess FROM " + docdb("Access") + " WHERE id=? AND dbid=?");
    int i=0;
    query.bindValue(i++,id);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());
    if(!query.next())
        return false;
    opencount = query.value("opencount").toInt();
    lastaccess = query.value("lastaccess").toLongLong();

    return true;
}
bool DocumentSql::GetTagSelectedAndChecked(const qint64& tagid, bool& bSel, bool& bCheck)
{
    MYQMODIFIER QSqlQuery query = myPrepare("SELECT selected,checked FROM " + docdb("Tag") + " WHERE tagid=? AND dbid=?");
    int i=0;
    query.bindValue(i++,tagid);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());
    if(!query.next())
    {
        Q_ASSERT(false);
        return false;
    }
    bSel = query.value("selected").toInt() != 0;
    bCheck = query.value("checked").toInt() != 0;
    return true;
}
bool DocumentSql::SetTagSelectedAndChecked(const qint64& tagid, const bool bSel, const bool bCheck)
{
    MYQMODIFIER QSqlQuery query = myPrepare("UPDATE " + docdb("Tag") + " SET selected=?,checked=? WHERE tagid=? AND dbid=?");
    int i=0;
    query.bindValue(i++,bSel ? 1:0);
    query.bindValue(i++, bCheck?1:0);
    query.bindValue(i++,tagid);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());
    Q_ASSERT(query.numRowsAffected()==1);
    return true;
}
bool DocumentSql::GetAllDirs(QList<DirectoryItem*>& dirs) const
{
    MYQMODIFIER QSqlQuery query("SELECT * FROM " + docdb("Directories"));
    SQC(query,exec());
    while(query.next())
    {
        DirectoryItem* di = new DirectoryItem(
                    query.value("id").toLongLong(),
                    DirectoryItem::DirectoryItemType::DI_NORMAL_MY,
                    query.value("directory").toString());
        di->setSelected(query.value("selected").toInt() != 0);
        di->setCheckState(query.value("checked").toInt() != 0 ? Qt::Checked : Qt::Unchecked);

        dirs.append(di);
    }
    return true;
}
bool DocumentSql::InsertDirectory(const QString& dirOrig, DirectoryItem*& newdi)
{
    MYQMODIFIER QSqlQuery query = myPrepare("INSERT INTO " + docdb("Directories") + " (directory) VALUES (?)");

    QString dir = normalizeDir(dirOrig);

    int i = 0;
    query.bindValue(i++, dir);
    SQC(query, exec());

    bool ok=false;
    qint64 dirid = query.lastInsertId().toLongLong(&ok);
    Q_ASSERT(ok);
    Q_ASSERT(dirid > 0);
    newdi = new DirectoryItem(dirid,
                              DirectoryItem::DirectoryItemType::DI_NORMAL_MY,
                              dir);

    return true;
}
//bool DocumentSql::DeleteDirectory(DirectoryItem* di)
//{
//    MYQMODIFIER QSqlQuery query = myq("DELETE FROM " + docdb("Directories") + " WHERE id=? AND directory=? LIMIT 1");
//    int i=0;
//    query.bindValue(i++, di->getID());
//    query.bindValue(i++, di->text());
//    SQC(query, exec());
//    Q_ASSERT(query.numRowsAffected()==1);
//    return true;
//}
