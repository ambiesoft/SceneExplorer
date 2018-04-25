#include "helper.h"

#include "docinfodialog.h"
#include "ui_docinfodialog.h"

DocinfoDialog::DocinfoDialog(QWidget *parent,
                             const QString& exe,
                             const QString& doc,
                             const QString& ini,
                             const QString& db
                             ) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::DocinfoDialog),
    exe_(exe),
    doc_(doc),
    ini_(ini),
    db_(db)
{
    ui->setupUi(this);

    ui->lineExe->setText(exe);
    ui->lineDoc->setText(doc);
    ui->lineIni->setText(ini);
    ui->lineDb->setText(db);
}

DocinfoDialog::~DocinfoDialog()
{
    delete ui;
}
