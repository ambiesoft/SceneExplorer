#ifndef COMMANDOPTION_H
#define COMMANDOPTION_H

#include <QString>


class CommandOption
{
    QString dbdir_;
    QString doc_;
public:
    CommandOption(const QString& dbdir,
                  const QString& doc);

    QString DBDir() const {
        return dbdir_;
    }
    QString doc() const {
        return doc_;
    }
};

#endif // COMMANDOPTION_H
