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

#include "../../lsMisc/stdosd/blockedbool.h"
#include "../../lsMisc/stdQt/stdQt.h"

#include "taginputdialog.h"
#include "ui_taginputdialog.h"
#include "helper.h"

using namespace AmbiesoftQt;

TagInputDialog::TagInputDialog(QWidget *parent, TagDialogType tdt, Document* pDoc) :
    QDialog(parent,GetDefaultDialogFlags()),
    tdt_(tdt),
    pDoc_(pDoc),
    ui(new Ui::TagInputDialog)
{
    ui->setupUi(this);
    Q_ASSERT(tdt_==TAGDIALOG_ADD || tdt_==TAGDIALOG_EDIT);
}

TagInputDialog::~TagInputDialog()
{
    delete ui;
}

void TagInputDialog::on_lineTag_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(tdt_==TAGDIALOG_ADD)
    {
        if(pDoc_->IsTagExist(ui->lineTag->text()))
        {
            ui->labelWarn->setText(tr("Tag \"%1\" already exists.").arg(ui->lineTag->text()));
            ui->lineYomi->setEnabled(false);
            return;
        }
        else
        {
            ui->labelWarn->setText(QString());
            ui->lineYomi->setEnabled(true);
        }
    }

    if(!yomiChanged_)
    {
        if(lastTagText_==ui->lineYomi->text())
        {
            Ambiesoft::BlockedBool bb(&yomiChangging_);
            ui->lineYomi->setText(ui->lineTag->text());
        }
    }
    lastTagText_ = ui->lineTag->text();
}

void TagInputDialog::on_lineYomi_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(yomiChangging_)
        return;

    yomiChanged_ = true;
}

QString TagInputDialog::tag() const
{
    return ui->lineTag->text();
}
void TagInputDialog::setTag(const QString& txt)
{
    ui->lineTag->setText(txt);
}
QString TagInputDialog::yomi() const
{
    return ui->lineYomi->text();
}
void TagInputDialog::setYomi(const QString& txt)
{
    ui->lineYomi->setText(txt);
}
void TagInputDialog::done(int r)
{
    if (r != DialogCode::Rejected)
    {
        QString tag = ui->lineTag->text();
        if (tag.isEmpty())
        {
            Alert(this, tr("Tag is empty."));
            return;
        }
        if (tag.contains("\t") || tag.contains("\n"))
        {
            Alert(this, tr("Tagname cound not have '\\t' or/and '\\n'"));
            return;
        }
    }
    QDialog::done(r);
}
