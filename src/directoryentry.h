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

#ifndef DIRECTORYENTRY_H
#define DIRECTORYENTRY_H

#include <QListWidget>

#include "directoryitem.h"

class DirectoryEntry : public QListWidget
{
public:
    DirectoryEntry(QWidget *parent = Q_NULLPTR):
        QListWidget(parent)
    {}
	DirectoryItem* getShowAllItem();
	DirectoryItem* takeShowAllItem();
	DirectoryItem* takeMissingItem();
	
	void addItem(DirectoryItem* item);


    QList<DirectoryItem*> GetCheckedItems();
	QList<DirectoryItem*> GetAllNormalItems();
    void SetCheck(const QStringList& dirs, bool bRemoveSelection);

    bool IsAllItemSelectedOrChecked() const;
    bool IsMissingItemSelectedOrChecked() const;

    bool IsTopNormalItem(int row) const;
    bool IsBottomNormalItem(int row) const;

    void SortNormalItems();

    QList<DirectoryItem*> selectedOrCheckedItems();
};

#endif // DIRECTORYENTRY_H
