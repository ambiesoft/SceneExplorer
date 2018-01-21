#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QSqlDatabase>

#include "consts.h"

class Settings
{
public:
	static const char* KEY_FAVORITE_COUNT;
	static const char* KEY_FAVORITE_NAME;
	static const char* KEY_FAVORITE_FOLDERS;
	static const char* GROUPPREFIX_FAVORITE;
private:
    QSettings s_;

//    QSqlDatabase db_;
//    QStringList allColumns_;
//    bool ok_ = false;
    typedef QList<QPair<QString,QStringList> > FAVTYPE;
    FAVTYPE favorites_;

    void init();
public:
    Settings() : s_(
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
        for(int i=0 ; i < favorites_.count(); ++i)
        {
            if(favorites_[i].first == name)
                return true;
        }
        return false;
    }
    QString valueString(const QString& key, const QString& defaultvalue=QString());
    int valueInt(const QString& key, int defaultvalue=0);
    QVariant value(const QString& key);

    QString fileName() const {
        return s_.fileName();
    }
    void setValue(const QString& key, const QVariant& v);

    void beginGroup(const QString& group) {
        s_.beginGroup(group);
    }
    void endGroup() {
        s_.endGroup();
    }

};

#endif // SETTINGS_H
