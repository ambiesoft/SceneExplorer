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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <memory>
#include <QFont>

#include "commandoption.h"

extern volatile bool gPaused;
extern volatile bool gStop;
class Sql;
extern Sql* gpSQL;
extern volatile int gLoopId;
extern volatile bool gReboot;
class CommandOption;
extern std::unique_ptr<CommandOption> gpCommandOption;
#if !defined(VERIFY)
# if !defined(QT_NO_DEBUG)
#  define VERIFY Q_ASSERT
# else
#  define VERIFY(expr)          \
    do                          \
    {                           \
        (void) (expr);          \
    } while (0)
# endif
#endif

enum ImageCacheType {
    IC_NEVER,
    IC_PERDIRECTORY,
    IC_ALWAYS,
};


enum SORTCOLUMNMY{
    SORT_NONE,
    SORT_FILENAME,
    SORT_FULLNAME,
    SORT_SIZE,
    SORT_WTIME,
    SORT_RESOLUTION,
    SORT_DURATION,
    SORT_BITRATE,
    SORT_OPENCOUNT,
    SORT_LASTACCESS,
    SORT_FPS,
    SORT_FORMAT,
    SORT_VCODEC,
    SORT_ACODEC,

    COUNT_SORTCOLUMN,
};

extern std::unique_ptr<QFont> gpMenuFont_;

extern std::unique_ptr<CommandOption> gpCommandOption;

#define STR_DIRECTORY_ENTRY_SIGNATURE "# SceneExplorer Directory Entries 2AA6B5CF-C02C-4E1C-B996-CE89F37BEB90"
#define STR_TAG_ENTRY_SIGNATURE "# SceneExplorer Tag Entries D462D547-C22A-4D03-B0FC-87513E0EC9EF"

/**
#define DISABLE_FOR_VIRTUALCLASS(clazz)              \
    clazz() = delete;                                \
    clazz(const clazz&) = delete;                    \
    ~clazz() = delete;                               \
    clazz& operator=(const clazz&) = delete;         \
**/


#endif // GLOBALS_H
