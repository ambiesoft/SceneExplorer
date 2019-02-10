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

#ifndef EXTENSION_H
#define EXTENSION_H

#include <QReadWriteLock>
#include <QSet>

#include "../../lsMisc/stdQt/settings.h"

class Extension
{
    static QReadWriteLock sLock_;
    static bool bOrderAllow_;

    static QSet<QString> sExtsAllowCache_;
    static QStringList sExtsAllow_;
    static bool bAllAllow_;
    static bool bNoExtAllow_;

    static QSet<QString> sExtsDenyCache_;
    static QStringList sExtsDeny_;
    static bool bAllDeny_;
    static bool bNoExtDeny_;

    Extension();

public:
    static bool IsOrderAllow();
    static void SetOrderAllow(bool b);

    static QStringList GetDefaultAllow();
    static QString GetDefaultAllowAsString();
    static void SetMovieExtensionAllow(const QStringList& exts);
    static QStringList GetMovieExtensionAllow();
    static QString GetMovieExtensionAllowAsString();

    static QStringList GetDefaultDeny();
    static QString GetDefaultDenyAsString();
    static void SetMovieExtensionDeny(const QStringList& exts);
    static QStringList GetMovieExtensionDeny();
    static QString GetMovieExtensionDenyAsString();


    static bool IsMovieExtension(const QString& file);

    static void Load(AmbiesoftQt::IniSettings& settings);
    static void Save(AmbiesoftQt::IniSettings& settings);
};

#endif // EXTENSION_H
