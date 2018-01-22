#ifndef OPTIONEXTENSION_H
#define OPTIONEXTENSION_H

#include "settings.h"
#include "ui_optionextension.h"

class OptionExtension : public QDialog
{
    Q_OBJECT

private:
    Ui::OptionExtension ui;
    Settings& settings_;
public:
    OptionExtension(QWidget* parent, Settings& settings);

protected:
    void showEvent(QShowEvent *) override;
    void on_radioAllow_clicked_common();
private slots:
    void on_buttonBox_accepted();
    void on_btnRestoreDefault_clicked();
    void on_radioAllow_clicked();
    void on_radioDeny_clicked();
};

#endif // OPTIONEXTENSION_H
