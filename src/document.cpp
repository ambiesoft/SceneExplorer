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

#include <QVariant>
#include <QException>
#include <QListWidget>
#include <QFileInfo>
#include <QDebug>

#include "directoryentry.h"
#include "helper.h"
#include "documentsql.h"
#include "tableitemdata.h"
#include "tagitem.h"
#include "document.h"

//const char KEY_KEY_USERENTRY_DIRECTORY_ALL_SELECTED[] = "entrydirectoryallselected";
//const char KEY_KEY_USERENTRY_DIRECTORY_ALL_CHECKED[] = "entrydirectoryallchecked";

//const char KEY_USERENTRY_COUNT[] = "directoryentrycount";
//const char KEY_USERENTRY_GROUPPRIFIX[] = "directoryentry_";
//const char KEY_USERENTRY_DIRECTORY[] = "entrydirectory";
//const char KEY_USERENTRY_SELECTED[] = "entryselected";
//const char KEY_USERENTRY_CHECKED[] = "entrychecked";
//const char KEY_SCROLL_POS[] = "scrollpos";
//const char KEY_TABLE_INDEX_ROW[] = "tableindexrow";
//const char KEY_TABLE_INDEX_COLUMN[] = "tableindexcolumn";

//const char KEY_IDENTITY_SCENEEXPLORER_DOCUMENT[] = "identity";


//static std::string getIdentity()
//{
//    std::string ret="SceneExplorer_";
//    ret += "Document_";
//    ret += "Identity";
//    return ret;
//}
bool Document::Load(const QString& file, const bool bMustExists)
{
    file_ = file;
    const bool bExist = QFileInfo(file).exists();
    if(bMustExists && !bExist)
    {
        lastErr_ = tr("\"%1\" does not exist.").arg(file);
        return false;
    }

    qDebug() << "Start opening document " << file << __FUNCTION__;

    if(docSql_)
        delete docSql_;
    docSql_ = new DocumentSql(file);
    if(!docSql_->isOK())
    {
        lastErr_ = docSql_->getLastError();
        return false;
    }
    return true;
}

bool Document::StoreTagsSelectionState(QListWidget* pListTag)
{
    // save tag select state, each tag is saved when it's created or editted,
    // here save only selection state
    bool success = true;
    for(int i=0 ; i < pListTag->count(); ++i)
    {
        TagItem* ti = (TagItem*)pListTag->item(i);
        if(ti->IsAllItem())
        {
            success &= docSql_->setTagAllSelected(ti->isSelected());
        }
        else if(ti->IsNormalItem())
        {
            success &= docSql_->SetTagSelectedAndChecked(ti->tagid(), ti->isSelected(),ti->IsChecked());
        }
        else if(ti->IsNotagItem())
        {
            success &= docSql_->setTagNotagsSelected(ti->isSelected());
        }
        else
        {
            Q_ASSERT(false);
        }
    }
    return success;
}
bool Document::Store(QListWidget* pLW,
                     QListWidget* pListTag,
                     const QModelIndex& index)
{
    bool success=true;
    int normalCount = 0;
    for(int i=0 ; i <pLW->count();++i)
    {
        DirectoryItem* item = (DirectoryItem*)pLW->item(i);
        if(item->IsAllItem())
        {
            success &= docSql_->setDirAllSelected(item->isSelected());
            success &= docSql_->setDirAllChecked(item->checkState() == Qt::Checked);
        }
        else if(item->IsNormalItem())
        {
            if(bReordered_)
            {
                normalCount++;
                success &= docSql_->setDirectory(normalCount,item);
            }
            else
            {
                success &= docSql_->setDirNormalItemState(item);
            }
        }
        else if(item->IsMissingItem())
        {
            // nothing
        }
    }
    if(bReordered_)
        success &= docSql_->removeDirectoryOver(normalCount);

    bReordered_ = false;




    success &= StoreTagsSelectionState(pListTag);

    success &= docSql_->SetLastPos(index.row(),index.column());

    return success;
}

int Document::dirCount() const
{
    return docSql_->dirCount();
}
QString Document::GetFileName() const
{
    return QFileInfo(file_).completeBaseName();
}
QString Document::GetFullName() const {
    return file_;
}

//int Document::scrollPos() const
//{
//    QVariant v = s_->value(KEY_SCROLL_POS);
//    if(!v.isValid())
//        return 0;

//    return v.toInt();
//}
bool Document::getLastPos(int& row, int& column) const
{
    return docSql_->GetLastPos(row,column);
}


//static bool stRev;
//static bool openCountLessThan(const TableItemDataPointer &v1, const TableItemDataPointer &v2)
//{
//    if(stRev)
//        return v2->getOpenCount() < v1->getOpenCount();
//    else
//        return v1->getOpenCount() < v2->getOpenCount();
//}
//static bool lastaccessLessThan(const TableItemDataPointer &v1, const TableItemDataPointer &v2)
//{
//    if(stRev)
//        return v2->getLastAccess() < v1->getLastAccess();
//    else
//        return v1->getLastAccess() < v2->getLastAccess();
//}

//bool Document::sort(QList<TableItemDataPointer>& all, SORTCOLUMNMY sc, bool rev) const
//{
//    if(sc==SORT_OPENCOUNT)
//    {
//        stRev = !rev;
//        std::sort(all.begin(),all.end(),openCountLessThan);
//    }
//    else if(sc==SORT_LASTACCESS)
//    {
//        stRev = rev;
//        std::sort(all.begin(),all.end(),lastaccessLessThan);
//    }
//    return true;
//}
