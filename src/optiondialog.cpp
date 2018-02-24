#include <QWidget>
#include <QFileDialog>

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

    ui.chkOpenLastDocument->setChecked(openlastdoc_);

    ui.lineffprobe->setText(ffprobe_);
    ui.lineffmpeg->setText(ffmpeg_);

    on_chkUseCustomDatabaseDirectory_stateChanged(0);

    QDialog::showEvent(ev);
}
void OptionDialog::on_buttonBox_accepted()
{
    maxgd_ = ui.spinBoxGetDir->value();
    maxff_ = ui.spinBoxThumb->value();

    thumbCount_ = ui.cmbThumbCount->currentIndex()==0 ? 3 : 5;

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
