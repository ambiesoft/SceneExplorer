#include <QDesktopServices>
#include <QMessageBox>
#include <QDialog>
#include <QDebug>
#include <QThreadPool>
#include <QFileDialog>
#include <QListWidget>

#include "consts.h"
#include "globals.h"
#include "waitcursor.h"

#include "taskmodel.h"
#include "tableitemdata.h"

#include "optiondialog.h"

#include "taskgetdir.h"
#include "directoryentry.h"

#include "settings.h"
#include "sql.h"
#include "helper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::openVideo(const QString& movieFile)
{
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(movieFile)))
    {
        QMessageBox msgBox;
        msgBox.setText(Consts::APPNAME);
        msgBox.setInformativeText(QString(tr("failed to launch %1.")).arg(movieFile));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    gpSQL->IncrementOpenCount(movieFile);
	tableModel_->UpdateItem(movieFile);
}
void MainWindow::openVideoInFolder(const QString& movieFile)
{
    if(!showInGraphicalShell(this, movieFile))
        Alert(this, tr("Failed to open folder."));
}
void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QVariant v = proxyModel_->data(index, TableModel::TableRole::MovieFile);
    Q_ASSERT(v.isValid());
    Q_ASSERT(!v.toString().isEmpty());

    openVideo(v.toString());
}


void MainWindow::on_action_Options_triggered()
{
    OptionDialog dlg(this);
    dlg.maxff_ = threadcountFFmpeg_;
    if(QDialog::Accepted != dlg.exec())
        return;

    threadcountFFmpeg_ = dlg.maxff_;
    getPoolFFmpeg()->setMaxThreadCount(threadcountFFmpeg_);
}

void MainWindow::onMenuTask_AboutToShow()
{
    qDebug() << "gPaused" << gPaused;
    ui->action_Pause->setChecked(gPaused);
}
void MainWindow::onMenuDocking_windows_AboutToShow()
{
    ui->action_DockFolder->setChecked(!ui->dockLog->isHidden());
}
void MainWindow::on_FavoriteFolder_triggered(bool checked)
{
    Q_UNUSED(checked);
    QAction* qa = (QAction*)QObject::sender();
    Q_ASSERT(qa);
    QStringList dirs = settings_.GetFavorite(qa->text());
    ui->directoryWidget->SetCheck(dirs, true);
    directoryChangedCommon(true);
}
void MainWindow::onMenu_Favorites_AboutToShow()
{
    int sepIndex = 2;
    // TODO
    QMenu* menu = ui->menu_Favorites;
    foreach (QAction *action, menu->actions()) {
         if (action->isSeparator()) {
             qDebug("this action is a separator");
         } else if (action->menu()) {
             qDebug("action: %s", qUtf8Printable(action->text()));
             qDebug(">>> this action is associated with a submenu, iterating it recursively...");
             // enumerateMenu(action->menu());
             qDebug("<<< finished iterating the submenu");
         } else {
             qDebug("action: %s", qUtf8Printable(action->text()));
         }
     }

    while(ui->menu_Favorites->actions().count() > sepIndex)
    {
        QAction *qa = ui->menu_Favorites->actions()[sepIndex];
        ui->menu_Favorites->removeAction(qa);
        QObject::disconnect(qa, &QAction::triggered,
                            this, &MainWindow::on_FavoriteFolder_triggered);
        delete qa;
    }
    QStringList favs = settings_.GetFavorites();
    for(int i=0; i < favs.count(); ++i)
    {
        QAction* qa = new QAction(this);
        qa->setText(favs[i]);
        QObject::connect(qa, &QAction::triggered,
                         this, &MainWindow::on_FavoriteFolder_triggered);
        ui->menu_Favorites->addAction(qa);
    }
}
#include <QInputDialog>
void MainWindow::on_action_Add_current_check_states_triggered()
{
    bool ok;
    QString name = QInputDialog::getText(this,
                                         Consts::APPNAME,
                                         tr("Name:"),
                                         QLineEdit::Normal,
                                         QString(),
                                         &ok,
                                         (Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowFlags()) & ~Qt::WindowContextHelpButtonHint);
    if(!ok)
        return;
    if(name.isEmpty())
        return;

    if(settings_.IsNameExists(name))
    {
        if(!YesNo(this,
              QString(tr("\"%1\" already exists. Do you want to overwirte it?")).
              arg(name)))
        {
            return;
        }
    }

    QStringList dirs;
    foreach(DirectoryItem* item, ui->directoryWidget->GetCheckedItems())
    {
        dirs.append(item->text());
    }
    if(dirs.isEmpty())
    {
        if(!YesNo(this,
              tr("No checkboxes is checked. Do you want to continue?")))
        {
            return;
        }
    }
    settings_.AddToFavorites(name, dirs);
}

void MainWindow::on_action_Pause_triggered()
{
    gPaused = !gPaused;

    if(gPaused)
    {
        slPaused_->setText(tr("Paused"));
        slPaused_->show();
    }
    else
    {
        slPaused_->setText(QString());
        slPaused_->hide();
    }
}

void MainWindow::AddUserEntryDirectory(
        DirectoryItem::DirectoryItemType itemType,
        const QString& cdir,
        bool sel,
        bool check)
{
    QString text;
    if(itemType==DirectoryItem::DI_NORMAL)
    {
        QDir di(cdir);

        for(int i=0 ; i < ui->directoryWidget->count(); ++i)
        {
            QListWidgetItem* item = ui->directoryWidget->item(i);
            QDir d(item->text());
            if(di == d)
            {
                return;
            }
        }
        text=canonicalDir(cdir);
    }
    else if(itemType==DirectoryItem::DI_ALL)
    {
        text=tr("All");
    }
    else if(itemType==DirectoryItem::DI_MISSING)
    {
        text=tr("Missing");
    }
    else
    {
        Q_ASSERT(false);
        return;
    }
    DirectoryItem* newitem = new DirectoryItem(ui->directoryWidget, itemType);
    newitem->setText(text);
    newitem->setFlags(newitem->flags() | Qt::ItemIsUserCheckable);

    newitem->setSelected(sel);
    newitem->setCheckState(check ? Qt::Checked : Qt::Unchecked);

    if(itemType==DirectoryItem::DI_NORMAL)
        newitem->setIcon(fiProvider_.icon(QFileIconProvider::Folder));
    ui->directoryWidget->addItem(newitem);
}

bool IsSubDir(const QString& parent, const QString& child)
{
    QDir childDir(child);
    do
    {
        if(QDir(parent)==childDir)
            return true;
    } while(childDir.cdUp());

    return false;
}
bool MainWindow::IsDirSelected(const QString& dir) const
{
    if(ui->directoryWidget->IsAllItemSelectedOrChecked())
        return true;

    foreach(const QString& d, currentDirs_)
    {
        if(IsSubDir(d, dir))
            return true;
    }
    return false;
}
void MainWindow::on_action_Do_It_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),lastSelectedDir_);
    if(dir.isEmpty())
        return;
    lastSelectedDir_ = dir;

    StartScan(dir);
}
void MainWindow::StartScan(QListWidgetItem* item)
{
    StartScan2(item->text());
}
void MainWindow::StartScan(const QString& dir)
{
    AddUserEntryDirectory(DirectoryItem::DI_NORMAL, canonicalDir(dir), false, false);
    StartScan2(dir);
}
void MainWindow::StartScan2(const QString& dir)
{
    TaskGetDir* pTaskGetDir = new TaskGetDir(gLoopId, idManager_->Increment(IDKIND_GetDir), dir);
    pTaskGetDir->setAutoDelete(true);
    QObject::connect(pTaskGetDir, &TaskGetDir::afterGetDir,
                     this, &MainWindow::afterGetDir);
    QObject::connect(pTaskGetDir, &TaskGetDir::finished_GetDir,
                     this, &MainWindow::finished_GetDir);
    getPoolGetDir()->start(pTaskGetDir);

    onTaskStarted();
    insertLog(TaskKind::GetDir, idManager_->Get(IDKIND_GetDir), QString(tr("Task registered. %1")).arg(dir));
}

void MainWindow::onTaskStarted()
{
    if(!taskMonitorTimer)
    {
        taskMonitorTimer = new QTimer(this);
        connect(taskMonitorTimer, SIGNAL(timeout()),
                this, SLOT(onTaskTimerTick()));

        taskMonitorTimer->start(1000);
    }
}
void MainWindow::onTaskTimerTick()
{
    if(IsClosed())
        return;

    QApplication::processEvents();
}
void MainWindow::onTaskEnded()
{
    if(taskMonitorTimer)
    {
        disconnect(taskMonitorTimer, SIGNAL(timeout()),
                this, SLOT(onTaskTimerTick()));
        delete taskMonitorTimer;
        taskMonitorTimer=nullptr;
    }
}
void MainWindow::on_action_Stop_triggered()
{
    gPaused = false;
    WaitCursor wc;

    clearAllPool();
}

void MainWindow::on_action_Close_triggered()
{
    this->close();
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
    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Information);
    msgbox.setText(text);
    msgbox.setWindowTitle(title);
    msgbox.exec();
}

void MainWindow::on_actionSort_by_file_name_triggered()
{
    WaitCursor wc;
    tableModel_->Sort(TableModel::SORT_FILENAME);
}
void MainWindow::on_actionSort_by_file_size_triggered()
{
    WaitCursor wc;
    tableModel_->Sort(TableModel::SORT_SIZE);
}
void MainWindow::on_actionSort_by_wtime_triggered()
{
    WaitCursor wc;
    tableModel_->Sort(TableModel::SORT_WTIME);
}
void MainWindow::on_actionSort_by_resolution_triggered()
{
    WaitCursor wc;
    tableModel_->Sort(TableModel::SORT_RESOLUTION);
}
void MainWindow::on_actionSort_by_duration_triggered()
{
    WaitCursor wc;
    tableModel_->Sort(TableModel::SORT_DURATION);
}
void MainWindow::on_actionSort_by_bitrate_triggered()
{
    WaitCursor wc;
    tableModel_->Sort(TableModel::SORT_BITRATE);
}

void MainWindow::on_actionSort_by_open_count_triggered()
{
    WaitCursor wc;
    tableModel_->Sort(TableModel::SORT_OPENCOUNT);
}

void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction actionOpen("&Open", this);
    connect(&actionOpen, SIGNAL(triggered()),
            this, SLOT(openSelectedVideo()));
    contextMenu.addAction(&actionOpen);


    QAction actionOpenFolder("Open &Folder", this);
    connect(&actionOpenFolder, SIGNAL(triggered()),
            this, SLOT(openSelectedVideoInFolder()));
    contextMenu.addAction(&actionOpenFolder);

    contextMenu.addSeparator();

    QAction actionCopyPath("&Copy Path");
    connect(&actionCopyPath, SIGNAL(triggered()),
            this, SLOT(copySelectedVideoPath()));
    contextMenu.addAction(&actionCopyPath);


    // sub menu
    QMenu menuCopyOther(tr("C&opy others..."), this);

    QAction actionCopyFilename("&Filename");
    connect(&actionCopyFilename, SIGNAL(triggered()),
            this, SLOT(copySelectedVideoFilename()));
    menuCopyOther.addAction(&actionCopyFilename);

    contextMenu.addMenu(&menuCopyOther);

    contextMenu.exec(ui->tableView->mapToGlobal(pos));
}

void MainWindow::on_Rescan()
{
    if(ui->directoryWidget->selectedItems().isEmpty())
        return;

    DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->selectedItems()[0];
	if (item->IsMissingItem())
		return;
	else if(item->IsNormalItem())
		StartScan(item);
	else if(item->IsAllItem())
	{ 
		foreach(DirectoryItem* di, ui->directoryWidget->GetAllNormalItems())
		{
			StartScan(di);
		}
	}
	else
	{
		Q_ASSERT(false);
	}
}
void MainWindow::on_directoryWidget_Remove()
{
    if(ui->directoryWidget->selectedItems().isEmpty())
        return;

	DirectoryItem* item = (DirectoryItem*) ui->directoryWidget->selectedItems()[0];
	if (!item->IsNormalItem())
		return;
	if (!YesNo(this,QString(tr("Are you sure you want to remove \"%1\" from the lists?")).arg(item->text())))
		return;
    
	delete ui->directoryWidget->takeItem(ui->directoryWidget->row(item));
}
void MainWindow::on_directoryWidget_UncheckAll()
{
	directoryChanging_ = true;
	for (int i = 0; i < ui->directoryWidget->count(); ++i)
	{
		QListWidgetItem* item = ui->directoryWidget->item(i);
		item->setCheckState(Qt::Unchecked);
	}
	directoryChanging_ = false;
	directoryChangedCommon();
}
//static bool widgetsListItemLessThan(const QListWidgetItem* v1, const QListWidgetItem* v2)
//{
//	return v1->text() < v2->text();
//}
void MainWindow::on_directoryWidget_SortByName()
{
	directoryChanging_ = true;
	
	DirectoryItem* item = ui->directoryWidget->takeShowAllItem();
	ui->directoryWidget->sortItems();
	ui->directoryWidget->insertItem(0, item);

	directoryChanging_ = false;

	//directoryChanging_ = true;
	//QList<QListWidgetItem*> allitems;
	//for (int i = 0; i < ui->directoryWidget->count(); ++i)
	//{
	//	QListWidgetItem* item = ui->directoryWidget->takeItem(i);
	//	allitems.append(allitems);
	//}
	//std::sort(allitems.begin(), allitems.end(), &widgetsListItemLessThan);
	//for (int i = 0; i < allitems.count(); ++i)
	//{
	//	ui->directoryWidget->addItem(allitems[i]);
	//}

	//directoryChanging_ = false;
	// directoryChangedCommon();
}
void MainWindow::on_directoryWidget_customContextMenuRequested(const QPoint &pos)
{
	DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->itemAt(pos);
	if(!item)
        return;

    QMenu menu(this);

	QAction actRescan(tr("&Rescan to create thumbnails"));
	actRescan.setEnabled(!item->IsMissingItem());
	connect(&actRescan, SIGNAL(triggered(bool)),
		this, SLOT(on_Rescan()));
	menu.addAction(&actRescan);

	QAction actRemove(tr("Re&move"));
	actRemove.setEnabled(item->IsNormalItem());
	connect(&actRemove, SIGNAL(triggered(bool)),
		this, SLOT(on_directoryWidget_Remove()));
	menu.addAction(&actRemove);

	QAction actUncheckAll(tr("&Uncheck all"));
	connect(&actUncheckAll, SIGNAL(triggered(bool)),
		this, SLOT(on_directoryWidget_UncheckAll()));
	menu.addAction(&actUncheckAll);

	QAction actSortByName(tr("&Sort by name"));
	connect(&actSortByName, SIGNAL(triggered(bool)),
		this, SLOT(on_directoryWidget_SortByName()));
	menu.addAction(&actSortByName);

    menu.exec(ui->directoryWidget->mapToGlobal(pos));
}

void MainWindow::on_action_ShowMissing(bool bToggle)
{
	Q_UNUSED(bToggle);

	if (!IsInitialized() || IsClosed())
		return;
    //if(bShowMissing_==bToggle)
    //    return;
    //bShowMissing_ = bToggle;



    GetSqlAllSetTable(currentDirs_);
}
