#include "historylist.h"

void HistoryList::OnItemChanged(const qint64& id, const QString& movie)
{
    HistoryItemType item(id,movie);
    if(list_.isEmpty())
    {
        list_.push_back(item);
        current_=0;
    }
    else
    {
        Q_ASSERT(0 <= current_ && current_ < list_.length());

        // Remove after current
        while( (current_+1) != list_.length())
            list_.removeLast();
        current_ = list_.length()-1;

        // if same skip
        if(!list_.isEmpty() && list_.last()==item)
            return;

        list_.push_back(item);
        ++current_;
    }
}
void HistoryList::OnGoBack()
{
    if(list_.isEmpty())
        return;

    if(current_-1 < 0)
        return;

    current_--;

    HistoryItemType item = list_[current_];
    parent_->selectItem(item.second);
}
void HistoryList::OnGoForward()
{
    if(list_.isEmpty())
        return;

    if(current_+1 >= list_.length())
        return;

    current_++;

    HistoryItemType item = list_[current_];
    parent_->selectItem(item.second);
}
void HistoryList::doSelect(int index)
{
    if(!(0 <= index && index < list_.length()))
        return;

    current_=index;
    parent_->selectItem(list_[index].second);
}
