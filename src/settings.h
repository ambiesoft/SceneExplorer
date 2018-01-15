#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "consts.h"
class Settings : public QSettings
{
public:
    Settings() : QSettings(
                     QSettings::Format::IniFormat,
                     QSettings::Scope::UserScope,
                     Consts::ORGANIZATION,
                     Consts::APPNAME)
    {}
};

#endif // SETTINGS_H
