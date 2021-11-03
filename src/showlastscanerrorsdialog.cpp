#include "showlastscanerrorsdialog.h"
#include "ui_showlastscanerrorsdialog.h"

#include "helper.h"

ShowLastScanErrorsDialog::ShowLastScanErrorsDialog(QWidget *parent, const QString& error) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::ShowLastScanErrorsDialog)
{
    ui->setupUi(this);
    ui->tbErrors->setText(error);
}

ShowLastScanErrorsDialog::~ShowLastScanErrorsDialog()
{
    delete ui;
}
