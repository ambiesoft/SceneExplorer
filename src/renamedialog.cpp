#include "helper.h"
#include "renamedialog.h"
#include "ui_renamedialog.h"

RenameDialog::RenameDialog(QWidget *parent) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::RenameDialog)
{
    ui->setupUi(this);
}

RenameDialog::~RenameDialog()
{
    delete ui;
}
void RenameDialog::showEvent(QShowEvent *)
{
//    ui->lineName->setCursorPosition(0);
//    ui->lineName->setSelection(0, ui->lineName->text().length());
    ui->lineName->selectAll();
}
QString RenameDialog::basename() const
{
    return ui->lineName->text();
}
QString RenameDialog::ext() const
{
    return ui->lineExt->text();
}

void RenameDialog::setBasename(const QString& basename)
{
    ui->lineName->setText(basename);
}
void RenameDialog::setExt(const QString& ext)
{
    ui->lineExt->setText(ext);
}
