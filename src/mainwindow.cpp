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
#include "taskmodel.h"
#include "tableitemdata.h"
#include "settings.h"
#include "waitcursor.h"

#include "optiondialog.h"
#include "globals.h"

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Settings& settings) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(Consts::APPNAME);

    QObject::connect(ui->menu_Task, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuTask_AboutToShow);
    QObject::connect(ui->menu_Docking_windows, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuDocking_windows_AboutToShow);

    tableModel_=new TableModel(this);
    // QStandardItemModel* model = new QStandardItemModel;
    // ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(tableModel_);


    treeModel_ = new TreeModel;
    ui->treeView->setModel(treeModel_);

    taskModel_ = new TaskModel;
    ui->listTask->setModel(taskModel_);



    poolFFMpeg_ = new QThreadPool();
    poolFFMpeg_->setExpiryTimeout(-1);
    // threadcountFFmpeg_ = QThread::idealThreadCount()-2;
    if(threadcountFFmpeg_ <= 0)
        threadcountFFmpeg_=1;
    qDebug() << QString("threadcount=%1").arg(threadcountFFmpeg_);
    Q_ASSERT(threadcountFFmpeg_ > 0);
    poolFFMpeg_->setMaxThreadCount(threadcountFFmpeg_);


    QVariant vVal;

    vVal = settings.value(Consts::KEY_SIZE);
    if(vVal.isValid())
        resize(vVal.toSize());

    vVal = settings.value(Consts::KEY_LASTSELECTEDDIRECTORY);
    if(vVal.isValid())
        lastSelectedDir_ = vVal.toString();

    vVal = settings.value(Consts::KEY_TREESIZE);
    if(vVal.isValid())
        ui->treeView->setMaximumSize(vVal.toSize());

    vVal = settings.value(Consts::KEY_TXTLOGSIZE);
    if(vVal.isValid())
        ui->txtLog->setMaximumSize(vVal.toSize());

    vVal = settings.value(Consts::KEY_LISTTASKSIZE);
    if(vVal.isValid())
        ui->listTask->setMaximumSize(vVal.toSize());
}

QThreadPool* MainWindow::getPoolGetDir()
{
    if(!pPoolGetDir_)
    {
        pPoolGetDir_ = new QThreadPool;
        pPoolGetDir_->setExpiryTimeout(-1);
        Q_ASSERT(threadcountGetDir_ > 0);
        pPoolGetDir_->setMaxThreadCount(threadcountGetDir_);
    }
    return pPoolGetDir_;
}
void MainWindow::clearAllPool()
{
    if(poolFFMpeg_)
        poolFFMpeg_->clear();
    if(pPoolGetDir_)
        pPoolGetDir_->clear();

    delete pPoolGetDir_;
    pPoolGetDir_ = nullptr;

    delete poolFFMpeg_;
    poolFFMpeg_ = nullptr;
}

void MainWindow::clearPoolGetDir()
{
    delete pPoolGetDir_;
    pPoolGetDir_=nullptr;
}

MainWindow::~MainWindow()
{
    delete tableModel_;
    delete treeModel_;
    delete taskModel_;
    delete ui;
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
    resizeDock(ui->dockTask, ui->listTask->size());
    QMainWindow::resizeEvent(event);
}


void MainWindow::afterGetDir(int id,
                 const QStringList& dirs)
{
    Q_UNUSED(id);
    WaitCursor wc;

//    int saveThreadCount = poolFFMpeg_->maxThreadCount();
//    poolFFMpeg_->setMaxThreadCount(1);

    QStringListIterator it(dirs);
    while (it.hasNext()) {
        QString file = it.next();
        TaskFFMpeg* pTask = new TaskFFMpeg(++idFFMpeg_,file);
        pTask->setAutoDelete(true);
//        QObject::connect(pTask, &TaskFFMpeg::sayBorn,
//                         this, &MainWindow::sayBorn);
        QObject::connect(pTask, &TaskFFMpeg::sayHello,
                         this, &MainWindow::sayHello);
        QObject::connect(pTask, &TaskFFMpeg::sayNo,
                         this, &MainWindow::sayNo);
        QObject::connect(pTask, &TaskFFMpeg::sayGoodby,
                         this, &MainWindow::sayGoodby);
        QObject::connect(pTask, &TaskFFMpeg::sayDead,
                         this, &MainWindow::sayDead);

        taskModel_->AddTask(new TaskListData(pTask->GetId(),pTask->GetMovieFile()));
        poolFFMpeg_->start(pTask);

        insertLog(TaskKind::FFMpeg, idFFMpeg_, tr("Task registered"));
    }
    // poolFFMpeg_->setMaxThreadCount(saveThreadCount);
}






