#include <QFileInfo>
#include <QDateTime>

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
                           const QString& movieFile,
                           int thumbwidth,
                           int thumbheight,
                           const double& duration,
                           const QString& format,
                           const QString& vcodec,
                           const QString& acodec,
                           int vWidth,int vHeight)
{
    QFileInfo fi(movieFile);

    TableItemData* pTID = new TableItemData(
                            files,
                            fi.absolutePath(),
                            fi.fileName(),
                            fi.size(),
                            fi.birthTime().toSecsSinceEpoch(),
                            fi.lastModified().toSecsSinceEpoch(),
                            thumbwidth,thumbheight,
                            duration,
                            format,
                            vcodec,acodec,
                            vWidth,vHeight
                );

    tableModel_->AppendData(pTID);

    int sqlError = gpSQL->AppendData(*pTID);
    if(sqlError==0)
    {
        insertLog(TaskKind::SQL, id, QString("%1 \"%2\"").arg(tr("Written in Database"), movieFile));
    }
    else
    {
        insertLog(TaskKind::SQL, id, QString("%1 \"%2\"").arg(tr("Failed to write on Database"), movieFile));
    }

    // setTableSpan();
    insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Done"), movieFile));


    //listModel_->AppendData(new ListItemData(files, width, height, movieFile));
    //ui->listView->update(ui->listView->model()->index(0,0));
    // ui->listView->dataChanged( model()->dataChanged(QModelIndex(),QModelIndex());
}
void MainWindow::sayDead(int id)
{
    taskModel_->RemoveTask(id);
}
