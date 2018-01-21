#ifndef EXTERNALTOOLITEM_H
#define EXTERNALTOOLITEM_H

#include <QString>
#include "settings.h"
class ExternalToolItem
{
    QString name_;
    QString exe_;
    QString arg_;
public:
    ExternalToolItem(const QString& name,
                     const QString& exe,
                     const QString& arg):
        name_(name),
        exe_(exe),
        arg_(arg) {}

    ExternalToolItem(const ExternalToolItem& that) {
        if(this==&that)
            return;
        this->name_ = that.name_;
        this->exe_ = that.exe_;
        this->arg_ = that.arg_;
    }
    static ExternalToolItem Load(int i, Settings& settings);
    void Save(int i, Settings& settings);

    QString GetName() const {
        return name_;
    }
    void SetName(const QString& name) {
        name_=name;
    }

    QString GetExe() const {
        return exe_;
    }
    void SetExe(const QString& exe) {
        exe_ = exe;
    }

    QString GetArg() const {
        return arg_;
    }
    void SetArg(const QString& arg) {
        arg_=arg;
    }
};

#endif // EXTERNALTOOLITEM_H
