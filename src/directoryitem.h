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
#include <QFileIconProvider>

class DirectoryItem : public QListWidgetItem
{
public:
    enum DirectoryItemType
    {
        DI_NORMAL_MY,
        DI_ALL_MY,
        DI_MISSING_MY,
    } ;
private:
    qint64 dirid_;
    DirectoryItemType itemType_;
    QFileIconProvider fiProvider_;

public:
    static QListWidget* parent_;
    DirectoryItem(const qint64& dirid,
                  DirectoryItemType itemType,
                  const QString& text);
    qint64 dirid() const {
        return dirid_;
    }
    bool IsNormalItem() const {
        return itemType_==DI_NORMAL_MY;
    }
    bool IsAllItem() const {
        return itemType_==DI_ALL_MY;
    }
    bool IsMissingItem() const {
        return itemType_==DI_MISSING_MY;
    }

    bool IsChecked() const {
        return checkState()==Qt::Checked;
    }
    void setChecked(bool bCheck) {
        setCheckState(bCheck ? Qt::Checked : Qt::Unchecked);
    }
    int IsSelectedInt() const {
        return isSelected() ? 1:0;
    }
    bool IsCheckedInt() const {
        return IsChecked() ? 1:0;
    }

};

#endif // DIRECTORYITEM_H
