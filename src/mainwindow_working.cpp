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

#include "../../lsMisc/blockedbool.h"

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
    tableSortParameterChanged(sortManager_.GetCurrentSort(), sortManager_.GetCurrentRev());  // tableModel_->GetSortColumn(), tableModel_->GetSortReverse());


    OnTableViewScrollChanged(-1);

    // Alert(this, QString("ScrollIndex:%1-%2").arg(pDoc_->modeIndexRow()).arg(pDoc_->modeIndexColumn()));
    int row = 0;
    int column = 0;
    if(pDoc_ && pDoc_->getLastPos(row,column))
    {
        QModelIndex mi = proxyModel_->index(row,column);
        // Alert(this, QString("ScrollIndex:%1-%2").arg(mi.row()).arg(mi.column()));


        // ui->tableView->selectionModel()->select(mi,	QItemSelectionModel::ClearAndSelect);
        ui->tableView->selectionModel()->setCurrentIndex(mi, QItemSelectionModel::ClearAndSelect);

        proxyModel_->ensureIndex(mi);
        QApplication::processEvents();
        ui->tableView->scrollTo(mi);

        OnTableViewScrollChanged(-1);


        proxyModel_->ensureIndex(mi);
        QApplication::processEvents();
        ui->tableView->scrollTo(mi);
    }
}

void MainWindow::OnTableViewScrollChanged(int pos)
{
//    if(tableModel_->GetImageCache() != ImageCacheType::IC_NEVER)
//        return;
//    QAbstractItemModel* sourceModel = proxyModel_->sourceModel();
//    Q_ASSERT(proxyModel==proxyModel_);

    QModelIndex indexTop = ui->tableView->indexAt(ui->tableView->rect().topLeft());
    QModelIndex indexBottom = ui->tableView->indexAt(ui->tableView->rect().bottomLeft());
    int rowCountPerScreen = indexBottom.row()-indexTop.row()+1;

    static auto func = [&](int i){
//        QModelIndex indexProxy = proxyModel_->index(i,0);
//        QModelIndex indexSource = proxyModel_->mapToSource(indexProxy);
//        tableModel_->data(indexSource, Qt::DecorationRole);
//        tableModel_->data(indexSource, Qt::DisplayRole);
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
//        QModelIndex mi = proxyModel->index(i,0);
//        proxyModel->data(mi, Qt::DecorationRole);
//        proxyModel->data(mi, Qt::DisplayRole);
        // ui->tableView->resizeRowToContents(mi.row());
    }
}
