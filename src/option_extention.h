#ifndef OPTIONEXTENTION_H
#define OPTIONEXTENTION_H

#include "ui_Option_Extention.h"

class Option_Extention : public QDialog
{
    Q_OBJECT

private:
    Ui::Option_Extention ui;

public:
    Option_Extention(QWidget* parent = nullptr);

    QString strAllow_;
    QString strDeny_;

protected:
    void showEvent(QShowEvent *) override;
private slots:
    void on_buttonBox_accepted();
    void on_button_RestoreDefault_clicked();


};

#endif // OPTIONEXTENTION_H
