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

#ifndef TAGINPUTDIALOG_H
#define TAGINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class TagInputDialog;
}

class TagInputDialog : public QDialog
{
    Q_OBJECT

    QString lastTagText_;
    bool yomiChangging_=false;
    bool yomiChanged_ = false;
public:
    explicit TagInputDialog(QWidget *parent = 0);
    ~TagInputDialog();

    QString tag() const;
    void setTag(const QString& txt);

    QString yomi() const;
    void setYomi(const QString& txt);

private slots:
    void on_lineTag_textChanged(const QString &arg1);

    void on_lineYomi_textChanged(const QString &arg1);

    virtual void done(int r);
private:
    Ui::TagInputDialog *ui;
};

#endif // TAGINPUTDIALOG_H
