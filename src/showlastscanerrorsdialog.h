#ifndef SHOWLASTSCANERRORSDIALOG_H
#define SHOWLASTSCANERRORSDIALOG_H

#include <QDialog>

namespace Ui {
class ShowLastScanErrorsDialog;
}

class ShowLastScanErrorsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowLastScanErrorsDialog(QWidget *parent, const QString& error);
    ~ShowLastScanErrorsDialog();

private slots:
    void on_pbSave_clicked();

    void on_pbOk_clicked();

private:
    Ui::ShowLastScanErrorsDialog *ui;
    QString m_errorLog;
};

#endif // SHOWLASTSCANERRORSDIALOG_H
