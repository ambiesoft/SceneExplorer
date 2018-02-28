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


extern volatile bool gPaused;
extern volatile bool gStop;
class Sql;
extern Sql* gpSQL;
extern volatile int gLoopId;
extern volatile bool gReboot;

#if !defined(VERIFY)
# if !defined(QT_NO_DEBUG)
#  define VERIFY Q_ASSERT
# else
#  define VERIFY(expr)  \
    do                    \
    {                     \
        (void) (expr);    \
    } while (0)
# endif
#endif

enum ImageCacheType {
    IC_NEVER,
    IC_PERFOLDER,
    IC_ALWAYS,
};


enum SORTCOLUMN{
    SORT_NONE,
    SORT_FILENAME,
    SORT_SIZE,
    SORT_WTIME,
    SORT_RESOLUTION,
    SORT_DURATION,
    SORT_BITRATE,
    SORT_OPENCOUNT,

	COUNT_SORTCOLUMN,
} ;


#endif // GLOBALS_H
