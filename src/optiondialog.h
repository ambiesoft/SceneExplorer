#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H


#include "ui_option.h"

class OptionDialog:public QDialog
{
    Q_OBJECT

private:
    Ui::Option ui;

public:
    OptionDialog(QWidget* parent = nullptr);

    int maxff_;

protected:
    void showEvent(QShowEvent *) override;
private slots:
    void on_buttonBox_accepted();
};

#endif // OPTIONDIALOG_H
