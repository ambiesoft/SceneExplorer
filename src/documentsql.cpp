#include "documentsql.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "helper.h"

static void showFatal(const QString& error)
{
    Alert(nullptr, error);
}

#define SQC(Q,siki) do { if(!( Q.siki )) { showFatal((Q).lastError().text()); Q_ASSERT(false); return false;}} while(false)

static QString getDBConnection()
{
    static int i=0;
    ++i;
    return "DOC" + QString::number(i);
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

    
    db_.exec("CREATE TABLE Settings ( "
               "id INT NOT NULL PRIMARY KEY,"
               "allselected INT NOT NULL DEFAULT '0',"
               "allchecked INT NOT NULL DEFAULT '0',"
               "lastrow INT NOT NULL DEFAULT '0',"
               "lastcolumn INT NOT NULL DEFAULT '0')"
               );
    qDebug() << db_.lastError().text();
	QSqlQuery query = db_.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Settings';");
    query.next();
    if("Settings" != query.value("name").toString())
    {
        lastError_ = tr("Table Settings could not be created.");
        return;
    }

	db_.exec("CREATE TABLE Directories ( "
               "id INT NOT NULL PRIMARY KEY,"
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

//    query.exec("SELECT * FROM Directories");
//    while(query.next())
//    {
//        int id = query.value("id").toInt();
//        QString dir = query.value("directory").toString();
//        bool
//    }
    ok_ = true;
}
bool DocumentSql::isAllSelected() const
{
    QSqlQuery query(db_);
	SQC(query, exec("SELECT allselected FROM Settings WHERE id=1"));
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
	SQC(query, prepare("INSERT OR REPLACE INTO Settings (id, allselected) VALUES (1,?)"));
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
	SQC(query, prepare("INSERT OR REPLACE INTO Settings (id, allchecked) VALUES (1,?)"));
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
    SQC(query,prepare("INSERT OR REPLACE INTO Settings (id, lastrow, lastcolumn) VALUES (1,?,?)"));
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
