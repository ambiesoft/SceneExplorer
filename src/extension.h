#ifndef EXTENSION_H
#define EXTENSION_H

#include <QReadWriteLock>
#include <QSet>

class Extension
{
    static QReadWriteLock sLock_;
    static QSet<QString> sExtsCache_;
    static QStringList sExts_;

    Extension();

public:
    static QStringList GetDefault();
    static void SetMovieExtension(const QStringList& exts);
    static QStringList GetMovieExtension();
    static bool IsMovieExtension(const QString& file);
};

#endif // EXTENSION_H
