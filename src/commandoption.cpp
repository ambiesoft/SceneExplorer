#include <QFileInfo>

#include "commandoption.h"


CommandOption::CommandOption(const QString& dbdir,
              const QString& doc)
{
    if(!dbdir.isEmpty())
    {
        dbdir_ = QFileInfo(dbdir).absoluteFilePath();
    }
    if(!doc.isEmpty())
    {
        doc_ = QFileInfo(doc).absoluteFilePath();
    }
}
