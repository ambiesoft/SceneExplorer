#include <QReadLocker>
#include <QReadWriteLock>

#include "consts.h"
#include "helper.h"
#include "ffmpeg.h"

QReadWriteLock FFMpeg::sLock_;

QString FFMpeg::ffprobe_;
QString FFMpeg::ffmpeg_;

QString FFMpeg::GetFFprobe(Settings& setting)
{
    QReadLocker locker(&sLock_);
    ffprobe_ = setting.valueString(Consts::KEY_FFPROBE_EXECUTABLE, GetDefaultFFprobe());
    return ffprobe_;
}
void FFMpeg::SetFFprobe(Settings& setting, const QString& file)
{
    QWriteLocker locker(&sLock_);
    ffprobe_ = file;
    setting.setValue(Consts::KEY_FFPROBE_EXECUTABLE, file);
}

QString FFMpeg::GetFFmpeg(Settings& setting)
{
    QReadLocker locker(&sLock_);
    ffmpeg_ = setting.valueString(Consts::KEY_FFMPEG_EXECUTABLE, GetDefaultFFmpeg());
    return ffmpeg_;
}
void FFMpeg::SetFFmpeg(Settings& setting, const QString& file)
{
    QWriteLocker locker(&sLock_);
    ffmpeg_ = file;
    setting.setValue(Consts::KEY_FFMPEG_EXECUTABLE, file);
}
