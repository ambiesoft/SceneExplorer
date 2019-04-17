#ifndef HISTORYLIST_H
#define HISTORYLIST_H

#include <QObject>

class IHistoryList
{
public:
    virtual void selectItem(const QString& movie) = 0;
};

class HistoryList
{
    int current_=0;
    typedef QPair<qint64,QString> HistoryItemType;
    QList<HistoryItemType> list_;
    IHistoryList* parent_;
public:
    HistoryList(IHistoryList* pParent):parent_(pParent){}

    void OnItemChanged(const qint64& id, const QString& movie);
    void OnGoBack();
    void OnGoForward();

    int count() const {
        return list_.count();
    }
    QString getMovie(int i) const {
        if(!(0 <= i && i < list_.length()))
            return QString();

        return list_[i].second;
    }
    int current() const {
        return current_;
    }
    void doSelect(int index);
};

#endif // HISTORYLIST_H
