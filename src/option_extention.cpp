
#include "helper.h"
#include "option_extention.h"


Option_Extention::Option_Extention(QWidget* parent)
    : QDialog(parent,
              ((Qt::WindowTitleHint | Qt::WindowFlags()) & ~Qt::WindowContextHelpButtonHint))
{
    ui.setupUi(this);
    setWindowTitle(tr("Option") + " | " + tr("Extension"));
}
void Option_Extention::showEvent(QShowEvent *ev)
{
    ui.txtAllow->setPlainText(strAllow_);
    ui.txtDeny->setPlainText(strDeny_);

    QDialog::showEvent(ev);
}
void Option_Extention::on_buttonBox_accepted()
{
    strAllow_ = ui.txtAllow->toPlainText();
    strDeny_ = ui.txtDeny->toPlainText();
}

void Option_Extention::on_button_RestoreDefault_clicked()
{

}
