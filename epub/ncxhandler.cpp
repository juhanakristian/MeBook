#include "ncxhandler.h"

#include <QDebug>


bool NCXHandler::startElement(const QString &namespaceURI, 
                  const QString &localName, 
                  const QString &qName, 
                  const QXmlAttributes &atts)
{
    if(localName == "navPoint"){
        NavPoint np;
        np.id = atts.value("id");
        navpoints.append(np);
        m_parsingNavPoint = true;
    } else if(localName == "content"){
        navpoints.last().href = atts.value("src");
    }

    return true;
}

bool NCXHandler::endElement(const QString &namespaceURI, 
                const QString &localName, 
                const QString &qName)
{
    if(localName == "text" && m_parsingNavPoint)
        navpoints.last().title = tempText;
    else if(localName == "navPoint")
        m_parsingNavPoint = false;
    return true;
}

bool NCXHandler::characters(const QString &ch)
{
    if(!ch.isEmpty())
        tempText = ch;

    return true;
}

bool NCXHandler::fatalError(const QXmlParseException &exception)
{
    qDebug() << "Fatal error while parsing NCX at line" 
             << exception.lineNumber() 
             << ", column" 
             << exception.columnNumber()
             << ":"
             << exception.message();

    return false;
}
