#include "helper.h"

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QFile licensefile(":/resource/license.html");
    if (licensefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->textBrowser->setHtml(QString::fromUtf8(licensefile.readAll().constData()));
        licensefile.close();
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
