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

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QVariant v = tableModel_->data(index, TableModel::TableRole::MovieFile);
    Q_ASSERT(v.isValid());
    Q_ASSERT(!v.toString().isEmpty());

//    QProcess process;
//    QString file = v.toString();
//    process.start("",file);

    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(v.toString())))
    {
        QMessageBox msgBox;
        msgBox.setText(Consts::APPNAME);
        msgBox.setInformativeText(tr("failed to launch."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}


void MainWindow::on_action_Options_triggered()
{
    OptionDialog dlg(this);
    dlg.exec();
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

    insertLog(TaskKind::GetDir, idGetDir_, tr("Task registered"));
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
    QMessageBox::about(this,title,text);
}
//void MainWindow::OnTimer()
//{
//    if(taskModel_)
//        taskModel_->UpdateList();
//}
