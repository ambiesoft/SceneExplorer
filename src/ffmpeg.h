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

#ifndef FFMPEG_H
#define FFMPEG_H

#include <QReadWriteLock>

#include "../../lsMisc/stdQt/inisettings.h"
using namespace AmbiesoftQt;

class FFMpeg
{
    FFMpeg();
    static QReadWriteLock sLock_;

    //    static QString ffprobe_;
    //    static QString ffmpeg_;

public:
    //    static QString GetFFprobe() {
    //        return ffprobe_;
    //    }
    static QString GetFFprobe(IniSettings& setting);
    static void SetFFprobe(IniSettings& setting, const QString& file);

    //    static QString GetFFmpeg() {
    //        return ffmpeg_;
    //    }
    static QString GetFFmpeg(IniSettings& setting);
    static void SetFFmpeg(IniSettings& setting, const QString& file);
};

#endif // FFMPEG_H
