#include "helper.h"

#include "optionexternaltoolsdialog.h"
#include "ui_optionexternaltoolsdialog.h"


OptionExternalToolsDialog::OptionExternalToolsDialog(QWidget *parent) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::OptionExternalToolsDialog)
{
    ui->setupUi(this);
}
void OptionExternalToolsDialog::showEvent(QShowEvent *ev)
{
    for(int i=0 ; i < items_.count(); ++i)
    {
        ui->listWidget->addItem(items_[i].GetName());
    }
    QDialog::showEvent(ev);
}
OptionExternalToolsDialog::~OptionExternalToolsDialog()
{
    delete ui;
}

void OptionExternalToolsDialog::on_listWidget_itemSelectionChanged()
{

}
int OptionExternalToolsDialog::GetItemIndex(QListWidgetItem* item) const
{
    for(int i=0 ; i < ui->listWidget->count(); ++i)
    {
        if(ui->listWidget->item(i)==item)
            return i;
    }
    Q_ASSERT(false);
    return -1;
}
void OptionExternalToolsDialog::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current==nullptr && previous==nullptr)
        return;

    if(previous)
    {
        int prevI = GetItemIndex(previous);
        items_[prevI].SetName(ui->lineName->text());
        items_[prevI].SetExe(ui->lineExe->text());
        items_[prevI].SetArg(ui->lineArg->text());
    }

    if(current)
    {
        int currentI = GetItemIndex(current);
        ui->lineName->setText(items_[currentI].GetName());
        ui->lineExe->setText(items_[currentI].GetExe());
        ui->lineArg->setText(items_[currentI].GetArg());
    }
}

void OptionExternalToolsDialog::on_pbAdd_clicked()
{

}

void OptionExternalToolsDialog::on_pbRemove_clicked()
{

}

void OptionExternalToolsDialog::on_pbMoveUp_clicked()
{

}

void OptionExternalToolsDialog::on_pbMoveDown_clicked()
{

}
