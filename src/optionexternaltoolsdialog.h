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

#ifndef OPTIONEXTERNALTOOLSDIALOG_H
#define OPTIONEXTERNALTOOLSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

#include "../../lsMisc/stdQt/inisettings.h"

#include "externaltoolitem.h"

namespace Ui {
class OptionExternalToolsDialog;
}

class ExternalToolWidgetItem : public QListWidgetItem
{
    Q_DISABLE_COPY(ExternalToolWidgetItem)

    ExternalToolItem item_;
public:
    ExternalToolWidgetItem(const ExternalToolItem& item) :
        item_(item){
        this->setText(item.GetName());
        this->setIcon(item.GetIcon());
    }
    // ~ExternalToolWidgetItem() {}

    ExternalToolItem GetItem() {
        return item_;
    }

    QString GetItemName() const {
        return item_.GetName();
    }
    QString GetItemExe() const {
        return item_.GetExe();
    }
    QString GetItemArg() const {
        return item_.GetArg();
    }

    void SetItemName(const QString& name) {
        item_.SetName(name);
    }
    void SetItemExe(const QString& exe) {
        item_.SetExe(exe);
    }
    void SetItemArg(const QString& arg) {
        item_.SetArg(arg);
    }

    bool IsCountAsOpen() const {
        return item_.IsCountAsOpen();
    }
    void SetCountAsOpen(bool b) {
        item_.SetCountAsOpen(b);
    }
};

class OptionExternalToolsDialog : public QDialog
{
    Q_OBJECT

    bool selectionChanging_=false;

    bool HasItemWithName(const QString& name);
    void UpdateData();
    AmbiesoftQt::IniSettings& settings_;
    QString lastSelectedExeDir_;
public:
    explicit OptionExternalToolsDialog(AmbiesoftQt::IniSettings& settings, QWidget *parent = nullptr);
    ~OptionExternalToolsDialog() override;

    QList<ExternalToolItem> items_;
    int GetItemIndex(QListWidgetItem* item) const;

protected:
    void showEvent(QShowEvent *) override;

    void constructTitleTemplateMenu(
            QMenu& contextMenu,
            QList< QSharedPointer<QAction> >& acts);

private Q_SLOTS:
    void on_listWidget_itemSelectionChanged();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pbAdd_clicked();

    void on_pbRemove_clicked();

    void on_pbMoveUp_clicked();

    void on_pbMoveDown_clicked();


    void on_lineName_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_tbExecutable_clicked();

    void on_tbArguments_clicked();

    void on_lineExe_textChanged(const QString &arg1);

    void on_lineArg_textChanged(const QString &arg1);

    void on_chkCountAsOpen_stateChanged(int arg1);

    void on_context_titleTemplateCommonMain();

private:
    Ui::OptionExternalToolsDialog *ui;
};

#endif // OPTIONEXTERNALTOOLSDIALOG_H
