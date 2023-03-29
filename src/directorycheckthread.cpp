
#include "directorycheckthread.h"

DirectoryCheckThread::DirectoryCheckThread(QString dir, QObject *parent)
    : QThread{parent}, dir_(dir)
{

}

void DirectoryCheckThread::run()
{
    const bool bResult = QDir(dir_).exists();
    emit finished_CheckDir(bResult, this);
}
