#ifndef ITEMPROPERTYDIALOG_H
#define ITEMPROPERTYDIALOG_H

#include <QDialog>

namespace Ui {
class ItemPropertyDialog;
}

class ItemPropertyDialog : public QDialog
{
    Q_OBJECT

    void updateData(bool bUpdate);

public:
    explicit ItemPropertyDialog(QWidget *parent = nullptr);
    ~ItemPropertyDialog();

    qint64 id_ = -1;
    QString file_;
    qint64 openCount_;

protected:
      void showEvent(QShowEvent *ev);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ItemPropertyDialog *ui;
};

#endif // ITEMPROPERTYDIALOG_H
