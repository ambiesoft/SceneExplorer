#ifndef ERRORINFOEXCEPTION_H
#define ERRORINFOEXCEPTION_H

#include <QException>

class ErrorInfoException : public QException
{
    QString error_;
public:
    ErrorInfoException(const QString& error) :
        error_(error){}

    QString getErrorInfo() const {
        return error_;
    }
};

#endif // ERRORINFOEXCEPTION_H
