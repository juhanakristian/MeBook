#include "annotation.h"

using namespace MeBook;

Annotation::Annotation(int s, float pr, QString p, QString t, QString a, QColor c) : 
        section(s), 
        percentage(pr), 
        paragraph(p), 
        text(t), 
        annotation(a), 
        color(c)
{

}

const QString& Annotation::getAnnotation() const
{
    return annotation;
}

const QString& Annotation::getParagraph() const
{
    return paragraph;
}

const QString& Annotation::getText() const
{
    return text;
}

int Annotation::getSection() const
{
    return section;
}

float Annotation::getPercentage() const
{
    return percentage;
}

const QColor& Annotation::getColor() const
{
    return color;
}

bool Annotation::operator==(const Annotation &lhs) const
{
    return lhs.getAnnotation() == annotation && lhs.getSection() == section &&
            lhs.getPercentage() == percentage;
}

bool Annotation::operator!=(const Annotation &lhs) const
{
    return !(*this == lhs);
}
