#ifndef IMAINWINDOW_H
#define IMAINWINDOW_H


class IMainWindow
{
public:
    virtual SORTCOLUMNMY GetCurrentSort() = 0;
    virtual QString GetTags(const qint64& id) = 0;
};

#endif // IMAINWINDOW_H
