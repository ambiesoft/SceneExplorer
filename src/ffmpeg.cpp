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

#include <QReadLocker>
#include <QReadWriteLock>

#include "consts.h"
#include "helper.h"
#include "osd.h"

#include "ffmpeg.h"

using namespace Consts;

QReadWriteLock FFMpeg::sLock_;

//QString FFMpeg::ffprobe_;
//QString FFMpeg::ffmpeg_;

QString FFMpeg::GetFFprobe(Settings& setting)
{
    QReadLocker locker(&sLock_);
    QString ffprobe = setting.valueString(KEY_FFPROBE_EXECUTABLE);
    return ffprobe.isEmpty() ? GetDefaultFFprobe() : ffprobe;
}
void FFMpeg::SetFFprobe(Settings& setting, const QString& file)
{
    QWriteLocker locker(&sLock_);
    // ffprobe_ = file;
    setting.setValue(KEY_FFPROBE_EXECUTABLE, file);
}

QString FFMpeg::GetFFmpeg(Settings& setting)
{
    QReadLocker locker(&sLock_);
    QString ffmpeg = setting.valueString(KEY_FFMPEG_EXECUTABLE);
    return ffmpeg.isEmpty() ? GetDefaultFFmpeg() : ffmpeg;
}
void FFMpeg::SetFFmpeg(Settings& setting, const QString& file)
{
    QWriteLocker locker(&sLock_);
    // ffmpeg_ = file;
    setting.setValue(KEY_FFMPEG_EXECUTABLE, file);
}
