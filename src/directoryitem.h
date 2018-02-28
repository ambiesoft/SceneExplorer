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

#ifndef DIRECTORYITEM_H
#define DIRECTORYITEM_H

#include <QListWidgetItem>

class DirectoryItem : public QListWidgetItem
{
public:
    enum DirectoryItemType
    {
        DI_NORMAL,
        DI_ALL,
        DI_MISSING,
    } ;
private:
    DirectoryItemType itemType_;

public:
    DirectoryItem(QListWidget* parent,
                  DirectoryItemType itemType) :
        QListWidgetItem(parent),
        itemType_(itemType)
    {

    }
    bool IsNormalItem() const {
        return itemType_==DI_NORMAL;
    }
    bool IsAllItem() const {
        return itemType_==DI_ALL;
    }
    bool IsMissingItem() const {
        return itemType_==DI_MISSING;
    }

    bool IsChecked() const {
        return checkState()==Qt::Checked;
    }
	
};

#endif // DIRECTORYITEM_H
