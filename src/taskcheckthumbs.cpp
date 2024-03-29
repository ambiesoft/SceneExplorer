#include "helper.h"
#include "taskcheckthumbs.h"

void TaskCheckThumbs::run()
{
    QThread::Priority* priority = pFF2M_->GetTaskPriority();
    if(priority)
        QThread::currentThread()->setPriority(*priority);

    emit afterCheckThumbs(
                loopid_,
                taskindex_,
                getdirid_,
                dir_,
                file_,
                thumbid_,
                width_,height_,
                ThumbFilesExist(thumbid_,thumbext_,width_,height_)
                );
    emit finished_CheckThumbs(loopid_, taskindex_);
}


