#ifndef IFFTASK2MAIN_H
#define IFFTASK2MAIN_H

#include <QThread>

class IFFTask2Main
{
public:
    virtual QThread::Priority* GetTaskPriority() const = 0;
};



#endif // IFFTASK2MAIN_H
