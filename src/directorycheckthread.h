
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
