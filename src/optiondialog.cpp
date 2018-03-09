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

#include "consts.h"
#include "helper.h"
#include "optiondialog.h"

OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent,GetDefaultDialogFlags()),
      imagecache_(IC_NEVER)
{
    ui.setupUi(this);

    ui.cmbThumbCount->addItem("3");
    ui.cmbThumbCount->addItem("5");

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

    ui.lineInfoMain->setText(mainText_);
    ui.lineInfoSub->setText(subText_);

    switch(imagecache_)
    {
    case IC_NEVER:
        ui.rbICNever->setChecked(true);
        break;
    case IC_PERFOLDER:
        ui.rbICPerfolder->setChecked(true);
        break;
    case IC_ALWAYS:
        ui.rbICAlways->setChecked(true);
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

    on_chkUseCustomDatabaseDirectory_stateChanged(0);
    on_chkLimitItems_stateChanged(0);

    QDialog::showEvent(ev);
}
void OptionDialog::on_buttonBox_accepted()
{
    maxgd_ = ui.spinBoxGetDir->value();
    maxff_ = ui.spinBoxThumb->value();

    thumbCount_ = ui.cmbThumbCount->currentIndex()==0 ? 3 : 5;

    mainText_ = ui.lineInfoMain->text();
    subText_ = ui.lineInfoSub->text();

    if(ui.rbICNever->isChecked())
        imagecache_=IC_NEVER;
    else if(ui.rbICPerfolder->isChecked())
        imagecache_=IC_PERFOLDER;
    else if(ui.rbICAlways->isChecked())
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

void OptionDialog::on_tbffprobe_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);

    if(!dlg.exec())
        return;

    ui.lineffprobe->setText(dlg.selectedFiles()[0]);
}

void OptionDialog::on_tbffmpeg_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);

    if(!dlg.exec())
        return;

    ui.lineffmpeg->setText(dlg.selectedFiles()[0]);
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

void OptionDialog::on_tbInfoMain_clicked()
{
    ui.lineInfoMain->setText(Consts::DEFAULT_ITEM_MAIN_TEXT);
}

void OptionDialog::on_tbInfoSub_clicked()
{
    ui.lineInfoSub->setText(Consts::DEFAULT_ITEM_SUB_TEXT);
}
