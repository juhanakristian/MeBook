#include "opfhandler.h"

#include <QListIterator>
#include <QDebug>

OPFHandler::OPFHandler(const QString &tag, bool readContent, QStringList atts) : 
    QXmlDefaultHandler(),
    m_ncx(false),
    m_tag(tag),
    m_readContent(readContent),
    m_atts(atts)
{

}

OPFHandler::OPFHandler() :
    QXmlDefaultHandler(),
    m_ncx(false),
    m_readContent(true)
{

}
    
void OPFHandler::setParsingParameters(const QString &tag, bool readContent, QStringList atts)
{
    m_tag = tag;
    m_readContent = readContent;
    m_atts = atts;
}
    
QList<OPFHandler::OPFData> OPFHandler::parsedData() const
{
    return m_data;
}


bool OPFHandler::hasNCX() const
{
    return m_ncx;
}

bool OPFHandler::startElement(const QString &namespaceURI,
                  const QString &localName, 
                  const QString &qName, 
                  const QXmlAttributes &atts)
{
    if(localName == m_tag){
        OPFData data;
        if(!m_atts.isEmpty()){
            QListIterator<QString> iter(m_atts);
            while(iter.hasNext()){
                QString a = iter.next();
                data.atts.insert(a, atts.value(a));
            }
        }
        m_data << data;
    }
    else if(localName == "spine" && atts.value("toc") == "ncx")
        m_ncx = true;

    return true;
        
}

bool OPFHandler::endElement(const QString &namespaceURI, 
                const QString &localName, 
                const QString &qName)
{
    if(localName != m_tag)
        return true;

    if(m_readContent)
        m_data.last().content = tempText;

    return true;
}

bool OPFHandler::characters(const QString &ch)
{
    if(!ch.isEmpty())
        tempText = ch;
    
    return true;
}

bool OPFHandler::fatalError(const QXmlParseException &exception)
{
    qDebug() << "Fatal error while parsing OPF at line" 
             << exception.lineNumber() 
             << ", column" 
             << exception.columnNumber()
             << ":"
             << exception.message();

    return false;
}
