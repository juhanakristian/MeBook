#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QColor>

namespace MeBook{

    class Bookmark
    {
    public:
        Bookmark(QString, QColor, int, float);
        QColor getColor() const;
        const QString& getName() const;
        int getSection() const;
        float getMultiplier() const;
        const QString percentageAsString() const;

        bool operator==(const Bookmark&) const;
        bool operator!=(const Bookmark&) const;

    private:
        QString name;
        QColor color;
        int section;
        float multiplier;

    };

}
#endif
