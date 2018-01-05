#include <QString>
#include "tasklistdata.h"


QString TaskListData::GetIdString() const
{
    return QString::number(id_);
}
QString TaskListData::GetProgressString() const
{
    switch(progress_)
    {
    case TaskFFMpeg::Progress::Uninitialized:
        return " ";
    case TaskFFMpeg::Progress::Progressing:
        return ">";
    case TaskFFMpeg::Progress::Finished:
        return "O";
    default:
        Q_ASSERT(false);
    }
    return "";
}
