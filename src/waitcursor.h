#ifndef WAITCURSOR_H
#define WAITCURSOR_H


class WaitCursor
{
public:
    WaitCursor()
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }
    ~WaitCursor()
    {
        QApplication::restoreOverrideCursor();
    }
};

#endif // WAITCURSOR_H
