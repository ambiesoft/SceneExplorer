#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QSqlDatabase>

#include "consts.h"

class Settings : public QSettings
{
public:
	static const char* KEY_FAVORITE_COUNT;
	static const char* KEY_FAVORITE_NAME;
	static const char* KEY_FAVORITE_FOLDERS;
	static const char* GROUPPREFIX_FAVORITE;
private:
    QSqlDatabase db_;
    QStringList allColumns_;
    bool ok_ = false;

    QMap<QString, QStringList> favorites_;

    void init();
public:
    Settings() : QSettings(
                     QSettings::Format::IniFormat,
                     QSettings::Scope::UserScope,
                     Consts::ORGANIZATION,
                     Consts::APPNAME)
    {
        init();
    }
    ~Settings();

    void AddToFavorites(const QString& name, const QStringList& dirs);
    QStringList GetFavorites() const;
    QStringList GetFavorite(const QString& name) const;
    bool IsNameExists(const QString& name) const {
        return favorites_.contains(name);
    }
};

#endif // SETTINGS_H
