#ifndef DOCUMENTSQL_H
#define DOCUMENTSQL_H

#include "tableitemdata.h"

class DocumentSql : public QObject
{
    Q_OBJECT

    QSqlDatabase db_;
    bool ok_ = false;
    QString lastError_;
public:
    DocumentSql(const QString& file);

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
    bool setOpenCountAndLascAccess(const QList<TableItemDataPointer>& all);

    bool GetAllTags(QStringList& ret) const;
    bool IsTagExist(const QString& tag) const;
    bool InsertOrReplaceTag(const qint64& id,const QString& tag, const QString& yomi) const;
};

#endif // DOCUMENTSQL_H
