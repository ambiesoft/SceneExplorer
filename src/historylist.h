#ifndef HISTORYLIST_H
#define HISTORYLIST_H

#include <QObject>


class IHistoryList
{
public:
    virtual void selectItem(const QString& movie) = 0;
    virtual void updateToolButton() = 0;
};

class HistoryList
{
    int current_ = -1;
    typedef QPair<qint64,QString> HistoryItemType;
    QList<HistoryItemType> list_;
    IHistoryList* parent_;
public:
    HistoryList(IHistoryList* pParent):parent_(pParent){}

    void OnItemChanged(const qint64& id, const QString& movie);
    void OnGoBack();
    void OnGoForward();
    void OnGoLast();

    int count() const {
        return list_.count();
    }
    qint64 currentDbID() const;
    QString getMovie(int i) const {
        if(!(0 <= i && i < list_.length()))
            return QString();

        return list_[i].second;
    }
    qint64 getDbID(int i) const {
        if(!(0 <= i && i < list_.length()))
            return -1;

        return list_[i].first;
    }
    int current() const {
        return current_;
    }
    void doSelect(int index);

    bool canGoBack() const;
    bool canGoForward() const;

    void Clear();
};

#endif // HISTORYLIST_H
