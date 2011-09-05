#include "settings.h"

#include <QDebug>

using namespace MeBook;

Settings Settings::m_instance;

Settings &Settings::instance()
{
    return m_instance;
}

int Settings::viewMode() const
{
    return m_settings.value("viewmode", 1).toInt();
}

void Settings::setViewMode(int mode)
{
    m_settings.setValue("viewmode", mode);
}


int Settings::orientation() const
{
    return m_settings.value("orientation", static_cast<int>(Settings::Portrait)).toInt();
}

void Settings::setOrientation(int orientation)
{
    m_settings.setValue("orientation", orientation); 
}


bool Settings::invertedColors() const
{
    return m_settings.value("colors", true).toBool();
}

void Settings::setInvertedColors(bool invert)
{
    m_settings.setValue("colors", invert);
}


QString Settings::fontFamily() const
{
    return m_settings.value("font-family", "Sans Serif").toString();
}

void Settings::setFontFamily(const QString &font)
{
    m_settings.setValue("font-family", font);
}


int Settings::fontPixelSize() const
{
    return m_settings.value("font-size", 24).toInt();
}

void Settings::setFontPixelSize(int size)
{
    m_settings.setValue("font-size", size);
}


bool Settings::backlightForcing() const
{
    return m_settings.value("backlight", true).toBool();
}

void Settings::setBacklightForcing(bool backlightforcing)
{
    m_settings.setValue("backlight", backlightforcing);
}

int Settings::sorting() const
{
    return m_settings.value("sorting", 2).toInt();
}

void Settings::setSorting(int column)
{
    m_settings.setValue("sorting", column);
}

