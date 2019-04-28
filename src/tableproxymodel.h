#ifndef TABLEPROXYMODEL_H
#define TABLEPROXYMODEL_H

#include <QSortFilterProxyModel>

class FileMissingFilterProxyModel : public QSortFilterProxyModel
{

public:
    FileMissingFilterProxyModel(QWidget*parent) : QSortFilterProxyModel(parent){}

    bool filterAcceptsRow(
            int sourceRow,
            const QModelIndex &sourceParent) const;

    // void ensureBottom();
    void ensureIndex(const QModelIndex& mi);
    int GetItemCount() const {
        return rowCount()/3;
    }
    //    QString pathInfo(const QModelIndex& mi) const {
    //        int amari = mi.row() % 3;
    //        QModelIndex newI = createIndex(mi.row()-amari, mi.column());
    //        QVariant v=data(newI);
    //        if(!v.isValid())
    //            return QString;
    //        return v.toString();
    //    }
    enum class FIND_INDEX {
        FIND_INDEX_TITLE,
        FIND_INDEX_MOVIE,
        FIND_INDEX_DESCRIPTION,
    };

    QModelIndex findIndex(const QString& selPath, const FIND_INDEX& fi) const;

};

#endif // TABLEPROXYMODEL_H
