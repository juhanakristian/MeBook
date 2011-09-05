#ifndef OPFHANDLER_H
#define OPFHANDLER_H

#include <QXmlDefaultHandler>
#include <QString>
#include <QStringList>
#include <QMap>

class OPFHandler : public QXmlDefaultHandler{
public:
    OPFHandler(const QString &tag, bool readContent = true, QStringList atts = QStringList());
    OPFHandler();

    void setParsingParameters(const QString &tag, bool readContent = true, QStringList atts = QStringList());

    struct OPFData{
        QString content;
        QMap<QString, QString> atts;
    };

    QList<OPFData> parsedData() const;
    bool hasNCX() const;

    bool startElement(const QString &namespaceURI, 
                      const QString &localName, 
                      const QString &qName, 
                      const QXmlAttributes &atts);

    bool endElement(const QString &namespaceURI, 
                    const QString &localName, 
                    const QString &qName);

    bool characters(const QString &ch);

    bool fatalError(const QXmlParseException &exception);

private:
    QString tempText;
    QString m_tag;
    QStringList m_atts;
    bool m_readContent;

    QList<OPFData> m_data;

    bool m_ncx;
};

#endif
