#ifndef TASKGETDIR_H
#define TASKGETDIR_H

#include <QRunnable>
#include <QObject>

class TaskGetDir : public QObject, public QRunnable
{
    Q_OBJECT
private:
    int id_;
    QString dir_;
public:
    TaskGetDir(int id, const QString& dir):id_(id), dir_(dir){}

    void run() override;

signals:
    void afterGetDir(int id, const QStringList& dirs);
};

#endif // TASKGETDIR_H
