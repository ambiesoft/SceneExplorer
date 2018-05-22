#include "documentsql.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "tableitemdata.h"
#include "helper.h"
#include "sql.h"

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
bool DocumentSql::CreateDBInfoTableDoc()
{
    QSqlQuery query(db_);

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
int DocumentSql::GetDBVersionDoc()
{
    QSqlQuery query(db_);
    SQCI(query,exec("SELECT version FROM DbInfoDoc WHERE id=1"));
    if(!query.next())
        return -1;
    return query.value("version").toInt();
}
DocumentSql::DocumentSql(const QString& file) :
    db_(QSqlDatabase:: addDatabase("QSQLITE",getDBConnection())),
    ok_(false)
{
	qDebug() << db_.databaseName();

    db_.setDatabaseName(file);
    if (!db_.open() || !db_.isOpen() || !db_.isValid())
    {
        lastError_ = db_.lastError().text();
        return;
    }

    if(!CreateDBInfoTableDoc())
        return;
    int version = GetDBVersionDoc();

    db_.exec("CREATE TABLE Settings ( "
               "id INTEGER NOT NULL PRIMARY KEY,"
               "allselected INT NOT NULL DEFAULT '0',"
               "allchecked INT NOT NULL DEFAULT '0',"
               "lastrow INT NOT NULL DEFAULT '0',"
               "lastcolumn INT NOT NULL DEFAULT '0'"
               ")"
               );
    qDebug() << db_.lastError().text();

    {
        // Create record 1
        QSqlQuery query = db_.exec("SELECT id FROM Settings WHERE id=1");
        query.exec();
        qDebug() << db_.lastError().text();
        if(!query.next())
        {
            db_.exec("INSERT INTO Settings (id) VALUES (1)");
            qDebug() << db_.lastError().text();
        }
    }

	QSqlQuery query = db_.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Settings';");
    query.next();
    if("Settings" != query.value("name").toString())
    {
        lastError_ = tr("Table Settings could not be created.");
        return;
    }



	db_.exec("CREATE TABLE Directories ( "
               "id INTEGER NOT NULL PRIMARY KEY,"
               "directory TEXT,"
               "selected INT,"
               "checked INT)"
               );
    qDebug() << query.lastError().text();

	query = db_.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Directories';");
	query.next();
    if("Directories" != query.value("name").toString())
    {
        lastError_ = tr("Table Directories could not be created.");
        return;
    }



    db_.exec("CREATE TABLE Access ( "
               "id INTEGER NOT NULL,"
               "opencount INT NOT NULL DEFAULT 0,"
               "lastaccess INT,"
               "dbid TEXT NOT NULL)"
               );
    qDebug() << query.lastError().text();
    db_.exec("CREATE UNIQUE INDEX idx_Access_id_dbid ON Access(id,dbid)");
    qDebug() << db_.lastError().text();
    query = db_.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Access';");
    query.next();
    if("Access" != query.value("name").toString())
    {
        lastError_ = tr("Table Access could not be created.");
        return;
    }


    db_.exec("CREATE TABLE Tag ( "
             "tagid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
             "tag,"
             "yomi,"
             "dbid TEXT NOT NULL)"
             );
    qDebug() << query.lastError().text();
    db_.exec("CREATE UNIQUE INDEX idx_Tag_tagid_dbid ON Tag(tagid,dbid)");
    qDebug() << db_.lastError().text();
    query = db_.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Tag';");
    query.next();
    if("Tag" != query.value("name").toString())
    {
        lastError_ = tr("Table Tag could not be created.");
        return;
    }

    db_.exec("CREATE TABLE Tagged ( "
             "id INTEGER NOT NULL,"
             "tagid INTEGER NOT NULL,"
             "dbid TEXT NOT NULL)"
             );
    qDebug() << query.lastError().text();
    db_.exec("CREATE UNIQUE INDEX idx_Tagged_id_tagid_dbid ON Tagged(id,tagid,dbid)");
    qDebug() << query.lastError().text();
    query = db_.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Tagged';");
    query.next();
    if("Tagged" != query.value("name").toString())
    {
        lastError_ = tr("Table Tagged could not be created.");
        return;
    }

    if(version != DBVERSIONDOC)
    {
        SQCN(query,prepare("UPDATE DbInfoDoc SET version=?"));
        query.bindValue(0, DBVERSIONDOC);
        SQCN(query,exec());
    }
    ok_ = true;
}
bool DocumentSql::isAllSelected() const
{
    QSqlQuery query(db_);

    SQC(query,exec("SELECT allselected FROM Settings WHERE id=1"));
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
	QSqlQuery query(db_);
    SQC(query, prepare("UPDATE Settings SET allselected=? WHERE id=1"));
	query.bindValue(0, b ? 1 : 0);
	SQC(query,exec());
	return true;
}

bool DocumentSql::isAllChecked() const
{
    QSqlQuery query(db_);
    SQC(query,exec("SELECT allchecked FROM Settings WHERE id=1"));
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
	QSqlQuery query(db_);
    SQC(query, prepare("UPDATE Settings SET allchecked=? WHERE id=1"));
	query.bindValue(0, b ? 1 : 0);
	SQC(query, exec());
	return true;
}


int DocumentSql::dirCount() const
{
    QSqlQuery query(db_);
    query.exec("SELECT count(*) FROM Directories");
    if(!query.next())
        return 0;
    return query.value(0).toInt();
}
QString DocumentSql::getDirText(int index) const
{
    QSqlQuery query(db_);
    if(!query.prepare("SELECT directory FROM Directories WHERE id=?"))
        return QString();
	query.bindValue(0, index);
    if(!query.exec())
        return QString();
    if(!query.next())
        return QString();
    return query.value(0).toString();
}
bool DocumentSql::setDirectory(int index, const QString& text, bool bSel, bool bCheck)
{
	QSqlQuery query(db_);
	SQC(query, prepare("INSERT OR REPLACE INTO Directories (id, directory, selected, checked) VALUES (?,?,?,?)"));
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
	QSqlQuery query(db_);
	SQC(query, prepare("SELECT selected FROM Directories WHERE id=?"));
	query.bindValue(0, index);
	SQC(query, exec());
	if (!query.next())
		return false;
	return query.value(0).toInt() != 0;
}
//bool DocumentSql::setDirSelected(int index, bool b)
//{
//	QSqlQuery query(db_);
//	SQC(query, prepare("INSERT OR REPLACE INTO Directories (id, selected) VALUES (?,?)"));
//	query.bindValue(0, index);
//	query.bindValue(1, b ? 1 : 0);
//	SQC(query, exec());
//	return true;
//}

bool DocumentSql::isDirChecked(int index) const
{
    QSqlQuery query(db_);
	SQC(query, prepare("SELECT checked FROM Directories WHERE id=?"));
	query.bindValue(0, index);
	SQC(query, exec());
	if(!query.next())
		return false;
    return query.value(0).toInt() != 0;
}
//bool DocumentSql::setDirChecked(int index, bool b)
//{
//	QSqlQuery query(db_);
//	SQC(query, prepare("INSERT OR REPLACE INTO Directories (id, checked) VALUES (?,?)")); 
//	query.bindValue(0, index);
//	query.bindValue(1, b ? 1 : 0);
//	SQC(query, exec());
//	return true;
//}

bool DocumentSql::removeDirectoryOver(int index)
{
	QSqlQuery query(db_);
	SQC(query, prepare("DELETE FROM Directories WHERE id > ?"));
	query.bindValue(0, index);
	SQC(query, exec());
	return true;
}

bool DocumentSql::SetLastPos(int row, int column)
{
    QSqlQuery query(db_);
    SQC(query,prepare("UPDATE Settings SET lastrow=?, lastcolumn=? WHERE id=1"));
    int i=0;
    query.bindValue(i++, row);
    query.bindValue(i++, column);
    SQC(query,exec());

    return true;
}
bool DocumentSql::GetLastPos(int& row, int& column) const
{
    QSqlQuery query(db_);
    query.exec("SELECT * FROM Settings WHERE id=1");
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
//    QFileInfo fi(movieFile);
//    QString dir = canonicalDir(fi.absolutePath());
//    QString file = fi.fileName();
    {
        QString state =
                "REPLACE into Access (id,opencount,lastaccess,dbid) VALUES "
                "(?,"
                "COALESCE((SELECT opencount FROM Access WHERE id=?),0)+1,"
                "?,"
                "?)";

        QSqlQuery query(db_);
        SQC(query,prepare(state));

        int i=0;
        query.bindValue(i++, id);
        query.bindValue(i++, id);
        query.bindValue(i++, QDateTime::currentSecsSinceEpoch());
        query.bindValue(i++, gpSQL->getDbID());



        SQC(query,exec());
        Q_ASSERT(query.numRowsAffected() == 1);
    }

//    {
//        QString state = "UPDATE Access SET opencount=opencount+1 WHERE "
//                        "id=?";

//        QSqlQuery query(db_);
//        SQC(query,prepare(state));

//        int i=0;
//        query.bindValue(i++, id);

//        SQC(query,exec());
//        Q_ASSERT(query.numRowsAffected() == 1);

//    }
    return true;
}

bool DocumentSql::setOpenCountAndLascAccess(const QList<TableItemDataPointer>& all)
{
    QString state =
            "SELECT id,opencount,lastaccess,dbid FROM Access WHERE dbid=?";

    QSqlQuery query(db_);
    SQC(query,prepare(state));
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
    QSqlQuery query(db_);
    SQC(query,prepare("SELECT * FROM Tag WHERE dbid=? ORDER BY yomi"));
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
    QSqlQuery query(db_);
    SQC(query,prepare("SELECT tagid FROM Tag WHERE tag=? AND dbid=?"));
    int i=0;
    query.bindValue(i++, tag);
    query.bindValue(i++, gpSQL->getDbID());
    SQC(query,exec());
    return query.next();
}
bool DocumentSql::Insert(const QString& tag, const QString& yomi) const
{
    QSqlQuery query(db_);
    SQC(query,prepare("REPLACE INTO Tag (tag,yomi,dbid) VALUES (?,?,?)"));
    int i=0;
    // query.bindValue(i++, id);
    query.bindValue(i++,tag);
    query.bindValue(i++,yomi);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());

    return true;
}

bool DocumentSql::GetTaggedIDs(const QList<qint64>& tagids, QList<qint64>& taggedids) const
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

    QSqlQuery query(db_);
    SQC(query,prepare(sql));

    for(int i=0; i < tagids.count(); ++i)
    {
        query.bindValue(i, tagids[i]);
    }

    SQC(query,exec());
    while(query.next())
    {
        taggedids.append(query.value("id").toLongLong());
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

	QSqlQuery query(db_);
	if (bSet)
	{
		SQC(query, prepare("REPLACE INTO Tagged (id,tagid,dbid) VALUES (?,?,?)"));
		int i = 0;
		query.bindValue(i++, id);
		query.bindValue(i++, tagid);
		query.bindValue(i++, gpSQL->getDbID());

		SQC(query, exec());
	}
	else
	{
		SQC(query, prepare("DELETE FROM Tagged WHERE id=?,tagid=?,dbid=?"));
		SQC(query, exec());
	}
    return true;
}
bool DocumentSql::GetTag(const qint64& tagid, QString& tag, QString& yomi) const
{
    QSqlQuery query(db_);
    SQC(query,prepare("SELECT tag,yomi FROM Tag WHERE tagid=? AND dbid=?"));
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
    QSqlQuery query(db_);
    SQC(query,prepare("UPDATE Tag SET tag=?,yomi=? WHERE tagid=? AND dbid=?"));
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
    QSqlQuery query(db_);
    SQC(query,prepare("DELETE FROM Tag WHERE tagid=? AND dbid=?"));
    int i=0;
    query.bindValue(i++,tagid);
    query.bindValue(i++,gpSQL->getDbID());
    SQC(query,exec());

    // query.prepare("DELETE FROM Tagged WHERE ")
    return true;
}
bool DocumentSql::GetTagsFromID(const qint64& id, QSet<qint64>& tagids)
{
    QSqlQuery query(db_);
    SQC(query,prepare("SELECT tagid FROM Tagged WHERE id=? AND dbid=?"));
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
