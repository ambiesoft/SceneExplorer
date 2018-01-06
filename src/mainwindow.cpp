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
#include <QTimer>
#include <QUrl>
#include <QVector>
#include <QWidget>

#include "ui_mainwindow.h"
#include "taskgetdir.h"
#include "taskffmpeg.h"

#include "tablemodel.h"
#include "foldermodel.h"
#include "taskmodel.h"
#include "tableitemdata.h"
#include "settings.h"
#include "waitcursor.h"

#include "optiondialog.h"
#include "globals.h"

#include "sql.h"

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

//    timer_ = new QTimer(this);
//    connect(timer_, SIGNAL(timeout()),
//            this, SLOT(OnTimer()));
//    timer_->start(5000);

    tableModel_=new TableModel(this);
    // QStandardItemModel* model = new QStandardItemModel;
    // ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(tableModel_);


    treeModel_ = new FolderModel;
    ui->treeView->setModel(treeModel_);

    taskModel_ = new TaskModel;
    ui->listTask->setModel(taskModel_);





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

void MainWindow::setTableSpan()
{
    int newRowFilename = ui->tableView->model()->rowCount()-TableModel::RowCountPerEntry;
    int newRowInfo = newRowFilename+1;
    int newRowImage = newRowFilename+2;

    ui->tableView->setSpan(newRowFilename,0,1,5);
    ui->tableView->setSpan(newRowInfo,0,1,5);
    // ui->tableView->resizeRowToContents(newRowFilename);
    // ui->tableView->resizeRowToContents(newRowInfo);

    static bool initColumnWidth=false;
    if(!initColumnWidth)
    {
        initColumnWidth=true;
        for(int i=0 ; i < 5 ; ++i)
        {
            ui->tableView->setColumnWidth(i, Consts::THUMB_WIDTH);
        }
    }

    ui->tableView->setRowHeight(newRowImage, Consts::THUMB_HEIGHT);
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
void MainWindow::clearPoolGetDir()
{
    delete pPoolGetDir_;
    pPoolGetDir_=nullptr;
}

QThreadPool* MainWindow::getPoolFFmpeg()
{
    if(!pPoolFFmpeg_)
    {
        pPoolFFmpeg_ = new QThreadPool;
        pPoolFFmpeg_->setExpiryTimeout(-1);
        Q_ASSERT(threadcountFFmpeg_ > 0);
        pPoolFFmpeg_->setMaxThreadCount(threadcountFFmpeg_);
    }
    return pPoolFFmpeg_;
}
void MainWindow::clearPoolFFmpeg()
{
    delete pPoolFFmpeg_;
    pPoolFFmpeg_=nullptr;
}
void MainWindow::clearAllPool()
{
    if(pPoolFFmpeg_)
        pPoolFFmpeg_->clear();
    if(pPoolGetDir_)
        pPoolGetDir_->clear();

    delete pPoolGetDir_;
    pPoolGetDir_ = nullptr;

    delete pPoolFFmpeg_;
    pPoolFFmpeg_ = nullptr;
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
    switch(kind)
    {
        case TaskKind::GetDir:
        {
            head.append(tr("Iterate"));
            head.append(QString::number(id));
        }
        break;
        case TaskKind::FFMpeg:
        {
            head.append(tr("Analyze"));
            head.append(QString::number(id));
        }
        break;
        case TaskKind::SQL:
        {
            head.append(tr("Database"));
        }
        break;

        default:
            Q_ASSERT(false);
            return;
    }

    head.append("> ");
    message.append(head);
    message.append(text);

    int scrollMax=ui->txtLog->verticalScrollBar()->maximum();
    int scrollCur=ui->txtLog->verticalScrollBar()->value();

    //int prevcursorPos = ui->txtLog->textCursor().position();
    ui->txtLog->appendPlainText(message);
    //int aftercursorPos = ui->txtLog->textCursor().position();

    //if(prevcursorPos < aftercursorPos)
    if(scrollMax==scrollCur)
    {
        ui->txtLog->verticalScrollBar()->setValue(
                ui->txtLog->verticalScrollBar()->maximum()); // Scrolls to the bottom
    }
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

    bool prevPaused = gPaused;
    gPaused=true;

    QVector<TaskListData*> tasks;
    QStringListIterator it(dirs);
    while (it.hasNext()) {
        QString file = it.next();
        if(gpSQL->hasThumb(file))
        {
            insertLog(TaskKind::SQL, 0, tr("Found in db. Skipping"));
            continue;
        }

        TaskFFmpeg* pTask = new TaskFFmpeg(++idFFMpeg_,file);
        pTask->setAutoDelete(true);
//        QObject::connect(pTask, &TaskFFMpeg::sayBorn,
//                         this, &MainWindow::sayBorn);
        QObject::connect(pTask, &TaskFFmpeg::sayHello,
                         this, &MainWindow::sayHello);
        QObject::connect(pTask, &TaskFFmpeg::sayNo,
                         this, &MainWindow::sayNo);
        QObject::connect(pTask, &TaskFFmpeg::sayGoodby,
                         this, &MainWindow::sayGoodby);
        QObject::connect(pTask, &TaskFFmpeg::sayDead,
                         this, &MainWindow::sayDead);

        tasks.append(new TaskListData(pTask->GetId(),pTask->GetMovieFile()));
        getPoolFFmpeg()->start(pTask);

        insertLog(TaskKind::FFMpeg, idFFMpeg_, tr("Task registered"));
    }
    taskModel_->AddTasks(tasks);

    // poolFFMpeg_->setMaxThreadCount(saveThreadCount);
    gPaused=prevPaused;
}






