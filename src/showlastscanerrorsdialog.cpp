#include "showlastscanerrorsdialog.h"
#include "ui_showlastscanerrorsdialog.h"

#include "../../lsMisc/stdQt/stdQt.h"

#include "helper.h"

using namespace AmbiesoftQt;

ShowLastScanErrorsDialog::ShowLastScanErrorsDialog(QWidget *parent, const QString& error) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::ShowLastScanErrorsDialog),
    m_errorLog(error)
{
    ui->setupUi(this);

    const int maxDisplayLength = 10 * 1000 * 1000;

    if (error.length() > maxDisplayLength) {
        QString warningMsg = tr("The error log is too large to display on the screen (total characters: %1). Please save the log as a file.")
                                .arg(error.length());
        ui->pteErrors->setPlainText(warningMsg);
    } else {
        ui->pteErrors->setPlainText(error);
    }
}

ShowLastScanErrorsDialog::~ShowLastScanErrorsDialog()
{
    delete ui;
}

void ShowLastScanErrorsDialog::on_pbSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Saving Error Logs"),
        "scan_errors.txt",
        tr("Text File (*.txt);;All Files (*.*)")
        );

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << m_errorLog;
            file.close();
        } else {
            Alert(this, tr("Failed to write to the file."));
        }
    }
}


void ShowLastScanErrorsDialog::on_pbOk_clicked()
{
    accept();
}

