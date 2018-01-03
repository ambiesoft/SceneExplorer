#include <QThread>
// #include <QProcess>
// #include <QIODevice>
#include <QDebug>
// #include <QJsonDocument>
// #include <QJsonObject>
#include <QDirIterator>
#include <QThreadPool>
#include <QObject>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"
#include "tablemodel.h"

//#include "listmodel.h"
#include "itemdata.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imageModel_=new TableModel(this);
    // QStandardItemModel* model = new QStandardItemModel;
    // ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(imageModel_);



//    listModel_ = new ListModel(this);
//    ui->listView->setModel(listModel_);

    pool_ = new QThreadPool();

    QSettings settings;
    QVariant vVal;

    vVal = settings.value("size");
    if(!vVal.Invalid)
        resize(vVal.toSize());

    vVal = settings.value("lastselecteddir");
    if(!vVal.Invalid)
        lastSelectedDir_ = vVal.toString();
}

MainWindow::~MainWindow()
{
    delete imageModel_;
    // delete listModel_;

    delete ui;
}

void MainWindow::on_action_Close_triggered()
{
    delete pool_;
    this->close();
}

void MainWindow::sayGoodby(int,
                           const QStringList& files,
                           int width,
                           int height,
                           const QString& movieFile,
                           const QString& format)
{
    if(false)
    {
        int newRow = ui->tableView->model()->rowCount();

        QStandardItem* infoItem = new QStandardItem();
        infoItem->setText(movieFile);
        ((QStandardItemModel*)ui->tableView->model())->setItem(newRow,0,infoItem);
        ui->tableView->setSpan(newRow,0,1,5);
        ++newRow;

        int i;
        for(i=0 ; i < 5 ; ++i)
        {
            QImage image(files[i]);
            QPixmap pix = QPixmap::fromImage(image);

            QStandardItem* item = new QStandardItem();
            item->setData(QVariant(pix), Qt::DecorationRole);
            ((QStandardItemModel*)ui->tableView->model())->setItem(newRow,i,item);

            ui->tableView->setColumnWidth(i, width);
            ui->tableView->setRowHeight(newRow, height);
        }
    }
    else
    {
        int newRowFilename = ui->tableView->model()->rowCount();
        int newRowInfo = newRowFilename+1;
        int newRowImage = newRowFilename+2;

        ui->tableView->setSpan(newRowFilename,0,1,5);
        ui->tableView->setSpan(newRowInfo,0,1,5);
        ui->tableView->resizeRowToContents(newRowFilename);
        ui->tableView->resizeRowToContents(newRowInfo);

        imageModel_->AppendData(new ItemData(files, width, height, movieFile, format));

        for(int i=0 ; i < 5 ; ++i)
        {
            ui->tableView->setColumnWidth(i, width);
            ui->tableView->setRowHeight(newRowImage, height);
        }

    }

    //listModel_->AppendData(new ListItemData(files, width, height, movieFile));
    //ui->listView->update(ui->listView->model()->index(0,0));
    // ui->listView->dataChanged( model()->dataChanged(QModelIndex(),QModelIndex());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::on_action_Do_It_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),lastSelectedDir_);
    if(dir.isEmpty())
        return;
    lastSelectedDir_ = dir;

    pool_->setMaxThreadCount(4);
    QDirIterator it(dir, QDirIterator::Subdirectories);

    int id=0;
    while (it.hasNext()) {
        QString file = it.next();
        qDebug() << file;
        if(it.fileInfo().isFile())
        {
            TaskFFMpeg* pTask = new TaskFFMpeg(++id,file);
            pTask->setAutoDelete(true);
            QObject::connect(pTask, &TaskFFMpeg::sayGoodby, this, &MainWindow::sayGoodby);
            pool_->start(pTask);
        }
    }
}
void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);

    QSettings settings;
    if(!this->isMaximized() && !this->isMinimized())
    {
        settings.setValue("size", this->size());
    }
    settings.setValue("lastselecteddir", lastSelectedDir_);
}
