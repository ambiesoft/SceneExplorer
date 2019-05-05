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

#ifndef TASKGETDIR_H
#define TASKGETDIR_H

#include <QObject>
#include <QRunnable>
#include <QThread>


class TaskGetDir : public QObject, public QRunnable
{
    Q_OBJECT
private:
    int loopId_;
    int id_;

    QString dir_;
    QThread::Priority* priority_ = nullptr;
    void runStuff(const QString& dir);

    TaskGetDir(QObject* =nullptr) {
        Q_ASSERT(false);
    }
public:
    static void RegisterMetaType();

    TaskGetDir(int loopId,
               int id,
               const QString& dir,
               QThread::Priority* priority);
    virtual ~TaskGetDir() override;

    void run() override;

Q_SIGNALS:
    void afterGetDir(int loopId, int id,
                     const QString& dir,
                     const QStringList& files,

                     const QList<qint64> sizes,
                     const QList<qint64> wtimes,

                     const QStringList& salients);
    void finished_GetDir(int loopId,int id,const QString& dir);
};

#endif // TASKGETDIR_H
