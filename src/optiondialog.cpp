#include <QWidget>
#include "optiondialog.h"

OptionDialog::OptionDialog(QWidget* parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("option"));


}
void OptionDialog::showEvent(QShowEvent *ev)
{
    ui.spinMaxFFmpegCount->setValue(maxff_);
    QDialog::showEvent(ev);
}
void OptionDialog::on_buttonBox_accepted()
{
    maxff_ = ui.spinMaxFFmpegCount->value();
}
