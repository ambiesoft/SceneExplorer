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

#ifndef DIRECTORYCHECKTHREAD_H
#define DIRECTORYCHECKTHREAD_H

#include <QThread>



class DirectoryCheckThread : public QThread
{
    Q_OBJECT
public:
    explicit DirectoryCheckThread(QString dir, QObject *parent = nullptr);
private:
    QString dir_;
public:
    void run();

Q_SIGNALS:
    void finished_CheckDir(const bool bExist, DirectoryCheckThread* pThread);
};

#endif // DIRECTORYCHECKTHREAD_H
