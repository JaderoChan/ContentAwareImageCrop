#pragma once

#include <qstring.h>
#include <qsize.h>
#include <qcolor.h>

#include "language.h"

struct Settings
{
    Language language;
    bool isMaximized;
    QSize lastWindowSize;

    size_t cropUpdateT;
    bool isHighlightLine;
    bool isAntialiasingLine;
    bool isLimitImageSize;
    QColor highlightColor;
    QSize imageLimitedSize;

    QString lastOpenDirectory;
};

Settings loadSettings();

void saveSettings(const Settings& settings);
