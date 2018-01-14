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
    }
}
void MainWindow::openVideoInFolder(const QString& movieFile)
{
    showInGraphicalShell(this, movieFile);
}
void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QVariant v = tableModel_->data(index, TableModel::TableRole::MovieFile);
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


    insertLog(TaskKind::GetDir, idManager_->Get(IDKIND_GetDir), QString(tr("Task registered. %1")).arg(dir));
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

    QListWidgetItem* item = ui->directoryWidget->selectedItems()[0];

    StartScan(item);
}
void MainWindow::on_directoryWidget_Remove()
{
    if(ui->directoryWidget->selectedItems().isEmpty())
        return;

    QListWidgetItem* item = ui->directoryWidget->selectedItems()[0];
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
void MainWindow::on_directoryWidget_customContextMenuRequested(const QPoint &pos)
{
    if(!ui->directoryWidget->itemAt(pos))
        return;

    QMenu menu(this);

	QAction actRescan(tr("&Rescan to create thumbnails"));
	connect(&actRescan, SIGNAL(triggered(bool)),
		this, SLOT(on_Rescan()));
	menu.addAction(&actRescan);

	QAction actRemove(tr("Re&move"));
	connect(&actRemove, SIGNAL(triggered(bool)),
		this, SLOT(on_directoryWidget_Remove()));
	menu.addAction(&actRemove);

	QAction actUncheckAll(tr("&Uncheck all"));
	connect(&actUncheckAll, SIGNAL(triggered(bool)),
		this, SLOT(on_directoryWidget_UncheckAll()));
	menu.addAction(&actUncheckAll);

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
