#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QRunnable>

class TaskFFmpeg : public QObject, public QRunnable
// class TaskFFMpeg : public QRunnable
{
    Q_OBJECT
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

public:
    TaskFFmpeg(int loopId, int id,const QString& file);
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

};

#endif // WORKER_H
