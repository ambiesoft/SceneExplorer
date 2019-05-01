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
#include <QIcon>
#include "../../lsMisc/stdQt/settings.h"

class ExternalToolItem
{
    QString name_;
    QString exe_;
    QIcon icon_;
    QString arg_;
    bool countAsOpen_;
public:
    ExternalToolItem(const QString& name,
                     const QString& exe,
                     const QString& arg,
                     bool bCountAsOpen):
        name_(name),
        arg_(arg),
        countAsOpen_(bCountAsOpen){
        SetExe(exe);
    }

    ExternalToolItem(const ExternalToolItem& that) {
        if(this==&that)
            return;
        this->name_ = that.name_;
        this->exe_ = that.exe_;
        this->icon_ = that.icon_;
        this->arg_ = that.arg_;
        this->countAsOpen_ = that.countAsOpen_;
    }
    static ExternalToolItem Load(int i, AmbiesoftQt::IniSettings& settings);
    void Save(int i, AmbiesoftQt::IniSettings& settings);

    QString GetName() const {
        return name_;
    }
    void SetName(const QString& name) {
        name_=name;
    }

    QString GetExe() const {
        return exe_;
    }
    QIcon GetIcon() const {
        return icon_;
    }

    void SetExe(const QString& exe);

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
