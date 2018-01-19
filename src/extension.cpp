

#include "extension.h"

QReadWriteLock Extension::sLock_;
QSet<QString> Extension::sExtsCache_;
QStringList Extension::sExts_;

QStringList Extension::GetDefault()
{
    static QStringList sqExts = {
        ".3g2",
        ".3gp",
        ".amv",
        ".asf",
        ".avi",
        ".avs",
        ".divx",
        ".drc",
        ".f4a",
        ".f4b",
        ".f4p",
        ".f4v",
        ".flv",
        ".m2v",
        ".m4p",
        ".m4v",
        ".mkv",
        ".mng",
        ".mov",
        ".mp2",
        ".mp4",
        ".mpe",
        ".mpeg",
        ".mpg",
        ".mpv",
        ".mxf",
        ".nsv",
        ".ogg",
        ".ogm",
        ".ogv",
        ".qt",
        ".rm",
        ".rmvb",
        ".roq",
        ".svi",
        ".swf",
        ".vob",
        ".webm",
        ".wmv",
        ".yuv",
    };

    return sqExts;
}
QStringList Extension::GetMovieExtension()
{
    QReadLocker locker(&sLock_);
    return sExts_;
}
void Extension::SetMovieExtension(const QStringList& exts)
{
    QWriteLocker locker(&sLock_);

    sExts_ = exts;
    foreach(const QString& ext, exts)
    {
        sExtsCache_.insert(ext);
    }
}
bool Extension::IsMovieExtension(const QString& file)
{
    QReadLocker locker(&sLock_);

    int li = file.lastIndexOf('.');
    if (li < 0)
        return false;

    QString ext = file.right(file.length() - li);
    ext = ext.toLower();
    return sExtsCache_.contains(ext);
}
