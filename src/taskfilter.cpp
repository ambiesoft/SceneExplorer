#include <QObject>
#include <QDirIterator>
#include <QThread>

#include "helper.h"
#include "globals.h"
#include "taskfilter.h"

void TaskFilter::run()
{
    runStuff(dir_);
}
void TaskFilter::runStuff(const QString& dir)
{
    if(gStop)
        return;
    while(gPaused)
        QThread::sleep(5);
    if(gStop)
        return;

    QStringList results;
    for(int i=0 ; i < filesin_.count(); ++i)
    {
        QString fileDisk = filesin_[i];
        QFileInfo fiDisk(pathCombine(dir, fileDisk));

        int index = entries_.indexOf(fileDisk);
        if(index >= 0)
        {
            if(sizes_[index]==fiDisk.size())
            {
                QString salient = createSalient(fiDisk.absoluteFilePath(), fiDisk.size());
                if(salients_[index]==salient)
                {
                    continue;
                }
            }
        }
        results.append(fileDisk);
    }
    emit afterFilter(id_, dir, results);
}
