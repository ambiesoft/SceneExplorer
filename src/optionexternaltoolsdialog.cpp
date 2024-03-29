//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <QFileDialog>
#include <QMenu>

#include "../../lsMisc/stdosd/blockedbool.h"
#include "../../lsMisc/stdQt/stdQt.h"

#include "helper.h"
#include "mycontextmenu.h"
#include "mycontextmenu.h"
#include "consts.h"

#include "optionexternaltoolsdialog.h"
#include "ui_optionexternaltoolsdialog.h"

using namespace Consts;
using namespace AmbiesoftQt;

OptionExternalToolsDialog::OptionExternalToolsDialog(IniSettings& settings, QWidget *parent) :
    QDialog(parent,GetDefaultDialogFlags()),
    settings_(settings),
    ui(new Ui::OptionExternalToolsDialog)
{
    ui->setupUi(this);

    lastSelectedExeDir_ = settings_.valueString(KEY_EXTERNALTOOLS_LASTSELECTEDEXEDIR);
    restoreGeometry(settings.value(KEY_EXTERNALTOOLS_DIALOGGEOMETRY).toByteArray());
    CenterWidgets(this);
    setWindowTitle(tr("Option") + " | " + tr("External tools"));
}
void OptionExternalToolsDialog::showEvent(QShowEvent *ev)
{
    ExternalToolWidgetItem* itemToSelect=nullptr;
    for(int i=0 ; i < items_.count(); ++i)
    {
        ExternalToolWidgetItem* item = new ExternalToolWidgetItem(items_[i]);
        item->setText(items_[i].GetName());

        ui->listWidget->addItem(item);
        if(i==0)
            itemToSelect=item;
    }
    if(itemToSelect)
        itemToSelect->setSelected(true);

    // make widget enable or disable
    on_listWidget_itemSelectionChanged();

    QDialog::showEvent(ev);
}
OptionExternalToolsDialog::~OptionExternalToolsDialog()
{
    delete ui;
}

void OptionExternalToolsDialog::on_listWidget_itemSelectionChanged()
{
    Ambiesoft::BlockedBool bb(&selectionChanging_);
    if(ui->listWidget->selectedItems().isEmpty())
    {
        ui->lineName->setEnabled(false);
        ui->lineExe->setEnabled(false);
        ui->tbExecutable->setEnabled(false);
        ui->lineArg->setEnabled(false);
        ui->tbArguments->setEnabled(false);
        ui->chkCountAsOpen->setEnabled(false);

        ui->pbMoveDown->setEnabled(false);
        ui->pbMoveUp->setEnabled(false);
        ui->pbRemove->setEnabled(false);
        return;
    }
    ui->lineName->setEnabled(true);
    ui->lineExe->setEnabled(true);
    ui->tbExecutable->setEnabled(true);
    ui->lineArg->setEnabled(true);
    ui->tbArguments->setEnabled(true);
    ui->chkCountAsOpen->setEnabled(true);

    ui->pbMoveDown->setEnabled(true);
    ui->pbMoveUp->setEnabled(true);
    ui->pbRemove->setEnabled(true);


    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    ui->lineName->setText(item->GetItemName());
    ui->lineExe->setText(item->GetItemExe());
    ui->lineArg->setText(item->GetItemArg());
    ui->chkCountAsOpen->setChecked(item->IsCountAsOpen());
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
    ui->lineName->setEnabled(current != nullptr);

    if(current==nullptr && previous==nullptr)
        return;

    if(previous)
    {
        //        ExternalToolWidgetItem* pItem = (ExternalToolWidgetItem*)previous;
        //        pItem->SetItemName(ui->lineName->text());
        //        pItem->SetItemExe(ui->lineExe->text());
        //        pItem->SetItemArg(ui->lineArg->text());
        //        int prevI = GetItemIndex(previous);
        //        items_[prevI].SetName(ui->lineName->text());
        //        items_[prevI].SetExe(ui->lineExe->text());
        //        items_[prevI].SetArg(ui->lineArg->text());
    }

    if(current)
    {
        //        ExternalToolWidgetItem* pItem = (ExternalToolWidgetItem*)current;
        //        ui->lineName->setText(pItem->GetItemName());
        //        ui->lineExe->setText(pItem->GetItemExe());
        //        ui->lineArg->setText(pItem->GetItemArg());

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

    ExternalToolWidgetItem* newitem = new ExternalToolWidgetItem(
                ExternalToolItem(newitemname, QString(), "\"${filefullpath}\"", false));
    ui->listWidget->addItem(newitem);
    ui->listWidget->setCurrentItem(newitem);
    //    for(int i=0 ; i < ui->listWidget->count(); ++i)
    //    {
    //        if(ui->listWidget->item(i) != newitem)
    //        {
    //            ui->listWidget->item(i)->setSelected(false);
    //        }
    //        else
    //        {
    //            ui->listWidget->item(i)->setSelected(true);
    //        }
    //    }
}

void OptionExternalToolsDialog::on_pbRemove_clicked()
{
    QListWidgetItem* current = ui->listWidget->currentItem();
    if(!current)
        return;

    if(!YesNo(this,
              tr("Are you sure you want to remove \"%1\"?").arg(current->text())))
    {
        return;
    }

    ui->listWidget->removeItemWidget(current);
    delete current;
}

void OptionExternalToolsDialog::on_pbMoveUp_clicked()
{
    QModelIndex current = ui->listWidget->currentIndex();
    if(current.row() <= 0)
        return;

    QListWidgetItem* hold = ui->listWidget->takeItem(current.row());
    Q_ASSERT(hold);
    int next = current.row() - 1;
    ui->listWidget->insertItem(next, hold);
    ui->listWidget->setCurrentItem(hold);
}

void OptionExternalToolsDialog::on_pbMoveDown_clicked()
{
    QModelIndex current = ui->listWidget->currentIndex();
    if( (current.row()+1) >= ui->listWidget->count())
        return;

    QListWidgetItem* hold = ui->listWidget->takeItem(current.row());
    Q_ASSERT(hold);
    int next = current.row() + 1;
    ui->listWidget->insertItem(next, hold);
    ui->listWidget->setCurrentItem(hold);
}



void OptionExternalToolsDialog::on_lineName_textChanged(const QString &arg1)
{
    if(selectionChanging_)
        return;
    if(ui->listWidget->selectedItems().isEmpty())
        return;

    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    item->SetItemName(arg1);
    item->setText(arg1);
}
void OptionExternalToolsDialog::on_lineExe_textChanged(const QString &arg1)
{
    if(selectionChanging_)
        return;

    if(ui->listWidget->selectedItems().isEmpty())
        return;

    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    item->SetItemExe(arg1);
}

void OptionExternalToolsDialog::on_lineArg_textChanged(const QString &arg1)
{
    if(selectionChanging_)
        return;

    if(ui->listWidget->selectedItems().isEmpty())
        return;

    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    item->SetItemArg(arg1);
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

    settings_.setValue(KEY_EXTERNALTOOLS_DIALOGGEOMETRY,saveGeometry());
}
void OptionExternalToolsDialog::UpdateData()
{
    bool bNoSel = ui->listWidget->selectedItems().isEmpty();
    ui->lineName->setEnabled(!bNoSel);
    if(bNoSel)
        return;

    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    item->SetItemName(ui->lineName->text());
    item->SetItemExe(ui->lineExe->text());
    item->SetItemArg(ui->lineArg->text());
    item->SetCountAsOpen(ui->chkCountAsOpen->isChecked());
}

void OptionExternalToolsDialog::on_context_titleTemplateCommonMain()
{
    QAction* act = (QAction*)QObject::sender();
    QVariantHash hash = act->data().toHash();
    QString target = hash["text"].toString();
    const QToolButton* button = reinterpret_cast<QToolButton*>( hash["button"].toLongLong());

    if(button==ui->tbExecutable)
        ui->lineExe->insert(target);
    else if(button==ui->tbArguments)
        ui->lineArg->insert(target);
    else
        Q_ASSERT(false);
}
void OptionExternalToolsDialog::constructTitleTemplateMenu(QMenu& contextMenu,
                                                           QList< QSharedPointer<QAction> >& acts,
                                                           const QToolButton* button)
{
    const char* alltargets[] = {
        "appfullpath",
        "appdirectoryfullpath",
        "filefullpath",
        "directoryfullpath",
        "filefullpathwithoutextension",
        "filename",
        "filenamewithoutextension",
    };

    // Create "Choose menu"
    {
        QSharedPointer<QAction> act(new QAction(tr("Choose...")));
        QVariantHash hash;
        hash["button"]=(qlonglong)(button);
        act->setData(hash);
        connect(act.data(), SIGNAL(triggered()),
                SLOT(on_tb_chooseExecutable()));
        contextMenu.addAction(act.data());
        acts.append(act);
    }

    for(size_t i=0 ; i < sizeof(alltargets)/sizeof(alltargets[0]); ++i )
    {
        QSharedPointer<QAction> act(new QAction(tr(alltargets[i])));
        QVariantHash hash;
        hash["button"]=(qlonglong)(button);
        hash["text"]=QString("${") + alltargets[i] + "}";
        act->setData(hash);
        connect(act.data(), SIGNAL(triggered()),
                SLOT(on_context_titleTemplateCommonMain()));
        contextMenu.addAction(act.data());
        acts.append(act);
    }
}
void OptionExternalToolsDialog::on_tb_chooseExecutable()
{
    QFileDialog dialog(this, tr("Choose Executable"), lastSelectedExeDir_);
    dialog.setFileMode(QFileDialog::ExistingFile);
    if(!dialog.exec())
        return;

    if(dialog.selectedFiles().isEmpty())
    {
        Alert(this, tr("No file selected."));
        return;
    }

    QString selectedFile = dialog.selectedFiles()[0];

    QAction* act = (QAction*)QObject::sender();
    QVariantHash hash = act->data().toHash();
    const QToolButton* button = reinterpret_cast<QToolButton*>( hash["button"].toLongLong());
    if(button==ui->tbExecutable)
        ui->lineExe->insert(selectedFile);
    else if(button==ui->tbArguments)
        ui->lineArg->insert(selectedFile);
    else
        Q_ASSERT(false);

    lastSelectedExeDir_ = QFileInfo(selectedFile).absoluteDir().absolutePath();
    settings_.setValue(KEY_EXTERNALTOOLS_LASTSELECTEDEXEDIR, lastSelectedExeDir_);
}

void OptionExternalToolsDialog::on_tbExecutable_clicked()
{
    on_navigateCoimmon(ui->tbExecutable);
}
void OptionExternalToolsDialog::on_tbArguments_clicked()
{
    on_navigateCoimmon(ui->tbArguments);
}
void OptionExternalToolsDialog::on_navigateCoimmon(const QToolButton* button)
{
    QPoint pos = button->pos();
    pos.setX(pos.x() + button->width());

    MyContextMenu contextMenu(tr("Context menu"), this);
    QList< QSharedPointer<QAction> > acts;

    constructTitleTemplateMenu(contextMenu, acts, button);

    contextMenu.exec(mapToGlobal(pos));
}

void OptionExternalToolsDialog::on_chkCountAsOpen_stateChanged(int arg1)
{
    if(selectionChanging_)
        return;

    if(ui->listWidget->selectedItems().isEmpty())
        return;

    ExternalToolWidgetItem* item = (ExternalToolWidgetItem*)ui->listWidget->selectedItems()[0];
    item->SetCountAsOpen(!!arg1);
}
