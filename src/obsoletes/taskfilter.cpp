
#include <QObject>
#include <QDirIterator>
#include <QThread>
#include <QMap>
#include <QSet>

#include "helper.h"
#include "globals.h"
#include "taskfilter.h"

void TaskFilter::run()
{
    runStuff(dir_);
    emit finished_Filter(loopId_,id_);
}
bool IsVideoExtention(const QString& file)
{
	// static const char* sexts[] = {
    static QSet<QString> sqExts = {
		"3g2",
		"3g2",
		"3gp",
		"3gp",
		"amv",
		"asf",
		"asf",
		"avi",
		"avi",
		"avs",
		"divx",
		"drc",
		"f4a",
		"f4b",
		"f4p",
		"f4v",
		"flv",
		"flv",
		"flv",
		"flv",
		"gif",
		"gifv",
		"m2v",
		"m4p",
		"m4v",
		"m4v",
		"m4v",
		"mkv",
		"mkv",
		"mng",
		"mov",
		"mov",
		"mp2",
		"mp4",
		"mp4",
		"mpe",
		"mpeg",
		"mpeg",
		"mpeg",
		"mpg",
		"mpg",
		"mpg",
		"mpv",
		"mxf",
		"nsv",
		"ogg",
		"ogm",
		"ogv",
		"qt",
		"rm",
		"rm",
		"rmvb",
		"rmvb",
		"roq",
		"svi",
		"swf",
		"vob",
		"webm",
		"wmv",
		"wmv",
		"yuv",
	};


	int li = file.lastIndexOf('.');
	if (li < 0)
        return false;

    QString ext = file.right(file.length() - li - 1);
	ext = ext.toLower();
	return sqExts.contains(ext);
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
        if(gStop)
            return;
        QString fileDisk = filesin_[i];
        if (!IsVideoExtention(fileDisk))
			continue;

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
    emit afterFilter(loopId_,id_, QDir(dir).canonicalPath(), newfiles, renameOlds, renameNews);
}
