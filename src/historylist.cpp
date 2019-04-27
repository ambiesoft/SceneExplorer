#include "historylist.h"

void HistoryList::OnItemChanged(const qint64& id, const QString& movie)
{
    HistoryItemType item(id,movie);
    if(list_.isEmpty())
    {
        list_.push_back(item);
        current_=0;
        parent_->updateToolButton();
    }
    else
    {
        Q_ASSERT(0 <= current_ && current_ < list_.length());

        // Remove after current
        while( (current_+1) != list_.length())
            list_.removeLast();

        do
        {
            current_ = list_.length()-1;

            // if same skip
            if(!list_.isEmpty() && list_.last()==item)
                break;

            list_.push_back(item);
            ++current_;
        } while(false);

        parent_->updateToolButton();
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
    parent_->updateToolButton();
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
    parent_->updateToolButton();
}
void HistoryList::OnGoLast()
{
    current_ = list_.length()-1;
    HistoryItemType item = list_[current_];
    parent_->selectItem(item.second);
    parent_->updateToolButton();
}


void HistoryList::doSelect(int index)
{
    if(!(0 <= index && index < list_.length()))
        return;

    current_=index;
    parent_->selectItem(list_[index].second);
    parent_->updateToolButton();
}
bool HistoryList::canGoBack() const
{
    if(list_.isEmpty())
        return false;
    if(current_ <= 0)
        return false;
    return true;
}
bool HistoryList::canGoForward() const
{
    if(list_.isEmpty())
        return false;
    if(current_ >= (list_.length()-1))
        return false;
    return true;
}
void HistoryList::Clear()
{
    list_.clear();
    current_ = -1;
    parent_->updateToolButton();
}

qint64 HistoryList::currentDbID() const
{
    Q_ASSERT(0 <= current() && current() < list_.length());

    if(list_.isEmpty())
        return -1;

    return list_[current_].first;
}
