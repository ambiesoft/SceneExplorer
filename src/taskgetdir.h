#ifndef TASKGETDIR_H
#define TASKGETDIR_H

#include <QObject>
#include <QRunnable>

class TaskGetDir : public QObject, public QRunnable
{
    Q_OBJECT
private:
    int loopId_;
    int id_;

    QString dir_;

    void runStuff(const QString& dir);

public:
    static void RegisterMetaType();

    TaskGetDir(int loopId, int id, const QString& dir):
        loopId_(loopId),
        id_(id),
        dir_(dir){}

    void run() override;

signals:
    void afterGetDir(int loopId, int id,
                     const QString& dir,
                     const QStringList& files,

                     const QList<qint64> sizes,
                     const QList<qint64> wtimes,

                     const QStringList& salients);
    void finished_GetDir(int loopId,int id,const QString& dir);
};

#endif // TASKGETDIR_H
