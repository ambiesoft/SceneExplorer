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
QString RenameDialog::filename() const
{
    if(ui->lineName->text().isEmpty() && ui->lineExt->text().isEmpty())
        return QString();
    return ui->lineName->text() + "." + ui->lineExt->text();
}
void RenameDialog::done(int r)
{
    QString name = filename();
    if(name.isEmpty())
    {
        Alert(this,tr("Name is empty."));
        return;
    }
    if(name.contains("/") || name.contains("\\"))
    {
        Alert(this,tr("Filename cound not have '/' or/and '\\'"));
        return;
    }
    QString error;
    if(!isLegalFilePath(name, &error))
    {
        Alert(this, error);
        return;
    }
    QDialog::done(r);
}
