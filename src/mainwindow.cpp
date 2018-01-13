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
#include <QClipboard>

#include "ui_mainwindow.h"
#include "taskgetdir.h"
#include "taskffmpeg.h"

#include "tablemodel.h"
#include "foldermodel.h"
#include "taskmodel.h"
#include "tableitemdata.h"
#include "settings.h"
#include "waitcursor.h"
// #include "taskfilter.h"

#include "optiondialog.h"
#include "globals.h"
#include "helper.h"

#include "sql.h"

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Settings& settings) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    idManager_ = new IDManager(this);

    this->setWindowTitle(Consts::APPNAME);

    QObject::connect(ui->menu_Task, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuTask_AboutToShow);
    QObject::connect(ui->menu_Docking_windows, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuDocking_windows_AboutToShow);

//    timer_ = new QTimer(this);
//    connect(timer_, SIGNAL(timeout()),
//            this, SLOT(OnTimer()));
//    timer_->start(5000);

    tableModel_=new TableModel(ui->tableView);
    // QStandardItemModel* model = new QStandardItemModel;
    // ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(tableModel_);
    QObject::connect(tableModel_, &TableModel::itemCountChanged,
                     this, &MainWindow::tableItemCountChanged);


    // treeModel_ = new FolderModel;
//	entryDirectoryModel_ = new EntryDirectoryModel;
//    ui->directoryWidget->setModel(entryDirectoryModel_);

    QItemSelectionModel* treeSelectionModel = ui->directoryWidget->selectionModel();
    QObject::connect(treeSelectionModel, &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::on_directoryWidget_selectionChanged);


    taskModel_ = new TaskModel(ui->listTask);
    ui->listTask->setModel(taskModel_);


    slPaused_ = new QLabel(this);
    slPaused_->hide();
    ui->statusBar->addPermanentWidget(slPaused_);

    slTask_ = new QLabel(this);
    ui->statusBar->addPermanentWidget(slTask_);
    idManager_->Clear();

    slItemCount_ = new QLabel(this);
    ui->statusBar->addPermanentWidget(slItemCount_);

    QVariant vVal;

    vVal = settings.value(Consts::KEY_SIZE);
    if(vVal.isValid())
        resize(vVal.toSize());

    vVal = settings.value(Consts::KEY_LASTSELECTEDDIRECTORY);
    if(vVal.isValid())
        lastSelectedDir_ = vVal.toString();

    vVal = settings.value(Consts::KEY_TREESIZE);
    if(vVal.isValid())
        ui->directoryWidget->setMaximumSize(vVal.toSize());

    vVal = settings.value(Consts::KEY_TXTLOGSIZE);
    if(vVal.isValid())
        ui->txtLog->setMaximumSize(vVal.toSize());

    vVal = settings.value(Consts::KEY_LISTTASKSIZE);
    if(vVal.isValid())
        ui->listTask->setMaximumSize(vVal.toSize());


    vVal = settings.value(Consts::KEY_USERENTRYDIRECTORIES);
    if(vVal.isValid())
    {
        QStringList dirs = vVal.toStringList();
        for(const QString& s : dirs)
            AddUserEntryDirectory(s);
    }
}

//void MainWindow::setTableSpan()
//{
//    int newRowFilename = ui->tableView->model()->rowCount()-TableModel::RowCountPerEntry;
//    int newRowInfo = newRowFilename+1;
//    int newRowImage = newRowFilename+2;

//    ui->tableView->setSpan(newRowFilename,0,1,5);
//    ui->tableView->setSpan(newRowInfo,0,1,5);
//    // ui->tableView->resizeRowToContents(newRowFilename);
//    // ui->tableView->resizeRowToContents(newRowInfo);

//    static bool initColumnWidth=false;
//    if(!initColumnWidth)
//    {
//        initColumnWidth=true;
//        for(int i=0 ; i < 5 ; ++i)
//        {
//            ui->tableView->setColumnWidth(i, Consts::THUMB_WIDTH);
//        }
//    }

//    ui->tableView->setRowHeight(newRowImage, Consts::THUMB_HEIGHT);
//}

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
//QThreadPool* MainWindow::getPoolFilter()
//{
//    if(!pPoolFilter_)
//    {
//        pPoolFilter_ = new QThreadPool;
//        pPoolFilter_->setExpiryTimeout(-1);
//        Q_ASSERT(threadcountFilter_ > 0);
//        pPoolFilter_->setMaxThreadCount(threadcountFilter_);
//    }
//    return pPoolFilter_;
//}





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

void MainWindow::clearAllPool()
{
    insertLog(TaskKind::App, 0, tr("Clearing all tasks..."));
    gStop = true;
    bool prevPause = gPaused;
    gPaused = false;


    do {
        if(pPoolFFmpeg_)
            pPoolFFmpeg_->clear();
        if(pPoolGetDir_)
            pPoolGetDir_->clear();
//        if(pPoolFilter_)
//            pPoolFilter_->clear();
        if(pPoolFFmpeg_)
            pPoolFFmpeg_->clear();

        if(pPoolGetDir_ && pPoolGetDir_->activeThreadCount() != 0)
            continue;
//        if(pPoolFilter_ && pPoolFilter_->activeThreadCount() != 0)
//            continue;
        if(pPoolFFmpeg_ && pPoolFFmpeg_->activeThreadCount() != 0)
            continue;

        break;
    } while(true);

//    while( !( (idGetDir_ == idGetDirDone_) &&
//           (idFilter_ == idFilterDone_) &&
//           (idFFMpeg_ == idFFMpegDone_) ) )
//    {
//        QApplication::processEvents();
//    }
    ++gLoopId;
    idManager_->Clear();

    delete pPoolGetDir_;
    pPoolGetDir_ = nullptr;

//    delete pPoolFilter_;
//    pPoolFilter_ = nullptr;

    delete pPoolFFmpeg_;
    pPoolFFmpeg_ = nullptr;

	taskModel_->ClearAllTasks();

    gPaused=prevPause;
    gStop = false;
    insertLog(TaskKind::App, 0, tr("All tasks Cleared."));
}



MainWindow::~MainWindow()
{
    delete tableModel_;
    delete taskModel_;
    delete ui;

#ifdef QT_DEBUG
	Q_ASSERT(TaskListData::isAllClear());
	Q_ASSERT(TableItemData::isAllClear());
#endif

}



void MainWindow::insertLog(TaskKind kind, int id, const QString& text, bool bError)
{
    QVector<int> ids;
    ids.append(id);

    QStringList l;
    l.append(text);

    insertLog(kind, ids, l, bError);
}
void MainWindow::insertLog(TaskKind kind, const QVector<int>& ids, const QStringList& texts, bool bError)
{
    Q_UNUSED(bError);
    if(ids.isEmpty())
    {
        Q_ASSERT(texts.isEmpty());
        return;
    }
    QString message;

    message.append(QTime::currentTime().toString());
    message.append(" ");

    Q_ASSERT(ids.count()==texts.count());
    for(int i=0 ; i < texts.length(); ++i)
    {
        QString text=texts[i];
        int id=ids[i];

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

            case TaskKind::App:
            {

            }
            break;

//            case TaskKind::Filter:
//            {
//                head.append(tr("Filter"));
//            }
//            break;

            default:
                Q_ASSERT(false);
                return;
        }

        head.append("> ");
        message.append(head);
        message.append(text);
        if( (i+1) != texts.length())
            message.append("\n");
    }
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



void MainWindow::afterGetDir(int loopId, int id,
                             const QString& dirc,
                             const QStringList& filesIn,
							 const QStringList& salients)
{
    if(loopId != gLoopId)
        return;

	struct PauseBack
	{
		bool bBack_;
		PauseBack(bool bBack) :bBack_(bBack) 
		{
			gPaused = true;
		}
		~PauseBack() {
			gPaused = bBack_;
		}
	} pback(gPaused);

    Q_UNUSED(id);
    WaitCursor wc;

    QString dir = QDir(dirc).canonicalPath() + '/';

    QStringList filteredFiles;

    // check file is in DB
    for(int ifs = 0 ; ifs < filesIn.count();++ifs)
    {
		const QString& file = filesIn[ifs];
		const QString& sa = salients[ifs];
        QFileInfo fi(pathCombine(dir, file));

        // QString sa = createSalient(fi.absoluteFilePath(), fi.size());

        if(gpSQL->hasEntry(dir,file,sa))
        {
            if(true) // gpSQL->hasThumb(dir, file))
            {
                insertLog(TaskKind::GetDir, id, QString(tr("Already exist. \"%1\"")).
                          arg(fi.absoluteFilePath()));
                continue;
            }
            else
            {
                // thumbs not exist
                // may be crashed or failed to create
                // remove entry
                gpSQL->RemoveEntry(dir, file);
            }
        }

        QStringList dirsDB;
        QStringList filesDB;
        QList<qint64> sizesDB;
        gpSQL->getEntryFromSalient(sa, dirsDB, filesDB, sizesDB);
        bool renamed = false;
        for(int i=0 ; i < filesDB.count(); ++i)
        {
            const QString& dbFile = pathCombine(dirsDB[i], filesDB[i]);
            // same salient in db
            QFileInfo fidb(dbFile);
            if(!fidb.exists())
            {
                // file info in db does not exist on disk
                if(fi.size()==sizesDB[i])
                {
                    // db size is same size with disk
                    // and salient is same ( conditonal queried from db )
                    // we assume file is moved
                    insertLog(TaskKind::GetDir, id, QString(tr("Rename detected. \"%1\" -> \"%2\"")).
                              arg(dbFile).
                              arg(pathCombine(dir,file)));
                    if(gpSQL->RenameEntry(dirsDB[i], filesDB[i], dir, file))
                    {
                        tableModel_->RenameEntry(dirsDB[i], filesDB[i], dir, file);
                    }
                    renamed = true;
                    break;
                }
            }
        }

        if(renamed)
            continue;
        filteredFiles.append(file);
    }
    // now file must be thumbnailed
    afterFilter2(loopId,
                id,
                dir,
                filteredFiles
                );


//	QStringList entries;
//    QVector<qint64> sizes;
//    QVector<qint64> ctimes;
//    QVector<qint64> wtimes;
//    QStringList salients;

//    gpSQL->GetAllEntry(dir, entries, sizes, ctimes, wtimes, salients);
//    TaskFilter* pTaskFilter = new TaskFilter(gLoopId, idManager_->Increment(IDKIND_Filter),
//                                             dir,
//                                             filesIn,
//                                             entries,
//                                             sizes,
//                                             ctimes,
//                                             wtimes,
//                                             salients);
//    pTaskFilter->setAutoDelete(true);

//    QObject::connect(pTaskFilter, &TaskFilter::afterFilter,
//                     this, &MainWindow::afterFilter);
//    QObject::connect(pTaskFilter, &TaskFilter::finished_Filter,
//                     this, &MainWindow::finished_Filter);
//    getPoolFilter()->start(pTaskFilter);

//    insertLog(TaskKind::Filter, id, QString(tr("Task Registered. %1")).arg(dir));
}
void MainWindow::finished_GetDir(int loopId, int id)
{
    if(loopId != gLoopId)
        return;

    idManager_->IncrementDone(IDKIND_GetDir);
    Q_ASSERT(id >= idManager_->GetDone(IDKIND_GetDir));

    checkTaskFinished();
}
void MainWindow::afterFilter2(int loopId,int id,
                             const QString& dir,
                             const QStringList& filteredFiles)
{
    if(loopId != gLoopId)
        return;

    Q_UNUSED(id);



//    QStringList filteredFiles;
//    int ret = gpSQL->filterWithEntry(dir, filesIn, filteredFiles);
//    if(ret != 0)
//    {
//        insertLog(TaskKind::SQL, 0, Sql::getErrorStrig(ret), true);
//    }

    if(filteredFiles.isEmpty())
    {
        insertLog(TaskKind::SQL, 0, QString(tr("No new files found in %1")).arg(dir));
    }
    else
    {
        insertLog(TaskKind::SQL, 0, QString(tr("%1 new items found in %2")).
                  arg(QString::number(filteredFiles.count())).
                  arg(dir));
    }

    // afterfilter must perform salient check from SQL, for filter-passed files


    QVector<TaskListDataPointer> tasks;
    QVector<int> logids;
    QStringList logtexts;
    for(int i=0 ; i < filteredFiles.length(); ++i)
    {
        QString file = pathCombine(dir, filteredFiles[i]);
        TaskFFmpeg* pTask = new TaskFFmpeg(gLoopId, idManager_->Increment(IDKIND_FFmpeg), file);
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
        QObject::connect(pTask, &TaskFFmpeg::finished_FFMpeg,
                         this, &MainWindow::finished_FFMpeg);

        tasks.append(TaskListData::Create(pTask->GetId(),pTask->GetMovieFile()));
        getPoolFFmpeg()->start(pTask);

        logids.append(idManager_->Get(IDKIND_FFmpeg));
        logtexts.append(QString(tr("Task registered. %1")).arg(file));
    }
    insertLog(TaskKind::FFMpeg, logids, logtexts);
    taskModel_->AddTasks(tasks);

    checkTaskFinished();


}
void MainWindow::checkTaskFinished()
{
    if(idManager_->isAllTaskFinished())
    {
        insertLog(TaskKind::App, 0, tr("All Tasks finished."));
    }
}
//void MainWindow::afterFilter(int loopId,int id,
//                             const QString& dir,
//                             const QStringList& filteredFiles,
//                             const QStringList& renameOlds,
//                             const QStringList& renameNews)
//{
//    if(loopId != gLoopId)
//        return;

//    Q_UNUSED(id);

//    bool prevPaused = gPaused;
//    gPaused=true;

////    QStringList filteredFiles;
////    int ret = gpSQL->filterWithEntry(dir, filesIn, filteredFiles);
////    if(ret != 0)
////    {
////        insertLog(TaskKind::SQL, 0, Sql::getErrorStrig(ret), true);
////    }
//    Q_ASSERT(renameOlds.count()==renameNews.count());
//    if(!renameOlds.isEmpty())
//    {
//        QString logMessage = QString(tr("%1 renamed items found in %2.")).
//                arg(QString::number(renameOlds.count())).
//                arg(dir);

//        for(int i=0 ; i < renameOlds.count(); ++i)
//        {
//            QString oldfile = renameOlds[i];
//            QString newfile = renameNews[i];
//            logMessage += "\n";
//            logMessage += "  " + QString("\"%1\" -> \"%2\"").arg(oldfile).arg(newfile);
//        }
//        insertLog(TaskKind::SQL,0,logMessage);

//        gpSQL->RenameEntries(dir, renameOlds, renameNews);
//        tableModel_->RenameEntries(dir, renameOlds, renameNews);
//    }


//    if(filteredFiles.isEmpty())
//    {
//        insertLog(TaskKind::SQL, 0, QString(tr("No new files found in %1")).arg(dir));
//    }
//    else
//    {
//        insertLog(TaskKind::SQL, 0, QString(tr("%1 new items found in %2")).
//                  arg(QString::number(filteredFiles.count())).
//                  arg(dir));
//    }

//    // afterfilter must perform salient check from SQL, for filter-passed files


//    QVector<TaskListDataPointer> tasks;
//    QVector<int> logids;
//    QStringList logtexts;
//    for(int i=0 ; i < filteredFiles.length(); ++i)
//    {
//        QString file = pathCombine(dir, filteredFiles[i]);
//        TaskFFmpeg* pTask = new TaskFFmpeg(gLoopId, idManager_->Increment(IDKIND_FFmpeg), file);
//        pTask->setAutoDelete(true);
////        QObject::connect(pTask, &TaskFFMpeg::sayBorn,
////                         this, &MainWindow::sayBorn);
//        QObject::connect(pTask, &TaskFFmpeg::sayHello,
//                         this, &MainWindow::sayHello);
//        QObject::connect(pTask, &TaskFFmpeg::sayNo,
//                         this, &MainWindow::sayNo);
//        QObject::connect(pTask, &TaskFFmpeg::sayGoodby,
//                         this, &MainWindow::sayGoodby);
//        QObject::connect(pTask, &TaskFFmpeg::sayDead,
//                         this, &MainWindow::sayDead);
//        QObject::connect(pTask, &TaskFFmpeg::finished_FFMpeg,
//                         this, &MainWindow::finished_FFMpeg);

//        tasks.append(TaskListData::Create(pTask->GetId(),pTask->GetMovieFile()));
//        getPoolFFmpeg()->start(pTask);

//        logids.append(idManager_->Get(IDKIND_FFmpeg));
//        logtexts.append(QString(tr("Task registered. %1")).arg(file));
//    }
//    insertLog(TaskKind::FFMpeg, logids, logtexts);
//    taskModel_->AddTasks(tasks);

//    if(idManager_->isAllTaskFinished())
//    {
//        insertLog(TaskKind::App, 0, tr("All Tasks finished."));
//    }

//    gPaused=prevPaused;
//}

//void MainWindow::finished_Filter(int loopId, int id)
//{
//    if(loopId != gLoopId)
//        return;

//    Q_UNUSED(id);
//    idManager_->IncrementDone(IDKIND_Filter);
//    Q_ASSERT(idManager_->Get(IDKIND_Filter) >= idManager_->GetDone(IDKIND_Filter));
//}



QString MainWindow::getSelectedVideo()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();

    Q_ASSERT(select->hasSelection());

    QVariant v = tableModel_->data(select->selectedIndexes()[0], TableModel::MovieFile);
    QString s = v.toString();
    Q_ASSERT(!s.isEmpty());

    return QDir::toNativeSeparators(s);
}
void MainWindow::openSelectedVideo()
{
    openVideo(getSelectedVideo());
}
void MainWindow::openSelectedVideoInFolder()
{
    openVideoInFolder(getSelectedVideo());
}

void MainWindow::copySelectedVideoPath()
{
    QApplication::clipboard()->setText(getSelectedVideo());
}

void MainWindow::IDManager::updateStatus()
{
    QString s = QString("D: %1/%2   M: %5/%6").
            arg(idGetDirDone_).arg(idGetDir_).
            // arg(idFilterDone_).arg(idFilter_).
            arg(idFFMpegDone_).arg(idFFMpeg_);

    win_->slTask_->setText(s);
}


void MainWindow::on_directoryWidget_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    directoryChangedCommon();
}
void MainWindow::directoryChangedCommon()
{
    QStringList dirs;
    for(int i=0 ; i < ui->directoryWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->directoryWidget->item(i);
        if(item->checkState()==Qt::Checked)
        {
            dirs.append(item->text());
            continue;
        }
        if(item->isSelected())
        {
            dirs.append(item->text());
            continue;
        }
    }

    QList<TableItemDataPointer> all;
    gpSQL->GetAll(all, dirs);

    tableModel_->ResetData(all);
}

void MainWindow::on_action_Top_triggered()
{
    ui->tableView->scrollToTop();
}

void MainWindow::on_action_Bottom_triggered()
{
    ui->tableView->scrollToBottom();
}


void MainWindow::on_directoryWidget_itemChanged(DirectoryEntry *item)
{
    Q_UNUSED(item);
    directoryChangedCommon();
}

void MainWindow::tableItemCountChanged()
{
    slItemCount_->setText(QString(tr("Items: %1")).arg(tableModel_->GetItemCount()));
}
