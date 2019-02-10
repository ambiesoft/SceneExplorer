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

#ifndef EXTERNALTOOLITEM_H
#define EXTERNALTOOLITEM_H

#include <QString>

#include "../../lsMisc/stdQt/settings.h"

class ExternalToolItem
{
    QString name_;
    QString exe_;
    QString arg_;
    bool countAsOpen_;
public:
    ExternalToolItem(const QString& name,
                     const QString& exe,
                     const QString& arg,
                     bool bCountAsOpen):
        name_(name),
        exe_(exe),
        arg_(arg),
        countAsOpen_(bCountAsOpen){}

    ExternalToolItem(const ExternalToolItem& that) {
        if(this==&that)
            return;
        this->name_ = that.name_;
        this->exe_ = that.exe_;
        this->arg_ = that.arg_;
        this->countAsOpen_ = that.countAsOpen_;
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

    bool IsCountAsOpen() const {
        return countAsOpen_;
    }
    void SetCountAsOpen(bool b) {
        countAsOpen_ = b;
    }
};

#endif // EXTERNALTOOLITEM_H
