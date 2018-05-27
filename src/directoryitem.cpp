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
#include "directoryitem.h"

DirectoryItem::DirectoryItem(QListWidget* parent,
                             const qint64& dirid,
                             DirectoryItemType itemType,
                             const QString& text) :
    QListWidgetItem(parent),
    dirid_(dirid),
    itemType_(itemType)
{
    setFlags(flags() | Qt::ItemIsUserCheckable);
    setText(text);
    if(itemType==DirectoryItem::DI_ALL)
    {
        setIcon(QIcon(":resource/images/mailbox.png"));
    }
    else if(itemType==DirectoryItem::DI_NORMAL)
    {
        Q_ASSERT(dirid > 0);
        setIcon(fiProvider_.icon(QFileIconProvider::Folder));
    }
    else if(itemType==DirectoryItem::DI_MISSING)
    {
        setIcon(QIcon(":resource/images/missing.png"));
    }
    else
    {
        Q_ASSERT(false);
    }
}
