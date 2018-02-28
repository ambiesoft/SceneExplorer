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
#include "consts.h"

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // QString title = Consts::APPNAME_DISPLAY;

    QString txtVersion = Consts::APPNAME_DISPLAY;
    txtVersion.append(" ");
    txtVersion.append("ver ");
    txtVersion.append(Consts::VERSION);
    ui->lblVersion->setText(txtVersion);

    QString txtCopyright;
    txtCopyright.append("copyright 2018 ");
    txtCopyright.append(Consts::ORGANIZATION);
    ui->lblCopyright->setText(txtCopyright);

    QString webText;

    webText += QString("<a href=\"http://ambiesoft.fam.cx/main/index.php?page=sceneexplorer\">%1</a>").
            arg(tr("webpage"));
    webText += " ";
    webText += QString("<a href=\"https://github.com/ambiesoft/SceneExplorer\">%1</a>").
            arg(tr("development"));
    ui->lblWebpage->setText(webText);
    ui->lblWebpage->setTextFormat(Qt::RichText);
    ui->lblWebpage->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->lblWebpage->setOpenExternalLinks(true);

    QFile licensefile(":/resource/license.html");
    if (licensefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->tbLicense->setHtml(QString::fromUtf8(licensefile.readAll().constData()));
        licensefile.close();
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
