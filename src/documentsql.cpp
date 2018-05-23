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

#include "sqlcommon.h"
#include "tableitemdata.h"
#include "helper.h"
#include "sql.h"

#include "documentsql.h"

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
    gpSQL->DetachDocument();
}

bool DocumentSql::isAllSelected() const
{
    MYQMODIFIER QSqlQuery query("SELECT allselected FROM Settings WHERE id=1");

    SQC(query,exec());
    if(!query.next())
        return false;
    bool ok;
    bool ret = query.value("allselected").toInt(&ok) != 0;
    if(!ok)
        return false;
    return ret;
}
bool DocumentSql::setAllSelected(bool b)
{
    MYQMODIFIER QSqlQuery query("UPDATE Settings SET allselected=? WHERE id=1");

	query.bindValue(0, b ? 1 : 0);
	SQC(query,exec());
	return true;
}

bool DocumentSql::isAllChecked() const
{
    MYQMODIFIER QSqlQuery query("SELECT allchecked FROM Settings WHERE id=1");
    SQC(query,exec());
    if(!query.next())
        return false;
    bool ok;
    bool ret = query.value("allchecked").toInt(&ok) != 0;
    if(!ok)
        return false;
    return ret;
}
bool DocumentSql::setAllChecked(bool b)
{
    MYQMODIFIER QSqlQuery query("UPDATE Settings SET allchecked=? WHERE id=1");

	query.bindValue(0, b ? 1 : 0);
	SQC(query, exec());
	return true;
}


int DocumentSql::dirCount() const
{
    MYQMODIFIER QSqlQuery query("SELECT count(*) FROM Directories");
    SQC(query,exec());
    if(!query.next())
        return 0;
    return query.value(0).toInt();
}
QString DocumentSql::getDirText(int index) const
{
    MYQMODIFIER QSqlQuery query("SELECT directory FROM Directories WHERE id=?");

    query.bindValue(0, index);
    if(!query.exec())
        return QString();
    if(!query.next())
        return QString();
    return query.value(0).toString();
}
bool DocumentSql::setDirectory(int index, const QString& text, bool bSel, bool bCheck)
{
    MYQMODIFIER QSqlQuery query = myq("INSERT OR REPLACE INTO Directories (id, directory, selected, checked) VALUES (?,?,?,?)");

	int i = 0;
	query.bindValue(i++, index);
	query.bindValue(i++, text);
	query.bindValue(i++, bSel?1:0);
	query.bindValue(i++, bCheck?1:0);
	SQC(query, exec());
	return true;
}

bool DocumentSql::isDirSelected(int index) const
{
    MYQMODIFIER QSqlQuery query("SELECT selected FROM Directories WHERE id=?");

	query.bindValue(0, index);
	SQC(query, exec());
	if (!query.next())
		return false;
	return query.value(0).toInt() != 0;
}

bool DocumentSql::isDirChecked(int index) const
{
    MYQMODIFIER QSqlQuery query("SELECT checked FROM Directories WHERE id=?");

	query.bindValue(0, index);
	SQC(query, exec());
	if(!query.next())
		return false;
    return query.value(0).toInt() != 0;
}

bool DocumentSql::removeDirectoryOver(int index)
{
    MYQMODIFIER QSqlQuery query("DELETE FROM Directories WHERE id > ?");

	query.bindValue(0, index);
	SQC(query, exec());
	return true;
}

bool DocumentSql::SetLastPos(int row, int column)
{
    // this caused sigsegv in linux
    // QSqlQuery query("UPDATE Settings SET lastrow=?, lastcolumn=? WHERE id=1");
    QSqlQuery query;
    SQC(query,prepare("UPDATE Settings SET lastrow=?, lastcolumn=? WHERE id=1"));

    int i=0;
    query.bindValue(i++, row);
    query.bindValue(i++, column);
    SQC(query,exec());

    return true;
}
bool DocumentSql::GetLastPos(int& row, int& column) const
{
    MYQMODIFIER QSqlQuery query("SELECT * FROM Settings WHERE id=1");
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

bool DocumentSql::IncrementOpenCount(const qint64& id)
{
    MYQMODIFIER QString state =
            "REPLACE into Access (id,opencount,lastaccess,dbid) VALUES "
            "(?,"
            "COALESCE((SELECT opencount FROM Access WHERE id=?),0)+1,"
            "?,"
            "?)";

    MYQMODIFIER QSqlQuery query = myq(state);

    int i=0;
    query.bindValue(i++, id);
    query.bindValue(i++, id);
    query.bindValue(i++, QDateTime::currentSecsSinceEpoch());
    query.bindValue(i++, gpSQL->getDbID());

    SQC(query,exec());
    Q_ASSERT(query.numRowsAffected() == 1);

    return true;
}

bool DocumentSql::setOpenCountAndLascAccess_obsolete(const QList<TableItemDataPointer>& all)
{
    MYQMODIFIER QSqlQuery query("SELECT id,opencount,lastaccess,dbid FROM Access WHERE dbid=?");

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

bool DocumentSql::GetAllTags(QMap<qint64, QString>& tags) const
{
    MYQMODIFIER QSqlQuery query("SELECT * FROM Tag WHERE dbid=? ORDER BY yomi");

    int i=0;
    query.bindValue(i++, gpSQL->getDbID());
    SQC(query,exec());
    while(query.next())
    {
        tags[query.value("tagid").toLongLong()] = query.value("tag").toString();
        // ret.append(query.value("tag").toString());
    }
    return true;
}
bool DocumentSql::IsTagExist(const QString& tag) const
{
    MYQMODIFIER QSqlQuery query = myq("SELECT tagid FROM Tag WHERE tag=? AND dbid=?");

    int i=0;
    query.bindValue(i++, tag);
    query.bindValue(i++, gpSQL->getDbID());
    SQC(query,exec());
    return query.next();
}
bool DocumentSql::Insert(const QString& tag, const QString& yomi) const
{
    MYQMODIFIER QSqlQuery query = myq("REPLACE INTO Tag (tag,yomi,dbid) VALUES (?,?,?)");

    int i=0;
    // query.bindValue(i++, id);
    query.bindValue(i++,tag);
    query.bindValue(i++,yomi);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());

    return true;
}

bool DocumentSql::GetTaggedIDs(const QList<qint64>& tagids, QSet<qint64>& taggedids) const
{
    if(tagids.isEmpty())
    {
        taggedids.clear();
        return true;
    }
    QString sql = "SELECT id FROM Tagged WHERE ";
    for(int i=0 ; i < tagids.count(); ++i)
    {
        sql += "tagid=? ";
        if((i+1)!=tagids.count())
            sql += " OR ";
    }

    QSqlQuery query;
    SQC(query,prepare(sql));

    for(int i=0; i < tagids.count(); ++i)
    {
        query.bindValue(i, tagids[i]);
    }

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
        MYQMODIFIER QSqlQuery query = myq("REPLACE INTO Tagged (id,tagid,dbid) VALUES (?,?,?)");

		int i = 0;
		query.bindValue(i++, id);
		query.bindValue(i++, tagid);
		query.bindValue(i++, gpSQL->getDbID());

		SQC(query, exec());
	}
	else
	{
        QSqlQuery query = myq("DELETE FROM Tagged WHERE id=? AND tagid=? AND dbid=?");

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
    MYQMODIFIER QSqlQuery query = myq("SELECT tag,yomi FROM Tag WHERE tagid=? AND dbid=?");
    qDebug() << query.isValid();
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
    MYQMODIFIER QSqlQuery query = myq("UPDATE Tag SET tag=?,yomi=? WHERE tagid=? AND dbid=?");

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
    MYQMODIFIER QSqlQuery query = myq("DELETE FROM Tag WHERE tagid=? AND dbid=?");

    int i=0;
    query.bindValue(i++,tagid);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());

    return true;
}
bool DocumentSql::GetTagsFromID(const qint64& id, QSet<qint64>& tagids)
{
//    QSqlQuery query;
//    SQC(query,prepare("SELECT tagid FROM Tagged WHERE id=? AND dbid=?"));

    MYQMODIFIER QSqlQuery query = myq("SELECT tagid FROM Tagged WHERE id=? AND dbid=?");

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
bool DocumentSql::GetOpenCounts(QMap<qint64,int>& opencounts)
{
    MYQMODIFIER QSqlQuery query("SELECT id,opencount FROM Access");
    while(query.next())
    {
        opencounts[query.value("id").toLongLong()]=query.value("opencount").toInt();
    }
    return true;
}
bool DocumentSql::GetLastAccesses(QMap<qint64,qint64>& lastaccesses)
{
    MYQMODIFIER QSqlQuery query("SELECT id,lastaccess FROM Access");
//    MYQMODIFIER bool prepared=false;
//    if(!prepared)
//    {
//        SQC(query,prepare("SELECT id,lastaccess FROM Access"));
//        prepared=true;
//    }
    SQC(query,exec());
    while(query.next())
    {
        lastaccesses[query.value("id").toLongLong()]=query.value("lastaccess").toLongLong();
    }
    return true;
}
