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
//#include <QStringList>
//#include <QDesktopServices>
//#include <QDirIterator>
//#include <QFileDialog>
//#include <QMessageBox>
//#include <QObject>
//#include <QProcess>
//#include <QScrollBar>
//#include <QScopedPointer>
//#include <QSizePolicy>
//#include <QStandardItemModel>
//#include <QThread>
//#include <QThreadPool>
//#include <QTime>
//#include <QTimer>
//#include <QUrl>
//#include <QVector>
//#include <QWidget>
//#include <QClipboard>
//#include <QToolButton>
//#include <QPushButton>
//#include <QCheckBox>
//#include <QComboBox>
//#include <QElapsedTimer>
//#include <QSortFilterProxyModel>
//#include <QFontDialog>
//#include <QInputDialog>

//#include "../../lsMisc/blockedbool.h"
//#include "../../lsMisc/stdQt/stdQt.h"
//#include "../../lsMisc/stdQt/settings.h"
//#include "../../lsMisc/stdQt/waitingcursor.h"

//#include "taskgetdir.h"
//#include "taskffmpeg.h"
// #include "optionfontdialog.h"
//#include "tablemodel.h"
//#include "taskmodel.h"
//#include "tableitemdata.h"
// #include "taskfilter.h"
//#include "errorinfoexception.h"
//#include "consts.h"
//#include "optiondialog.h"
//#include "optionextension.h"
//#include "optionexternaltoolsdialog.h"
//#include "ffmpeg.h"
//#include "globals.h"
//#include "helper.h"
//#include "osd.h"
//#include "extension.h"
//#include "sql.h"
//#include "tableitemdelegate.h"
//#include "docinfodialog.h"
//#include "tagitem.h"
//#include "taginputdialog.h"
//#include "mycontextmenu.h"
//#include "tagidsinfo.h"
//#include "itempropertydialog.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

//using namespace Consts;
//using namespace AmbiesoftQt;


void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    qDebug() << "activated" << index;
}

void MainWindow::OnTableItemChanged(QModelIndex newIndex, QModelIndex oldIndex)
{
    if(bHistoryActivating_)
        return;

    qint64 id = getIDFromIndex(newIndex);
    QString movieFile = getVideoFromIndex(newIndex);
    qDebug() << "old=" << oldIndex << ",new=" << newIndex << ":ID=" << id << ":" << movieFile << __FUNCTION__;
    if(id < 0)
        return;

    hisList_.OnItemChanged(id,movieFile);
}
void MainWindow::selectItem(const QString& movie)
{
    Ambiesoft::BlockedBool bb(&bHistoryActivating_);

    QModelIndex toSelectTitle = proxyModel_->findIndex(movie, FileMissingFilterProxyModel::FIND_INDEX::FIND_INDEX_TITLE);
    if(!toSelectTitle.isValid())
    {
        statusBar()->showMessage(tr("Item not found"), 2000);
        return;
    }
    QModelIndex toSelectMovie = proxyModel_->findIndex(movie, FileMissingFilterProxyModel::FIND_INDEX::FIND_INDEX_MOVIE);
    Q_ASSERT(toSelectMovie.isValid());

    // Select multiple items one by one for ensuring visible
//    ui->tableView->selectionModel()->select(toSelectMovie,
//                                            QItemSelectionModel::ClearAndSelect);
    proxyModel_->ensureIndex(toSelectMovie);
    //QApplication::processEvents();
    ui->tableView->scrollTo(toSelectMovie);

    // TODO: not selected
    proxyModel_->ensureIndex(toSelectTitle);
    QApplication::processEvents();
    ui->tableView->scrollTo(toSelectTitle);
    ui->tableView->selectionModel()->select(toSelectTitle,
                                            QItemSelectionModel::ClearAndSelect);

}

void MainWindow::on_action_GoBack_triggered()
{
    if(bHistoryActivating_)
        return;
    hisList_.OnGoBack();
}

void MainWindow::on_action_GoForward_triggered()
{
    if(bHistoryActivating_)
        return;
    hisList_.OnGoForward();
}

void MainWindow::onArbitraryHistory()
{
    if(bHistoryActivating_)
        return;

    QAction* action = static_cast<QAction*>(sender());
    if(!action)
        return;

    int index = action->data().toInt();

    hisList_.doSelect(index);
}
void MainWindow::onMenuHistory_AboutToShow()
{
    // Index which indicate last pos of fixed item on top
    const int nSepIndex = 2;

    // remove actions below pos
    while(ui->menu_History->actions().count() > (nSepIndex+1))
        delete ui->menu_History->actions().takeAt(ui->menu_History->actions().count()-1);

    for(int i=hisList_.count()-1 ; i >=0; --i)
    {
        QAction* qa = new QAction();
        qa->setText(QFileInfo(hisList_.getMovie(i)).fileName());
        qa->setData(i);
        if(i==hisList_.current())
        {
            qa->setCheckable(true);
            qa->setChecked(true);
        }
        connect(qa, &QAction::triggered,
                this, &MainWindow::onArbitraryHistory);
        ui->menu_History->addAction(qa);
    }
}
