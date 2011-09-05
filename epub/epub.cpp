#include <QDebug>
#include <QFileInfo>
#include "epub.h"
#include "opfhandler.h"
#include "ncxhandler.h"

Epub::Epub(const QString &filename, QObject *parent) : 
    QObject(parent),
    m_filename(filename),
    m_valid(false),
    m_zip(filename)
{
    m_valid = m_zip.isValid() && parseContainer(m_zip.fileAsString("META-INF/container.xml"));
}

Epub::~Epub()
{
    
}

QString Epub::title()
{
    return opfMetadata(Title);
}

QString Epub::author()
{
    return opfMetadata(Author);
}

QString Epub::publisher()
{
    return opfMetadata(Publisher);
}

QString Epub::description()
{
    return opfMetadata(Description);
}

QString Epub::published()
{
    return opfMetadata(Published);
}

QStringList Epub::subjects()
{
    QList<OPFHandler::OPFData> data;
    readOPF(Subjects, &data);

    if(data.count() == 0)
        return QStringList();

    QStringList list;
    QListIterator<OPFHandler::OPFData> iter(data);
    while(iter.hasNext()){
        OPFHandler::OPFData d = iter.next();
        list << d.content;
    }

    return list;

}

QStringList Epub::spine()
{
    QList<OPFHandler::OPFData> data;
    readOPF(Spine, &data);

    if(data.count() == 0)
        return QStringList();

    QStringList list;
    QListIterator<OPFHandler::OPFData> iter(data);
    while(iter.hasNext()){
        OPFHandler::OPFData d = iter.next();
        list << d.atts["idref"];
    }

    return list;
}

QList<Epub::ManifestItem> Epub::manifest()
{
    QList<OPFHandler::OPFData> data;
    readOPF(Manifest, &data);

    QList<ManifestItem> list;
    QListIterator<OPFHandler::OPFData> iter(data);
    while(iter.hasNext()){
        OPFHandler::OPFData d = iter.next();
        ManifestItem item;
        item.id = d.atts["id"];
        item.href = d.atts["href"];
        item.media_type = d.atts["media-type"];
        list << item;
    }

    return list;
}

QList<Epub::GuideItem> Epub::guide()
{
    QList<OPFHandler::OPFData> data;
    readOPF(Guide, &data);

    QList<GuideItem> list;
    QListIterator<OPFHandler::OPFData> iter(data);
    while(iter.hasNext()){
        OPFHandler::OPFData d = iter.next();
        GuideItem item;
        item.title = d.atts["title"];
        item.href = d.atts["href"];
        list << item;
    }

    return list;
}


bool Epub::hasNCX() const
{
    return m_hasNCX;
}


QList<Epub::NCXItem> Epub::ncx()
{
    qDebug() << __PRETTY_FUNCTION__;
    QList<NCXItem> list;

    if(!m_hasNCX){
        qDebug() << "Epub has no NCX!";
        return list;
    }

    QList<ManifestItem> m = manifest();
    QListIterator<ManifestItem> iter(m);
    QString ncxpath;
    while(iter.hasNext()){
        ManifestItem item = iter.next();
        if(item.id == "ncx")
            ncxpath = item.href;
    }

    if(ncxpath.isEmpty()){
        qDebug() << "Can't find NCX path!";
        return list;
    }

    QXmlInputSource source;
    source.setData(m_zip.fileAsString(m_rootpath + "/" + ncxpath));

    QXmlSimpleReader reader;
    NCXHandler handler;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    bool ok = reader.parse(&source);

    if(!ok){
        qDebug() << "Parsing NCX failed!";
        return list;
    }

    QList<NCXHandler::NavPoint> navpoints = handler.navPoints();
    QListIterator<NCXHandler::NavPoint> i(navpoints);
    while(i.hasNext()){
        NCXHandler::NavPoint np = i.next();
        NCXItem ncxitem;
        ncxitem.id = np.id;
        ncxitem.href = np.href;
        ncxitem.title = np.title;
        list.append(ncxitem);
    }

    return list;

}

QByteArray Epub::manifestItem(const QString &id) const
{

}

QString Epub::guideItem(const QString &title)
{
    QList<Epub::GuideItem> n = guide(); 
    QListIterator<Epub::GuideItem> iter(n);
    while(iter.hasNext()){
        Epub::GuideItem item = iter.next();
        if(item.title == title)
            return m_zip.fileAsString(m_rootpath + "/" + item.href);
    }

    return QString();
}

QString Epub::ncxItem(const QString &id)
{
    QList<Epub::NCXItem> n = ncx(); 
    QListIterator<Epub::NCXItem> iter(n);
    while(iter.hasNext()){
        Epub::NCXItem item = iter.next();
        if(item.id == id)
            return m_zip.fileAsString(m_rootpath + "/" + item.href);
    }

    return QString();
}

QByteArray Epub::file(const QString &link)
{
    qDebug() << "Getting file:" << m_rootpath + "/" + link;
    QByteArray ba = m_zip.fileAsData(m_rootpath + "/" + link);
    return ba;
}

bool Epub::parseContainer(const QString &container)
{
    QXmlInputSource source;
    source.setData(container);

    QXmlSimpleReader reader;
    ContainerHandler handler;
    reader.setContentHandler(&handler);

    bool ok = reader.parse(&source);
    m_rootfile = handler.rootfile();

    QFileInfo info(m_rootfile);
    m_rootpath = info.path();

    return (ok && !m_rootfile.isEmpty());
}

bool Epub::readOPF(OPFDataType type, QList<OPFHandler::OPFData> *result)
{

    if(m_rootfile.isEmpty() || m_rootfile.isNull())
        return false;

    QXmlInputSource source;
    source.setData(m_zip.fileAsString(m_rootfile));

    QXmlSimpleReader reader;
    QStringList atts;
    OPFHandler *handler = NULL;
    
    switch(type){
    default:
        return false;
        break;
    case Title:
        handler = new OPFHandler("title");
        break;
    case Author:
        handler = new OPFHandler("creator");
        break;
    case Publisher:
        handler = new OPFHandler("publisher");
        break;
    case Description:
        handler = new OPFHandler("description");
        break;
    case Published:
        atts << "original-publication" << "ops-publication";
        handler = new OPFHandler("date", true, atts);
        break;
    case Subjects:
        handler = new OPFHandler("subject");
        break;
    case Spine:
        atts << "idref";
        handler = new OPFHandler("itemref", false, atts);
        break;
    case Manifest:
        atts << "id" << "href" << "media-type";
        handler = new OPFHandler("item", false, atts);
        break;
    case Guide:
        atts << "type" << "title" << "href";
        handler = new OPFHandler("reference", false, atts);
        break;
    }

    if(!handler)
        return false;

    reader.setContentHandler(handler);
    reader.setErrorHandler(handler);
    bool ok = reader.parse(&source);


    if(!ok)
        return false;

    *result = handler->parsedData();
    m_hasNCX = handler->hasNCX();

    delete handler;

    return true;
}
    
QString Epub::opfMetadata(OPFDataType type)
{
    QList<OPFHandler::OPFData> data;
    readOPF(type, &data);

    if(data.count() == 0)
        return QString();

    return data.first().content;
}

bool Epub::isValid() const
{
    return m_valid;
}

