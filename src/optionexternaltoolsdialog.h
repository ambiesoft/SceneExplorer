#ifndef OPTIONEXTERNALTOOLSDIALOG_H
#define OPTIONEXTERNALTOOLSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

#include "externaltoolitem.h"

namespace Ui {
class OptionExternalToolsDialog;
}

class OptionExternalToolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionExternalToolsDialog(QWidget *parent = 0);
    ~OptionExternalToolsDialog();

    QList<ExternalToolItem> items_;
    int GetItemIndex(QListWidgetItem* item) const;
protected:
    void showEvent(QShowEvent *) override;

private slots:
    void on_listWidget_itemSelectionChanged();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pbAdd_clicked();

    void on_pbRemove_clicked();

    void on_pbMoveUp_clicked();

    void on_pbMoveDown_clicked();

private:
    Ui::OptionExternalToolsDialog *ui;
};

#endif // OPTIONEXTERNALTOOLSDIALOG_H
