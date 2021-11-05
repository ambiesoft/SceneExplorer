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

#include "../../lsMisc/stdosd/blockedbool.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::showEvent( QShowEvent* event )
{
    QMainWindow::showEvent( event );

    if(initShown_)
        return;

    Ambiesoft::BlockedBool bbInit(&initShown_, false, true);

    ui->directoryWidget->setMaximumSize(10000,10000);
    ui->txtLog->setMaximumSize(10000,10000);
    ui->listTask->setMaximumSize(10000,10000);
    ui->listTag->setMaximumSize(10000,10000);

    itemChangedCommon();
    tableSortParameterChanged(sortManager_.GetCurrentSort(), sortManager_.IsSortAscending());  // tableModel_->GetSortColumn(), tableModel_->GetSortReverse());

    OnTableViewScrollChanged(-1);

    int row = 0;
    int column = 0;
    if(pDoc_ && pDoc_->getLastPos(row,column))
    {
        QModelIndex mi = tableModel_->index(row,column);
        ui->tableView->selectionModel()->setCurrentIndex(mi, QItemSelectionModel::ClearAndSelect);

        tableModel_->ensureIndex(mi);
        QApplication::processEvents();
        ui->tableView->scrollTo(mi);

        OnTableViewScrollChanged(-1);

        tableModel_->ensureIndex(mi);
        QApplication::processEvents();
        ui->tableView->scrollTo(mi);
    }

    RefreshDirectoryTree();

    tableModel_->setScrolled(false);
}

void MainWindow::OnTableViewScrollChanged(int pos)
{
    QModelIndex indexTop = ui->tableView->indexAt(ui->tableView->rect().topLeft());
    QModelIndex indexBottom = ui->tableView->indexAt(ui->tableView->rect().bottomLeft());
    int rowCountPerScreen = indexBottom.row()-indexTop.row()+1;

    static auto func = [&](int i){
        tableModel_->data(tableModel_->index(i,0), Qt::DecorationRole);
        tableModel_->data(tableModel_->index(i,0), Qt::DisplayRole);
    };
    int top = qMax(0, indexTop.row()-rowCountPerScreen);
    int topEnd = pos < 0 ? indexBottom.row() : indexTop.row();
    for(int i=top ; i <= topEnd; ++i)
    {
        func(i);
    }

    int rowBottom = indexBottom.row();
    int maxBottom = rowBottom + rowCountPerScreen; // upto next page
    for(int i=rowBottom; i < maxBottom; ++i)
    {
        func(i);
    }

    tableModel_->setScrolled();
}
