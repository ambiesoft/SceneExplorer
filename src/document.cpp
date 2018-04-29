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


#include "directoryentry.h"
#include "helper.h"
#include "documentsql.h"

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
    const bool bExist = QFile(file).exists();
    if(bMustExists && !bExist)
    {
        lastErr_ = QString(tr("\"%1\" does not exist.")).arg(file);
        return false;
    }

    qDebug() << "Start opening document " << file;

    if(docSql_)
        delete docSql_;
    docSql_ = new DocumentSql(file);
    if(!docSql_->isOK())
    {
        lastErr_ = docSql_->getLastError();
        return false;
    }
    return true;

//    file_ = file;
//    const bool bExist = QFile(file).exists();
//    if(bMustExists && !bExist)
//    {
//        lastErr_ = QString(tr("\"%1\" does not exist.")).arg(file);
//        return false;
//    }
//    if(QFile(file).size() > (10*1024*1024))
//    {
//        lastErr_ = tr("File size is too big.");
//        return false;
//    }

//    qDebug() << "Start opening document " << file;

//    if(bExist)
//    {
//        QFile theFile(file);
//        theFile.open(QIODevice::ReadWrite);
//        if(!theFile.isReadable())
//        {
//            lastErr_=QString(tr("\"%1\" is not readable.")).arg(file);
//            return false;
//        }
//        if(!theFile.isWritable())
//        {
//            lastErr_=QString(tr("\"%1\" is not writable.")).arg(file);
//            return false;
//        }

//        // check if this is a SceneExplorer Document
//        std::string bare = theFile.readAll().toStdString();
//        std::string::size_type pos = bare.find(getIdentity());
//        if(pos == std::string::npos)
//        {
//            lastErr_=QString(tr("%1 is not a SceneExplorer document.")).arg(file);
//            return false;
//        }
//    }

//    if(s_)
//        delete s_;
//    s_ = new QSettings(file, QSettings::Format::IniFormat);

//    QVariant vVal = s_->value(KEY_KEY_USERENTRY_DIRECTORY_ALL_SELECTED);
//    if(vVal.isValid())
//        bAllSel_=vVal.toBool();
//    vVal = s_->value(KEY_KEY_USERENTRY_DIRECTORY_ALL_CHECKED);
//    if(vVal.isValid())
//        bAllChecked_=vVal.toBool();

//    QVariant v = s_->value(KEY_USERENTRY_COUNT);
//    int groupcount = 0;
//    if(v.isValid())
//    {
//        groupcount = v.toInt();
//    }

//    try
//    {
//        for(int i=0 ; i < groupcount; ++i)
//        {
//            QString group = KEY_USERENTRY_GROUPPRIFIX + QString::number(i);

//            s_->beginGroup(group);

//            QVariant vValDir = s_->value(KEY_USERENTRY_DIRECTORY);
//            QVariant vValSelected = s_->value(KEY_USERENTRY_SELECTED);
//            QVariant vValChecked = s_->value(KEY_USERENTRY_CHECKED);
//            if(vValDir.isValid() && !vValDir.toString().isEmpty())
//            {
//                QString dir = vValDir.toString();
//                bool sel = vValSelected.toBool();
//                bool chk = vValChecked.toBool();
//                delist_.append(DE(dir,chk,sel));
//            }
//            s_->endGroup();
//        }
//        return true;
//    }
//    catch(QException&)
//    {
//        lastErr_ = tr("QException");
//        return false;
//    }
//    catch(...)
//    {
//        lastErr_ = tr("... Exception");
//        return false;
//    }
//    return false;
}

void Document::Store(QListWidget* pLW,
                     const QModelIndex& index)
{
	int normalCount = 0;
    for(int i=0 ; i <pLW->count();++i)
    {
        DirectoryItem* item = (DirectoryItem*)pLW->item(i);
        if(item->IsAllItem())
        {
			docSql_->setAllSelected(item->isSelected());
			docSql_->setAllChecked(item->checkState() == Qt::Checked);
            //s_->setValue(KEY_KEY_USERENTRY_DIRECTORY_ALL_SELECTED, item->isSelected());
            //s_->setValue(KEY_KEY_USERENTRY_DIRECTORY_ALL_CHECKED, item->checkState()==Qt::Checked);
        }
        else if(item->IsNormalItem())
        {
			normalCount++;
			docSql_->setDirectory(normalCount, item->text(), item->isSelected(), item->checkState() == Qt::Checked);
        }
        else if(item->IsMissingItem())
        {
            // nothing
        }
    }
	docSql_->removeDirectoryOver(normalCount);
    docSql_->SetLastPos(index.row(),index.column());
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

//    QVariant vRow = s_->value(KEY_TABLE_INDEX_ROW);
//    if(!vRow.isValid())
//        return 0;

//    return vRow.toInt();
}
//int Document::modeIndexColumn() const
//{
//    QVariant vColumn = s_->value(KEY_TABLE_INDEX_COLUMN);
//    if(!vColumn.isValid())
//        return 0;

//    return vColumn.toInt();
//}
