#ifndef MYDIRMODEL_H
#define MYDIRMODEL_H

#include <QDirModel>
#include <QObject>

class MyDirModel : public QDirModel
{
public:
    MyDirModel(QObject* parent) : QDirModel(parent){}
};

#endif // MYDIRMODEL_H
