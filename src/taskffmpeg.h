#ifndef WORKER_H
#define WORKER_H
#include <QRunnable>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

class TaskFFMpeg : public QObject, public QRunnable
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
    int id_;
    QString movieFile_;
    // QMutex mutex_;
    // int fileIndex_=0;
    Progress progress_;
public:
    TaskFFMpeg(int id,const QString& file);
    virtual ~TaskFFMpeg();
    void run() override ;
    bool run2();
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
    void sayHello(int id,
                   const QString& movieFile);
    void sayNo(int id,
                   const QString& movieFile);

    void sayGoodby(int id,
                   const QStringList& files,
                   int width,
                   int height,
                   const QString& movieFile,
                   const QString& format);
    void sayDead(int id);

};

#endif // WORKER_H
