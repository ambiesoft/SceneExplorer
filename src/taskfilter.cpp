#include <QObject>
#include <QDirIterator>
#include <QThread>
#include <QMap>

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

    QStringList newfiles;
    QStringList renameOlds;
    QStringList renameNews;

    for(int i=0 ; i < filesin_.count(); ++i)
    {
        QString fileDisk = filesin_[i];
        QFileInfo fiDisk(pathCombine(dir, fileDisk));

        int index = entries_.indexOf(fileDisk);
		QString salient = createSalient(fiDisk.absoluteFilePath(), fiDisk.size());
        if(index >= 0)
        {
            if(sizes_[index]==fiDisk.size())
            {
                if(salients_[index]==salient)
                {
                    continue;
                }
            }
        }
		else
		{
			// database does not have name
			// Check file eixts and same salient, 
			// if salients are same make db rename
			int indexSalient = salients_.indexOf(salient);
			if (indexSalient >= 0)
			{
				// same salient exists.
				if (sizes_[indexSalient] == fiDisk.size())
				{
					// size is same
					if (!QFile(entries_[indexSalient]).exists())
					{
						// file is gone, it means db entry is obsolete.
						// rename db
                        renameOlds.append( entries_[indexSalient] );
                        renameNews.append( fileDisk );
                        continue;
					}
				}
			}
		}
        newfiles.append(fileDisk);
    }
    emit afterFilter(id_, dir, newfiles, renameOlds, renameNews);
}
