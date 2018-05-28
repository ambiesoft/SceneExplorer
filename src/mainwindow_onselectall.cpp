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


#include "tagitem.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::on_action_Select_All_triggered()
{
    if(ui->directoryWidget->hasFocus())
    {
        BlockedBool bd(&directoryChanging_);
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
        BlockedBool bt(&tagChanging_);
        {
            for(int i=0 ; i < ui->listTag->count(); ++i)
            {
                TagItem* ti = (TagItem*)ui->listTag->item(i);
                if(ti->IsAllItem())
                    ti->setSelected(false);
                else if(ti->IsNormalItem())
                    ti->setSelected(true);
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
