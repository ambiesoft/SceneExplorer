#ifndef MYCONTEXTMENU_H
#define MYCONTEXTMENU_H

#include <QMenu>
#include "globals.h"
class MyContextMenu : public QMenu
{
    void init()
    {
        if(gpMenuFont_)
            setFont(*gpMenuFont_);
    }
public:
    MyContextMenu() {
        init();
    }

    MyContextMenu(QWidget* parent) :
        QMenu(parent) {
        init();
    }
    MyContextMenu(const QString& text, QWidget* parent) :
        QMenu(text,parent) {
        init();
    }

    void addEnablingAction(QAction* pA, bool bEnable = true) {
        pA->setEnabled(bEnable);
        addAction(pA);
    }
};

#endif // MYCONTEXTMENU_H
