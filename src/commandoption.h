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

#ifndef COMMANDOPTION_H
#define COMMANDOPTION_H

#include <QString>


class CommandOption
{
    QString dbdir_;
    QString doc_;
    bool noRecent_;
public:
    CommandOption(const QString& dbdir,
                  const QString& doc,
                  bool noRecent);

    QString DBDir() const {
        return dbdir_;
    }
    QString doc() const {
        return doc_;
    }
    bool noRecent() const {
        return noRecent_;
    }
};

#endif // COMMANDOPTION_H
