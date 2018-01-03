#include <QDebug>
#include <QDesktopServices>
#include <QDirIterator>
#include <QFileDialog>
#include <QMessageBox>
#include <QObject>
#include <QProcess>
#include <QScrollBar>
#include <QSizePolicy>
#include <QStandardItemModel>
#include <QThread>
#include <QThreadPool>
#include <QTime>
#include <QUrl>
#include <QWidget>

#include "ui_mainwindow.h"
#include "taskgetdir.h"
#include "taskffmpeg.h"

#include "tablemodel.h"
#include "treemodel.h"
#include "itemdata.h"
#include "settings.h"
#include "waitcursor.h"

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Settings& settings) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(Consts::APPNAME);

    tableModel_=new TableModel(this);
    // QStandardItemModel* model = new QStandardItemModel;
    // ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(tableModel_);


    treeModel_ = new TreeModel;
    ui->treeView->setModel(treeModel_);

    poolGetDir_ = new QThreadPool();
    poolGetDir_->setMaxThreadCount(1);

    poolFFMpeg_ = new QThreadPool();
    int threadcount = QThread::idealThreadCount()-2;
    if(threadcount <= 0)
        threadcount=0;
    qDebug() << QString("threadcount=%1").arg(threadcount);
    poolFFMpeg_->setMaxThreadCount(threadcount);


    QVariant vVal;

    vVal = settings.value("size");
    if(vVal.isValid())
        resize(vVal.toSize());

    vVal = settings.value("lastselecteddir");
    if(vVal.isValid())
        lastSelectedDir_ = vVal.toString();

    vVal = settings.value("treesize");
    if(vVal.isValid())
        ui->treeView->setMaximumSize(vVal.toSize());

    vVal = settings.value("txtlogsize");
    if(vVal.isValid())
        ui->txtLog->setMaximumSize(vVal.toSize());


}

MainWindow::~MainWindow()
{

    delete tableModel_;
    delete treeModel_;
    delete ui;
}

void MainWindow::on_action_Close_triggered()
{
    this->close();
}
void MainWindow::sayHello(int id,
               const QString& movieFile)
{
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

        tableModel_->AppendData(new ItemData(files, width, height, movieFile, format));

        for(int i=0 ; i < 5 ; ++i)
        {
            ui->tableView->setColumnWidth(i, width);
            ui->tableView->setRowHeight(newRowImage, height);
        }

        insertLog(TaskKind::FFMpeg, id, QString("%1 \"%2\"").arg(tr("Done"), movieFile));
    }

    //listModel_->AppendData(new ListItemData(files, width, height, movieFile));
    //ui->listView->update(ui->listView->model()->index(0,0));
    // ui->listView->dataChanged( model()->dataChanged(QModelIndex(),QModelIndex());
}
void MainWindow::insertLog(TaskKind kind, int id, const QString& text)
{
    QString message;

    message.append(QTime::currentTime().toString());
    message.append(" ");

    QString head;
    if(kind==TaskKind::GetDir)
    {
        head.append(tr("Getting files"));
    }
    else
    {
        head.append(tr("Analyze file"));
    }
    head.append(QString::number(id));
    head.append("> ");
    message.append(head);
    message.append(text);

    ui->txtLog->appendPlainText(message);

    ui->txtLog->verticalScrollBar()->setValue(
                ui->txtLog->verticalScrollBar()->maximum()); // Scrolls to the bottom
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


    TaskGetDir* pTaskGetDir = new TaskGetDir(++idGetDir_, dir);
    pTaskGetDir->setAutoDelete(true);
    QObject::connect(pTaskGetDir, &TaskGetDir::afterGetDir,
                     this, &MainWindow::afterGetDir);
    poolGetDir_->start(pTaskGetDir);

    insertLog(TaskKind::GetDir, idGetDir_, tr("Task registered"));
}
void MainWindow::afterGetDir(int id,
                 const QStringList& dirs)
{
    Q_UNUSED(id);
    WaitCursor wc;

    int saveThreadCount = poolFFMpeg_->maxThreadCount();
    poolFFMpeg_->setMaxThreadCount(0);

    QStringListIterator it(dirs);
    while (it.hasNext()) {
        QString file = it.next();
        TaskFFMpeg* pTask = new TaskFFMpeg(++idFFMpeg_,file);
        pTask->setAutoDelete(true);
        QObject::connect(pTask, &TaskFFMpeg::sayHello,
                         this, &MainWindow::sayHello);
        QObject::connect(pTask, &TaskFFMpeg::sayNo,
                         this, &MainWindow::sayNo);
        QObject::connect(pTask, &TaskFFMpeg::sayGoodby,
                         this, &MainWindow::sayGoodby);
        poolFFMpeg_->start(pTask);

        insertLog(TaskKind::FFMpeg, idFFMpeg_, tr("Task registered"));
    }
    poolFFMpeg_->setMaxThreadCount(saveThreadCount);
}


void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);

    WaitCursor wc;

    poolFFMpeg_->clear();
    poolGetDir_->clear();

    delete poolGetDir_;
    poolGetDir_ = nullptr;

    delete poolFFMpeg_;
    poolFFMpeg_ = nullptr;


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

void MainWindow::showEvent( QShowEvent* event )
{
    QMainWindow::showEvent( event );

    ui->treeView->setMaximumSize(10000,10000);
    ui->txtLog->setMaximumSize(10000,10000);

}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QVariant v = tableModel_->data(index, TableModel::TableRole::MovieFile);
    Q_ASSERT(v.isValid());
    Q_ASSERT(!v.toString().isEmpty());

//    QProcess process;
//    QString file = v.toString();
//    process.start("",file);

    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(v.toString())))
    {
        QMessageBox msgBox;
        msgBox.setText(Consts::APPNAME);
        msgBox.setInformativeText(tr("failed to launch."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}
