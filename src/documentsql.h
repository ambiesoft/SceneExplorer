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

#ifndef DOCUMENTSQL_H
#define DOCUMENTSQL_H

#include <QSqlDatabase>

#include "tableitemdata.h"

#define DBVERSIONDOC 1
class DirectoryItem;
class TagItem;

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
    bool isAllSelectedCommon(const QString& col) const;
    bool setAllSelectedCommon(const QString& col, bool b);

    bool isDirAllSelected() const;
    bool setDirAllSelected(bool b);
    
    bool isDirAllChecked() const;
    bool setDirAllChecked(bool b);

    bool isTagAllSelected() const;
    bool setTagAllSelected(bool b);

    bool isTagNotagsSelected() const;
    bool setTagNotagsSelected(bool b);

    int dirCount() const;
    QString getDirText(int i) const;
    bool setDirectory(int index, DirectoryItem* di);

    bool isDirSelected(int index) const;
    // bool setDirSelected(int index, bool b);

    bool isDirChecked(int index) const;
    // bool setDirChecked(int index, bool b);

    bool removeDirectoryOver(int index);

    bool SetLastPos(int row, int column);
    bool GetLastPos(int& row, int& column) const;

    bool IncrementOpenCountAndLastAccess(const qint64& id);
    bool setOpenCountAndLascAccess_obsolete(const QList<TableItemDataPointer>& all);

    bool getOpenCount(const qint64& id, qint64& openCount);
    bool setOpenCount(const qint64& id, const qint64& openCount);

    bool GetAllTags(QList<TagItem*>& tags,bool bHasParent) const;
    bool GetAllTaggedTagids(QList<qint64>& alltaggedids) const;
    bool GetTaggedCount(const qint64& tagid, int& count) const;
    bool IsTagExist(const QString& tag) const;
    bool Insert(const QString& tag, const QString& yomi, qint64& insertedTag) const;
    bool GetTaggedIDs(const QList<qint64>& tagids, QSet<qint64>& taggedids) const;
    bool SetTagged(const qint64& id, const qint64& tagid, const bool bSet) const;
    bool GetTag(const qint64& tagid, QString& tag, QString& yomi) const;
    bool ReplaceTag(const qint64& tagid, const QString& tag, const QString& yomi);
    bool DeleteTag(const qint64& tagid);
    bool GetTagsFromID(const qint64& id, QSet<qint64>& tagids);
    //    bool GetOpenCounts(QMap<qint64,int>& opencounts);
    //    bool GetLastAccesses(QMap<qint64,qint64>& lastaccesses);
    bool GetOpenCountAndLastAccess(const qint64& id, int& opencount, qint64& lastaccess);
    bool GetTagSelectedAndChecked(const qint64& tagid, bool& bSel, bool& bCheck);
    bool SetTagSelectedAndChecked(const qint64& tagid, const bool bSel, const bool bCheck);
    bool setDirNormalItemState(const DirectoryItem* item);
    bool GetAllDirs(QList<DirectoryItem*>& dirs) const;
    bool InsertDirectory(const QString& dir, DirectoryItem*& newdi);
    // bool DeleteDirectory(DirectoryItem* di);
};

#endif // DOCUMENTSQL_H
