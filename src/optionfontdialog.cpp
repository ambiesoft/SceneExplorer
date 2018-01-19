#include "optionfontdialog.h"
#include "ui_optionfontdialog.h"

OptionFontDialog::OptionFontDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionFontDialog)
{
    ui->setupUi(this);
}

OptionFontDialog::~OptionFontDialog()
{
    delete ui;
}

void OptionFontDialog::showEvent(QShowEvent *)
{

}
void OptionFontDialog::on_buttonBox_accepted()
{
    fontInfo_ = ui->fontComboBox->currentFont();
}
