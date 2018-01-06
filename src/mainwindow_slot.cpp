#include "taskmodel.h"
#include "tableitemdata.h"

#include "globals.h"
#include "sql.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::sayHello(int id,
               const QString& movieFile)
{
    taskModel_->SetProgress(id, TaskFFmpeg::Progressing);
    insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Started"), movieFile));
}
void MainWindow::sayNo(int id,
               const QString& movieFile)
{
    insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Not a video"), movieFile));
}
void MainWindow::sayGoodby(int id,
                           const QStringList& files,
                           int width,
                           int height,
                           const QString& movieFile,
                           const QString& format)
{


    tableModel_->AppendData(new TableItemData(files, width, height, movieFile, format));
    int sqlError = gpSQL->AppendData(files, width, height, movieFile, format);
    if(sqlError==0)
    {
        insertLog(TaskKind::SQL, id, QString("%1 \"%2\"").arg(tr("Written in Database"), movieFile));
    }
    else
    {
        insertLog(TaskKind::SQL, id, QString("%1 \"%2\"").arg(tr("Failed to write on Database"), movieFile));
    }

    setTableSpan();
    insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Done"), movieFile));


    //listModel_->AppendData(new ListItemData(files, width, height, movieFile));
    //ui->listView->update(ui->listView->model()->index(0,0));
    // ui->listView->dataChanged( model()->dataChanged(QModelIndex(),QModelIndex());
}
void MainWindow::sayDead(int id)
{
    taskModel_->RemoveTask(id);
}
