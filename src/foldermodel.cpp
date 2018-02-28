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

#include "foldermodel.h"

FolderModel::FolderModel()
{
    setFilter( QDir::Dirs | QDir::NoDotAndDotDot );
    // setRootPath("");
    setReadOnly(true);
}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::CheckStateRole)
    {
        if(checkedIndexes_.contains(index))
            return Qt::Checked;
        else if(partcheckedIndexes_.contains(index))
            return Qt::PartiallyChecked;
        else
            return Qt::Unchecked;
    }
    else if(role==Qt::DisplayRole)
    {
        return MYFOLDERMODEL::data(index, role);
    }
    return MYFOLDERMODEL::data(index, role);
}
bool FolderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::CheckStateRole)
    {
        if(value == Qt::Checked)
        {
            checkedIndexes_.insert(index);
            partcheckedIndexes_.remove(index);
            // recursiveCheck(index, value);
        }
        else if(value == Qt::PartiallyChecked)
        {
            checkedIndexes_.remove(index);
            partcheckedIndexes_.insert(index);
        }
        else
        {
            checkedIndexes_.remove(index);
            partcheckedIndexes_.remove(index);
            // recursiveCheck(index, value);
        }
        emit dataChanged(index, index);
        return true;
    }
    return MYFOLDERMODEL::setData(index, value, role);
}
bool FolderModel::recursiveCheck(const QModelIndex &index, const QVariant &value)
{
    if(hasChildren(index))
    {
        int i;
        int childrenCount = rowCount(index);
        QModelIndex child;
        for(i=0; i<childrenCount; i++)
        {
            child = MYFOLDERMODEL::index(i, 0, index);
            setData(child, value, Qt::CheckStateRole);
        }
    }
    return true;
}
