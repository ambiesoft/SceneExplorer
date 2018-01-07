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

    ui->treeView->setMaximumSize(10000,10000);
    ui->txtLog->setMaximumSize(10000,10000);
    ui->listTask->setMaximumSize(10000,10000);

    QList<TableItemData*> v;
    if(!gpSQL->GetAll(v))
    {
        insertLog(TaskKind::SQL,0,tr("Failed to get data from database."));
        return;
    }

    for(int i=0 ; i < v.size(); ++i)
    {
        tableModel_->AppendData(v[i]);
        // setTableSpan();
    }

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
        settings.setValue(Consts::KEY_TREESIZE, ui->treeView->size());
        settings.setValue(Consts::KEY_TXTLOGSIZE, ui->txtLog->size());
        settings.setValue(Consts::KEY_LISTTASKSIZE, ui->listTask->size());
    }
    settings.setValue(Consts::KEY_LASTSELECTEDDIRECTORY, lastSelectedDir_);
}
