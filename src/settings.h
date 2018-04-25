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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QSqlDatabase>

#include "consts.h"

class Settings
{
public:
	static const char* KEY_FAVORITE_COUNT;
	static const char* KEY_FAVORITE_NAME;
	static const char* KEY_FAVORITE_FOLDERS;
	static const char* GROUPPREFIX_FAVORITE;
private:
    QSettings s_;

//    QSqlDatabase db_;
//    QStringList allColumns_;
//    bool ok_ = false;

//    typedef QList<QPair<QString,QStringList> > FAVTYPE;
//    FAVTYPE favorites_;

    void init();
public:
    Settings() : s_(QSettings::Format::IniFormat,
                    QSettings::Scope::UserScope,
                    Consts::ORGANIZATION,
                    Consts::APPNAME)
    {
        init();
    }
    Settings(const QString& inifile) : s_(inifile, QSettings::IniFormat)
    {
        init();
    }
    ~Settings();

//    void AddToFavorites(const QString& name, const QStringList& dirs);
//    QStringList GetFavorites() const;
//    QStringList GetFavorite(const QString& name) const;
//    bool IsNameExists(const QString& name) const {
//        for(int i=0 ; i < favorites_.count(); ++i)
//        {
//            if(favorites_[i].first == name)
//                return true;
//        }
//        return false;
//    }
    QString valueString(const QString& key, const QString& defaultvalue=QString());
    int valueInt(const QString& key, int defaultvalue=0);
    bool valueBool(const QString& key, bool defaultvalue=false);
    QStringList valueStringList(const QString& key);
    QVariant value(const QString& key);

    QString fileName() const {
        return s_.fileName();
    }
    void setValue(const QString& key, const QVariant& v);

    void beginGroup(const QString& group) {
        s_.beginGroup(group);
    }
    void endGroup() {
        s_.endGroup();
    }

    void sync() {
        s_.sync();
    }

};

#endif // SETTINGS_H
