#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "shared.h"

namespace MeBook{
#define MSettings Settings::instance()
    class Settings : public QObject{
        Q_OBJECT
        Q_ENUMS(BookViewMode)
        Q_ENUMS(Sorting)
        Q_ENUMS(Orientation)
        Q_PROPERTY(int viewMode READ viewMode NOTIFY viewModeChanged)
    private:
        Settings(const Settings&){}
        Settings &operator=(const Settings&){}
        static Settings m_instance;    
        QSettings m_settings;
    public:
        Settings() : m_settings("durriken.org", "MeBook"){}
        static Settings& instance();

        enum BookViewMode{
            ScrollingMode, 
            PageMode
        };

        enum Sorting{
            byAuthor,
            byTitle,
            None
        };

        enum Orientation{
            Landscape, 
            Portrait
        };

        Q_INVOKABLE int sorting() const;
        Q_INVOKABLE void setSorting(int column);

        Q_INVOKABLE int viewMode() const; 
        Q_INVOKABLE void setViewMode(int mode);

        Q_INVOKABLE int orientation() const;
        Q_INVOKABLE void setOrientation(int mode);

        Q_INVOKABLE bool invertedColors() const;
        Q_INVOKABLE void setInvertedColors(bool invert);

        Q_INVOKABLE QString fontFamily() const;
        Q_INVOKABLE void setFontFamily(const QString &font);

        Q_INVOKABLE int fontPixelSize() const;
        Q_INVOKABLE void setFontPixelSize(int size);

        Q_INVOKABLE bool backlightForcing() const;
        Q_INVOKABLE void setBacklightForcing(bool backlightforcing);

    signals:
        void viewModeChanged();
    };

}

#endif
