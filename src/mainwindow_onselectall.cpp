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

#include "../../lsMisc/stdQt/stdQt.h"

#include "helper.h"

#include "tagitem.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

using namespace AmbiesoftQt;

void MainWindow::on_action_SelectAll_triggered()
{
    if(ui->directoryWidget->hasFocus())
    {
        Ambiesoft::BlockedBool bd(&directoryChanging_);
        {
            for(int i=0 ; i < ui->directoryWidget->count();++i)
            {
                DirectoryItem* di = (DirectoryItem*) ui->directoryWidget->item(i);
                if(di->IsAllItem())
                    di->setSelected(false);
                else if(di->IsNormalItem())
                    di->setSelected(true);
                else if(di->IsMissingItem())
                    di->setSelected(false);
                else
                    Q_ASSERT(false);
            }
        }
        itemChangedCommon(true);
        return;
    }

    if(ui->listTag->hasFocus())
    {
        Ambiesoft::BlockedBool bt(&tagChanging_);
        {
            for(int i=0 ; i < ui->listTag->count(); ++i)
            {
                TagItem* ti = (TagItem*)ui->listTag->item(i);
                if(ti->IsAllItem())
                    ti->setSelected(false);
                else if(ti->IsNormalItem())
                    ti->setSelected(true);
                else if(ti->IsNotagItem())
                    ti->setSelected(false);
                else
                    Q_ASSERT(false);
            }
        }
        itemChangedCommon(true);
        return;
    }

    if(ui->txtLog->hasFocus())
    {
        ui->txtLog->selectAll();
        return;
    }

    if(ui->listTask->hasFocus())
    {
        ui->listTask->selectAll();
        return;
    }
}

DirectoryItem* MainWindow::getDeepestDirectory(const QString& videoFile) const
{
    if(videoFile.isEmpty())
        return nullptr;
    QString videoDir = QFileInfo(videoFile).absolutePath();

    std::function<DirectoryItem*(bool bOnlySelected)> searchfunc = [&](bool bOnlySelected) {
        DirectoryItem* foundItem=nullptr;
        for (int i = 0; i < ui->directoryWidget->count(); ++i)
        {
            DirectoryItem* item = static_cast<DirectoryItem*>(ui->directoryWidget->item(i));
            if(!item->IsNormalItem())
                continue;

            if(bOnlySelected)
                if(!item->isSelected())
                    continue;
            if(isSamePath(item->directory(), videoDir))
                return item;
            if(IsSubDir(item->directory(), videoDir))
            {
                if(!foundItem)
                {
                    foundItem = item;
                }
                else
                {
                    if(item->directory().length() > foundItem->directory().length())
                    {
                        foundItem=item;
                    }
                }
            }
        }
        return foundItem;
    };

    DirectoryItem* retItem = searchfunc(true);
    if(retItem)
        return retItem;
    return searchfunc(false);
}
bool MainWindow::isDeepestDirectorySelected(const QString& videoFile) const
{
    DirectoryItem* foundItem = getDeepestDirectory(videoFile);
    if(!foundItem)
        return false;
    return foundItem->isSelected();
}
void MainWindow::on_action_SelectDeepestDirectory_triggered()
{
    QString videoFile = getSelectedVideo();
    if(videoFile.isEmpty())
    {
        Alert(this, TR_NOVIDEO_SELECTED());
        return;
    }

    // Find deepest directory
    DirectoryItem* foundItem = getDeepestDirectory(videoFile);

    if(!foundItem)
    {
        Alert(this, tr("There are no directories which hold the selected video."));
        return;
    }

    const bool bNewSelect = foundItem->isSelected() ? false : true;
    if(!bNewSelect)
    {
        // will deselect
        if(ui->directoryWidget->selectedOrCheckedItems().count() <= 1)
        {
            // but only this
            // should not deselect all
            return;
        }
    }
    Q_ASSERT(!directoryChanging_);
    Ambiesoft::BlockedBool bt(&directoryChanging_);
    foundItem->setSelected(bNewSelect);
}

