#ifndef WORKER_H
#define WORKER_H
#include <QRunnable>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

class TaskFFMpeg : public QObject, public QRunnable
{
    Q_OBJECT
private:
    int id_;
    QString file_;
    // QMutex mutex_;
    // int fileIndex_=0;
public:
    TaskFFMpeg(int id,const QString& file)
    {
        id_=id;
        file_=file;
    }
    void run() override ;
signals:
    void sayGoodby(int id,
                   const QStringList& files,
                   int width,
                   int height,
                   const QString& movieFile,
                   const QString& format);

};

#endif // WORKER_H
