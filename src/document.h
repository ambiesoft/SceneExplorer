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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QSettings>
#include <QList>
#include <QListWidget>

#include "documentsql.h"
#include "globals.h"


//struct DE
//{
//    QString dir_;
//    bool chk_;
//    bool sel_;

//    DE(const QString& dir, bool chk, bool sel) :
//        dir_(dir),
//        chk_(chk),
//        sel_(sel){}

//};

class Document : public QObject
{
    Q_OBJECT

    QSettings* s_ = nullptr;
    QString lastErr_;
    QString file_;
    DocumentSql* docSql_ = nullptr;
    bool bReordered_ = false;
public:
    Document(QObject* =nullptr){}
    ~Document() {
        delete s_;
        delete docSql_;
    }
    bool Load(const QString& file, const bool bExists);

    QString GetLastErr() const {
        return lastErr_;
    }
    void Store(QListWidget* pLW,
               QListWidget* pListTag,
               const QModelIndex& index);


    bool IsDirAllSelected() const {
        return docSql_->isDirAllSelected();
    }
    bool IsDirAllChecked_obsolete() const {
        return docSql_->isDirAllChecked();
    }

    bool IsTagAllSelected() const {
        return docSql_->isTagAllSelected();
    }
    bool IsTagNotagsSelected() const {
        return docSql_->isTagNotagsSelected();
    }

    int dirCount() const;
    QString GetDEText(int index) const {
        return docSql_->getDirText(index);
    }
    bool IsDESelected(int index) const {
        return docSql_->isDirSelected(index);
    }
    bool IsDEChecked(int index) const {
        return docSql_->isDirChecked(index);
    }

    QString GetFileName() const;
    QString GetFullName() const;

    // int scrollPos() const ;
    bool getLastPos(int& row, int& column) const;

    // bool bOpenCountAndLastAccessClean_ = false;
    bool IncrementOpenCountAndLastAccess(const qint64& id) {
        return docSql_->IncrementOpenCountAndLastAccess(id);
    }
    //    bool IsOpenCountAndLastAccessClean() const {
    //        return bOpenCountAndLastAccessClean_;
    //    }
    //    void SetOpenCountAndLastAccessClean() {
    //        bOpenCountAndLastAccessClean_ = true;
    //    }
    bool setOpenCountAndLascAccess_obsolete(const QList<TableItemDataPointer>& all)
    {
        return docSql_->setOpenCountAndLascAccess_obsolete(all);
    }
    bool getOpenCount(const qint64& id, qint64& openCount)
    {
        return docSql_->getOpenCount(id,openCount);
    }
    bool setOpenCount(const qint64& id, const qint64& openCount)
    {
        return docSql_->setOpenCount(id,openCount);
    }
    // bool sort(QList<TableItemDataPointer>& all, SORTCOLUMNMY sc, bool rev) const;

    bool GetAllTags(QList<TagItem*>& tags, bool bHasParent) const
    {
        return docSql_->GetAllTags(tags,bHasParent);
    }
    bool IsTagExist(const QString& tag) const
    {
        return docSql_->IsTagExist(tag);
    }
    bool Insert(const QString& tag, const QString& yomi, qint64& insertedTag) const
    {
        return docSql_->Insert(tag,yomi,insertedTag);
    }
    bool GetTaggedIDs(const QList<qint64>& tagids, QSet<qint64>& taggedids) const
    {
        return docSql_->GetTaggedIDs(tagids, taggedids);
    }
    bool SetTagged(const qint64& id, const qint64& tagid, const bool bSet) const
    {
        return docSql_->SetTagged(id,tagid,bSet);
    }
    bool GetAllTaggedTagIDs(QList<qint64>& alltaggedtagids) const {
        return docSql_->GetAllTaggedTagids(alltaggedtagids);
    }
    bool GetTag(const qint64& tagid, QString& tag, QString& yomi) const
    {
        return docSql_->GetTag(tagid,tag,yomi);
    }
    bool ReplaceTag(const qint64& tagid, const QString& tag, const QString& yomi)
    {
        return docSql_->ReplaceTag(tagid,tag,yomi);
    }
    bool DeleteTag(const qint64& tagid)
    {
        return docSql_->DeleteTag(tagid);
    }
    bool GetTagsFromID(const qint64& id, QSet<qint64>& tagids) const
    {
        return docSql_->GetTagsFromID(id,tagids);
    }
    //    bool GetOpenCounts(QMap<qint64,int>& opencounts)
    //    {
    //        return docSql_->GetOpenCounts(opencounts);
    //    }
    //    bool GetLastAccesses(QMap<qint64,qint64>& lastaccesses)
    //    {
    //        return docSql_->GetLastAccesses(lastaccesses);
    //    }
    bool GetOpenCountAndLastAccess(const qint64& id, int& opencount, qint64& lastaccess) const
    {
        return docSql_->GetOpenCountAndLastAccess(id,opencount,lastaccess);
    }
    bool GetTagSelectedAndChecked(const qint64& tagid, bool& bSel, bool& bCheck) const
    {
        return docSql_->GetTagSelectedAndChecked(tagid,bSel,bCheck);
    }
    bool GetAllDirs(QList<DirectoryItem*>& dirs) const
    {
        return docSql_->GetAllDirs(dirs);
    }
    bool InsertDirectory(const QString& dir, DirectoryItem*& newdi)
    {
        return docSql_->InsertDirectory(dir,newdi);
    }
    void SetReordered() {
        bReordered_=true;
    }
    // set reorder to delete directory
    //    bool DeleteDirectory(DirectoryItem* di) {
    //        return docSql_->DeleteDirectory(di);
    //    }

};

#endif // DOCUMENT_H
