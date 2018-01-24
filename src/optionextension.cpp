#include <QTextBlock>

#include "helper.h"
#include "extension.h"
#include "optionextension.h"


OptionExtension::OptionExtension(QWidget* parent, Settings& settings)
    : QDialog(parent, GetDefaultDialogFlags()),
      settings_(settings)
{
    ui.setupUi(this);
    setWindowTitle(tr("Option") + " | " + tr("Extension"));
}
void OptionExtension::showEvent(QShowEvent *ev)
{
    ui.txtAllow->setPlainText(Extension::GetMovieExtensionAllowAsString());
    ui.txtDeny->setPlainText(Extension::GetMovieExtensionDenyAsString());
    if(Extension::IsOrderAllow())
        ui.radioAllow->setChecked(true);
    else
        ui.radioDeny->setChecked(true);
    on_radioAllow_clicked_common();

    QDialog::showEvent(ev);
}

QStringList GetAllLine(QPlainTextEdit* edit)
{
    QStringList ret;
    QTextDocument *doc = edit->document();
    for(int i=0 ; i < doc->lineCount(); ++i)
    {
        QTextBlock tb = doc->findBlockByLineNumber(i);
        ret << tb.text();
    }
    return ret;
}
void OptionExtension::on_buttonBox_accepted()
{
    QStringList arAllow = GetAllLine(ui.txtAllow);
    QStringList arDeny = GetAllLine(ui.txtDeny);

    Extension::SetMovieExtensionAllow(arAllow);
    Extension::SetMovieExtensionDeny(arDeny);
    Extension::SetOrderAllow(ui.radioAllow->isChecked());
}

void OptionExtension::on_btnRestoreDefault_clicked()
{
    ui.txtAllow->setPlainText(Extension::GetDefaultAllowAsString());
    ui.txtDeny->setPlainText(Extension::GetDefaultDenyAsString());
}

void OptionExtension::on_radioAllow_clicked_common()
{
    bool bAllow = ui.radioAllow->isChecked();

    ui.txtAllow->setEnabled(bAllow);
    ui.txtDeny->setEnabled(!bAllow);
}
void OptionExtension::on_radioAllow_clicked()
{
    on_radioAllow_clicked_common();
}

void OptionExtension::on_radioDeny_clicked()
{
    on_radioAllow_clicked_common();
}
