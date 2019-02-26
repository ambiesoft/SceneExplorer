#include "itempropertydialog.h"
#include "ui_itempropertydialog.h"

ItemPropertyDialog::ItemPropertyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemPropertyDialog)
{
    ui->setupUi(this);
}

ItemPropertyDialog::~ItemPropertyDialog()
{
    delete ui;
}
