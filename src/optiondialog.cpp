#include <QWidget>
#include "helper.h"
#include "optiondialog.h"

OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent,GetDefaultDialogFlags()),
      imagecache_(IC_NEVER)
{
    ui.setupUi(this);
    setWindowTitle(tr("option"));
}

void OptionDialog::showEvent(QShowEvent *ev)
{
    ui.spinBoxGetDir->setValue(maxgd_);
    ui.spinBoxThumb->setValue(maxff_);

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
    QDialog::showEvent(ev);
}
void OptionDialog::on_buttonBox_accepted()
{
    maxgd_ = ui.spinBoxGetDir->value();
    maxff_ = ui.spinBoxThumb->value();

    if(ui.rbICNever->isChecked())
        imagecache_=IC_NEVER;
    else if(ui.rbICPerfolder->isChecked())
        imagecache_=IC_PERFOLDER;
    else if(ui.rbICAlways->isChecked())
        imagecache_=IC_ALWAYS;
    else
        Q_ASSERT(false);
}
