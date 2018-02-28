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

#ifndef FINDCOMBOBOX_H
#define FINDCOMBOBOX_H

#include <QComboBox>
#include <QKeyEvent>
class FindComboBox : public QComboBox
{
    Q_OBJECT
public:
    FindComboBox(QWidget* parent) : QComboBox(parent){}

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE
    {
        // qDebug() << e->key();
        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
            emit on_EnterPressed();

        QComboBox::keyPressEvent(e);
    }

signals:
    void on_EnterPressed();
};

#endif // FINDCOMBOBOX_H
