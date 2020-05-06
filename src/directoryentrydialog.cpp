#include <QFileDialog>

#include "../../lsMisc/stdQt/stdQt.h"

#include "helper.h"

#include "directoryentrydialog.h"
#include "ui_directoryentrydialog.h"

using namespace AmbiesoftQt;

DirectoryEntryDialog::DirectoryEntryDialog(QWidget *parent) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::DirectoryEntryDialog)
{
    ui->setupUi(this);
}

DirectoryEntryDialog::~DirectoryEntryDialog()
{
    delete ui;
}

void DirectoryEntryDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);

    // Variable to Dialog
    updateData(true);
}
void DirectoryEntryDialog::updateData(const bool bDataToDialog)
{
    if(bDataToDialog)
    {
        ui->lineDirectory->setText(directory_);
        ui->lineDisplayText->setText(displaytext_);
    }
    else
    {
        directory_ = ui->lineDirectory->text();
        displaytext_ = ui->lineDisplayText->text();
    }
}
void DirectoryEntryDialog::on_btnBrowse_clicked()
{
}

void DirectoryEntryDialog::on_buttonBox_accepted()
{
    updateData(false);
}

void DirectoryEntryDialog::on_tbBrowse_triggered(QAction*)
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Directory"),
                                                    lastselecteddirectory_.isEmpty() ?
                                                        (directory_.isEmpty() ? GetUserDocumentDirectory():directory_) :
                                                        lastselecteddirectory_);
    if(dir.isEmpty())
        return;

    ui->lineDirectory->setText(dir);
}
