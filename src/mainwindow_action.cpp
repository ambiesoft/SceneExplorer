#include <QDesktopServices>
#include <QMessageBox>
#include <QDialog>
#include <QDebug>
#include <QThreadPool>
#include <QFileDialog>

#include "consts.h"
#include "globals.h"
#include "waitcursor.h"

#include "taskmodel.h"
#include "tableitemdata.h"

#include "optiondialog.h"

#include "taskgetdir.h"

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
        statusLabel_->setText(tr("Paused"));
        statusLabel_->show();
    }
    else
    {
        statusLabel_->setText(QString());
        statusLabel_->hide();
    }
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
    getPoolGetDir()->start(pTaskGetDir);

    insertLog(TaskKind::GetDir, idGetDir_, QString(tr("Task registered. %1")).arg(dir));
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
    tableModel_->SortByFileName();
}
void MainWindow::on_actionSort_by_file_size_triggered()
{
    tableModel_->SortBySize();
}

void MainWindow::on_actionSort_by_wtime_triggered()
{
    tableModel_->SortByWtime();
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

    contextMenu.exec(ui->tableView->mapToGlobal(pos));

}
