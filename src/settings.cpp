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

#include <QApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QMap>
#include <QList>

#include "helper.h"
#include "settings.h"


void Settings::init()
{

}
Settings::~Settings()
{
}
int Settings::valueInt(const QString& key, int defaultvalue)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return defaultvalue;

    return v.toInt();
}
bool Settings::valueBool(const QString& key, bool defaultvalue)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return defaultvalue;

    return v.toBool();
}
QString Settings::valueString(const QString& key, const QString& defaultvalue)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return defaultvalue;
    return v.toString();
}
QStringList Settings::valueStringList(const QString& key)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return QStringList();
    return v.toStringList();
}
QVariant Settings::value(const QString& key)
{
    return s_.value(key);
}
void Settings::setValue(const QString& key, const QVariant& v)
{
    s_.setValue(key,v);
}
