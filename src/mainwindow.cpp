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
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QElapsedTimer>
#include <QSortFilterProxyModel>

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
#include "errorinfoexception.h"

#include "optiondialog.h"
#include "option_extention.h"

#include "globals.h"
#include "helper.h"
#include "blockedbool.h"


#include "sql.h"

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Settings& settings) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings_(settings)
{
    ui->setupUi(this);
    idManager_ = new IDManager(this);

    this->setWindowTitle(Consts::APPNAME_DISPLAY);

	// menu
    QObject::connect(ui->menu_Task, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuTask_AboutToShow);
    QObject::connect(ui->menu_Docking_windows, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuDocking_windows_AboutToShow);
    QObject::connect(ui->menu_Favorites, &QMenu::aboutToShow,
                     this, &MainWindow::onMenu_Favorites_AboutToShow);


	// table
    QObject::connect(ui->tableView->verticalScrollBar(), &QScrollBar::valueChanged,
                     this, &MainWindow::on_tableView_scrollChanged);
    tableModel_=new TableModel(ui->tableView);
	proxyModel_ = new FileMissingFilterProxyModel(ui->tableView);
	proxyModel_->setSourceModel(tableModel_);
	// very slow
	// ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setModel(proxyModel_);
	
    QObject::connect(tableModel_, &TableModel::itemCountChanged,
                     this, &MainWindow::tableItemCountChanged);
    QObject::connect(tableModel_, &TableModel::sortParameterChanged,
                     this, &MainWindow::tableSortParameterChanged);
	// not called
	// ui->tableView->setItemDelegate(new ImageSizeDelegate(ui->tableView));


	// tree
    QItemSelectionModel* treeSelectionModel = ui->directoryWidget->selectionModel();
    QObject::connect(treeSelectionModel, &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::on_directoryWidget_selectionChanged);
	QObject::connect(ui->directoryWidget, &DirectoryEntry::itemChanged,
                     this, &MainWindow::on_directoryWidget_itemChanged);


    // tool bar
    // tool bar show missing
	btnShowNonExistant_ = new QToolButton(ui->mainToolBar);
	btnShowNonExistant_->setText(tr("Show missing files"));
	btnShowNonExistant_->setCheckable(true);
    QObject::connect(btnShowNonExistant_, &QToolButton::toggled,
                     this, &MainWindow::on_action_ShowMissing);



//    QToolButton* myTooButton = new QToolButton(ui->mainToolBar);
//    ui->mainToolBar->addWidget(myTooButton);

    comboFind_ = new FindComboBox(ui->mainToolBar);
    comboFind_->setMinimumWidth(160);
    comboFind_->setMaximumWidth(160);
    comboFind_->setEditable(true);
    QObject::connect(comboFind_, &FindComboBox::on_EnterPressed,
                     this, &MainWindow::on_FindCombo_EnterPressed);


    ui->mainToolBar->insertWidget(ui->placeHolder_ShowMissing, btnShowNonExistant_);
    ui->mainToolBar->insertWidget(ui->placeHolder_ComboBox, comboFind_);

    ui->mainToolBar->removeAction(ui->placeHolder_ShowMissing);
    ui->mainToolBar->removeAction(ui->placeHolder_ComboBox);


    // status bar
    taskModel_ = new TaskModel(ui->listTask);
    ui->listTask->setModel(taskModel_);

    slItemSort_ = new QLabel(this);
    ui->statusBar->addPermanentWidget(slItemSort_);

    slItemCount_ = new QLabel(this);
    ui->statusBar->addPermanentWidget(slItemCount_);

    slTask_ = new QLabel(this);
    ui->statusBar->addPermanentWidget(slTask_);
    idManager_->Clear();

    slPaused_ = new QLabel(this);
    slPaused_->hide();
    ui->statusBar->addPermanentWidget(slPaused_);



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

	vVal = settings.value(Consts::KEY_SHOWMISSING);
	if (vVal.isValid())
		btnShowNonExistant_->setChecked(vVal.toBool());

    bool bAllSel=false;
    bool bAllCheck=false;
    vVal = settings.value(Consts::KEY_KEY_USERENTRY_DIRECTORY_ALL_SELECTED);
    if(vVal.isValid())
        bAllSel=vVal.toBool();
    vVal = settings.value(Consts::KEY_KEY_USERENTRY_DIRECTORY_ALL_CHECKED);
    if(vVal.isValid())
        bAllCheck=vVal.toBool();

    AddUserEntryDirectory(DirectoryItem::DI_ALL, QString(), bAllSel,bAllCheck);

    bool dirOK = false;
    do
    {
        QVariant vValDirs = settings.value(Consts::KEY_USERENTRY_DIRECTORIES);
        QVariant vValSelecteds = settings.value(Consts::KEY_USERENTRY_SELECTED);
        QVariant vValCheckeds = settings.value(Consts::KEY_USERENTRY_CHECKEDS);

        if(vValDirs.isValid())
        {
            QStringList dirs = vValDirs.toStringList();
            QList<QVariant> sels;
            if(vValSelecteds.isValid())
            {
                sels = vValSelecteds.toList();
            }
            else
            {
                for(int i =0 ; i < dirs.count(); ++i)
                    sels.append(false);
            }

            QList<QVariant> checks;
            if(vValCheckeds.isValid())
            {
                checks= vValCheckeds.toList();
            }
            else
            {
                for(int i =0 ; i < dirs.count(); ++i)
                    checks.append(false);
            }





            Q_ASSERT(dirs.count()==sels.count());
            Q_ASSERT(dirs.count()==checks.count());
            for(int i=0 ; i < dirs.count(); ++i)
            {
                QString dir = dirs[i];

                if(!sels[i].isValid())
                    break;
                bool sel = sels[i].toBool();

                if(!checks[i].isValid())
                    break;
                bool chk = checks[i].toBool();
                AddUserEntryDirectory(DirectoryItem::DI_NORMAL, dir, sel,chk);
            }
        }
        dirOK = true;
    } while(false);

    if(!dirOK)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(Consts::APPNAME_DISPLAY);
        msgBox.setText(QString(tr("Failed to load user directory data from %1. Do you want to continue?")).
                       arg(settings.fileName()));
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);
        if(msgBox.exec() != QMessageBox::Yes)
        {
            return;
        }
    }
	AddUserEntryDirectory(DirectoryItem::DI_MISSING, QString(), false, false);

    optionThreadcountGetDir_ = settings_.valueInt(Consts::KEY_MAX_GETDIR_THREADCOUNT, optionThreadcountGetDir_);
    optionThreadcountThumbnail_ = settings_.valueInt(Consts::KEY_MAX_THUMBNAIL_THREADCOUNT, optionThreadcountThumbnail_);


    initialized_ = true;
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
        Q_ASSERT(optionThreadcountGetDir_ > 0);
        pPoolGetDir_->setMaxThreadCount(optionThreadcountGetDir_);
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
        Q_ASSERT(optionThreadcountThumbnail_ > 0);
        pPoolFFmpeg_->setMaxThreadCount(optionThreadcountThumbnail_);
    }
    return pPoolFFmpeg_;
}

void MainWindow::clearAllPool(bool bAppendLog)
{
    if(bAppendLog)
        insertLog(TaskKind::App, 0, tr("Clearing all tasks..."));

    BlockedBool btStop(&gStop, true, false);
    BlockedBool btPaused(&gPaused, false, gPaused);

    do {
        if(pPoolFFmpeg_)
            pPoolFFmpeg_->clear();
        if(pPoolGetDir_)
            pPoolGetDir_->clear();
        if(pPoolFFmpeg_)
            pPoolFFmpeg_->clear();

        if(pPoolGetDir_ && pPoolGetDir_->activeThreadCount() != 0)
            continue;
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

    delete pPoolFFmpeg_;
    pPoolFFmpeg_ = nullptr;

	taskModel_->ClearAllTasks();

    if(bAppendLog)
        insertLog(TaskKind::App, 0, tr("All tasks Cleared."));
}



MainWindow::~MainWindow()
{
	closed_ = true;

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
                head.append(tr("Thumbnail"));
                head.append(QString::number(id));
            }
            break;
            case TaskKind::SQL:
            {
                head.append(tr("Database"));
				head.append(QString::number(id));
            }
            break;

            case TaskKind::App:
            {
				head.append(tr("Application"));
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

                             const QList<qint64> sizes,
                             const QList<qint64> wtimes,

                             const QStringList& salients
                             )
{
    if(loopId != gLoopId)
        return;

    BlockedBool btPause(&gPaused, true, gPaused);

    Q_UNUSED(id);
    WaitCursor wc;

    bool needUpdate = false;

    QString dir = QDir(dirc).canonicalPath() + '/';

    QStringList filteredFiles;

    // check file is in DB
    for(int ifs = 0 ; ifs < filesIn.count();++ifs)
    {
		const QString& file = filesIn[ifs];
		const QString& sa = salients[ifs];
        const qint64 size = sizes[ifs];
        const qint64 wtime = wtimes[ifs];

        QFileInfo fi(pathCombine(dir, file));

        // QString sa = createSalient(fi.absoluteFilePath(), fi.size());

        if(gpSQL->hasEntry(dir,file,size,wtime,sa))
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
                        if(!tableModel_->RenameEntry(dirsDB[i], filesDB[i], dir, file))
                        {
                            needUpdate = true;
                        }
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


    if(needUpdate)
        directoryChangedCommon();
}
void MainWindow::finished_GetDir(int loopId, int id, const QString& dir)
{
    if(loopId != gLoopId)
        return;

    idManager_->IncrementDone(IDKIND_GetDir);
    Q_ASSERT(id >= idManager_->GetDone(IDKIND_GetDir));

    insertLog(TaskKind::GetDir, id, QString(tr("Scan directory finished. %1")).arg(dir));

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
        insertLog(TaskKind::GetDir, id, QString(tr("No new files found in %1")).arg(dir));
    }
    else
    {
        insertLog(TaskKind::GetDir, id, QString(tr("%1 new items found in %2")).
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
        onTaskEnded();
        insertLog(TaskKind::App, 0, tr(Consts::ALL_TASK_FINISHED));
    }
}

QString MainWindow::getSelectedVideo(bool bNativeFormat)
{
    QItemSelectionModel *select = ui->tableView->selectionModel();

    Q_ASSERT(select->hasSelection());
    if(!select->hasSelection())
        return QString();
    QVariant v = proxyModel_->data(select->selectedIndexes()[0], TableModel::MovieFileFull);
    QString s = v.toString();
    Q_ASSERT(!s.isEmpty());

    return bNativeFormat ? QDir::toNativeSeparators(s) : s;
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
void MainWindow::removeFromDatabase()
{
    QString movieFile = getSelectedVideo(false);
    if(movieFile.isEmpty())
    {
        Alert(this, tr("Movie file is empty."));
        return;
    }


    QMessageBox msgbox(this);
    QCheckBox cb(tr("Also remove from external media"));
    cb.setEnabled(QFile(movieFile).exists());
    msgbox.setText(QString(tr("Do you want to remove \"%1\" from database?")).
                   arg(movieFile));
    msgbox.setIcon(QMessageBox::Icon::Question);
    msgbox.addButton(QMessageBox::Yes);
    msgbox.addButton(QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::No);
    msgbox.setCheckBox(&cb);

//    QObject::connect(&cb, &QCheckBox::stateChanged, [this](int state){
//        if (static_cast<Qt::CheckState>(state) == Qt::CheckState::Checked) {
//            this->showMsgBox = false;
//        }
//    });

    if(msgbox.exec() != QMessageBox::Yes)
        return;
    bool bRemoveFromHardDisk = cb.checkState()==Qt::Checked;

//    if(!YesNo(this,
//          QString(tr("Do you want to remove \"%1\" from database?")).
//          arg(movieFile)))
//    {
//        return;
//    }
    QString error;
    QString dir,name;
    canonicalDirAndName(movieFile, dir, name);
    if(dir.isEmpty() || name.isEmpty())
    {
        Alert(this, tr("Directory or name is empty."));
        return;
    }
    if(!gpSQL->RemoveEntry(dir,name,&error))
    {
        Alert(this,
              QString(tr("Failed to remove \"%1\".")).
              arg(movieFile));
        return;
    }
    tableModel_->RemoveItem(movieFile);

    if(bRemoveFromHardDisk)
    {
        try
        {
            MoveToTrashImpl(movieFile);
        }
        catch(ErrorInfoException& ex)
        {
            Alert(this, ex.getErrorInfo());
        }
    }
}

void MainWindow::copySelectedVideoFilename()
{
    QFileInfo fi(getSelectedVideo());
    QApplication::clipboard()->setText(fi.fileName());
}

void MainWindow::IDManager::updateStatus()
{
    QString s = QString("D: %1/%2   M: %3/%4").
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

void MainWindow::directoryChangedCommon(bool bForceRead)
{
	if (!initialized_ || closed_)
		return;

	if (directoryChanging_)
		return;

	WaitCursor wc;

    QStringList dirs;
    bool bOnlyMissing = false;
    for(int i=0 ; i < ui->directoryWidget->count(); ++i)
    {
        DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->item(i);
        if(item->IsAllItem())
        {
            if(item->isSelected())
            {
                dirs = QStringList();
				bForceRead = true;
                break;
            }
        }
        else if(item->IsMissingItem())
        {
            if(item->isSelected())
            {
                dirs = QStringList();
                bOnlyMissing = true;
                bForceRead = true;
                break;
            }
        }


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

    btnShowNonExistant_->setEnabled(!bOnlyMissing);

    if(!bForceRead)
    {
        if (dirs == currentDirs_)
            return;
    }
    currentDirs_ = dirs;


    GetSqlAllSetTable(dirs, bOnlyMissing);
}

void MainWindow::GetSqlAllSetTable(const QStringList& dirs, bool bOnlyMissing)
{
	tableModel_->SetShowMissing(btnShowNonExistant_->isChecked() );
    QElapsedTimer timer;
    timer.start();

    QList<TableItemDataPointer> all;
    gpSQL->GetAll(all, dirs, comboFind_->currentText(), bOnlyMissing);

    insertLog(TaskKind::App,
              0,
              QString(tr("Quering Database takes %1 milliseconds.")).arg(timer.elapsed()));

    timer.start();
    tableModel_->ResetData(all);

    insertLog(TaskKind::App,
              0,
              QString(tr("Resetting data takes %1 milliseconds.")).arg(timer.elapsed()));
}

void MainWindow::on_action_Top_triggered()
{
	WaitCursor wc;
    ui->tableView->scrollToTop();
}

void MainWindow::on_action_Bottom_triggered()
{
	WaitCursor wc;
    proxyModel_->ensureBottom();
	QApplication::processEvents();
    ui->tableView->scrollToBottom();
}


void MainWindow::on_directoryWidget_itemChanged(QListWidgetItem *item)
{
    Q_UNUSED(item);
    directoryChangedCommon();
}

void MainWindow::tableItemCountChanged()
{
    // slItemCount_->setText(QString(tr("Items: %1")).arg(tableModel_->GetItemCount()));
    slItemCount_->setText(QString(tr("Items: %1")).arg(proxyModel_->GetItemCount()));
}
void MainWindow::tableSortParameterChanged(TableModel::SORTCOLUMN sc, bool rev)
{
    QString text = QString(tr("Sort: %1 %2")).
            arg(TableModel::GetSortColumnName(sc)).
            arg(rev ? "-" : "+");
    slItemSort_->setText(text);
}


bool MainWindow::IsInitialized() const
{
    return initialized_;
}

void MainWindow::on_action_Find_triggered()
{
    directoryChangedCommon(true);
}

void MainWindow::on_action_Clear_triggered()
{
    comboFind_->setCurrentText(QString());
    directoryChangedCommon(true);
}
void MainWindow::on_FindCombo_EnterPressed()
{
    directoryChangedCommon(true);
}


void MainWindow::on_tableView_scrollChanged(int pos)
{
    Q_UNUSED(pos);

    QModelIndex indexTop = ui->tableView->indexAt(ui->tableView->rect().topLeft());
    QModelIndex indexBottom = ui->tableView->indexAt(ui->tableView->rect().bottomLeft());
    int rowCountPerScreen = indexBottom.row()-indexTop.row()+1;

    int top = qMax(0, indexTop.row()-rowCountPerScreen);
    for(int i=top ; i <= indexTop.row(); ++i)
    {
        QModelIndex mi = proxyModel_->index(i,0);
        proxyModel_->data(mi, Qt::DecorationRole);
    }


    int rowBottom = indexBottom.row();
    int maxBottom = rowBottom + rowCountPerScreen; // upto next page
    for(int i=rowBottom; i < maxBottom; ++i)
    {
        QModelIndex mi = proxyModel_->index(i,0);
        proxyModel_->data(mi, Qt::DecorationRole);
    }
}

void MainWindow::on_action_Add_Folder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),lastSelectedDir_);
    if(dir.isEmpty())
        return;
    lastSelectedDir_ = dir;

    AddUserEntryDirectory(DirectoryItem::DI_NORMAL, canonicalDir(dir), false, false);
}

void MainWindow::on_action_Extentions_triggered()
{
    Option_Extention dlg(this);
    dlg.strAllow_ = optionAllowExtention_;
    dlg.strDeny_ = optionDenyExtention_;

    if(QDialog::Accepted != dlg.exec())
        return;

    optionAllowExtention_ = dlg.strAllow_;
    optionDenyExtention_ = dlg.strDeny_;
}
