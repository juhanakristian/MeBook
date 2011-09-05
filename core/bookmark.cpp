#include "bookmark.h"

using namespace MeBook;

Bookmark::Bookmark(QString n, QColor c, int s, float pr) : 
        name(n),
        color(c),
        section(s),
        multiplier(pr)
{

    
}

QColor Bookmark::getColor() const
{
    return color;
}

const QString& Bookmark::getName() const
{
    return name;
}

int Bookmark::getSection() const
{
    return section;
}

float Bookmark::getMultiplier() const
{
    return multiplier;
}

const QString Bookmark::percentageAsString() const
{
    int p = multiplier * 100.00;
    return QString::number(p);
}

bool Bookmark::operator==(const Bookmark &lhs) const
{
    return lhs.getName() == name && lhs.getSection() == section &&
            lhs.getMultiplier() == multiplier;
}

bool Bookmark::operator!=(const Bookmark &lhs) const
{
    return !(*this == lhs);
}
