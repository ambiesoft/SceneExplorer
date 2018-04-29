#ifndef DOCINFODIALOG_H
#define DOCINFODIALOG_H

#include <ui_docinfodialog.h>

//namespace Ui {
//class DocinfoDialog;
//}

class DocinfoDialog : public QDialog
{
    Q_OBJECT

    Ui::DocinfoDialog *ui;

    QString exe_;
    QString doc_;
    QString ini_;
    QString db_;
public:
    explicit DocinfoDialog(QWidget *parent,
                           const QString& exe,
                           const QString& doc,
                           const QString& ini,
                           const QString& db
                           );
    ~DocinfoDialog();



};

#endif // DOCINFODIALOG_H
