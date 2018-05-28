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

#include "helper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::on_action_Paste_triggered()
{
    if(ui->directoryWidget->hasFocus())
    {
        OnPasteDirectory();
        return;
    }
    if(ui->listTag->hasFocus())
    {
        OnPasteTag();
        return;
    }
    return;
}
void MainWindow::OnPasteDirectory()
{
    CHECK_DOCUMENT(pDoc_);

    QString clipText = getClipboardText();
    if(clipText.isEmpty())
    {
        Alert(this, TR_CLIPBOARD_IS_EMPTY());
        return;
    }
    QStringList lines = clipText.split('\n');
    if(lines.isEmpty())
    {
        Alert(this, TR_CLIPBOARD_IS_EMPTY());
        return;
    }
    if(lines[0] != STR_DIRECTORY_ENTRY_SIGNATURE)
    {
        Alert(this, tr("Invalid Signature"));
        return;
    }
    if(lines.count()==1)
    {
        Alert(this, tr("Directory entriy is empty."));
        return;
    }

    QStringList all;
    for(int i=1; i < lines.count(); ++i)
    {
        QString line=lines[i];
        line = line.trimmed();
        if(line.isEmpty())
            continue;
        if(line[0]=='#')
            continue;
        line = undq(line);
        all.append(line);
    }

    QString question = tr("Are you sure you want to add follwing directories?");
    question += "\n\n";
    question += all.join("\n");
    if(!YesNo(this, question))
        return;

    for(QString dir : all)
    {
        DirectoryItem* newdi=nullptr;
        if(!pDoc_->InsertDirectory(dir, newdi))
        {
            Alert(this,TR_FAILED_TO_INSERT_DIRECTORY_INTO_DATABASE());
            continue;
        }
        ui->directoryWidget->addItem(newdi);
    }
}
void MainWindow::OnPasteTag()
{
    CHECK_DOCUMENT(pDoc_);

    QString clipText = getClipboardText();
    if(clipText.isEmpty())
    {
        Alert(this, TR_CLIPBOARD_IS_EMPTY());
        return;
    }
    QStringList lines = clipText.split('\n');
    if(lines.isEmpty())
    {
        Alert(this, TR_CLIPBOARD_IS_EMPTY());
        return;
    }
    if(lines[0] != STR_TAG_ENTRY_SIGNATURE)
    {
        Alert(this, tr("Invalid Signature"));
        return;
    }
    if(lines.count()==1)
    {
        Alert(this, tr("Tag entriy is empty."));
        return;
    }

    QList<QPair<QString,QString> > all;
    for(int i=1; i < lines.count(); ++i)
    {
        QString line=lines[i];
        line = line.trimmed();
        if(line.isEmpty())
            continue;

        QStringList ty= line.split('\t');
        if(ty.isEmpty())
        {
            Alert(this, tr("Unexpected error"));
            return;
        }

        QString tag = ty[0];
        QString yomi = ty[1];
        all.append(QPair<QString,QString>(tag,yomi));
    }

    QString question = tr("Are you sure you want to add follwing tags?");
    question += "\n\n";
    for(QPair<QString,QString>& pair : all)
    {
        question += pair.first;
        if(!pair.second.isEmpty())
        {
            question += " (" + pair.second + ")";
        }
        question += "\n";
    }
    if(!YesNo(this, question))
        return;

    for(QPair<QString,QString>& pair : all)
    {
        CreateNewTag(pair.first, pair.second);
    }
}
