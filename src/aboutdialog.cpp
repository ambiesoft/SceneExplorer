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

#include <QFile>
#include <QSysInfo>

#include "helper.h"
#include "consts.h"

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

using namespace Consts;

namespace {
    int gCurrentIndex;
}
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent,GetDefaultDialogFlags()),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(gCurrentIndex);

    QString txtVersion = qAppName();
    txtVersion.append(" ");
    txtVersion.append("ver ");
    txtVersion.append(APPVERSION);
    ui->lblVersion->setText(txtVersion);

    QString txtCopyright;
    txtCopyright.append("copyright 2018 ");
    txtCopyright.append(QString("<a href=\"http://ambiesoft.fam.cx/main/index.php?page=sceneexplorer\">%1</a> ").
                        arg(ORGANIZATION));
    txtCopyright.append(QString("(<a href=\"https://github.com/ambiesoft/SceneExplorer\">%1</a>)").
                        arg("github"));

    ui->lblCopyright->setText(txtCopyright);
    ui->lblCopyright->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->lblCopyright->setOpenExternalLinks(true);



    QString aboutText;
    aboutText += QString() + "<h1>" + qAppName() + "</h1>";
    aboutText +=
            "Copyright (C) 2018 Ambiesoft<br />"
            "<br />"
            "This program is free software: you can redistribute it and/or modify<br />"
            "it under the terms of the GNU General Public License as published by<br />"
            "the Free Software Foundation, either version 3 of the License, or<br />"
            "(at your option) any later version.<br />"
            "<br />"
            "This program is distributed in the hope that it will be useful,<br />"
            "but WITHOUT ANY WARRANTY; without even the implied warranty of<br />"
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br />"
            "GNU General Public License for more details.<br />"
            "<br />"
            "You should have received a copy of the GNU General Public License<br />"
            "along with this program.  If not, see &lt;https://www.gnu.org/licenses/&gt;.<br />";

    aboutText += "<h1>Qt</h1>";
    aboutText += "<p>This software uses Qt.<br />";
    aboutText += "Source code: https://github.com/ambiesoft/qt5/tree/5.10</p>";

    aboutText += "<h1>FFmpeg</h1>";
    aboutText += "<p>This software uses code of <a href=http://ffmpeg.org>FFmpeg</a> licensed under the "
                 "<a href=https://www.gnu.org/licenses/gpl-3.0.html>GPLv3</a> and its "
                 "source can be downloaded <a href=https://github.com/ambiesoft/SceneExplorer>here</a><br />";
    aboutText += "Source code: https://github.com/FFmpeg/FFmpeg/tree/40102a21374096ce0ba05c67c6e7474f176af2d0</p>";

    aboutText += "<h1>Icons</h1>";
    aboutText += "<div>Icons made by <a href=\"http://www.freepik.com\" title=\"Freepik\">Freepik</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\">www.flaticon.com</a> is licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative Commons BY 3.0\" target=\"_blank\">CC 3.0 BY</a></div>";
    aboutText += "<div>Icons made by <a href=\"https://www.flaticon.com/authors/kiranshastry\" title=\"Kiranshastry\">Kiranshastry</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\">www.flaticon.com</a> is licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative Commons BY 3.0\" target=\"_blank\">CC 3.0 BY</a></div>";
    aboutText += "<div>Icons made by <a href=\"https://www.flaticon.com/authors/gregor-cresnar\" title=\"Gregor Cresnar\">Gregor Cresnar</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\">www.flaticon.com</a> is licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative Commons BY 3.0\" target=\"_blank\">CC 3.0 BY</a></div>";
    aboutText += "<div>Icons made by <a href=\"https://www.flaticon.com/authors/dave-gandy\" title=\"Dave Gandy\">Dave Gandy</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\">www.flaticon.com</a> is licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative Commons BY 3.0\" target=\"_blank\">CC 3.0 BY</a></div>";
    aboutText += "<div>Icons made by <a href=\"https://www.flaticon.com/authors/smashicons\" title=\"Smashicons\">Smashicons</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\">www.flaticon.com</a> is licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative Commons BY 3.0\" target=\"_blank\">CC 3.0 BY</a></div>";
    aboutText += "<div>Icons made by <a href=\"https://www.flaticon.com/authors/pixelmeetup\" title=\"Pixelmeetup\">Pixelmeetup</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\">www.flaticon.com</a> is licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative Commons BY 3.0\" target=\"_blank\">CC 3.0 BY</a></div>";
    ui->tbAbout->setHtml(aboutText);



    QFile licensefile(":/resource/license/GPLv3.html");
    if (licensefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->tbLicense->setHtml(QString::fromUtf8(licensefile.readAll().constData()));
        licensefile.close();
    }
    else
    {
        ui->tbLicense->setText(tr("<Resource not found>"));
    }


    QString txtPlatform;
    txtPlatform += "ByteOrder:\n";
    switch(QSysInfo::ByteOrder)
    {
    case QSysInfo::LittleEndian:
        txtPlatform += tr("LittleEndian");
        break;
    case QSysInfo::BigEndian:
        txtPlatform += tr("BigEndian");
        break;
    }
    txtPlatform += "\n\n";

    txtPlatform += tr("WordSize") + ":\n";
    txtPlatform += QString::number(QSysInfo::WordSize);
    txtPlatform += "\n\n";

    txtPlatform += "buildAbi:\n";
    txtPlatform += QSysInfo::buildAbi();
    txtPlatform += "\n\n";

    txtPlatform += "buildCpuArchitecture:\n";
    txtPlatform += QSysInfo::buildCpuArchitecture();
    txtPlatform += "\n\n";

    txtPlatform += "currentCpuArchitecture:\n";
    txtPlatform += QSysInfo::currentCpuArchitecture();
    txtPlatform += "\n\n";

    txtPlatform += "kernelType:\n";
    txtPlatform += QSysInfo::kernelType();
    txtPlatform += "\n\n";

    txtPlatform += "kernelVersion:\n";
    txtPlatform += QSysInfo::kernelVersion();
    txtPlatform += "\n\n";



    ui->tbPlatform->setText(txtPlatform);


    QString qtText;
    qtText += "Qt: ";
    qtText += QT_VERSION_STR;
    ui->tbOtherVersion->setText(qtText);

    ui->tbDonate->setOpenExternalLinks(true);
    ui->tbDonate->setHtml(tr("<p>Support our software by making <a href=\"http://ambiesoft.fam.cx/donate/\">donation</a>.</p>"));
}

AboutDialog::~AboutDialog()
{
    gCurrentIndex = ui->tabWidget->currentIndex();
    delete ui;
}
