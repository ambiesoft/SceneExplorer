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
    txtCopyright.append(QString("<a href=\"http://ambiesoft.fam.cx/main/index.php?page=sceneexplorer\">%1</a> ").
                        arg(Consts::ORGANIZATION));
    txtCopyright.append(QString("(<a href=\"https://github.com/ambiesoft/SceneExplorer\">%1</a>)").
                        arg("github"));

    ui->lblCopyright->setText(txtCopyright);
    ui->lblCopyright->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->lblCopyright->setOpenExternalLinks(true);


//    QString webText;
//    webText += QString("<a href=\"http://ambiesoft.fam.cx/main/index.php?page=sceneexplorer\">%1</a>").
//            arg(tr("webpage"));
//    webText += QString("<a href=\"https://github.com/ambiesoft/SceneExplorer\">%1</a>").
//            arg(tr("development"));
//    ui->lblWebpage->setText(webText);
//    ui->lblWebpage->setTextFormat(Qt::RichText);
//    ui->lblWebpage->setTextInteractionFlags(Qt::TextBrowserInteraction);
//    ui->lblWebpage->setOpenExternalLinks(true);

    QString aboutText;
    aboutText += "<h1>Qt</h1>";
    aboutText += "<p>This software uses Qt.</p>";
    aboutText += "<p>Source code: https://github.com/ambiesoft/qt5/tree/5.10";

    aboutText += "<h1>FFmpeg</h1>";
    aboutText += "<p>This software uses code of <a href=http://ffmpeg.org>FFmpeg</a> licensed under the "
            "<a href=https://www.gnu.org/licenses/gpl-3.0.html>GPLv3</a> and its "
            "source can be downloaded <a href=https://github.com/ambiesoft/SceneExplorer>here</a></p>";
    aboutText += "<p>Source code: https://github.com/FFmpeg/FFmpeg/tree/40102a21374096ce0ba05c67c6e7474f176af2d0</p>";
    ui->tbAbout->setHtml(aboutText);


    QFile licensefile(":/resource/license.html");
    if (licensefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->tbLicense->setHtml(QString::fromUtf8(licensefile.readAll().constData()));
        licensefile.close();
    }

    QString qtText;
    qtText += "Qt: ";
    qtText += QT_VERSION_STR;
    ui->tbOtherVersion->setText(qtText);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
