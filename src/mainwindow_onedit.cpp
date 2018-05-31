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

void MainWindow::onMenuEdit_AboutToShow()
{
    // open video menu
    ui->action_OpenVideo->setEnabled(
                ui->tableView->hasFocus() &&
                ui->tableView->selectionModel()->hasSelection());

    // open folder menu
    bool bOpenFolder = false;
    if(ui->tableView->hasFocus() && ui->tableView->selectionModel()->hasSelection())
        bOpenFolder = true;
    else if(ui->directoryWidget->hasFocus() && !ui->directoryWidget->selectedItems().isEmpty())
        bOpenFolder = true;
    ui->action_OpenFolder->setEnabled(bOpenFolder);

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
    ui->action_Select_All->setEnabled(bSelectAllEnable);

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
