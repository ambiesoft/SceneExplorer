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

#ifndef IMAINWINDOW_H
#define IMAINWINDOW_H

#include "globals.h"

class IMainWindow
{
public:
    virtual SORTCOLUMNMY GetCurrentSort() = 0;
    virtual QString GetTags(const qint64& id) = 0;
    virtual int GetThumbWidth() = 0;
    virtual int GetThumbHeight() = 0;
};

#endif // IMAINWINDOW_H
