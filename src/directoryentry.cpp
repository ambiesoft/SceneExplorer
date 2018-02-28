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

#include "directoryentry.h"
#include "directoryitem.h"

DirectoryItem* DirectoryEntry::getShowAllItem()
{
	for (int i = 0; i < count(); ++i)
	{
		DirectoryItem* item = (DirectoryItem*)this->item(i);
		if (item->IsAllItem())
			return item;
	}
	Q_ASSERT(false);
	return nullptr;
}

DirectoryItem* DirectoryEntry::takeShowAllItem()
{
	for (int i = 0; i < count(); ++i)
	{
		DirectoryItem* item = (DirectoryItem*)this->item(i);
		if (item->IsAllItem())
		{
			this->takeItem(i);
			return item;
		}
	}
	Q_ASSERT(false);
	return nullptr;
}
DirectoryItem* DirectoryEntry::takeMissingItem()
{
	for (int i = 0; i < count(); ++i)
	{
		DirectoryItem* item = (DirectoryItem*)this->item(i);
		if (item->IsMissingItem())
		{
			this->takeItem(i);
			return item;
		}
	}
	return nullptr;
}
void DirectoryEntry::addItem(DirectoryItem* item)
{
	DirectoryItem* missingItem = takeMissingItem();
	QListWidget::addItem(item);
	if(missingItem)
		QListWidget::addItem(missingItem);
}
QList<DirectoryItem*> DirectoryEntry::GetCheckedItems()
{
    QList<DirectoryItem*> ret;
    for (int i = 0; i < count(); ++i)
    {
        DirectoryItem* item = (DirectoryItem*)this->item(i);
        if (item->IsNormalItem() && item->IsChecked())
        {
            ret.append(item);
        }
    }
    return ret;
}
QList<DirectoryItem*> DirectoryEntry::GetAllNormalItems()
{
	QList<DirectoryItem*> ret;
	for (int i = 0; i < count(); ++i)
	{
		DirectoryItem* item = (DirectoryItem*)this->item(i);
		if (item->IsNormalItem())
		{
			ret.append(item);
		}
	}
	return ret;
}
void DirectoryEntry::SetCheck(const QStringList& dirs, bool bRemoveSelection)
{
    for (int i = 0; i < count(); ++i)
    {
        DirectoryItem* item = (DirectoryItem*)this->item(i);

        if(bRemoveSelection)
            item->setSelected(false);

        if (item->IsNormalItem())
        {
            item->setCheckState(dirs.contains(item->text()) ?
                                 Qt::Checked : Qt::Unchecked);
        }
    }
}
bool DirectoryEntry::IsAllItemSelectedOrChecked() const
{
    for (int i = 0; i < count(); ++i)
    {
        DirectoryItem* item = (DirectoryItem*)this->item(i);

        if (item->IsAllItem())
        {
            if(item->isSelected())
                return true;
            if(item->IsChecked())
                return true;

            return false;
        }
    }
    Q_ASSERT(false);
    return false;
}
bool DirectoryEntry::IsMissingItemSelectedOrChecked() const
{
    for (int i = 0; i < count(); ++i)
    {
        DirectoryItem* item = (DirectoryItem*)this->item(i);

        if (item->IsMissingItem())
        {
            if(item->isSelected())
                return true;
            if(item->IsChecked())
                return true;

            return false;
        }
    }
    Q_ASSERT(false);
    return false;
}
bool DirectoryEntry::IsTopNormalItem(int row) const
{
    if(count() < row)
        return false;

    Q_ASSERT(((DirectoryItem*)item(0))->IsAllItem());
    return row==1;
}
bool DirectoryEntry::IsBottomNormalItem(int row) const
{
    if(count() < row)
        return false;

    Q_ASSERT(((DirectoryItem*)item(count()-1))->IsMissingItem());
    return row==count()-2;
}
void DirectoryEntry::SortNormalItems()
{
    DirectoryItem* itemAll = this->takeShowAllItem();
    Q_ASSERT(itemAll);
    DirectoryItem* itemMissing = this->takeMissingItem();
    Q_ASSERT(itemMissing);
    sortItems();
    insertItem(0, itemAll);
    addItem(itemMissing);
}
