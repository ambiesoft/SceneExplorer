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

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "tagitem.h"
#include "helper.h"


void MainWindow::OnCopyTable()
{
    QString videoFile = getSelectedVideo(true);
    if(videoFile.isEmpty())
    {
        Alert(this, TR_NOVIDEO_SELECTED());
        return;
    }
    setClipboardText(videoFile);
}
void MainWindow::OnCopyDirectory()
{
    QList<QListWidgetItem*> sels = ui->directoryWidget->selectedItems();
    if(sels.isEmpty())
    {
        Alert(this, TR_NODIRECTORY_SELECTED());
        return;
    }
    DirectoryItem* di = (DirectoryItem*)sels[0];
    if(di->IsAllItem())
    {
        Alert(this, TR_ALLITEM_COULDNOTBE_COPIED());
        return;
    }
    if(di->IsMissingItem())
    {
        Alert(this, TR_MISSINGITEM_COULDNOTBE_COPIED());
        return;
    }
    if(!di->IsNormalItem())
    {
        Alert(this, TR_SELECTEDITEM_NOT_NORMALITEM());
        return;
    }
    setClipboardText(di->text());
}
void MainWindow::OnCopyTask()
{
    QStringList list;
    foreach(const QModelIndex &index,
            ui->listTask->selectionModel()->selectedIndexes())
    {
        list.append(index.data().toString());
    }
    setClipboardText(list.join("\n"));
}
void MainWindow::OnCopyLog()
{
    ui->txtLog->copy();
}
void MainWindow::OnCopyTag()
{
    QList<QListWidgetItem*> sels = ui->listTag->selectedItems();
    if(sels.isEmpty())
    {
        Alert(this, TR_NOTAG_SELECTED());
        return;
    }
    TagItem* ti = (TagItem*)sels[0];
    if(ti->IsAllItem())
    {
        Alert(this, TR_ALLITEM_COULDNOTBE_COPIED());
        return;
    }
    if(!ti->IsNormalItem())
    {
        Alert(this, TR_SELECTEDITEM_NOT_NORMALITEM());
        return;
    }

    QByteArray dirdata("aaa");
    QMimeData mimeDir;
    mimeDir.setData("text/SceneExplorerTag", dirdata);

    QClipboard* clip = QApplication::clipboard();
    clip->setText(ti->text());
    clip->setMimeData(&mimeDir);
}

void MainWindow::on_action_Copy_triggered()
{
    if(ui->tableView->hasFocus())
    {
        OnCopyTable();
        return;
    }
    else if(ui->directoryWidget->hasFocus())
    {
        OnCopyDirectory();
        return;
    }
    else if(ui->listTask->hasFocus())
    {
        OnCopyTask();
        return;
    }
    else if(ui->txtLog->hasFocus())
    {
        OnCopyLog();
        return;
    }
    else if(ui->listTag->hasFocus())
    {
        OnCopyTag();
        return;
    }
}


void MainWindow::OnUpdateCopyTable(QAction* pAction)
{
    pAction->setEnabled(ui->tableView->selectionModel()->hasSelection());
}
void MainWindow::OnUpdateCopyDirectory(QAction* pAction)
{
    pAction->setEnabled(!ui->directoryWidget->selectedItems().isEmpty());
}
void MainWindow::OnUpdateCopyTask(QAction* pAction)
{
    pAction->setEnabled(ui->listTask->selectionModel()->hasSelection());
}
void MainWindow::OnUpdateCopyLog(QAction* pAction)
{
    pAction->setEnabled(ui->txtLog->textCursor().hasSelection());
}
void MainWindow::OnUpdateCopyTag(QAction* pAction)
{
    pAction->setEnabled(!ui->listTag->selectedItems().isEmpty());
}

void MainWindow::OnUpdateEditCopy(QAction* pAction)
{
    if(ui->tableView->hasFocus())
    {
        OnUpdateCopyTable(pAction);
        return;
    }
    else if(ui->directoryWidget->hasFocus())
    {
        OnUpdateCopyDirectory(pAction);
        return;
    }
    else if(ui->listTask->hasFocus())
    {
        OnUpdateCopyTask(pAction);
        return;
    }
    else if(ui->txtLog->hasFocus())
    {
        OnUpdateCopyLog(pAction);
        return;
    }
    else if(ui->listTag->hasFocus())
    {
        OnUpdateCopyTag(pAction);
        return;
    }

}
