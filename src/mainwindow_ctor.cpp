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

#include <QScrollBar>
#include <QLabel>
#include <QDebug>

#include "taskmodel.h"
#include "extension.h"
#include "ffmpeg.h"
#include "tagitem.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

using namespace Consts;

MainWindow::MainWindow(QWidget *parent,
                       Settings& settings,
                       const QString& docToOpen) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings_(settings)
{
    ui->setupUi(this);
    initLangMenus();
    idManager_ = new IDManager(this);
    DirectoryItem::parent_ = ui->directoryWidget;
    TagItem::parent_ = ui->listTag;

    this->setWindowTitle(APPNAME_DISPLAY);



    // menu
    QObject::connect(ui->menu_View, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuView_AboutToShow);
    QObject::connect(ui->menu_Edit, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuEdit_AboutToShow);
    QObject::connect(ui->menu_Task, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuTask_AboutToShow);
    QObject::connect(ui->menu_Folder, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuFolder_AboutToShow);
    QObject::connect(ui->menu_Tag, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuTag_AboutToShow);
    QObject::connect(ui->menu_Docking_windows, &QMenu::aboutToShow,
                     this, &MainWindow::onMenuDocking_windows_AboutToShow);
//    QObject::connect(ui->menu_Favorites, &QMenu::aboutToShow,
//                     this, &MainWindow::onMenu_Favorites_AboutToShow);
    QObject::connect(ui->menu_Recent_documets, &QMenu::aboutToShow,
                     this, &MainWindow::onMenu_RecentDocuments_AboutToShow);
    connect(ui->menu_Language, &QMenu::aboutToShow,
            this, &MainWindow::onMenuLanguage_AboutToShow);


    // sortManager <-> action
    sortManager_.setAction(SORT_FILENAME,ui->actionSort_by_file_name);
    sortManager_.setAction(SORT_FULLNAME,ui->actionSort_by_full_name);
    sortManager_.setAction(SORT_SIZE,ui->actionSort_by_file_size);
    sortManager_.setAction(SORT_WTIME,ui->actionSort_by_wtime);
    sortManager_.setAction(SORT_RESOLUTION,ui->actionSort_by_resolution);
    sortManager_.setAction(SORT_DURATION,ui->actionSort_by_duration);
    sortManager_.setAction(SORT_BITRATE,ui->actionSort_by_bitrate);
    sortManager_.setAction(SORT_OPENCOUNT,ui->actionSort_by_open_count);
    sortManager_.setAction(SORT_LASTACCESS,ui->actionSort_by_last_access);




    // table
    ui->tableView->setSelectionBehavior( QAbstractItemView::SelectItems );
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection );

    connect(ui->tableView->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &MainWindow::OnTableViewScrollChanged);


    tableModel_=new TableModel(ui->tableView, this);


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


    // folder
    QItemSelectionModel* treeSelectionModel = ui->directoryWidget->selectionModel();
    connect(treeSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &MainWindow::OnDirectorySelectionChanged);
    connect(ui->directoryWidget, &DirectoryEntry::itemChanged,
            this, &MainWindow::OndirectoryItemChanged);

    // Tag
    ui->listTag->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listTag, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showTagContextMenu(QPoint)));

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
    connect(tbShowNonExistant_, SIGNAL(toggled(bool)),
            this, SLOT(OnShowMissingClicked()));



//    QToolButton* myTooButton = new QToolButton(ui->mainToolBar);
//    ui->mainToolBar->addWidget(myTooButton);

    cmbFind_ = new FindComboBox(ui->mainToolBar);
    cmbFind_->setMinimumWidth(100);
    cmbFind_->setMaximumWidth(100);
    cmbFind_->setEditable(true);
    QStringList findtexts = settings_.valueStringList(KEY_COMBO_FINDTEXTS);
    cmbFind_->addItems(findtexts);
    cmbFind_->setEditText("");
    connect(cmbFind_, &FindComboBox::on_EnterPressed,
            this, &MainWindow::OnFindComboEnterPressed);


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

    vVal = settings.value(KEY_SIZE);
    if(vVal.isValid())
        resize(vVal.toSize());

    vVal = settings.value(KEY_LASTSELECTEDADDFOLDERDIRECTORY);
    if(vVal.isValid())
        lastSelectedAddFolderDir_ = vVal.toString();

    vVal = settings.value(KEY_LASTSELECTEDSCANDIRECTORY);
    if(vVal.isValid())
        lastSelectedScanDir_ = vVal.toString();

    vVal = settings.value(KEY_LASTSELECTEDDOCUMENT);
    if(vVal.isValid())
        lastSelectedDocumentDir_ = vVal.toString();

    vVal = settings.value(KEY_TREESIZE);
    if(vVal.isValid())
        ui->directoryWidget->setMaximumSize(vVal.toSize());

    vVal = settings.value(KEY_TXTLOGSIZE);
    if(vVal.isValid())
        ui->txtLog->setMaximumSize(vVal.toSize());

    vVal = settings.value(KEY_LISTTASKSIZE);
    if(vVal.isValid())
        ui->listTask->setMaximumSize(vVal.toSize());

    vVal = settings.value(KEY_SHOWMISSING);

    // make default true
    bool bShowMissing = (!vVal.isValid() || vVal.toBool());
    if(bShowMissing)
    {
        ui->action_ShowMissingFiles->setChecked(true);
        tbShowNonExistant_->setChecked(true);
    }



    optionThreadcountGetDir_ = settings_.valueInt(KEY_MAX_GETDIR_THREADCOUNT, optionThreadcountGetDir_);
    optionThreadcountThumbnail_ = settings_.valueInt(KEY_MAX_THUMBNAIL_THREADCOUNT, optionThreadcountThumbnail_);
    optionThumbCount_ = settings_.valueInt(KEY_THUMBNAIL_COUNT, 3);
    if(optionThumbCount_ != 3 && optionThumbCount_ != 5)
        optionThumbCount_ = 3;
    optionThumbFormat_ = settings_.valueString(KEY_THUMBNAIL_FORMAT, "jpg");
    SetTaskPriorityAsInt(settings_.valueInt(KEY_TASK_PRIORITY, -1));
    tableModel_->SetColumnCountImage(optionThumbCount_);
    tableModel_->SetTitleTextTemplate(settings_.valueString(KEY_TITLE_TEXT_TEMPLATE, DEFAULT_ITEM_MAIN_TEXT));
    tableModel_->SetInfoTextTemplate(settings_.valueString(KEY_INFO_TEXT_TEMPLATE, DEFAULT_ITEM_SUB_TEXT));

    tableModel_->SetImageCache((ImageCacheType)settings_.valueInt(KEY_IMAGECACHETYPE,1));


    // extension
    Extension::Load(settings_);


    // font
    QFont font;
    vVal = settings_.value(KEY_FONT_TABLEINFO);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        tableModel_->SetInfoFont(font);
    }
    else
    {
        tableModel_->SetInfoFont(ui->tableView->font());
    }

    vVal = settings_.value(KEY_FONT_TABLEDETAIL);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        tableModel_->SetDetailFont(font);
    }
    else
    {
        tableModel_->SetDetailFont(ui->tableView->font());
    }

    vVal = settings_.value(KEY_FONT_OUPUT);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        ui->txtLog->setFont(font);
    }
    vVal = settings_.value(KEY_FONT_DIRECTORY);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        ui->directoryWidget->setFont(font);
    }
    vVal = settings_.value(KEY_FONT_TASK);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        ui->listTask->setFont(font);
    }
    vVal = settings_.value(KEY_FONT_TAG);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        ui->listTag->setFont(font);
    }
    vVal = settings_.value(KEY_FONT_STATUSBAR);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        ui->statusBar->setFont(font);
        foreach(QWidget* pW, getAllStatusBarWidgets())
        {
            pW->setFont(font);
        }
    }
    vVal = settings_.value(KEY_FONT_DOCKINGWINDOW);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        for(QWidget* pW : getAllDockingWindow())
        {
            pW->setFont(font);
        }
    }
    vVal = settings_.value(KEY_FONT_MENU);
    if(vVal.isValid() && !vVal.toString().isEmpty() && font.fromString(vVal.toString()))
    {
        setMenuFont(font);
    }


    // external tools
    int externalToolsCount = settings_.valueInt(KEY_EXTERNALTOOLS_COUNT, 0);
    for(int i=0 ; i < externalToolsCount; ++i)
    {
        externalTools_.append(ExternalToolItem::Load(i, settings_));
    }

    FFMpeg::GetFFprobe(settings);
    FFMpeg::GetFFmpeg(settings);


    // sort
    sortManager_.InitCurrentSort((SORTCOLUMNMY)settings.valueInt(KEY_SORT, (int)SORT_NONE),
        settings_.valueBool(KEY_SORTREV, false));


    // recents
    recents_ = settings_.valueStringList(KEY_RECENT_OPENDOCUMENTS);
    qDebug() << "recents" << recents_;


    if(!docToOpen.isEmpty())
    {
        // Alert(this, "docToOpen" + docToOpen);
        if(!OpenDocument(docToOpen, false))
            return ;
    }
    else
    {
        if(settings_.valueBool(KEY_OPEN_LASTOPENEDDOCUMENT, true))
        {
            // Alert(this, "recnet[0]=" +recents_[0]);
            if(recents_.isEmpty() ||
                    !QFile(recents_[0]).exists() ||
                    !OpenDocument(recents_[0], true))
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
    }


    restoreGeometry(settings.value(KEY_GEOMETRY).toByteArray());
    restoreState(settings.value(KEY_WINDOWSTATE).toByteArray());
    UpdateTitle(QStringList(), UpdateTitleType::INIT);
    initialized_ = true;
}
