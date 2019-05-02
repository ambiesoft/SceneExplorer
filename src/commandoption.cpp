#include <QFileInfo>

#include "commandoption.h"


CommandOption::CommandOption(const QString& dbdir,
                             const QString& doc,
                             bool noRecent)
{
    if(!dbdir.isEmpty())
    {
        dbdir_ = QFileInfo(dbdir).absoluteFilePath();
    }
    if(!doc.isEmpty())
    {
        doc_ = QFileInfo(doc).absoluteFilePath();
    }
    noRecent_ = noRecent;
}
