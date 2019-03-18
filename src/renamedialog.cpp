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

#include "../../lsMisc/stdQt/stdQt.h"

#include "helper.h"
#include "osd.h"

#include "renamedialog.h"
#include "ui_renamedialog.h"

using namespace AmbiesoftQt;

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
    if (r != DialogCode::Rejected)
    {
        QString name = filename();
        if (name.isEmpty())
        {
            Alert(this, tr("Name is empty."));
            return;
        }
        if (name.contains("/") || name.contains("\\"))
        {
            Alert(this, tr("Filename cound not have '/' or/and '\\'"));
            return;
        }
        QString error;
        if (!isLegalFilePath(name, &error))
        {
            Alert(this, error);
            return;
        }
    }
    QDialog::done(r);
}
