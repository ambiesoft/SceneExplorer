#include "taskmodel.h"
#include "tableitemdata.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::sayHello(int id,
               const QString& movieFile)
{
    taskModel_->SetProgress(id, TaskFFMpeg::Progressing);
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
//    if(false)
//    {
//        int newRow = ui->tableView->model()->rowCount();

//        QStandardItem* infoItem = new QStandardItem();
//        infoItem->setText(movieFile);
//        ((QStandardItemModel*)ui->tableView->model())->setItem(newRow,0,infoItem);
//        ui->tableView->setSpan(newRow,0,1,5);
//        ++newRow;

//        int i;
//        for(i=0 ; i < 5 ; ++i)
//        {
//            QImage image(files[i]);
//            QPixmap pix = QPixmap::fromImage(image);

//            QStandardItem* item = new QStandardItem();
//            item->setData(QVariant(pix), Qt::DecorationRole);
//            ((QStandardItemModel*)ui->tableView->model())->setItem(newRow,i,item);

//            ui->tableView->setColumnWidth(i, width);
//            ui->tableView->setRowHeight(newRow, height);
//        }
//    }


    int newRowFilename = ui->tableView->model()->rowCount();
    int newRowInfo = newRowFilename+1;
    int newRowImage = newRowFilename+2;

    ui->tableView->setSpan(newRowFilename,0,1,5);
    ui->tableView->setSpan(newRowInfo,0,1,5);
    ui->tableView->resizeRowToContents(newRowFilename);
    ui->tableView->resizeRowToContents(newRowInfo);

    tableModel_->AppendData(new TableItemData(files, width, height, movieFile, format));

    for(int i=0 ; i < 5 ; ++i)
    {
        ui->tableView->setColumnWidth(i, width);
        ui->tableView->setRowHeight(newRowImage, height);
    }

    insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Done"), movieFile));


    //listModel_->AppendData(new ListItemData(files, width, height, movieFile));
    //ui->listView->update(ui->listView->model()->index(0,0));
    // ui->listView->dataChanged( model()->dataChanged(QModelIndex(),QModelIndex());
}
void MainWindow::sayDead(int id)
{
    taskModel_->RemoveTask(id);
}
