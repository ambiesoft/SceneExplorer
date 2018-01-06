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
    case TaskFFmpeg::Progress::Uninitialized:
        return " ";
    case TaskFFmpeg::Progress::Progressing:
        return ">";
    case TaskFFmpeg::Progress::Finished:
        return "O";
    default:
        Q_ASSERT(false);
    }
    return "";
}
