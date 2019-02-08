//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
