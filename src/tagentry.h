#ifndef TAGENTRY_H
#define TAGENTRY_H

#include <QListWidget>

class TagEntry : public QListWidget
{
    using ParentClass = QListWidget;
public:
    TagEntry(QWidget *parent = Q_NULLPTR):
        QListWidget(parent)
    {}

protected:
    void mouseReleaseEvent(QMouseEvent *e) override;
};

#endif // TAGENTRY_H
