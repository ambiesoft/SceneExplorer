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

#include <QDesktopServices>
#include <QMessageBox>
#include <QDialog>
#include <QDebug>
#include <QThreadPool>
#include <QFileDialog>
#include <QListWidget>
#include <QInputDialog>

#include "consts.h"
#include "globals.h"
#include "waitcursor.h"
#include "ffmpeg.h"
#include "taskmodel.h"
#include "tableitemdata.h"

#include "optiondialog.h"

#include "taskgetdir.h"
#include "directoryentry.h"

#include "settings.h"
#include "sql.h"
#include "helper.h"
#include "blockedbool.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "aboutdialog.h"

void MainWindow::openVideo(const QString& movieFile)
{
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(movieFile)))
    {
        QMessageBox msgBox;
        msgBox.setText(Consts::APPNAME_DISPLAY);
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
    QVariant v = proxyModel_->data(index, TableModel::TableRole::MovieFileFull);
    Q_ASSERT(v.isValid());
    Q_ASSERT(!v.toString().isEmpty());

    openVideo(v.toString());
}


void MainWindow::on_action_Options_triggered()
{
    OptionDialog dlg(this);
    dlg.maxgd_ = optionThreadcountGetDir_;
    dlg.maxff_ = optionThreadcountThumbnail_;
    dlg.thumbCount_ = optionThumbCount_;
    dlg.imagecache_ = tableModel_->GetImageCache();
    dlg.useCustomDBDir_ = settings_.valueBool(Consts::KEY_USE_CUSTOMDATABASEDIR);
    bool prevUseCustomDBDir = dlg.useCustomDBDir_;
    dlg.dbdir_ = QDir::currentPath();
    dlg.limitItems_ = settings_.valueBool(Consts::KEY_LIMIT_ITEMS, false);
    dlg.maxRows_ = settings_.valueInt(Consts::KEY_LIMIT_NUMBEROFROWS, 1000);
    dlg.openlastdoc_ = settings_.valueBool(Consts::KEY_OPEN_LASTOPENEDDOCUMENT, true);
    dlg.ffprobe_ = settings_.valueString(Consts::KEY_FFPROBE_EXECUTABLE); // FFMpeg::GetFFprobe(settings_);
    dlg.ffmpeg_ = settings_.valueString(Consts::KEY_FFMPEG_EXECUTABLE); //FFMpeg::GetFFmpeg(settings_);

    if(QDialog::Accepted != dlg.exec())
        return;

    optionThreadcountGetDir_ = dlg.maxgd_;
    optionThreadcountThumbnail_ = dlg.maxff_;
    optionThumbCount_ = dlg.thumbCount_;
    tableModel_->SetColumnCountImage(dlg.thumbCount_);
    tableModel_->SetImageCache(dlg.imagecache_);
    settings_.setValue(Consts::KEY_LIMIT_ITEMS, dlg.limitItems_);
    settings_.setValue(Consts::KEY_LIMIT_NUMBEROFROWS, dlg.maxRows_);
    settings_.setValue(Consts::KEY_OPEN_LASTOPENEDDOCUMENT,dlg.openlastdoc_);
    settings_.setValue(Consts::KEY_USE_CUSTOMDATABASEDIR, dlg.useCustomDBDir_);
    settings_.setValue(Consts::KEY_DATABASE_PATH, dlg.dbdir_);
    FFMpeg::SetFFprobe(settings_, dlg.ffprobe_);
    FFMpeg::SetFFmpeg(settings_, dlg.ffmpeg_);

    CreateLimitManager();

    if( (prevUseCustomDBDir != dlg.useCustomDBDir_) || (QDir::current() != QDir(dlg.dbdir_)) )
    {
        if(YesNo(this,
              tr("Application needs to restart to effect the change. Do you want to restart application now?")))
        {
            gReboot = true;
            this->close();
            return;
        }
    }
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
//void MainWindow::on_FavoriteFolder_triggered(bool checked)
//{
//    Q_UNUSED(checked);
//    QAction* qa = (QAction*)QObject::sender();
//    Q_ASSERT(qa);
//    QStringList dirs = settings_.GetFavorite(qa->text());
//    ui->directoryWidget->SetCheck(dirs, true);
//    directoryChangedCommon(true);
//}

//void MainWindow::onMenu_Favorites_AboutToShow()
//{
//    int sepIndex = 2;
//    // TODO
//    QMenu* menu = ui->menu_Favorites;
//    foreach (QAction *action, menu->actions()) {
//         if (action->isSeparator()) {
//             qDebug("this action is a separator");
//         } else if (action->menu()) {
//             qDebug("action: %s", qUtf8Printable(action->text()));
//             qDebug(">>> this action is associated with a submenu, iterating it recursively...");
//             // enumerateMenu(action->menu());
//             qDebug("<<< finished iterating the submenu");
//         } else {
//             qDebug("action: %s", qUtf8Printable(action->text()));
//         }
//     }

//    while(ui->menu_Favorites->actions().count() > sepIndex)
//    {
//        QAction *qa = ui->menu_Favorites->actions()[sepIndex];
//        ui->menu_Favorites->removeAction(qa);
//        QObject::disconnect(qa, &QAction::triggered,
//                            this, &MainWindow::on_FavoriteFolder_triggered);
//        delete qa;
//    }
//    QStringList favs = settings_.GetFavorites();
//    for(int i=0; i < favs.count(); ++i)
//    {
//        QAction* qa = new QAction(this);
//        qa->setText(favs[i]);
//        QObject::connect(qa, &QAction::triggered,
//                         this, &MainWindow::on_FavoriteFolder_triggered);
//        ui->menu_Favorites->addAction(qa);
//    }
//}
void MainWindow::on_RecentDocuments_triggered(bool checked)
{
    Q_UNUSED(checked);
    QAction* qa = (QAction*)QObject::sender();
    Q_ASSERT(qa);

    QString file = qa->text();
    OpenDocument(file, true);
}
void MainWindow::onMenu_RecentDocuments_AboutToShow()
{
    QString current = pDoc_ ? pDoc_->GetFullName() : QString();


    while(ui->menu_Recent_documets->actions().count() > 0)
    {
        QAction *qa = ui->menu_Recent_documets->actions()[0];
        ui->menu_Recent_documets->removeAction(qa);
        QObject::disconnect(qa, &QAction::triggered,
                            this, &MainWindow::on_RecentDocuments_triggered);
        delete qa;
    }

    for(int i=0; i < recents_.count(); ++i)
    {
        QAction* qa = new QAction(this);
        qa->setText(recents_[i]);
        if(!current.isEmpty() && recents_[i]==current)
        {
            qa->setCheckable(true);
            qa->setChecked(true);
        }
        QObject::connect(qa, &QAction::triggered,
                         this, &MainWindow::on_RecentDocuments_triggered);
        ui->menu_Recent_documets->addAction(qa);
    }

}

//void MainWindow::on_action_Add_current_check_states_triggered()
//{
//    bool ok;
//    QString name = QInputDialog::getText(this,
//                                         Consts::APPNAME_DISPLAY,
//                                         tr("Name:"),
//                                         QLineEdit::Normal,
//                                         QString(),
//                                         &ok,
//                                         (Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowFlags()) & ~Qt::WindowContextHelpButtonHint);
//    if(!ok)
//        return;
//    if(name.isEmpty())
//        return;

//    if(settings_.IsNameExists(name))
//    {
//        if(!YesNo(this,
//              QString(tr("\"%1\" already exists. Do you want to overwirte it?")).
//              arg(name)))
//        {
//            return;
//        }
//    }

//    QStringList dirs;
//    foreach(DirectoryItem* item, ui->directoryWidget->GetCheckedItems())
//    {
//        dirs.append(item->text());
//    }
//    if(dirs.isEmpty())
//    {
//        if(!YesNo(this,
//              tr("No checkboxes is checked. Do you want to continue?")))
//        {
//            return;
//        }
//    }
//    settings_.AddToFavorites(name, dirs);
//}

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
	bool hasCheckbox = true;
    if(itemType==DirectoryItem::DI_NORMAL)
    {
        QDir di(cdir);

        for(int i=0 ; i < ui->directoryWidget->count(); ++i)
        {
            DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->item(i);
            if(!item->IsNormalItem())
                continue;
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
		hasCheckbox = false;
    }
    else if(itemType==DirectoryItem::DI_MISSING)
    {
        text=tr("Missing");
		hasCheckbox = false;
    }
    else
    {
        Q_ASSERT(false);
        return;
    }
    DirectoryItem* newitem = new DirectoryItem(ui->directoryWidget, itemType);
    newitem->setText(text);
	if(hasCheckbox)
		newitem->setFlags(newitem->flags() | Qt::ItemIsUserCheckable);

    newitem->setSelected(sel);
	if(hasCheckbox)
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

void MainWindow::StartScan(QListWidgetItem* item)
{
    StartScan(item->text());
}


void MainWindow::StartScan(const QString& dir)
{
    QString errString;
    if(!checkFFprobe(errString) || !checkFFmpeg(errString))
    {
        insertLog(TaskKind::App, 0,
                  QString(tr("Failed to launch ffprobe or ffmpeg. (%1) ") + tr("Check the option setting.")).
                  arg(errString));
        return;
    }

    if(!QDir(dir).exists())
    {
        insertLog(TaskKind::App, 0, QString(tr("Directoy not found. (%1) ")).
                  arg(dir));
        return;
    }
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
    //if(!taskMonitorTimer)
    //{
    //    taskMonitorTimer = new QTimer(this);
    //    connect(taskMonitorTimer, SIGNAL(timeout()),
    //            this, SLOT(onTaskTimerTick()));

    //    taskMonitorTimer->start(1000);
    //}
}
//void MainWindow::onTaskTimerTick()
//{
//    if(IsClosed())
//        return;
//
//    QApplication::processEvents();
//}
void MainWindow::onTaskEnded()
{
    //if(taskMonitorTimer)
    //{
    //    disconnect(taskMonitorTimer, SIGNAL(timeout()),
    //            this, SLOT(onTaskTimerTick()));
    //    delete taskMonitorTimer;
    //    taskMonitorTimer=nullptr;
    //}

    // make pool null to make thread count effective
    clearAllPool(false);
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
    AboutDialog dlg(this);
    dlg.exec();

//    QString title = Consts::APPNAME_DISPLAY;
//    QString text = Consts::APPNAME;
//    text.append(" ");
//    text.append("ver ");
//    text.append(Consts::VERSION);
//    text.append("\n");
//    text.append("\n");
//    text.append("copyright 2018 ");
//    text.append(Consts::ORGANIZATION);
//    QMessageBox msgbox(this);
//    msgbox.setIcon(QMessageBox::Information);
//    msgbox.setText(text);
//    msgbox.setWindowTitle(title);
//    msgbox.exec();
}

void MainWindow::SortManager::setToolButton(SORTCOLUMN sc,
                                            QToolButton* tb,
                                            const QIcon& iconNormal,
                                            const QIcon& iconRev)
{
	Q_ASSERT(tbs_[sc] == nullptr);
	tbs_[sc] = tb;
    iconsAscend_[sc] = iconNormal;
    iconsDescend_[sc]=iconRev;
	tb->setText(GetSortColumnName(sc));
}

void MainWindow::SortManager::onSort(SORTCOLUMN sc)
{
	sort_ = sc;
    ascending_[sc] = !ascending_[sc];
	UpdateButtonText();
}
void MainWindow::SortManager::UpdateButtonText()
{
	for (size_t i = 0; i < (sizeof(tbs_) / sizeof(tbs_[0])); ++i)
	{
		if (!tbs_[i])
			continue;

		if (sort_ == i)
		{
            tbs_[i]->setText(GetSortColumnName((SORTCOLUMN)i) + (ascending_[i] ? " ASC" : " DESC"));
			tbs_[i]->setChecked(true);
            tbs_[i]->setIcon(ascending_[i] ? iconsAscend_[i] : iconsDescend_[i]);
		}
		else
		{
            tbs_[i]->setText(GetSortColumnName((SORTCOLUMN)i));
			tbs_[i]->setChecked(false);
            tbs_[i]->setIcon(ascending_[i] ? iconsAscend_[i] : iconsDescend_[i]);
		}
	}
}


void MainWindow::onSortCommon(SORTCOLUMN sortColumn)
{
	WaitCursor wc;
	sortManager_.onSort(sortColumn);
	if (limitManager_)
		limitManager_->Reset();
	GetSqlAllSetTable(currentDirs_);
}
void MainWindow::on_actionSort_by_file_name_triggered()
{
	onSortCommon(SORT_FILENAME);
}
void MainWindow::on_actionSort_by_file_size_triggered()
{
	onSortCommon(SORT_SIZE);
}
void MainWindow::on_actionSort_by_wtime_triggered()
{
	onSortCommon(SORT_WTIME);
}
void MainWindow::on_actionSort_by_resolution_triggered()
{
	onSortCommon(SORT_RESOLUTION);
}
void MainWindow::on_actionSort_by_duration_triggered()
{
	onSortCommon(SORT_DURATION);
}
void MainWindow::on_actionSort_by_bitrate_triggered()
{
	onSortCommon(SORT_BITRATE);
}

void MainWindow::on_actionSort_by_open_count_triggered()
{
	onSortCommon(SORT_OPENCOUNT);
}
void MainWindow::on_actionSort_by_last_access_triggered()
{
    onSortCommon(SORT_LASTACCESS);
}

void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction actionOpen("&Open", this);
    connect(&actionOpen, SIGNAL(triggered()),
            this, SLOT(on_context_openSelectedVideo()));
    contextMenu.addAction(&actionOpen);


    QAction actionOpenFolder("Open &Folder", this);
    connect(&actionOpenFolder, SIGNAL(triggered()),
            this, SLOT(on_context_openSelectedVideoInFolder()));
    contextMenu.addAction(&actionOpenFolder);

    contextMenu.addSeparator();

    QAction actionCopyPath("&Copy Path");
    connect(&actionCopyPath, SIGNAL(triggered()),
            this, SLOT(on_context_copySelectedVideoPath()));
    contextMenu.addAction(&actionCopyPath);


    // sub menu start --->
    QMenu menuCopyOther(tr("C&opy others..."), this);

    QAction actionCopyFilename("&Filename");
    connect(&actionCopyFilename, SIGNAL(triggered()),
            this, SLOT(on_context_copySelectedVideoFilename()));
    menuCopyOther.addAction(&actionCopyFilename);

    contextMenu.addMenu(&menuCopyOther);
    // <--- sub menu end

    contextMenu.addSeparator();

    // externl tools ----->
    QMenu menuExternalTools(tr("External &tools..."), this);
    QList< QSharedPointer<QAction> > actExts;
    if(externalTools_.isEmpty())
    {
        QSharedPointer<QAction> act(new QAction(tr("No exteral tools")));
        act->setEnabled(false);
        menuExternalTools.addAction(act.data());
        actExts.append(act);
    }
    else
    {
        for(int i=0 ; i < externalTools_.count(); ++i)
        {
            QSharedPointer<QAction> act(new QAction(externalTools_[i].GetName()));
            connect(act.data(), SIGNAL(triggered()),
                    this, SLOT(on_context_ExternalTools()));
            menuExternalTools.addAction(act.data());
            act->setData(i);
            actExts.append(act);
        }
    }
    contextMenu.addMenu(&menuExternalTools);
    // <---- external tools

    contextMenu.addSeparator();

    QAction actionRename(tr("Re&name"));
    connect(&actionRename, SIGNAL(triggered()),
            this, SLOT(on_context_Rename()));
    contextMenu.addAction(&actionRename);
    contextMenu.addSeparator();

    QAction actionRemoveFromDB(tr("&Remove from database"));
    connect(&actionRemoveFromDB, SIGNAL(triggered()),
            this, SLOT(on_context_removeFromDatabase()));
    contextMenu.addAction(&actionRemoveFromDB);

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
    
    BlockedBool bt(&directoryChanging_);
	delete ui->directoryWidget->takeItem(ui->directoryWidget->row(item));
}
void MainWindow::on_directoryWidget_RemoveMissingItems()
{
    if(ui->directoryWidget->selectedItems().isEmpty())
        return;

	DirectoryItem* item = (DirectoryItem*) ui->directoryWidget->selectedItems()[0];

	if (!YesNo(this,
		tr("Are you sure you want to remove missing items from database?")))
	{
		return;
	}
    
	QString dir;
	if(item->IsNormalItem())
		dir = item->text();
	
    gpSQL->RemoveAllMissingEntries(dir);

    directoryChangedCommon(true);
}
void MainWindow::on_directoryWidget_UncheckAll()
{
    {
        BlockedBool bt(&directoryChanging_, true, false);

        for (int i = 0; i < ui->directoryWidget->count(); ++i)
        {
            DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->item(i);
            if(item->IsNormalItem())
                item->setCheckState(Qt::Unchecked);
        }
    }

	directoryChangedCommon();
}
//static bool widgetsListItemLessThan(const QListWidgetItem* v1, const QListWidgetItem* v2)
//{
//	return v1->text() < v2->text();
//}
void MainWindow::on_directoryWidget_SortByName()
{
    Q_ASSERT(!directoryChanging_);
    BlockedBool bt(&directoryChanging_, true, false);
	
    ui->directoryWidget->SortNormalItems();



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
void MainWindow::on_directoryWidget_MoveUp()
{
    if (ui->directoryWidget->selectedItems().isEmpty())
        return;

    Q_ASSERT(!directoryChanging_);
    BlockedBool bt(&directoryChanging_, true, false);

    DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->selectedItems()[0];
    if (!item->IsNormalItem())
        return;

    int row = ui->directoryWidget->row(item);
    if (ui->directoryWidget->IsTopNormalItem(row))
        return;

    item = (DirectoryItem*)ui->directoryWidget->takeItem(row);
    ui->directoryWidget->insertItem(row - 1, item);
    item->setSelected(true);
    ui->directoryWidget->setFocus();
}
void MainWindow::on_directoryWidget_MoveDown()
{
    if (ui->directoryWidget->selectedItems().isEmpty())
        return;

    Q_ASSERT(!directoryChanging_);
    BlockedBool bt(&directoryChanging_, true, false);

    DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->selectedItems()[0];
    if (!item->IsNormalItem())
        return;

    int row = ui->directoryWidget->row(item);
    if (ui->directoryWidget->IsBottomNormalItem(row))
        return;

    item = (DirectoryItem*)ui->directoryWidget->takeItem(row);
    ui->directoryWidget->insertItem(row + 1, item);
    item->setSelected(true);
    ui->directoryWidget->setFocus();
}

void MainWindow::on_directoryWidget_customContextMenuRequested(const QPoint &pos)
{
	DirectoryItem* item = (DirectoryItem*)ui->directoryWidget->itemAt(pos);

	if(!item)
    {
        QMenu menu(this);
        menu.addAction(ui->action_Add_Folder);
        menu.exec(ui->directoryWidget->mapToGlobal(pos));
    }
    else
    {
        QMenu menu(this);

        QAction actRescan(tr("&Rescan to create thumbnails"));
        actRescan.setEnabled(!item->IsMissingItem());
        connect(&actRescan, SIGNAL(triggered(bool)),
            this, SLOT(on_Rescan()));
        menu.addAction(&actRescan);

        menu.addSeparator();

        QAction actUncheckAll(tr("&Uncheck all"));
        connect(&actUncheckAll, SIGNAL(triggered(bool)),
            this, SLOT(on_directoryWidget_UncheckAll()));
        menu.addAction(&actUncheckAll);

        menu.addSeparator();

        QAction actMoveUp(tr("Move &up"));
        connect(&actMoveUp, SIGNAL(triggered(bool)),
            this, SLOT(on_directoryWidget_MoveUp()));
        menu.addAction(&actMoveUp);

        QAction actMoveDown(tr("Move &down"));
        connect(&actMoveDown, SIGNAL(triggered(bool)),
            this, SLOT(on_directoryWidget_MoveDown()));
        menu.addAction(&actMoveDown);

        QAction actSortByName(tr("&Sort by name"));
        connect(&actSortByName, SIGNAL(triggered(bool)),
            this, SLOT(on_directoryWidget_SortByName()));
        menu.addAction(&actSortByName);


        menu.addSeparator();

        QAction actRemove(tr("Re&move this folder from database"));
        actRemove.setEnabled(item->IsNormalItem());
        connect(&actRemove, SIGNAL(triggered(bool)),
            this, SLOT(on_directoryWidget_Remove()));
        menu.addAction(&actRemove);

        QAction actRemoveMissingItems(tr("Remove &missing items from database"));
        connect(&actRemoveMissingItems, SIGNAL(triggered(bool)),
            this, SLOT(on_directoryWidget_RemoveMissingItems()));
        menu.addAction(&actRemoveMissingItems);

        menu.exec(ui->directoryWidget->mapToGlobal(pos));
    }
}

void MainWindow::on_ShowMissingClicked_common(bool bNextCheck)
{
    if (!IsInitialized() || IsClosed())
        return;

    static bool sIn = false;
    if(sIn)
        return;
    BlockedBool bb(&sIn);

    ui->actionShow_missing_files->setChecked(bNextCheck);
    tbShowNonExistant_->setChecked(bNextCheck);
    GetSqlAllSetTable(currentDirs_);
}
void MainWindow::on_action_ShowMissingClicked()
{
    on_ShowMissingClicked_common(tbShowNonExistant_->isChecked());
}

void MainWindow::on_LimitFirst_triggered(bool checked)
{
    Q_UNUSED(checked);

    Q_ASSERT(limitManager_);
    limitManager_->SetIndexFirst();
    directoryChangedCommon(true);
}
void MainWindow::on_LimitPrev_triggered(bool checked)
{
    Q_UNUSED(checked);

    Q_ASSERT(limitManager_);
    if(limitManager_->Decrement())
    {
        directoryChangedCommon(true);
    }
}

void MainWindow::on_LimitNext_triggered(bool checked)
{
    Q_UNUSED(checked);

    Q_ASSERT(limitManager_);
    if(limitManager_->Increment())
    {
        directoryChangedCommon(true);
    }
}
void MainWindow::on_LimitLast_triggered(bool checked)
{
    Q_UNUSED(checked);

    Q_ASSERT(limitManager_);
    limitManager_->SetIndexLast();
    directoryChangedCommon(true);
}
void MainWindow::onCmbLint_currentIndexChanged(int index)
{
    // qDebug() << index;
    Q_UNUSED(index);
    if(limitManager_->IsBlocking())
        return;
    directoryChangedCommon(true);
}

bool MainWindow::LimitManager::Decrement()
{
    int ci = cmb_->currentIndex();
    --ci;
    if(ci < 0)
        return false;
    BlockedBool bb(&blocking_);
    cmb_->setCurrentIndex(ci);
    return true;
}
bool MainWindow::LimitManager::Increment()
{
    int ci = cmb_->currentIndex();
    ++ci;
    if(cmb_->count() <= ci)
        return false;
    BlockedBool bb(&blocking_);
    cmb_->setCurrentIndex(ci);
    return true;
}
