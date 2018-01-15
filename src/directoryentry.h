#ifndef DIRECTORYENTRY_H
#define DIRECTORYENTRY_H

#include <QListWidget>

class DirectoryEntry : public QListWidget
{
public:
    DirectoryEntry(QWidget *parent = Q_NULLPTR):
        QListWidget(parent)
    {}

};

#endif // DIRECTORYENTRY_H
