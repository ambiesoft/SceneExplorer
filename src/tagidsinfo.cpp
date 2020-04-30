#include "tagidsinfo.h"

void TagidsInfo::constructRemoveTaggedCount() const
{
    if(bRemovedTagConstructed)
        return;
    bRemovedTagConstructed=true;

    removedTagids_.clear();
    removedTagids_ = alltaggedtagids_;
    for(auto&& id : tagids_)
        removedTagids_.removeAll(id);
}
int TagidsInfo::removedTaggedCount() const
{
    constructRemoveTaggedCount();
    return removedTagids_.count();
}
qint64 TagidsInfo::removedTag(int i) const
{
    constructRemoveTaggedCount();
    return removedTagids_[i];
}
