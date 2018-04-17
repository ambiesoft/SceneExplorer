#ifndef COMMANDOPTION_H
#define COMMANDOPTION_H


class CommandOption
{
    QString dbdir_;
public:
    CommandOption(const QString& dbdir) : dbdir_(dbdir) {}

    QString DBDir() const {
        return dbdir_;
    }
};

#endif // COMMANDOPTION_H
