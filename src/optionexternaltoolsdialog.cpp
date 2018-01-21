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
        ExternalToolWidgetItem* item = new ExternalToolWidgetItem(items_[i]);
        item->setText(items_[i].GetName());

        ui->listWidget->addItem(item);
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
        ExternalToolWidgetItem* pItem = (ExternalToolWidgetItem*)previous;
        pItem->SetItemName(ui->lineName->text());
        pItem->SetItemExe(ui->lineExe->text());
        pItem->SetItemArg(ui->lineArg->text());
//        int prevI = GetItemIndex(previous);
//        items_[prevI].SetName(ui->lineName->text());
//        items_[prevI].SetExe(ui->lineExe->text());
//        items_[prevI].SetArg(ui->lineArg->text());
    }

    if(current)
    {
        ExternalToolWidgetItem* pItem = (ExternalToolWidgetItem*)current;
        ui->lineName->setText(pItem->GetItemName());
        ui->lineExe->setText(pItem->GetItemExe());
        ui->lineArg->setText(pItem->GetItemArg());

//        int currentI = GetItemIndex(current);
//        ui->lineName->setText(items_[currentI].GetName());
//        ui->lineExe->setText(items_[currentI].GetExe());
//        ui->lineArg->setText(items_[currentI].GetArg());
    }
}

bool OptionExternalToolsDialog::HasItemWithName(const QString& name)
{
    for(int i=0 ; i < ui->listWidget->count(); ++i)
    {
        if(name==ui->listWidget->item(i)->text())
            return true;
    }
    return false;
}
void OptionExternalToolsDialog::on_pbAdd_clicked()
{
    QString newitemnamebase = tr("New Item");
    QString newitemname;
    for(int i=1 ; ; ++i)
    {
        QString name = newitemnamebase + " " + QString::number(i);
        if(!HasItemWithName(name))
        {
            newitemname=name;
            break;
        }
    }

    ExternalToolWidgetItem* newitem = new ExternalToolWidgetItem(ExternalToolItem(newitemname, QString(), QString()));
    ui->listWidget->addItem(newitem);
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

void OptionExternalToolsDialog::on_pbAddMacro_clicked()
{

}

void OptionExternalToolsDialog::on_lineName_textChanged(const QString &arg1)
{
    if(ui->listWidget->selectedItems().isEmpty())
        return;

    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    item->setText(arg1);
}

void OptionExternalToolsDialog::on_buttonBox_accepted()
{
    UpdateData();
    items_.clear();
    for(int i=0 ; i < ui->listWidget->count(); ++i)
    {
         ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->item(i);
         items_.append(item->GetItem());
    }
}
void OptionExternalToolsDialog::UpdateData()
{
    if(ui->listWidget->selectedItems().isEmpty())
        return;

    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    item->SetItemName(ui->lineName->text());
    item->SetItemExe(ui->lineExe->text());
    item->SetItemArg(ui->lineArg->text());
}

void OptionExternalToolsDialog::on_tbExecutable_clicked()
{

}

void OptionExternalToolsDialog::on_tbArguments_clicked()
{

}
