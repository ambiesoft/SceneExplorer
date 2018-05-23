#ifndef DOCUMENTSQL_H
#define DOCUMENTSQL_H

#include "tableitemdata.h"

#define DBVERSIONDOC 1

class DocumentSql : public QObject
{
    Q_OBJECT

    // QSqlDatabase db_;
    bool ok_ = false;
    mutable QString lastError_;

    int GetDBVersionDoc(QSqlDatabase& db);
    bool CreateDBInfoTableDoc(QSqlDatabase& db);

public:
    DocumentSql(const QString& file);
    ~DocumentSql();

    bool isOK() const {
        return ok_;
    }
    QString getLastError() const {
        return lastError_;
    }

    bool isAllSelected() const;
	bool setAllSelected(bool b);
    
	bool isAllChecked() const;
	bool setAllChecked(bool b);

    int dirCount() const;
    QString getDirText(int i) const;
	bool setDirectory(int index, const QString& text, bool bSel, bool bCheck);

    bool isDirSelected(int index) const;
	// bool setDirSelected(int index, bool b);

    bool isDirChecked(int index) const;
	// bool setDirChecked(int index, bool b);

	bool removeDirectoryOver(int index);

    bool SetLastPos(int row, int column);
    bool GetLastPos(int& row, int& column) const;

    bool IncrementOpenCount(const qint64& id);
    bool setOpenCountAndLascAccess_obsolete(const QList<TableItemDataPointer>& all);

    bool GetAllTags(QMap<qint64, QString>& tags) const;
    bool IsTagExist(const QString& tag) const;
    bool Insert(const QString& tag, const QString& yomi) const;
    bool GetTaggedIDs(const QList<qint64>& tagids, QSet<qint64>& taggedids) const;
    bool SetTagged(const qint64& id, const qint64& tagid, const bool bSet) const;
    bool GetTag(const qint64& tagid, QString& tag, QString& yomi) const;
    bool ReplaceTag(const qint64& tagid, const QString& tag, const QString& yomi);
    bool DeleteTag(const qint64& tagid);
    bool GetTagsFromID(const qint64& id, QSet<qint64>& tagids);
    bool GetOpenCounts(QMap<qint64,int>& opencounts);
    bool GetLastAccesses(QMap<qint64,qint64>& lastaccesses);
};

#endif // DOCUMENTSQL_H
