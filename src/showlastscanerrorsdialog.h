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

private:
    Ui::ShowLastScanErrorsDialog *ui;
};

#endif // SHOWLASTSCANERRORSDIALOG_H
