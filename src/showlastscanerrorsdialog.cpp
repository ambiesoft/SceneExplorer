#include "showlastscanerrorsdialog.h"
#include "ui_showlastscanerrorsdialog.h"

#include "../../lsMisc/stdQt/stdQt.h"

#include "helper.h"

using namespace AmbiesoftQt;

ShowLastScanErrorsDialog::ShowLastScanErrorsDialog(QWidget *parent, const QList<QString>* error) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::ShowLastScanErrorsDialog),
    m_errorLog(error)
{
    ui->setupUi(this);

    const int maxDisplayLength = 10 * 1000 * 1000;

    int nCharCount = 0;
    for (const QString &str : *m_errorLog) {
        nCharCount += str.length();
    }
    qDebug() << "nCharCount" << nCharCount << __FUNCTION__;

    if(nCharCount==0) {
        ui->pteErrors->setPlainText(tr("<No Error Logs>"));
    } else if (nCharCount > maxDisplayLength) {
        QString warningMsg = tr("The error log is too large to display on the screen (total characters: %1). Please save the log as a file.")
                                .arg(nCharCount);
        ui->pteErrors->setPlainText(warningMsg);
    } else {
        ui->pteErrors->setPlainText(error->join("\n"));
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
        try {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            for (const QString &str : *m_errorLog) {
                out << str << "\n";
            }
            file.close();
        } else {
            Alert(this, tr("Failed to write to the file."));
        }
        } catch(QException& ex) {
            Alert(this, ex.what());
        }
    }
}


void ShowLastScanErrorsDialog::on_pbOk_clicked()
{
    accept();
}

