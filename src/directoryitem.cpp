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
#include <QObject>

#include "directorycheckthread.h"
#include "directoryitem.h"
#include "mainwindow.h"

QListWidget* DirectoryItem::parent_;

DirectoryItem::DirectoryItem(const qint64& dirid,
                             DirectoryItemType itemType,
                             const QString& text,
                             const QString& displaytext) :
    QListWidgetItem(parent_),
    dirid_(dirid),
    itemType_(itemType)
{
    Q_ASSERT(parent_);

    setDirectory(text);
    setDisplayText(displaytext);

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
        this->setToolTip(text);
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
    if(!IsNormalItem())
        return;

    DirectoryCheckThread* pThread = new DirectoryCheckThread(directory());
    QObject::connect(pThread, &DirectoryCheckThread::finished_CheckDir,
                     this, &DirectoryItem::finished_CheckDir);
    pThread->start();
}
void DirectoryItem::finished_CheckDir(const bool bExist,DirectoryCheckThread* pThread)
{
    if(MainWindow::isClosing())
        return;
    static QBrush defaultFore = foreground();
    setForeground(bExist ? defaultFore : Qt::red);

    delete pThread;
}
