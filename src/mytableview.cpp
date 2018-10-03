#include "mytableview.h"

// https://stackoverflow.com/a/52185629/1368901
MyTableView::MyTableView(QWidget *parent)
    : super(parent)
    , m_dragState(DragStopped)
    , m_dragStartPos(-1)
{
}

void MyTableView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        m_dragState = DragStarted;
        m_dragStartPos = event->pos().y();
    } else
        super::mousePressEvent(event);
}

void MyTableView::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_dragState) {
        m_dragState = DragStopped;
        m_dragStartPos = -1;
        return;
    }
    super::mouseReleaseEvent(event);
}

void MyTableView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_dragState != DragStopped) {
        const int itemSize = sizeHintForRow(0) / 2;
        const int distance = qAbs(m_dragStartPos - event->pos().y());
        if(distance > 10)
            m_dragState = Dragged;
        if(distance > itemSize) {
            QScrollBar *scrollBar = this->verticalScrollBar();
            int stepCount = (distance/itemSize);
            if(m_dragStartPos < event->pos().y())
                stepCount = -stepCount; //scrolling up
            scrollBar->setValue(scrollBar->value() + (stepCount * scrollBar->singleStep()));
            m_dragStartPos = event->y();
        }
        return;
    }
    super::mouseMoveEvent(event);
}
