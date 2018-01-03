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
#include <QMessageBox>
#include <QWidget>

#include "ui_mainwindow.h"
#include "task.h"
#include "tablemodel.h"

#include "treemodel.h"
#include "itemdata.h"
#include "settings.h"

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(Consts::APPNAME);

    imageModel_=new TableModel(this);
    // QStandardItemModel* model = new QStandardItemModel;
    // ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(imageModel_);


    treeModel_ = new TreeModel;
    ui->treeView->setModel(treeModel_);

    pool_ = new QThreadPool();

    Settings settings;

    QVariant vVal;

    vVal = settings.value("size");
    if(vVal.isValid())
        resize(vVal.toSize());

    vVal = settings.value("lastselecteddir");
    if(vVal.isValid())
        lastSelectedDir_ = vVal.toString();

    show();
    vVal = settings.value("treesize");
    if(vVal.isValid())
        resizeDock(ui->dockTree, vVal.toSize());

    vVal = settings.value("txtlogsize");
    if(vVal.isValid())
        resizeDock(ui->dockLog, vVal.toSize());
}

MainWindow::~MainWindow()
{
    delete imageModel_;
    delete treeModel_;
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

void MainWindow::resizeDock(QDockWidget* dock, const QSize& size)
{
    // width
    switch(this->dockWidgetArea(dock))
    {
        case Qt::DockWidgetArea::LeftDockWidgetArea:
        case Qt::DockWidgetArea::RightDockWidgetArea:
        {
            QList<int> sizes;
            QList<QDockWidget*> docks;

            docks.append(dock);
            sizes.append(size.height());

            resizeDocks(docks,sizes,Qt::Orientation::Vertical);
        }
        break;

        case Qt::DockWidgetArea::TopDockWidgetArea:
        case Qt::DockWidgetArea::BottomDockWidgetArea:
        // height
        {
        QList<QDockWidget*> docks;
            QList<int> sizes;

            docks.append(dock);
            sizes.append(size.width());

            resizeDocks(docks,sizes,Qt::Orientation::Horizontal);
        }
        break;

    default:
        break;
    }
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeDock(ui->dockTree, ui->treeView->size());
    resizeDock(ui->dockLog, ui->txtLog->size());
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

    Settings settings;
    if(!this->isMaximized() && !this->isMinimized())
    {
        settings.setValue("size", this->size());
        settings.setValue("treesize", ui->treeView->size());
        settings.setValue("txtlogsize", ui->txtLog->size());
    }
    settings.setValue("lastselecteddir", lastSelectedDir_);
}

void MainWindow::on_action_About_triggered()
{
    QString title = Consts::APPNAME;
    QString text = Consts::APPNAME;
    text.append(" ");
    text.append("ver ");
    text.append(Consts::VERSION);
    text.append("\n");
    text.append("\n");
    text.append("copyright 2018 ");
    text.append(Consts::ORGANIZATION);
    QMessageBox::about(this,title,text);
}
