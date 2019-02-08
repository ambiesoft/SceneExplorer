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

#include "helper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

QList<QAction*> MainWindow::getAllSortAction()
{
    QList<QAction*> rets;
    rets << ui->actionSort_by_file_name;
    rets << ui->actionSort_by_full_name;
    rets << ui->actionSort_by_file_size;
    rets << ui->actionSort_by_wtime;
    rets << ui->actionSort_by_resolution;
    rets << ui->actionSort_by_duration;
    rets << ui->actionSort_by_bitrate;
    rets << ui->actionSort_by_open_count;
    rets << ui->actionSort_by_last_access;

    return rets;
}
void MainWindow::onMenuView_AboutToShow()
{
    // these are done in sortManager


    //    for(QAction* pSortAction : getAllSortAction())
    //    {
    //        pSortAction->setChecked(false);
    //    }

    //    switch(sortManager_.GetCurrentSort())
    //    {
    //    case SORT_NONE:
    //        break;
    //    case SORT_FILENAME:
    //        ui->actionSort_by_file_name->setChecked(true);
    //        break;
    //    case SORT_FULLNAME:
    //        ui->actionSort_by_full_name->setChecked(true);
    //        break;
    //    case SORT_SIZE:
    //        ui->actionSort_by_file_size->setChecked(true);
    //        break;
    //    case SORT_WTIME:
    //        ui->actionSort_by_wtime->setChecked(true);
    //        break;
    //    case SORT_RESOLUTION:
    //        ui->actionSort_by_resolution->setChecked(true);
    //        break;
    //    case SORT_DURATION:
    //        ui->actionSort_by_duration->setChecked(true);
    //        break;
    //    case SORT_BITRATE:
    //        ui->actionSort_by_bitrate->setChecked(true);
    //        break;
    //    case SORT_OPENCOUNT:
    //        ui->actionSort_by_open_count->setChecked(true);
    //        break;
    //    case SORT_LASTACCESS:
    //        ui->actionSort_by_last_access->setChecked(true);
    //        break;
    //    default:
    //        Q_ASSERT(false);
    //    }
}
void MainWindow::onMenuEdit_AboutToShow()
{
    // open video menu
    ui->action_OpenVideo->setEnabled(
                ui->tableView->hasFocus() &&
                ui->tableView->selectionModel()->hasSelection());

    // open directory menu
    bool bOpenDirectory = false;
    if(ui->tableView->hasFocus() && ui->tableView->selectionModel()->hasSelection())
        bOpenDirectory = true;
    else if(ui->directoryWidget->hasFocus() && !ui->directoryWidget->selectedItems().isEmpty())
        bOpenDirectory = true;
    ui->action_OpenDirectory->setEnabled(bOpenDirectory);

    // copy
    OnUpdateEditCopy(ui->action_Copy);

    // paste
    bool bPasteEnable = false;
    if(ui->directoryWidget->hasFocus())
        bPasteEnable=true;
    else if(ui->listTag->hasFocus())
        bPasteEnable=true;
    ui->action_Paste->setEnabled(bPasteEnable);

    // select all
    bool bSelectAllEnable = bPasteEnable;
    if(!bSelectAllEnable)
    {
        if(ui->txtLog->hasFocus())
            bSelectAllEnable=true;
        else if(ui->listTask->hasFocus())
            bSelectAllEnable=true;
    }
    ui->action_SelectAll->setEnabled(bSelectAllEnable);

    // clear
    ui->action_ClearContent->setEnabled(ui->txtLog->hasFocus());
}

void MainWindow::on_action_ClearContent_triggered()
{
    if(ui->txtLog->hasFocus())
    {
        if(!YesNo(this,tr("Are you sure you want to clear the Output log?")))
            return;

        ui->txtLog->clear();
    }
}
