#ifndef DIRECTORYENTRY_H
#define DIRECTORYENTRY_H

#include <QListWidget>

#include "directoryitem.h"

class DirectoryEntry : public QListWidget
{
public:
    DirectoryEntry(QWidget *parent = Q_NULLPTR):
        QListWidget(parent)
    {}
	DirectoryItem* getShowAllItem();
	DirectoryItem* takeShowAllItem();
	DirectoryItem* takeMissingItem();
	
	void addItem(DirectoryItem* item);


    QList<DirectoryItem*> GetCheckedItems();
	QList<DirectoryItem*> GetAllNormalItems();
    void SetCheck(const QStringList& dirs, bool bRemoveSelection);

    bool IsAllItemSelectedOrChecked() const;
    bool IsMissingItemSelectedOrChecked() const;
};

#endif // DIRECTORYENTRY_H
