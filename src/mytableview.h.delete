#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>
#include <QtWidgets>
#include <QMouseEvent>

// https://stackoverflow.com/a/52185629/1368901
class MyTableView : public QTableView
{
public:
    typedef QTableView super;
    explicit MyTableView(QWidget *parent = 0);

protected:
    // QWidget interface
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    enum DragState {
        DragStopped,
        DragStarted,
        Dragged
    };
    quint8 m_dragState;
    int m_dragStartPos;
};

#endif // MYTABLEVIEW_H
