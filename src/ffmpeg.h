#ifndef FFMPEG_H
#define FFMPEG_H

#include <QReadWriteLock>

#include "settings.h"

class FFMpeg
{
    FFMpeg();
    static QReadWriteLock sLock_;

    static QString ffprobe_;
    static QString ffmpeg_;

public:
    static QString GetFFprobe() {
        return ffprobe_;
    }
    static QString GetFFprobe(Settings& setting);
    static void SetFFprobe(Settings& setting, const QString& file);

    static QString GetFFmpeg() {
        return ffmpeg_;
    }
    static QString GetFFmpeg(Settings& setting);
    static void SetFFmpeg(Settings& setting, const QString& file);
};

#endif // FFMPEG_H
