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
#include <QDir>
//#include <QFileDialog>
//#include <QMessageBox>
//#include <QObject>
//#include <QProcess>
//#include <QScrollBar>
//#include <QScopedPointer>
//#include <QSizePolicy>
#include <QStandardPaths>
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
#include "../../lsMisc/stdQt/stdQt.h"
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
#include "consts.h"
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

using namespace Consts;
using namespace AmbiesoftQt;


QString MainWindow::GetDefaultDocumentPath()
{
    QString docdir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if(docdir.isEmpty())
    {
        Alert(this, tr("Failed to get document directory."));
        return QString();
    }
    docdir=pathCombine(docdir, APPNAME);
    QDir().mkpath(docdir);
    if(!QDir(docdir).exists())
    {
        Alert(this, tr("Failed to create directory. \"%1\"").arg(docdir));
        return QString();
    }

    return pathCombine(docdir,"default.scexd");
}

bool MainWindow::OpenDocument(const QString& file, const bool bExists)
{
    CloseDocument();

    Document* pNewDoc = new Document;
    if(!pNewDoc->Load(file, bExists))
    {
        Alert(this,
              QString("%0\n%1").arg(pNewDoc->GetLastErr()).arg(file));
        delete pNewDoc;
        return false;
    }

    qDebug() << "Document Opened: " << file;
    recents_.removeDuplicates();
    recents_.removeOne(file);
    recents_.insert(0, file);

    pDoc_ = pNewDoc;
    InitDocument();
    return true;
}
bool MainWindow::CloseDocument()
{
    if(!pDoc_)
        return true;

    Ambiesoft::BlockedBool dc(&directoryChanging_);
    Ambiesoft::BlockedBool tc(&tagChanging_);

    StoreDocument();
    delete pDoc_;
    pDoc_=nullptr;

    ui->directoryWidget->clear();
    ui->listTag->clear();

    return true;
}

void MainWindow::InitDocument()
{
    if(!pDoc_)
        return;

    Ambiesoft::BlockedBool btD(&directoryChanging_);

    ui->directoryWidget->clear();

    DirectoryItem* allitem = new DirectoryItem(-1,
                                               DirectoryItem::DI_ALL_MY,
                                               tr("All"));

    allitem->setSelected(pDoc_->IsDirAllSelected());
    ui->directoryWidget->addItem(allitem);

    QList<DirectoryItem*> dirs;
    pDoc_->GetAllDirs(dirs);
    for(DirectoryItem* di : dirs)
    {
        ui->directoryWidget->addItem(di);
    }

    DirectoryItem* missitem = new DirectoryItem(-2,
                                                DirectoryItem::DI_MISSING_MY,
                                                tr("Missing"));
    ui->directoryWidget->addItem(missitem);

    LoadTags();

    itemChangedCommon(true);
}
