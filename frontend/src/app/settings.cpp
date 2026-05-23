#include "settings.h"

#include <qsettings.h>

#define RD_FIELD(qsettings, settings, field, default) \
(settings.field = qsettings.value(#field, default).value<decltype(settings.field)>())
#define WR_FIELD(qsettings, settings, field) \
(qsettings.setValue(#field, settings.field))

Settings loadSettings()
{
    Settings settings;
    QSettings qsettings;

    RD_FIELD(qsettings, settings, language, getCurrentSystemLang());
    RD_FIELD(qsettings, settings, isMaximized, true);
    RD_FIELD(qsettings, settings, lastWindowGeo, QRect(0, 0, 0, 0));
    RD_FIELD(qsettings, settings, cropUpdateT, 0);
    RD_FIELD(qsettings, settings, isHighlightLine, true);
    RD_FIELD(qsettings, settings, isAntialiasingLine, true);
    RD_FIELD(qsettings, settings, isLimitImageSize, true);
    RD_FIELD(qsettings, settings, highlightColor, QColor(255, 255, 0));
    RD_FIELD(qsettings, settings, imageLimitedSize, QSize(640, 640));
    RD_FIELD(qsettings, settings, lastOpenDirectory, "");

    return settings;
}

void saveSettings(const Settings& settings)
{
    QSettings qsettings;

    WR_FIELD(qsettings, settings, language);
    WR_FIELD(qsettings, settings, isMaximized);
    WR_FIELD(qsettings, settings, lastWindowGeo);
    WR_FIELD(qsettings, settings, cropUpdateT);
    WR_FIELD(qsettings, settings, isHighlightLine);
    WR_FIELD(qsettings, settings, isAntialiasingLine);
    WR_FIELD(qsettings, settings, isLimitImageSize);
    WR_FIELD(qsettings, settings, highlightColor);
    WR_FIELD(qsettings, settings, imageLimitedSize);
    WR_FIELD(qsettings, settings, lastOpenDirectory);

    qsettings.setValue("Language", settings.language);
}
