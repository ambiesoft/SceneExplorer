#ifndef ITEMPROPERTYDIALOG_H
#define ITEMPROPERTYDIALOG_H

#include <QDialog>

namespace Ui {
class ItemPropertyDialog;
}

class ItemPropertyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemPropertyDialog(QWidget *parent = nullptr);
    ~ItemPropertyDialog();

private:
    Ui::ItemPropertyDialog *ui;
};

#endif // ITEMPROPERTYDIALOG_H
