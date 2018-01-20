#ifndef EXEFFMPEG_H
#define EXEFFMPEG_H

#include <QString>

class ExeFFmpeg
{
    QString ffmpeg_;
    QString ffprobe_;
    // ExeFFmpeg();

public:
    QString GetFFmepg() const {
        return ffmpeg_;
    }
    void SetFFmpeg(const QString& exe) {
        ffmpeg_=exe;
    }

    QString GetFFprobe() const {
        return ffprobe_;
    }
    void SetFFprobe(const QString& exe) {
        ffprobe_=exe;
    }
};

extern ExeFFmpeg theFFmepg;
#endif // EXEFFMPEG_H
