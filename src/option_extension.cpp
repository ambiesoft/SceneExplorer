
#include "helper.h"
#include "option_extension.h"


Option_Extension::Option_Extension(QWidget* parent)
    : QDialog(parent, GetDefaultDialogFlags())
{
    ui.setupUi(this);
    setWindowTitle(tr("Option") + " | " + tr("Extension"));
}
void Option_Extension::showEvent(QShowEvent *ev)
{
    ui.txtAllow->setPlainText(strAllow_);
    ui.txtDeny->setPlainText(strDeny_);

    QDialog::showEvent(ev);
}
void Option_Extension::on_buttonBox_accepted()
{
    strAllow_ = ui.txtAllow->toPlainText();
    strDeny_ = ui.txtDeny->toPlainText();
}

void Option_Extension::on_button_RestoreDefault_clicked()
{

}
