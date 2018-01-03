#include <QWidget>
#include "optiondialog.h"

OptionDialog::OptionDialog(QWidget* parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("option"));
}

void OptionDialog::on_buttonBox_accepted()
{

}
