#ifndef DIRECTORYITEM_H
#define DIRECTORYITEM_H

#include <QListWidgetItem>

class DirectoryItem : public QListWidgetItem
{
public:
    enum DirectoryItemType
    {
        DI_NORMAL,
        DI_ALL,
        DI_MISSING,
    } ;
private:
    DirectoryItemType itemType_;

public:
    DirectoryItem(QListWidget* parent,
                  DirectoryItemType itemType) :
        QListWidgetItem(parent),
        itemType_(itemType)
    {

    }
    bool IsNormalItem() const {
        return itemType_==DI_NORMAL;
    }
    bool IsAllItem() const {
        return itemType_==DI_ALL;
    }
    bool IsMissingItem() const {
        return itemType_==DI_MISSING;
    }

    bool IsChecked() const {
        return checkState()==Qt::Checked;
    }
	
};

#endif // DIRECTORYITEM_H
