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
    tableSortParameterChanged(tableModel_->GetSortColumn(), tableModel_->GetSortReverse());
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

    settings_.setValue(Consts::KEY_SHOWMISSING, btnShowNonExistant_->isChecked());


    pDoc_->Store(ui->directoryWidget);

    settings_.setValue(Consts::KEY_MAX_GETDIR_THREADCOUNT, optionThreadcountGetDir_);
    settings_.setValue(Consts::KEY_MAX_THUMBNAIL_THREADCOUNT, optionThreadcountThumbnail_);
    settings_.setValue(Consts::KEY_THUMBNAIL_COUNT, optionThumbCount_);

    settings_.setValue(Consts::KEY_IMAGECACHETYPE, (int)tableModel_->GetImageCache());

    Extension::Save(settings_);

    for(int i=0 ; i < externalTools_.count(); ++i)
    {
        externalTools_[i].Save(i, settings_);
    }
    settings_.setValue(Consts::KEY_EXTERNALTOOLS_COUNT, externalTools_.count());

    // recents
    settings_.setValue(Consts::KEY_RECENT_OPENDOCUMENTS,recents_);
}
