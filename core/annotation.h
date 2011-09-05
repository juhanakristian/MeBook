#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QString>
#include <QColor>
#include <QRect>

namespace MeBook{

    class Annotation
    {
    public:
        Annotation(int, float, QString, QString, QString, QColor);
        const QString& getAnnotation() const;
        const QString& getParagraph() const;
        const QString& getText() const;
        int getSection() const;
        float getPercentage() const;
        const QColor& getColor() const;

        bool operator==(const Annotation&) const;
        bool operator!=(const Annotation&) const;

    private:
        int section;
        float percentage;
        QString paragraph;
        QString text;
        QString annotation;
        QColor color;

    };

}
#endif
