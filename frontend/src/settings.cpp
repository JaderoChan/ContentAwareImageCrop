#include "settings.h"

#include <qsettings.h>

#include <config.h>

Settings loadSettings()
{
    Settings settings;
    QSettings qsettings(QSettings::NativeFormat, QSettings::UserScope, APP_ORGANIZATION, APP_TITLE);

    settings.language = qsettings.value("Language", getCurrentSystemLang()).value<Language>();

    return settings;
}

void saveSettings(const Settings& settings)
{
    QSettings qsettings(QSettings::NativeFormat, QSettings::UserScope, APP_ORGANIZATION, APP_TITLE);

    qsettings.setValue("Language", settings.language);
}
