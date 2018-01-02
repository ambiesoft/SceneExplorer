#include <QThread>
#include <QProcess>
#include <QIODevice>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDirIterator>
#include <QThreadPool>
#include <QObject>
#include <QStandardItemModel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"
#include "imagemodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imageModel_=new ImageModel(this);
    QStandardItemModel* model = new QStandardItemModel;
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(model);
    // ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // ui->tableView->setItemDelegate(new ImageDelegate(this));
}

MainWindow::~MainWindow()
{
    delete imageModel_;
    delete ui;
}

void MainWindow::on_action_Close_triggered()
{
    this->close();
}

void MainWindow::sayGoodby(int,const QStringList& files)
{
    int newRow = ui->tableView->model()->rowCount();
    for(int i=0 ; i < 5 ; ++i)
    {
        QImage image(files[i]);
        QPixmap pix = QPixmap::fromImage(image);
        QStandardItem* item = new QStandardItem();
        item->setData(QVariant(pix), Qt::DecorationRole);
        ((QStandardItemModel*)ui->tableView->model())->setItem(newRow,i,item);
        ui->tableView->setColumnWidth(i, pix.size().width());
        ui->tableView->setRowHeight(newRow, pix.size().height());
    }


//    for (int c = 0; c < ui->tableView->horizontalHeader()->count(); ++c)
//    {
//        ui->tableView->horizontalHeader()->setSectionResizeMode(
//            c, QHeaderView::Stretch);
//    }

}
void MainWindow::resizeEvent(QResizeEvent *event)
{
//    int colcount = 5;
//    for(int i=0; i<colcount;++i)
//    {
//        ui->tableView->setColumnWidth(i, this->width()/colcount);
//    }
//    int rowcount = ui->tableView->model()->rowCount();
//    for(int i=0;i < rowcount;++i)
//    {
//        ui->tableView->setRowHeight(i, this->);
//    }
    QMainWindow::resizeEvent(event);
}

void MainWindow::on_action_Do_It_triggered()
{
    QThreadPool::globalInstance()->setMaxThreadCount(1);
    QDirIterator it("C:\\T", QDirIterator::Subdirectories);

    int id=0;
    while (it.hasNext()) {
        QString file = it.next();
        qDebug() << file;
        if(it.fileInfo().isFile())
        {
            TaskFFMpeg* pTask = new TaskFFMpeg(++id,file);
            pTask->setAutoDelete(true);
            QObject::connect(pTask, &TaskFFMpeg::sayGoodby, this, &MainWindow::sayGoodby);
            QThreadPool::globalInstance()->start(pTask);
        }
    }
}
