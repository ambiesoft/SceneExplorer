#ifndef OPTIONFONTDIALOG_H
#define OPTIONFONTDIALOG_H

#include <QDialog>
#include <QFont>

namespace Ui {
class OptionFontDialog;
}

class OptionFontDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionFontDialog(QWidget *parent = 0);
    ~OptionFontDialog();

    QFont fontInfo_;

protected:
    void showEvent(QShowEvent *) override;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::OptionFontDialog *ui;
};

#endif // OPTIONFONTDIALOG_H
