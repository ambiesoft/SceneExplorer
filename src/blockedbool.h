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

#ifndef BLOCKEDTRUE_H
#define BLOCKEDTRUE_H


class BlockedBool
{
    volatile bool* target_;
    bool d_;
public:
    BlockedBool(volatile bool* target, bool c, volatile bool d) :
        target_(target),
        d_(d)
    {
        *target_ = c;
    }
    BlockedBool(volatile bool* target) :
        target_(target),
        d_(false)
    {
        *target_ = true;
    }
    ~BlockedBool()
    {
        *target_ = d_;
    }
};

#endif // BLOCKEDTRUE_H
