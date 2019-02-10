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

#ifndef OPTIONEXTENSION_H
#define OPTIONEXTENSION_H

#include "../../lsMisc/stdQt/settings.h"
#include "ui_optionextension.h"

class OptionExtension : public QDialog
{
    Q_OBJECT

private:
    Ui::OptionExtension ui;
    AmbiesoftQt::IniSettings& settings_;
public:
    OptionExtension(QWidget* parent, AmbiesoftQt::IniSettings& settings);

protected:
    void showEvent(QShowEvent *) override;
    void on_radioAllow_clicked_common();
private slots:
    void on_buttonBox_accepted();
    void on_btnRestoreDefault_clicked();
    void on_radioAllow_clicked();
    void on_radioDeny_clicked();
};

#endif // OPTIONEXTENSION_H
