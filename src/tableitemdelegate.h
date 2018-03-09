#ifndef TABLEITEMDELEGATE_H
#define TABLEITEMDELEGATE_H

#include <QStyledItemDelegate>

class TableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent){}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        QStyledItemDelegate::paint(painter,option,index);
    }
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        QSize baseSize = this->QStyledItemDelegate::sizeHint(option, index);
        baseSize.setHeight(10000);//something very high, or the maximum height of your text block

        QFontMetrics metrics(option.font);
        QRect outRect = metrics.boundingRect(QRect(QPoint(0, 0), baseSize), Qt::AlignLeft, option.text);
        baseSize.setHeight(outRect.height());
        return baseSize;
    }
protected:
    virtual void initStyleOption(QStyleOptionViewItem *option,
                                const QModelIndex &index) const
    {
        QStyledItemDelegate::initStyleOption(option,index);
    }

};

#endif // TABLEITEMDELEGATE_H
