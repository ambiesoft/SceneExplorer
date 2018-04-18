//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <QDebug>
#include <QStringList>
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
#include <QFontDialog>
#include <QInputDialog>

#include "ui_mainwindow.h"
#include "taskgetdir.h"
#include "taskffmpeg.h"
#include "optionfontdialog.h"
#include "tablemodel.h"
#include "foldermodel.h"
#include "taskmodel.h"
#include "tableitemdata.h"
#include "settings.h"
#include "waitcursor.h"
// #include "taskfilter.h"
#include "errorinfoexception.h"

#include "optiondialog.h"
#include "optionextension.h"
#include "optionexternaltoolsdialog.h"
#include "ffmpeg.h"
#include "globals.h"
#include "helper.h"
#include "blockedbool.h"

#include "extension.h"
#include "sql.h"
#include "tableitemdelegate.h"

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Settings& settings) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings_(settings)
{
    ui->setupUi(this);
    initLangMenus();
    idManager_ = new IDManager(this);

    this->setWindowTitle(Consts::APPNAME_DISPLAY);



	// menu
    QObject::connect(ui->menu_Task, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuTask_AboutToShow);
    QObject::connect(ui->menu_Docking_windows, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuDocking_windows_AboutToShow);
//    QObject::connect(ui->menu_Favorites, &QMenu::aboutToShow,
//                     this, &MainWindow::onMenu_Favorites_AboutToShow);
    QObject::connect(ui->menu_Recent_documets, &QMenu::aboutToShow,
                     this, &MainWindow::onMenu_RecentDocuments_AboutToShow);
    connect(ui->menu_Language, &QMenu::aboutToShow,
            this, &MainWindow::onMenuLanguage_AboutToShow);



	// table
    QObject::connect(ui->tableView->verticalScrollBar(), &QScrollBar::valueChanged,
                     this, &MainWindow::on_tableView_scrollChanged);

	// does not work to wordwrap
	//connect(
	//	ui->tableView->horizontalHeader(),
	//	SIGNAL(sectionResized(int, int, int)),
	//	ui->tableView,
	//	SLOT(resizeRowsToContents()));

    tableModel_=new TableModel(ui->tableView, this);

    //static TableItemDelegate* tigate = new TableItemDelegate(ui->tableView);
    //ui->tableView->setItemDelegateForRow(0,tigate);


    proxyModel_ = new FileMissingFilterProxyModel(ui->tableView);
    proxyModel_->setSourceModel(tableModel_);
    // very slow
    // ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setModel(proxyModel_);


    QObject::connect(tableModel_, &TableModel::itemCountChanged,
                     this, &MainWindow::tableItemCountChanged);
    //QObject::connect(tableModel_, &TableModel::sortParameterChanged,
    //                 this, &MainWindow::tableSortParameterChanged);
	// not called
	// ui->tableView->setItemDelegate(new ImageSizeDelegate(ui->tableView));


	// tree
    QItemSelectionModel* treeSelectionModel = ui->directoryWidget->selectionModel();
    QObject::connect(treeSelectionModel, &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::on_directoryWidget_selectionChanged);
	QObject::connect(ui->directoryWidget, &DirectoryEntry::itemChanged,
                     this, &MainWindow::on_directoryWidget_itemChanged);


    // tool bar
    tbLabelSort_ = new QToolButton(ui->mainToolBar);  // intensional leak
    QPalette sample_palette;
    sample_palette.setColor(QPalette::WindowText, Qt::blue);
    tbLabelSort_->setText(tr("Sort:"));
    tbLabelSort_->setEnabled(false);
    tbLabelSort_->setPalette(sample_palette);
    ui->mainToolBar->insertWidget(ui->placeHolder_Sort, tbLabelSort_);

	// ToolButton Sort
	tbNameSort_ = new QToolButton(ui->mainToolBar);  // intensional leak
	tbNameSort_->setCheckable(true);
    tbNameSort_->setToolTip(ui->actionSort_by_file_name->toolTip());
    sortManager_.setToolButton(SORT_FILENAME, tbNameSort_,
                               QIcon(":/resource/images/sort-by-alphabet.png"),
                               QIcon(":/resource/images/sort-by-alphabet-rev.png"));
	connect(tbNameSort_, SIGNAL(clicked()),
		this, SLOT(on_actionSort_by_file_name_triggered()));
    ui->mainToolBar->insertWidget(ui->placeHolder_Sort, tbNameSort_);

    tbSizeSort_ = new QToolButton(ui->mainToolBar);  // intensional leak
    tbSizeSort_->setCheckable(true);
    tbSizeSort_->setToolTip(ui->actionSort_by_file_size->toolTip());
    sortManager_.setToolButton(SORT_SIZE, tbSizeSort_,
                               QIcon(":/resource/images/sort-by-size.png"),
                               QIcon(":/resource/images/sort-by-size-rev.png"));
    connect(tbSizeSort_, SIGNAL(clicked()),
        this, SLOT(on_actionSort_by_file_size_triggered()));
    ui->mainToolBar->insertWidget(ui->placeHolder_Sort, tbSizeSort_);

    tbWtime_ = new QToolButton(ui->mainToolBar);  // intensional leak
    tbWtime_->setCheckable(true);
    tbWtime_->setToolTip(ui->actionSort_by_wtime->toolTip());
    sortManager_.setToolButton(SORT_WTIME, tbWtime_,
                               QIcon(":/resource/images/sort-wtime.png"),
                               QIcon(":/resource/images/sort-wtime-rev.png"));
    connect(tbWtime_, SIGNAL(clicked()),
        this, SLOT(on_actionSort_by_wtime_triggered()));
    ui->mainToolBar->insertWidget(ui->placeHolder_Sort, tbWtime_);

    tbOpenCount_ = new QToolButton(ui->mainToolBar);  // intensional leak
    tbOpenCount_->setCheckable(true);
    tbOpenCount_->setToolTip(ui->actionSort_by_open_count->toolTip());
    sortManager_.setToolButton(SORT_OPENCOUNT, tbOpenCount_,
                               QIcon(":/resource/images/sort-opencount.png"),
                               QIcon(":/resource/images/sort-opencount-rev.png"));
    connect(tbOpenCount_, SIGNAL(clicked()),
        this, SLOT(on_actionSort_by_open_count_triggered()));
    ui->mainToolBar->insertWidget(ui->placeHolder_Sort, tbOpenCount_);

    tbLastAccess_ = new QToolButton(ui->mainToolBar);  // intensional leak
    tbLastAccess_->setCheckable(true);
    tbLastAccess_->setToolTip(ui->actionSort_by_last_access->toolTip());
    sortManager_.setToolButton(SORT_LASTACCESS, tbLastAccess_,
                               QIcon(":/resource/images/sort-lastaccess.png"),
                               QIcon(":/resource/images/sort-lastaccess-rev.png"));
    connect(tbLastAccess_, SIGNAL(clicked()),
        this, SLOT(on_actionSort_by_last_access_triggered()));
    ui->mainToolBar->insertWidget(ui->placeHolder_Sort, tbLastAccess_);

    ui->mainToolBar->removeAction(ui->placeHolder_Sort);


    // tool bar show missing
    tbShowNonExistant_ = new QToolButton(ui->mainToolBar);
    tbShowNonExistant_->setText(tr("Show missing files"));
    tbShowNonExistant_->setIcon(QIcon(":/resource/images/missing.png"));
    //pixmap = style()->standardPixmap(QStyle::SP_MediaPlay);

    tbShowNonExistant_->setCheckable(true);
    QObject::connect(tbShowNonExistant_, SIGNAL(toggled(bool)),
                     this, SLOT(on_action_ShowMissingClicked()));



//    QToolButton* myTooButton = new QToolButton(ui->mainToolBar);
//    ui->mainToolBar->addWidget(myTooButton);

    cmbFind_ = new FindComboBox(ui->mainToolBar);
    cmbFind_->setMinimumWidth(100);
    cmbFind_->setMaximumWidth(100);
    cmbFind_->setEditable(true);
    QStringList findtexts = settings_.valueStringList(Consts::KEY_COMBO_FINDTEXTS);
    cmbFind_->addItems(findtexts);
    cmbFind_->setEditText("");
    QObject::connect(cmbFind_, &FindComboBox::on_EnterPressed,
                     this, &MainWindow::on_FindCombo_EnterPressed);


    ui->mainToolBar->insertWidget(ui->placeHolder_ShowMissing, tbShowNonExistant_);
    ui->mainToolBar->insertWidget(ui->placeHolder_ComboBox, cmbFind_);

    ui->mainToolBar->removeAction(ui->placeHolder_ShowMissing);
    ui->mainToolBar->removeAction(ui->placeHolder_ComboBox);

    CreateLimitManager();


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

    vVal = settings.value(Consts::KEY_LASTSELECTEDADDFOLDERDIRECTORY);
    if(vVal.isValid())
        lastSelectedAddFolderDir_ = vVal.toString();

    vVal = settings.value(Consts::KEY_LASTSELECTEDSCANDIRECTORY);
    if(vVal.isValid())
        lastSelectedScanDir_ = vVal.toString();

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

    // make default true
    bool bShowMissing = (!vVal.isValid() || vVal.toBool());
    if(bShowMissing)
    {
        ui->actionShow_missing_files->setChecked(true);
        tbShowNonExistant_->setChecked(true);
    }



    optionThreadcountGetDir_ = settings_.valueInt(Consts::KEY_MAX_GETDIR_THREADCOUNT, optionThreadcountGetDir_);
    optionThreadcountThumbnail_ = settings_.valueInt(Consts::KEY_MAX_THUMBNAIL_THREADCOUNT, optionThreadcountThumbnail_);
    optionThumbCount_ = settings_.valueInt(Consts::KEY_THUMBNAIL_COUNT, 3);
    if(optionThumbCount_ != 3 && optionThumbCount_ != 5)
        optionThumbCount_ = 3;
    tableModel_->SetColumnCountImage(optionThumbCount_);
    tableModel_->SetTitleTextTemplate(settings_.valueString(Consts::KEY_TITLE_TEXT_TEMPLATE, Consts::DEFAULT_ITEM_MAIN_TEXT));
    tableModel_->SetInfoTextTemplate(settings_.valueString(Consts::KEY_INFO_TEXT_TEMPLATE, Consts::DEFAULT_ITEM_SUB_TEXT));

    tableModel_->SetImageCache((ImageCacheType)settings_.valueInt(Consts::KEY_IMAGECACHETYPE,1));


    // extension
    Extension::Load(settings_);


    // font
    QFont font;
    vVal = settings_.value(Consts::KEY_FONT_TABLEINFO);
    if(vVal.isValid() && font.fromString(vVal.toString()))
    {
        tableModel_->SetInfoFont(font);
    }
    else
    {
        tableModel_->SetInfoFont(ui->tableView->font());
    }

    vVal = settings_.value(Consts::KEY_FONT_TABLEDETAIL);
    if(vVal.isValid() && font.fromString(vVal.toString()))
    {
        tableModel_->SetDetailFont(font);
    }
    else
    {
        tableModel_->SetDetailFont(ui->tableView->font());
    }

    vVal = settings_.value(Consts::KEY_FONT_OUPUT);
    if(vVal.isValid() && font.fromString(vVal.toString()))
    {
        ui->txtLog->setFont(font);
    }


    // external tools
    int externalToolsCount = settings_.valueInt(Consts::KEY_EXTERNALTOOLS_COUNT, 0);
    for(int i=0 ; i < externalToolsCount; ++i)
    {
        externalTools_.append(ExternalToolItem::Load(i, settings_));
    }

    FFMpeg::GetFFprobe(settings);
    FFMpeg::GetFFmpeg(settings);


	// sort
	sortManager_.InitCurrentSort((SORTCOLUMNMY)settings.valueInt(Consts::KEY_SORT, (int)SORT_NONE),
		settings_.valueBool(Consts::KEY_SORTREV, false));


    // recents
    recents_ = settings_.valueStringList(Consts::KEY_RECENT_OPENDOCUMENTS);

    if(settings_.valueBool(Consts::KEY_OPEN_LASTOPENEDDOCUMENT, true))
    {
        if(recents_.isEmpty() || !OpenDocument(recents_[0], true))
        {
            // recents is empty or OpenDocument fails.
            // open default document.
            QString defaultdoc = GetDefaultDocumentPath();
            if(defaultdoc.isEmpty())
                return;

            if(!OpenDocument(defaultdoc, false))
                return ;
		}
    }
    else
    {
        QString defaultdoc = GetDefaultDocumentPath();
        if(defaultdoc.isEmpty())
            return;

        if(!OpenDocument(defaultdoc, false))
            return;
    }

    restoreGeometry(settings.value(Consts::KEY_GEOMETRY).toByteArray());
    restoreState(settings.value(Consts::KEY_WINDOWSTATE).toByteArray());
    UpdateTitle(QStringList(), UpdateTitleType::INIT);
	initialized_ = true;
}
MainWindow::SortManager::SortManager()
{
	sort_ = SORT_NONE;
    for (size_t i = 0; i < (sizeof(ascending_)/sizeof(ascending_[0])); ++i)
	{
        ascending_[i] = false;
		tbs_[i] = nullptr;
	}

	ascending_[SORT_FILENAME] = true;
	ascending_[SORT_SIZE] = true;
}

void MainWindow::CreateLimitManager()
{
    if(settings_.valueBool(Consts::KEY_LIMIT_ITEMS, false))
    { // Create
        if(limitManager_)
        { // already exits
            limitManager_->SetNumberOfRows(settings_.valueInt(Consts::KEY_LIMIT_NUMBEROFROWS, 1000));
        }
        else
        {
            actionLimitFirst_ = new QAction("<<",ui->mainToolBar);
            QObject::connect(actionLimitFirst_, &QAction::triggered,
                             this, &MainWindow::on_LimitFirst_triggered);
            ui->mainToolBar->insertAction(ui->actionplaceHolder_Limit, actionLimitFirst_);

            actionLimitPrev_ = new QAction("<",ui->mainToolBar);
            QObject::connect(actionLimitPrev_, &QAction::triggered,
                             this, &MainWindow::on_LimitPrev_triggered);
            ui->mainToolBar->insertAction(ui->actionplaceHolder_Limit, actionLimitPrev_);

            if(!cmbLimit_)
            {
                cmbLimit_ = new QComboBox(ui->mainToolBar);  // intentional leak
                cmbLimit_->setEditable(true);
                cmbLimit_->lineEdit()->setReadOnly(true);
                cmbLimit_->lineEdit()->setAlignment(Qt::AlignCenter);

                QObject::connect(cmbLimit_, SIGNAL(currentIndexChanged(int)),
                                 this, SLOT(onCmbLint_currentIndexChanged(int)));

                ui->mainToolBar->insertWidget(ui->actionplaceHolder_Limit, cmbLimit_);
            }
            else
            {
                Q_ASSERT(false);
            }

            actionLimitNext_ = new QAction(">",ui->mainToolBar);
            QObject::connect(actionLimitNext_, &QAction::triggered,
                             this, &MainWindow::on_LimitNext_triggered);
            ui->mainToolBar->insertAction(ui->actionplaceHolder_Limit, actionLimitNext_);

            actionLimitLast_ = new QAction(">>",ui->mainToolBar);
            QObject::connect(actionLimitLast_, &QAction::triggered,
                             this, &MainWindow::on_LimitLast_triggered);


            ui->mainToolBar->insertAction(ui->actionplaceHolder_Limit, actionLimitLast_);

            sepLimit_ = ui->mainToolBar->insertSeparator(ui->actionplaceHolder_Limit);

            int numofrows = settings_.valueInt(Consts::KEY_LIMIT_NUMBEROFROWS, 1000);
            limitManager_ = new LimitManager(numofrows, cmbLimit_);
        }
    }
    else
    { //Destroy limit manager
        if(limitManager_)
        {
            Q_ASSERT(actionLimitFirst_);
            QObject::disconnect(actionLimitFirst_, &QAction::triggered,
                             this, &MainWindow::on_LimitFirst_triggered);
            ui->mainToolBar->removeAction(actionLimitFirst_);
            delete actionLimitFirst_;
            actionLimitFirst_ = nullptr;

            Q_ASSERT(actionLimitPrev_);
            QObject::disconnect(actionLimitPrev_, &QAction::triggered,
                             this, &MainWindow::on_LimitPrev_triggered);
            ui->mainToolBar->removeAction(actionLimitPrev_);
            delete actionLimitPrev_;
            actionLimitPrev_ = nullptr;

            Q_ASSERT(cmbLimit_);
            delete cmbLimit_;
            cmbLimit_ = nullptr;

            Q_ASSERT(actionLimitNext_);
            QObject::disconnect(actionLimitNext_, &QAction::triggered,
                             this, &MainWindow::on_LimitNext_triggered);
            ui->mainToolBar->removeAction(actionLimitNext_);
            delete actionLimitNext_;
            actionLimitNext_ = nullptr;

            Q_ASSERT(actionLimitLast_);
            QObject::disconnect(actionLimitLast_, &QAction::triggered,
                             this, &MainWindow::on_LimitLast_triggered);
            ui->mainToolBar->removeAction(actionLimitLast_);
            delete actionLimitLast_;
            actionLimitLast_ = nullptr;

            ui->mainToolBar->removeAction(sepLimit_);
            delete sepLimit_;
            sepLimit_ = nullptr;

            delete limitManager_;
            limitManager_ = nullptr;
        }
    }
    ui->actionplaceHolder_Limit->setVisible(false);
    //QAction* sep = ui->mainToolBar->findChild("sepAfterLimit");

    // ui->mainToolBar->removeAction(ui->actionplaceHolder_Limit);
}
QString MainWindow::GetDefaultDocumentPath()
{
    QString docdir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if(docdir.isEmpty())
    {
        Alert(this,QString(tr("Failed to get document directory.")));
        return QString();
    }
    docdir=pathCombine(docdir, Consts::APPNAME);
    QDir().mkpath(docdir);
    if(!QDir(docdir).exists())
    {
        Alert(this,QString(tr("Failed to create directory. \"%1\"")).arg(docdir));
        return QString();
    }

    return pathCombine(docdir,"default.scexd");
}

bool MainWindow::OpenDocument(const QString& file, const bool bExists)
{
    Document* pNewDoc = new Document;
    if(!pNewDoc->Load(file, bExists))
    {
        Alert(this, pNewDoc->GetLastErr());
        delete pNewDoc;
        return false;
    }

    qDebug() << "Document Loaded: " << file;
    recents_.removeDuplicates();
    recents_.removeOne(file);
    recents_.insert(0, file);

    if(pDoc_)
    {
        pDoc_->Store(ui->directoryWidget);
        delete pDoc_;
    }
    pDoc_ = pNewDoc;
    InitDocument();
    return true;
}
void MainWindow::InitDocument()
{
    BlockedBool btD(&directoryChanging_);

    ui->directoryWidget->clear();

    AddUserEntryDirectory(DirectoryItem::DI_ALL, QString(),
                          pDoc_->IsAllSelected(),
                          pDoc_->IsAllChecked());
    for(int i=0 ; i < pDoc_->dirCount() ; ++i)
    {
        AddUserEntryDirectory(DirectoryItem::DI_NORMAL,
                              pDoc_->GetDEText(i),
                              pDoc_->IsDESelected(i),
                              pDoc_->IsDEChecked(i));
    }
    AddUserEntryDirectory(DirectoryItem::DI_MISSING, QString(), false, false);

    directoryChangedCommon(true);
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
        insertLog(TaskKind::App, 0, tr("======== All tasks Cleared ========"));
}



MainWindow::~MainWindow()
{
	closed_ = true;

    delete pDoc_;
    pDoc_ = nullptr;

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

    // BlockedBool btPause(&gPaused, true, gPaused);

    Q_UNUSED(id);
    // WaitCursor wc;

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
                insertLog(TaskKind::GetDir, id, QString(tr("Already exists. \"%1\"")).
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
        TaskFFmpeg* pTask = new TaskFFmpeg(FFMpeg::GetFFprobe(settings_),
                                           FFMpeg::GetFFmpeg(settings_),
                                           gLoopId,
                                           idManager_->Increment(IDKIND_FFmpeg),
                                           file);
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
        insertLog(TaskKind::App, 0, tr("======== All Tasks finished ========"));
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
void MainWindow::on_context_openSelectedVideo()
{
    openVideo(getSelectedVideo());
}
void MainWindow::on_context_openSelectedVideoInFolder()
{
    openVideoInFolder(getSelectedVideo());
}

void MainWindow::on_context_copySelectedVideoPath()
{
    QApplication::clipboard()->setText(getSelectedVideo());
}
void MainWindow::on_context_Rename()
{
    QString oldfull = getSelectedVideo();
    QString olddir = canonicalDir(QFileInfo(oldfull).canonicalPath());
    QString oldname = QFileInfo(oldfull).fileName();

    bool ok;
    QString newname = QInputDialog::getText(this,
                                         tr("Rename"),
                                         tr("&New name:"),
                                         QLineEdit::Normal,
                                         oldname,
                                         &ok,
                                         GetDefaultDialogFlags());
    if(!ok)
        return;
    if(newname.isEmpty())
        return;

    if(newname==oldname)
    {
        Alert(this,tr("New name is same as old name."));
        return;
    }
	QFile filefull(oldfull);
    if(!filefull.rename(pathCombine(olddir, newname)))
    {
		Alert(this,
			QString(tr("Failed to rename file. (%1)")).arg(filefull.errorString()));
        return;
    }
    if(!gpSQL->RenameEntry(olddir, oldname, olddir,newname))
    {
        Alert(this, tr("Failed to rename in database."));
        return;
    }
    tableModel_->RenameEntry(olddir, oldname, olddir, newname);
}

void MainWindow::on_context_removeFromDatabase()
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
void MainWindow::on_context_ExternalTools()
{
    QString movieFile = getSelectedVideo(true);

    QAction* act = (QAction*)QObject::sender();
    int i = act->data().toInt();
    QString exe = externalTools_[i].GetExe();
    QString arg = externalTools_[i].GetArg();
    QString argparsed;

    static QRegExp rx("(\\$\\{\\w+\\})");

    int prevpos = 0;
    int pos = 0;

    while ((pos = rx.indexIn(arg, pos)) != -1)
    {
        argparsed += arg.mid(prevpos,pos-prevpos);
        int matchedlen = rx.matchedLength();
        QString s = arg.mid(pos,matchedlen);// rx.cap(i++);
        if(s=="${filefullpath}")
        {
            argparsed += movieFile;
        }
        else if(s=="${directoryfullpath}")
        {
            argparsed += QFileInfo(movieFile).dir().absolutePath();
        }
        pos += matchedlen;
        prevpos = pos;
    }
    argparsed += arg.mid(prevpos);




//    QStringList argconst;
//    arg << movieFile;

    QString command;
    command += dq(exe);
    command += " ";
    command += argparsed;
    qDebug() << "QProcess::startDetached:" << command;
    if(!QProcess::startDetached(command))
    {
        QString message;
        message += tr("Failed to start new process.");
        message += "\n\n";
        message += tr("Command line:");
        message += "\n";
        message += command;
        Alert(this, message);
        return;
    }

    if(externalTools_[i].IsCountAsOpen())
    {
        QString movieFileCanon = getSelectedVideo(false);
        gpSQL->IncrementOpenCount(movieFileCanon);
        tableModel_->UpdateItem(movieFileCanon);
    }
}

void MainWindow::on_context_copySelectedVideoFilename()
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
    if(limitManager_)
        limitManager_->Reset();
    directoryChangedCommon();
}

void MainWindow::directoryChangedCommon(bool bForceRead)
{
	if (!initialized_ || closed_)
		return;

    if (directoryChanging_ && !bForceRead)
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

    tbShowNonExistant_->setEnabled(!bOnlyMissing);

    if(!bForceRead)
    {
        if (dirs == currentDirs_)
            return;
    }
	if (limitManager_ && currentDirs_ != dirs)
	{
		limitManager_->Reset();
	}
    currentDirs_ = dirs;


    GetSqlAllSetTable(dirs, bOnlyMissing);
}

void MainWindow::GetSqlAllSetTable(const QStringList& dirs, bool bOnlyMissing)
{
    tableModel_->SetShowMissing(tbShowNonExistant_->isChecked() );
    QElapsedTimer timer;
    timer.start();

	if (limitManager_ && limitManager_->IsNotCounted())
	{
		qlonglong count = gpSQL->GetAllCount(dirs);
		limitManager_->SetAllCount(count);

		size_t cmbcount = (count / limitManager_->GetNumberOfRows());
		if ((count % limitManager_->GetNumberOfRows()) != 0)
			cmbcount++;

        BlockedBool blc(limitManager_->GetBlockPointer());
        cmbLimit_->clear();
		for (size_t i = 0; i < cmbcount; ++i)
		{
            cmbLimit_->addItem(QString("%1").arg(i+1));
            cmbLimit_->setItemData((int)i, Qt::AlignCenter, Qt::TextAlignmentRole);
		}
	}
    QList<TableItemDataPointer> all;
    gpSQL->GetAll(all,
                  dirs,
                  cmbFind_->currentText(),
                  bOnlyMissing,
                  sortManager_.GetCurrentSort(),
                  sortManager_.GetCurrentRev(),
                  limitManager_ ?
                      LimitArg(limitManager_->GetCurrentIndex(), limitManager_->GetNumberOfRows()): LimitArg());


    UpdateTitle(dirs, bOnlyMissing ? UpdateTitleType::ONLYMISSING : UpdateTitleType::DEFAULT);

    insertLog(TaskKind::App,
              0,
              QString(tr("Quering Database takes %1 milliseconds.")).arg(timer.elapsed()));

    timer.start();
    tableModel_->ResetData(all);

    insertLog(TaskKind::App,
              0,
              QString(tr("Resetting data takes %1 milliseconds.")).arg(timer.elapsed()));


	tableSortParameterChanged(sortManager_.GetCurrentSort(), sortManager_.GetCurrentRev());
}

void MainWindow::UpdateTitle(const QStringList& dirs, UpdateTitleType utt)
{
    QStringList titledirs;

    switch(utt)
    {
    case UpdateTitleType::ONLYMISSING:
        titledirs << tr("Missing");
        break;

    case UpdateTitleType::DEFAULT:
        if(dirs.empty())
        {
            titledirs << tr("All");
        }
        else
        {
            titledirs = dirs;
        }
        break;
    case UpdateTitleType::INIT:
        break;
    default:
        Q_ASSERT(false);
    }

    QString title;
    if(pDoc_)
    {
        title.append(pDoc_->GetFileName());
        title.append(" - ");
    }
    for(int i=0 ; i < titledirs.count(); ++i)
    {
        title.append(titledirs[i]);
        if((i+1) != titledirs.count())
            title.append(" | ");
    }

    if(title.isEmpty())
        title = Consts::APPNAME_DISPLAY;
    else
    {
        title.append(" - ");
        title.append(Consts::APPNAME_DISPLAY);
    }

    this->setWindowTitle(title);
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

    if(limitManager_)
        limitManager_->Reset();

    directoryChangedCommon();
}

void MainWindow::tableItemCountChanged()
{
    // slItemCount_->setText(QString(tr("Items: %1")).arg(tableModel_->GetItemCount()));
    slItemCount_->setText(QString(tr("Items: %1")).arg(proxyModel_->GetItemCount()));
}
void MainWindow::tableSortParameterChanged(SORTCOLUMNMY sc, bool rev)
{
    QString text = QString(tr("Sort: %1 %2")).
            arg(GetSortColumnName(sc)).
            arg(rev ? "-" : "+");
    slItemSort_->setText(text);
}


bool MainWindow::IsInitialized() const
{
    return initialized_;
}

void MainWindow::on_action_Focus_find_triggered()
{
    cmbFind_->setFocus();
}
void MainWindow::on_action_Find_triggered()
{
    if(limitManager_)
        limitManager_->Reset();

    directoryChangedCommon(true);

    QString cur = cmbFind_->currentText();
    InsertUniqueTextToComboBox(*cmbFind_, cur);
    cmbFind_->setCurrentIndex(0);
}
void MainWindow::on_action_Clear_triggered()
{
    if(limitManager_)
        limitManager_->Reset();

    cmbFind_->setCurrentText(QString());
    directoryChangedCommon(true);
}

void MainWindow::on_FindCombo_EnterPressed()
{
    on_action_Find_triggered();
}


void MainWindow::on_tableView_scrollChanged(int pos)
{
    QModelIndex indexTop = ui->tableView->indexAt(ui->tableView->rect().topLeft());
    QModelIndex indexBottom = ui->tableView->indexAt(ui->tableView->rect().bottomLeft());
    int rowCountPerScreen = indexBottom.row()-indexTop.row()+1;

    int top = qMax(0, indexTop.row()-rowCountPerScreen);
    int topEnd = pos < 0 ? indexBottom.row() : indexTop.row();
    for(int i=top ; i <= topEnd; ++i)
    {
        QModelIndex mi = proxyModel_->index(i,0);
        proxyModel_->data(mi, Qt::DecorationRole);
        proxyModel_->data(mi, Qt::DisplayRole);
        // ui->tableView->resizeRowToContents(mi.row());
    }


    int rowBottom = indexBottom.row();
    int maxBottom = rowBottom + rowCountPerScreen; // upto next page
    for(int i=rowBottom; i < maxBottom; ++i)
    {
        QModelIndex mi = proxyModel_->index(i,0);
        proxyModel_->data(mi, Qt::DecorationRole);
        proxyModel_->data(mi, Qt::DisplayRole);
        // ui->tableView->resizeRowToContents(mi.row());
    }
}

void MainWindow::on_action_Add_Folder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),lastSelectedAddFolderDir_);
    if(dir.isEmpty())
        return;
    lastSelectedAddFolderDir_ = dir;

    AddUserEntryDirectory(DirectoryItem::DI_NORMAL, canonicalDir(dir), false, false);
}

void MainWindow::on_action_Extentions_triggered()
{
    OptionExtension dlg(this, settings_);
    dlg.exec();
}

void MainWindow::on_action_Font_triggered()
{
//    OptionFontDialog dlg(this);
//    if(QDialog::Accepted != dlg.exec())
//        return;

    bool ok;
    QFont font = QFontDialog::getFont(
                &ok,
                tableModel_->GetInfoFont(),
                this);
    if (!ok)
        return;

    tableModel_->SetInfoFont(font);
    settings_.setValue(Consts::KEY_FONT_TABLEINFO, font.toString());
}

void MainWindow::on_action_FontDetail_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(
                &ok,
                tableModel_->GetDetailFont(),
                this);
    if (!ok)
        return;

    tableModel_->SetDetailFont(font);
    settings_.setValue(Consts::KEY_FONT_TABLEDETAIL, font.toString());
}

void MainWindow::on_action_Output_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(
                &ok,
                ui->txtLog->font(),
                this);
    if (!ok)
        return;

    ui->txtLog->setFont(font);
    settings_.setValue(Consts::KEY_FONT_OUPUT, font.toString());
}


void MainWindow::on_actionExternal_Tools_triggered()
{
    OptionExternalToolsDialog dlg(settings_, this);
    dlg.items_ = externalTools_;
    if(QDialog::Accepted != dlg.exec())
        return;
    externalTools_ = dlg.items_;
}

void MainWindow::on_action_New_triggered()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);

    QStringList filters;
    filters << tr("SceneExplorer Document (*.scexd)")
            << tr("Any files (*)");

    dlg.setNameFilters(filters);
    if(!dlg.exec())
        return;

    QString newFile = dlg.selectedFiles()[0];
    OpenDocument(newFile, QFile(newFile).exists());
}
void MainWindow::on_action_Open_triggered()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);

    QStringList filters;
    filters << tr("SceneExplorer Document (*.scexd)")
            << tr("Any files (*)");

    dlg.setNameFilters(filters);
    if(!dlg.exec())
        return;

    OpenDocument(dlg.selectedFiles()[0], true);
}

void MainWindow::on_action_Save_triggered()
{
    pDoc_->Store(ui->directoryWidget);
}

void MainWindow::on_actionSave_as_triggered()
{

}


bool MainWindow::checkExeCommon(const QString& exe, QString& errString) const
{
    QProcess process;
    process.setProgram(exe);
    process.setArguments( QStringList() <<
                          "-version"
                          );

    process.start(QProcess::ReadOnly);
    if(!process.waitForStarted(-1))
    {
        errString = process.errorString();
        return false;
    }
    if(!process.waitForFinished(-1))
    {
        errString = process.errorString();
        return false;
    }

    if(0 != process.exitCode())
    {
        errString = process.errorString();
        return false;
    }
    return true;
}

bool MainWindow::checkFFprobe(QString& errString) const
{
    return checkExeCommon(FFMpeg::GetFFmpeg(settings_), errString);
}
bool MainWindow::checkFFmpeg(QString& errString) const
{
    return checkExeCommon(FFMpeg::GetFFmpeg(settings_), errString);
}

void MainWindow::on_actionAbout_document_triggered()
{
    QString title = Consts::APPNAME_DISPLAY;
    QString text;

    text.append(tr("Executable"));
    text.append(": ");
    text.append(QCoreApplication::applicationFilePath());

    text.append("\n");

    text.append(tr("Document"));
    text.append(": ");
    text.append(pDoc_ ? pDoc_->GetFullName(): tr("<No document>"));

    text.append("\n");

    text.append(tr("Ini file"));
    text.append(": ");
    text.append(settings_.fileName());

    text.append("\n");

    text.append(tr("Database directory"));
    text.append(": ");
    text.append(QDir(".").absolutePath());


    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Information);
    msgbox.setText(text);
    msgbox.setWindowTitle(title);
    msgbox.exec();
}



void MainWindow::on_actionStart_scan_to_create_thumnails_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),lastSelectedScanDir_);
    if(dir.isEmpty())
        return;
    lastSelectedScanDir_ = dir;

    StartScan(dir);
}


void MainWindow::on_action_Empty_find_texts_triggered()
{
    int count = cmbFind_->count();
    if(count==0)
    {
        Info(this, tr("There are no find texts."));
        return;
    }
    if(!YesNo(this, QString(tr("Are you sure you want to empty %1 find texts?")).arg(count)))
        return;
    cmbFind_->clear();
}




void MainWindow::on_actionShow_missing_files_triggered()
{
    on_ShowMissingClicked_common(ui->actionShow_missing_files->isChecked());
}



void MainWindow::initLangMenus()
{
    QString lang = settings_.valueString(Consts::KEY_LANGUAGE);
    if(lang.isEmpty() || lang=="English")
        return;

    ui->menu_Language->setTitle(tr("Language") + " (" + ("Language") + ")");
    ui->action_System_default->setText(tr("System default") + " (" + ("System default") + ")");
    ui->action_English->setText(tr("English") + " (" + ("English") + ")");
    ui->action_Japanese->setText(tr("Japanese") + " (" + ("Japanese") + ")");
}

void MainWindow::on_action_Command_Line_triggered()
{
    QString helpText;
    processCommandLine(&helpText);

    Info(this,helpText);
}
