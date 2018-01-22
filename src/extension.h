#ifndef EXTENSION_H
#define EXTENSION_H

#include <QReadWriteLock>
#include <QSet>

#include "settings.h"

class Extension
{
    static QReadWriteLock sLock_;
    static bool bOrderAllow_;

    static QSet<QString> sExtsAllowCache_;
    static QStringList sExtsAllow_;
    static bool bAllAllow_;
    static bool bNoExtAllow_;

    static QSet<QString> sExtsDenyCache_;
    static QStringList sExtsDeny_;
    static bool bAllDeny_;
    static bool bNoExtDeny_;

    Extension();

public:
    static bool IsOrderAllow();
    static void SetOrderAllow(bool b);

    static QStringList GetDefaultAllow();
    static QString GetDefaultAllowAsString();
    static void SetMovieExtensionAllow(const QStringList& exts);
    static QStringList GetMovieExtensionAllow();
    static QString GetMovieExtensionAllowAsString();

    static QStringList GetDefaultDeny();
    static QString GetDefaultDenyAsString();
    static void SetMovieExtensionDeny(const QStringList& exts);
    static QStringList GetMovieExtensionDeny();
    static QString GetMovieExtensionDenyAsString();


    static bool IsMovieExtension(const QString& file);

    static void Load(Settings& settings);
    static void Save(Settings& settings);
};

#endif // EXTENSION_H
