#pragma once

#include <qcolor.h>
#include <qstring.h>
#include <qsize.h>
#include <qrect.h>

#include "language.h"

struct Settings
{
    Language language;
    bool isMaximized;
    QRect lastWindowGeo;

    size_t cropUpdateT;
    bool isHighlightLine;
    bool isAntialiasingLine;
    bool isLimitImageSize;
    QColor highlightColor;
    QSize imageLimitedSize;

    size_t maxRecordSteps;
    QString lastOpenDirectory;
};

Settings loadSettings();

void saveSettings(const Settings& settings);
