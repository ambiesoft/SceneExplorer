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

#ifndef DOCINFODIALOG_H
#define DOCINFODIALOG_H

#include <ui_docinfodialog.h>

//namespace Ui {
//class DocinfoDialog;
//}

class DocinfoDialog : public QDialog
{
    Q_OBJECT

    Ui::DocinfoDialog *ui;

    QString exe_;
    QString doc_;
    QString ini_;
    QString db_;
public:
    explicit DocinfoDialog(QWidget *parent,
                           const QString& exe,
                           const QString& doc,
                           const QString& ini,
                           const QString& db
                           );
    ~DocinfoDialog();



};

#endif // DOCINFODIALOG_H
