#include <QObject>
#include <QDirIterator>

#include "taskgetdir.h"

void TaskGetDir::run()
{
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
