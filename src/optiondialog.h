#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include "ui_option.h"

class OptionDialog:public QDialog
{
    Q_OBJECT

private:
    Ui::Option ui;

public:
    OptionDialog(QWidget* parent = nullptr);
private slots:
    void on_buttonBox_accepted();
};

#endif // OPTIONDIALOG_H
