#include <QObject>
#include <QDirIterator>
#include <QThread>

#include "globals.h"
#include "taskgetdir.h"

void TaskGetDir::run()
{
    while(gPaused)
        QThread::sleep(5);

    QStringList dirs;
    QDirIterator it(dir_, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        QString file = it.next();
        if(it.fileInfo().isFile())
        {
            dirs.append(file);
        }
    }
    emit afterGetDir(id_, dirs);
}
