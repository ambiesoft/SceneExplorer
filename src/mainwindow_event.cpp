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

#include <QListWidgetItem>

#include "consts.h"
#include "globals.h"

#include "waitcursor.h"

#include "settings.h"

#include "sql.h"
#include "extension.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::showEvent( QShowEvent* event )
{
    QMainWindow::showEvent( event );

    if(initShown)
        return;
    initShown=true;

    ui->directoryWidget->setMaximumSize(10000,10000);
    ui->txtLog->setMaximumSize(10000,10000);
    ui->listTask->setMaximumSize(10000,10000);

    directoryChangedCommon();
    tableSortParameterChanged(sortManager_.GetCurrentSort(), sortManager_.GetCurrentRev());  // tableModel_->GetSortColumn(), tableModel_->GetSortReverse());
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeDock(ui->dockTree, ui->directoryWidget->size());
    resizeDock(ui->dockLog, ui->txtLog->size());
    resizeDock(ui->dockTask, ui->listTask->size());
    QMainWindow::resizeEvent(event);
}
void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);

    gPaused = false;
    WaitCursor wc;

    clearAllPool();

    if(!this->isMaximized() && !this->isMinimized())
    {
        settings_.setValue(Consts::KEY_SIZE, this->size());
        settings_.setValue(Consts::KEY_TREESIZE, ui->directoryWidget->size());
        settings_.setValue(Consts::KEY_TXTLOGSIZE, ui->txtLog->size());
        settings_.setValue(Consts::KEY_LISTTASKSIZE, ui->listTask->size());
    }
    settings_.setValue(Consts::KEY_LASTSELECTEDDIRECTORY, lastSelectedDir_);

    QStringList findtexts;
    for(int i=0; i < cmbFind_->count(); ++i)
    {
        findtexts << cmbFind_->itemText(i);
    }
    settings_.setValue(Consts::KEY_COMBO_FINDTEXTS, findtexts);

    settings_.setValue(Consts::KEY_SHOWMISSING, tbShowNonExistant_->isChecked());


    pDoc_->Store(ui->directoryWidget);

    settings_.setValue(Consts::KEY_MAX_GETDIR_THREADCOUNT, optionThreadcountGetDir_);
    settings_.setValue(Consts::KEY_MAX_THUMBNAIL_THREADCOUNT, optionThreadcountThumbnail_);
    settings_.setValue(Consts::KEY_THUMBNAIL_COUNT, optionThumbCount_);

    settings_.setValue(Consts::KEY_TITLE_TEXT_TEMPLATE, tableModel_->GetTitleTextTemplate());
    settings_.setValue(Consts::KEY_INFO_TEXT_TEMPLATE, tableModel_->GetInfoTextTemplate());
    settings_.setValue(Consts::KEY_IMAGECACHETYPE, (int)tableModel_->GetImageCache());

    Extension::Save(settings_);

    for(int i=0 ; i < externalTools_.count(); ++i)
    {
        externalTools_[i].Save(i, settings_);
    }
    settings_.setValue(Consts::KEY_EXTERNALTOOLS_COUNT, externalTools_.count());

    // recents
    settings_.setValue(Consts::KEY_RECENT_OPENDOCUMENTS,recents_);

	// sort
	settings_.setValue(Consts::KEY_SORT, (int)sortManager_.GetCurrentSort());
	settings_.setValue(Consts::KEY_SORTREV, sortManager_.GetCurrentRev());
}
