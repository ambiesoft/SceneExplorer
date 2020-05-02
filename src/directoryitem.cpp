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

#include <QIcon>
#include <QDir>
#include "directoryitem.h"

QListWidget* DirectoryItem::parent_;

DirectoryItem::DirectoryItem(const qint64& dirid,
                             DirectoryItemType itemType,
                             const QString& text) :
    QListWidgetItem(parent_),
    dirid_(dirid),
    itemType_(itemType)
{
    Q_ASSERT(parent_);
    setText(text);
    if(itemType==DirectoryItem::DI_ALL_MY)
    {
        setFlags(flags() & ~Qt::ItemIsUserCheckable);
        setIcon(QIcon(":resource/images/mailbox.png"));
    }
    else if(itemType==DirectoryItem::DI_NORMAL_MY)
    {
        Q_ASSERT(dirid > 0);
        setFlags(flags() | Qt::ItemIsUserCheckable);
        setChecked(false);
        setIcon(fiProvider_.icon(QFileIconProvider::Folder));
    }
    else if(itemType==DirectoryItem::DI_MISSING_MY)
    {
        setFlags(flags() & ~Qt::ItemIsUserCheckable);
        setIcon(QIcon(":resource/images/missing.png"));
    }
    else
    {
        Q_ASSERT(false);
    }
}

void DirectoryItem::Refresh()
{
    static QBrush defaultFore = foreground();

    if(IsNormalItem())
    {
        if(!QDir(text()).exists())
            setForeground(Qt::red);
        else
            setForeground(defaultFore);
    }
}
