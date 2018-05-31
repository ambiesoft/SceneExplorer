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

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QProcess>

class TaskFFmpeg : public QObject, public QRunnable
// class TaskFFMpeg : public QRunnable
{
    Q_OBJECT

    static int waitMax_;

	QThread::Priority* priority_ = nullptr;

public:
    enum Progress
    {
        Uninitialized,
        Progressing,
        Finished,
    };

private:
    int loopId_;
    int id_;
    QString ffprobe_;
    QString ffmpeg_;

    QString movieFile_;
    // QMutex mutex_;
    // int fileIndex_=0;
    Progress progress_;

    bool getProbe(const QString& file,
                  double& outDuration,
                  QString& outFormat,
                  int& outBitrate,

                  QString& outVideoCodec,
                  QString& outAudioCodec,

                  int& outVWidth,
                  int& outVHeight,

                  QString& errorReason);
    void run2();
    bool run3(QString& errorReason);

    void setPriority(QProcess& process);
public:
    TaskFFmpeg(const QString& ffprobe,
               const QString& ffmpeg,
               int loopId,
               int id,
               const QString& file,
			QThread::Priority* priority	);
    virtual ~TaskFFmpeg();
    void run() override ;
    int GetId() const
    {
        return id_;
    }
    QString GetMovieFile() const {
        return movieFile_;
    }


signals:
//    void sayBorn(int id,
//                 const QString& movieFile);
    void sayHello(int loopId, int id,
                   const QString& movieFile);
    void sayNo(int loopId, int id,
                   const QString& movieFile,
                   const QString& errorReason);

    void sayGoodby(int loopId, int id,
                   const QStringList& files,
                   const QString& movieFile,
                   int thumbwidth,
                   int thumbheight,
                   const double& duration,
                   const QString& format,
                   int bitrate,
                   const QString& vcodec,
                   const QString& acodec,
                   int vWidth,int vHeight);
    void sayDead(int loopId, int id);
    void finished_FFMpeg(int loopId, int id);
    void warning_FFMpeg(int loopId, int id,
                        const QString& warning);

};

#endif // WORKER_H
