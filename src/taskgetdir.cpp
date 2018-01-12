#include <QObject>
#include <QDirIterator>
#include <QThread>

#include "helper.h"
#include "globals.h"
#include "taskgetdir.h"

void TaskGetDir::run()
{
    runStuff(dir_);
    emit finished_GetDir(loopId_, id_);
}
void TaskGetDir::runStuff(const QString& dir)
{
    if(gStop)
        return;
    while(gPaused)
        QThread::sleep(5);
    if(gStop)
        return;

    {
        QStringList files;
        QDirIterator itFile(dir, QDir::NoDotAndDotDot|QDir::Files); // ,QDirIterator::Subdirectories);
        while(itFile.hasNext())
        {
            if(gStop)
                return;
            itFile.next();
            Q_ASSERT(itFile.fileInfo().isFile());
            if(IsVideoExtention(itFile.fileName()))
                files.append(itFile.fileName());
        }
        emit afterGetDir(loopId_,id_, QDir(dir).canonicalPath(), files);
    }

    {
        QDirIterator itDir(dir, QDir::NoDotAndDotDot|QDir::Dirs);
        while(itDir.hasNext())
        {
            runStuff(itDir.next());
        }
    }
}
