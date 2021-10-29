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

#include <QWidget>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QSharedPointer>

#include "../../lsMisc/stdQt/stdQt.h"

#include "mycontextmenu.h"
#include "consts.h"
#include "helper.h"
#include "optiondialog.h"

using namespace AmbiesoftQt;

OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent,GetDefaultDialogFlags()),
      imagecache_(IC_NEVER)
{
    ui.setupUi(this);

    ui.cmbThumbCount->addItem("3");
    ui.cmbThumbCount->addItem("5");

    ui.cmbThumbImageFormat->addItem("jpg");
    ui.cmbThumbImageFormat->addItem("png");

    ui.cmbScrollMode->addItem(tr("item"));
    ui.cmbScrollMode->addItem(tr("pixel"));

    ui.cmbTaskPriority->addItem(tr("Default"), -1);
    ui.cmbTaskPriority->addItem(tr("HighestPriority"), 5);
    ui.cmbTaskPriority->addItem(tr("HighPriority"), 4);
    ui.cmbTaskPriority->addItem(tr("NormalPriority"), 3);
    ui.cmbTaskPriority->addItem(tr("LowPriority"), 2);
    ui.cmbTaskPriority->addItem(tr("LowestPriority"), 1);
    ui.cmbTaskPriority->addItem(tr("IdlePriority"), 0);

    setWindowTitle(tr("option"));
}

void OptionDialog::showEvent(QShowEvent *ev)
{
    ui.spinBoxGetDir->setValue(maxgd_);
    ui.spinBoxThumb->setValue(maxff_);

    Q_ASSERT(ui.cmbThumbCount->count()==2);
    if(thumbCount_==5)
        ui.cmbThumbCount->setCurrentIndex(1);
    else
        ui.cmbThumbCount->setCurrentIndex(0);

    originalThumbWidth_ = thumbWidth_;
    originalThumbHeight_ = thumbHeight_;

    ui.spinBoxThumbWidth->setValue(thumbWidth_);
    ui.spinBoxThumbHeight->setValue(thumbHeight_);

    Q_ASSERT(ui.cmbThumbImageFormat->count()==2);
    if(thumbFormat_=="jpg")
        ui.cmbThumbImageFormat->setCurrentIndex(0);
    else if(thumbFormat_=="png")
        ui.cmbThumbImageFormat->setCurrentIndex(1);
    else
        ui.cmbThumbImageFormat->setCurrentIndex(0);
    ui.cmbThumbImageFormat->setEditText(thumbFormat_);


    Q_ASSERT(ui.cmbScrollMode->count()==2);
    if(scrollMode_=="item")
        ui.cmbScrollMode->setCurrentIndex(0);
    else if(scrollMode_=="pixel")
        ui.cmbScrollMode->setCurrentIndex(1);
    else
        ui.cmbScrollMode->setCurrentIndex(0);

    ui.cmbTaskPriority->setCurrentIndex(0);
    for(int i=0 ; i < ui.cmbTaskPriority->count(); ++i)
    {
        if(taskPriority_==ui.cmbTaskPriority->itemData(i).toInt())
        {
            ui.cmbTaskPriority->setCurrentIndex(i);
            break;
        }
    }

    ui.lineInfoMain->setText(mainText_);
    ui.lineInfoSub->setText(subText_);

    switch(imagecache_)
    {
    case IC_NEVER:
        ui.rbICNever->setChecked(true);
        break;
    case IC_PERDIRECTORY:
        ui.rbICPerDirectory->setChecked(true);
        break;
    case IC_ALWAYS:
        ui.rbICAll->setChecked(true);
        break;
    default:
        Q_ASSERT(false);
    }

    ui.chkUseCustomDatabaseDirectory->setChecked(useCustomDBDir_);
    ui.lineDBDir->setText(dbdir_);

    ui.chkLimitItems->setChecked(limitItems_);
    ui.spinNumberOfRows->setValue(maxRows_);

    ui.chkOpenLastDocument->setChecked(openlastdoc_);

    ui.lineffprobe->setText(ffprobe_);
    ui.lineffmpeg->setText(ffmpeg_);

    ui.chkShowTagCount->setChecked(showtagcount_);

    on_chkUseCustomDatabaseDirectory_stateChanged(0);
    on_chkLimitItems_stateChanged(0);

    QDialog::showEvent(ev);
}
void OptionDialog::on_buttonBox_accepted()
{
}
void OptionDialog::accept()
{
    maxgd_ = ui.spinBoxGetDir->value();
    maxff_ = ui.spinBoxThumb->value();

    thumbCount_ = ui.cmbThumbCount->currentIndex()==0 ? 3 : 5;
    thumbWidth_ = ui.spinBoxThumbWidth->value();
    thumbHeight_ = ui.spinBoxThumbHeight->value();
    thumbFormat_ = ui.cmbThumbImageFormat->lineEdit()->text();
    scrollMode_ = ui.cmbScrollMode->currentIndex()==0 ? "item":"pixel";

    taskPriority_ = ui.cmbTaskPriority->currentData().toInt();

    mainText_ = ui.lineInfoMain->text();
    subText_ = ui.lineInfoSub->text();

    if(ui.rbICNever->isChecked())
        imagecache_=IC_NEVER;
    else if(ui.rbICPerDirectory->isChecked())
        imagecache_=IC_PERDIRECTORY;
    else if(ui.rbICAll->isChecked())
        imagecache_=IC_ALWAYS;
    else
        Q_ASSERT(false);

    useCustomDBDir_ = ui.chkUseCustomDatabaseDirectory->isChecked();
    dbdir_ = ui.lineDBDir->text();

    limitItems_ = ui.chkLimitItems->isChecked();
    maxRows_ = ui.spinNumberOfRows->value();

    openlastdoc_ = ui.chkOpenLastDocument->isChecked();

    ffprobe_ = ui.lineffprobe->text();
    ffmpeg_ = ui.lineffmpeg->text();

    showtagcount_ = ui.chkShowTagCount->isChecked();

    if(!(originalThumbWidth_ == thumbWidth_ && originalThumbHeight_ == thumbHeight_))
    {
        if(!YesNo(this, tr("By changing width or height of thumbnail, all thumbnails needs be recreated. Are you sure to continue?")))
            return;
    }
    ParentClass::accept();
}
void OptionDialog::on_tbDBDir_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::DirectoryOnly);
    dlg.setDirectory(dbdir_);

    if(!dlg.exec())
        return;

    ui.lineDBDir->setText(dlg.selectedFiles()[0]);
}

void OptionDialog::on_setFileCommon(QLineEdit* le)
{
    QFileDialog dlg(this);
    dlg.setDirectory(lastSelectedDir_);
    dlg.setFileMode(QFileDialog::ExistingFile);

    if(!dlg.exec())
        return;
    lastSelectedDir_ = QFileInfo(dlg.selectedFiles()[0]).absoluteDir().absolutePath();

    le->setText(dlg.selectedFiles()[0]);
}
void OptionDialog::on_tbffprobe_clicked()
{
    on_setFileCommon(ui.lineffprobe);
}

void OptionDialog::on_tbffmpeg_clicked()
{
    on_setFileCommon(ui.lineffmpeg);
}

void OptionDialog::on_chkUseCustomDatabaseDirectory_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    bool checked = ui.chkUseCustomDatabaseDirectory->isChecked();
    ui.labelDBDir->setEnabled(checked);
    ui.lineDBDir->setEnabled(checked);
    ui.tbDBDir->setEnabled(checked);
}

void OptionDialog::on_chkLimitItems_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    bool checked = ui.chkLimitItems->isChecked();
    ui.labelNumberOfRows->setEnabled(checked);
    ui.spinNumberOfRows->setEnabled(checked);
}
void OptionDialog::on_context_titleTemplateCommonMain()
{
    QAction* act = static_cast<QAction*>(QObject::sender());
    QString target = act->data().toString();

    ui.lineInfoMain->insert(target);
}
void OptionDialog::on_context_titleTemplateCommonSub()
{
    QAction* act = static_cast<QAction*>(QObject::sender());
    QString target = act->data().toString();

    ui.lineInfoSub->insert(target);
}

void OptionDialog::constructTitleTemplateMenu(QMenu& contextMenu,
                                              QList< QSharedPointer<QAction> >& acts,
                                              bool isMain)
{
    static const auto alltargets = []()
    {
        QList<QPair<QString,QString>> alltargets;
        alltargets.append(QPair<QString,QString>("id" ,tr("ID")));
        alltargets.append(QPair<QString,QString>("name" ,tr("Filename")));
        alltargets.append(QPair<QString,QString>("directory", tr("Directory")));

        alltargets.append(QPair<QString,QString>("size" ,tr("Size")));
        alltargets.append(QPair<QString,QString>("atime" ,tr("Last Access")));
        alltargets.append(QPair<QString,QString>("wtime" ,tr("Last Modified")));

        alltargets.append(QPair<QString,QString>("duration" ,tr("Duration")));
        alltargets.append(QPair<QString,QString>("format" ,tr("Format")));
        alltargets.append(QPair<QString,QString>("bitrate" ,tr("Bitrate")));
        alltargets.append(QPair<QString,QString>("acodec" ,tr("Audio codec")));
        alltargets.append(QPair<QString,QString>("vcodec" ,tr("Video codec")));
        alltargets.append(QPair<QString,QString>("resolution" ,tr("Resolution")));
        alltargets.append(QPair<QString,QString>("fps" ,tr("fps")));

        alltargets.append(QPair<QString,QString>("opencount" ,tr("Open count")));
        alltargets.append(QPair<QString,QString>("tags" ,tr("Tags")));

        return alltargets;
    }();

    //    QStringList all;
    //    for(size_t i=0 ; i < sizeof(alltargets)/sizeof(alltargets[0]); ++i )
    //    {
    //        if(all.contains(alltargets[i]))
    //        {
    //            Q_ASSERT(false);
    //            continue;
    //        }
    //        all.append(alltargets[i]);
    //    }
    //    std::sort(all.begin(),all.end());

    for(auto&& pair : alltargets)
    {
        QSharedPointer<QAction> act(new QAction(pair.second));
        act->setData(QString("${") + pair.first + "}");
        connect(act.data(), SIGNAL(triggered()),
                this, isMain ? SLOT(on_context_titleTemplateCommonMain()):SLOT(on_context_titleTemplateCommonSub()) );
        contextMenu.addAction(act.data());
        acts.append(act);
    }
}

void OptionDialog::on_tbInfoMain_clicked()
{
    QPoint pos = ui.tbInfoMain->pos();
    pos.setX(pos.x() + ui.tbInfoMain->width());

    MyContextMenu contextMenu(tr("Context menu"), this);
    QList< QSharedPointer<QAction> > acts;

    constructTitleTemplateMenu(contextMenu, acts, true);

    contextMenu.exec(mapToGlobal(pos));
}

void OptionDialog::on_tbInfoSub_clicked()
{
    QPoint pos = ui.tbInfoSub->pos();
    pos.setX(pos.x() + ui.tbInfoSub->width());

    MyContextMenu contextMenu(tr("Context menu"), this);
    QList< QSharedPointer<QAction> > acts;

    constructTitleTemplateMenu(contextMenu, acts, false);

    contextMenu.exec(mapToGlobal(pos));
}
void OptionDialog::done(int r)
{
    if (r != DialogCode::Rejected)
    {
        QString tf = ui.cmbThumbImageFormat->lineEdit()->text();
        if(tf.isEmpty())
        {
            Alert(this,
                  tr("Thumbnail Image format must be specified."));
            ui.cmbThumbImageFormat->setFocus();
            return;
        }
        if(!isLegalFileExt(tf))
        {
            Alert(this,
                  tr("Thumbnail Image format is illegal."));
            ui.cmbThumbImageFormat->setFocus();
            return;
        }
    }
    QDialog::done(r);
}
