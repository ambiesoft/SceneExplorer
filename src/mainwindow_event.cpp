#include <QListWidgetItem>

#include "consts.h"
#include "globals.h"

#include "waitcursor.h"

#include "settings.h"

#include "sql.h"

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


    // GetSqlAllSetTable(QStringList());
	directoryChangedCommon();
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

    Settings settings;
    if(!this->isMaximized() && !this->isMinimized())
    {
        settings.setValue(Consts::KEY_SIZE, this->size());
        settings.setValue(Consts::KEY_TREESIZE, ui->directoryWidget->size());
        settings.setValue(Consts::KEY_TXTLOGSIZE, ui->txtLog->size());
        settings.setValue(Consts::KEY_LISTTASKSIZE, ui->listTask->size());
    }
    settings.setValue(Consts::KEY_LASTSELECTEDDIRECTORY, lastSelectedDir_);

	settings.setValue(Consts::KEY_SHOWMISSING, btnShowNonExistant_->isChecked());

    QStringList userDirs;
    QList<QVariant> userSelecteds;
    QList<QVariant> userCheckeds;
    for(int i=0 ; i < ui->directoryWidget->count();++i)
    {
        DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->item(i);
        if(item->IsAllItem())
        {
            settings.setValue(Consts::KEY_KEY_USERENTRY_DIRECTORY_ALL_SELECTED, item->isSelected());
            settings.setValue(Consts::KEY_KEY_USERENTRY_DIRECTORY_ALL_CHECKED, item->checkState()==Qt::Checked);
        }
        else if(item->IsNormalItem())
        {
            userDirs.append(item->text());
            userSelecteds.append(item->isSelected());
            userCheckeds.append(item->checkState()==Qt::Checked);
        }
        else if(item->IsMissingItem())
        {
            // nothing
        }
    }
    settings.setValue(Consts::KEY_USERENTRY_DIRECTORIES, userDirs);
    settings.setValue(Consts::KEY_USERENTRY_SELECTED, userSelecteds);
    settings.setValue(Consts::KEY_USERENTRY_CHECKEDS, userCheckeds);
}
