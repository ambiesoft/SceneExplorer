#ifndef FINDCOMBOBOX_H
#define FINDCOMBOBOX_H

#include <QComboBox>
#include <QKeyEvent>
class FindComboBox : public QComboBox
{
    Q_OBJECT
public:
    FindComboBox(QWidget* parent) : QComboBox(parent){}

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE
    {
        // qDebug() << e->key();
        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
            emit on_EnterPressed();

        QComboBox::keyPressEvent(e);
    }

signals:
    void on_EnterPressed();
};

#endif // FINDCOMBOBOX_H
