#ifndef NCXHANDLER_H
#define NCXHANDLER_H

#include <QXmlDefaultHandler>

class NCXHandler : public QXmlDefaultHandler{
public:
    NCXHandler() : QXmlDefaultHandler(), 
        m_parsingNavPoint(false){}

    struct NavPoint{
        QString id;
        QString title;
        QString href;
    };

    bool startElement(const QString &namespaceURI, 
                      const QString &localName, 
                      const QString &qName, 
                      const QXmlAttributes &atts);

    bool endElement(const QString &namespaceURI, 
                    const QString &localName, 
                    const QString &qName);

    bool characters(const QString &ch);

    QList<NavPoint> navPoints() const{
        return navpoints;
    }

    bool fatalError(const QXmlParseException &exception);
private:
    QString tempText;
    QList<NavPoint> navpoints;
    bool m_parsingNavPoint;
};

#endif
