#include <QWidget>
#include "optiondialog.h"

OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent,
              ((Qt::WindowTitleHint | Qt::WindowFlags()) & ~Qt::WindowContextHelpButtonHint))
{
    ui.setupUi(this);
    setWindowTitle(tr("option"));
}

void OptionDialog::showEvent(QShowEvent *ev)
{
    ui.spinBoxGetDir->setValue(maxgd_);
    ui.spinBoxThumb->setValue(maxff_);
    QDialog::showEvent(ev);
}
void OptionDialog::on_buttonBox_accepted()
{
    maxgd_ = ui.spinBoxGetDir->value();
    maxff_ = ui.spinBoxThumb->value();
}
