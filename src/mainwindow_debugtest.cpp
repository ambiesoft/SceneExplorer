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

#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"

#ifdef QT_DEBUG
void MainWindow::onDebugTest()
{
    QModelIndex topIndexInView = ui->tableView->indexAt(QPoint(0,0));
    QModelIndex bottomIndexInView = ui->tableView->indexAt(QPoint(0, ui->tableView->size().height()));
    qDebug() << "First visible row:" << topIndexInView;
    qDebug() << "Last visible row:" << bottomIndexInView;

    QModelIndex indexTop = ui->tableView->indexAt(ui->tableView->rect().topLeft());
    QModelIndex indexBottom = ui->tableView->indexAt(ui->tableView->rect().bottomLeft());
    qDebug() << "First visible row:" << indexTop;
    qDebug() << "Last visible row:" << indexBottom;

}
#endif
