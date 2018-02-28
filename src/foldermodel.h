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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QDirModel>
#include <QSet>

#define MYFOLDERMODEL QDirModel
class FolderModel : public MYFOLDERMODEL
{
public:
    QSet<QPersistentModelIndex> checkedIndexes_;
    QSet<QPersistentModelIndex> partcheckedIndexes_;

    FolderModel();

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return 1;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        Q_UNUSED(section);
        Q_UNUSED(orientation);
        Q_UNUSED(role);
        return QVariant();
    }
    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        // return QFileSystemModel::flags(index) | Qt::ItemIsUserCheckable;
        return MYFOLDERMODEL::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsUserTristate;
    }

private:
    bool recursiveCheck(const QModelIndex &index, const QVariant &value);
};

#endif // TREEMODEL_H
