#include <QMouseEvent>

#include "tagentry.h"



void TagEntry::mouseReleaseEvent(QMouseEvent *e)
{
    QPoint pos = e->pos();
    QPersistentModelIndex index = indexAt(pos);

    if(!index.isValid())
    {
        // Cancel click of white area
        e->setAccepted(true);
        return;
    }

    ParentClass::mouseReleaseEvent(e);
}
