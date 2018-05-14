#ifndef TAGITEM_H
#define TAGITEM_H

#include <QListWidgetItem>

class TagItem : public QListWidgetItem
{
public:
    enum TagItemType
    {
        TI_NORMAL,
        TI_ALL,
        TI_MISSING,
    } ;
private:
    TagItemType itemType_;

public:
    TagItem(QListWidget* parent,
                  TagItemType itemType) :
        QListWidgetItem(parent),
        itemType_(itemType)
    {

    }
    bool IsNormalItem() const {
        return itemType_==TI_NORMAL;
    }
    bool IsAllItem() const {
        return itemType_==TI_ALL;
    }
    bool IsMissingItem() const {
        return itemType_==TI_MISSING;
    }

    bool IsChecked() const {
        return checkState()==Qt::Checked;
    }
};

#endif // TAGITEM_H
