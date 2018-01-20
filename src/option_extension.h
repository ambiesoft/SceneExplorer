#ifndef OPTIONEXTENSION_H
#define OPTIONEXTENSION_H

#include "ui_option_extension.h"

class Option_Extension : public QDialog
{
    Q_OBJECT

private:
    Ui::Option_Extension ui;

public:
    Option_Extension(QWidget* parent = nullptr);

    QString strAllow_;
    QString strDeny_;

protected:
    void showEvent(QShowEvent *) override;
private slots:
    void on_buttonBox_accepted();
    void on_button_RestoreDefault_clicked();


};

#endif // OPTIONEXTENSION_H
