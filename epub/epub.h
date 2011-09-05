#ifndef EPUB_H
#define EPUB_H

#include <QObject>
#include "ziphandler.h"
#include "opfhandler.h"
#include <QDebug>

#include <QXmlDefaultHandler>

class ContainerHandler : public QXmlDefaultHandler{
public:
    ContainerHandler() : QXmlDefaultHandler(){}

    QString rootfile() const{
        return m_rootfile;
    }

    bool startElement(const QString &namespaceURI, 
                      const QString &localName, 
                      const QString &qName, 
                      const QXmlAttributes &atts){
        if(localName == "rootfile"){
            m_rootfile = atts.value("full-path");
        }
        return true;
    }

    bool endElement(const QString &namespaceURI, 
                    const QString &localName, 
                    const QString &qName){
 
        return true;
    }

    bool characters(const QString &ch){
        if(!ch.isEmpty())
            tempText = ch;

        return true;
    }

private:
    QString m_rootfile;
    QString tempText;
};

class Epub : public QObject{
    Q_OBJECT
public:
    Epub(const QString &filename, QObject *parent = 0);
    ~Epub();

    enum OPFDataType{
        Title,
        Author,
        Publisher,
        Description,
        Published,
        Subjects,
        Spine,
        Manifest,
        Guide
    };

    struct ManifestItem{
        QString id;
        QString href;
        QString media_type;
    };


    struct GuideItem{
        QString title;
        QString href;
    };

    struct NCXItem{
        QString title;
        QString href;
        QString id;
    };

    bool isValid() const;

    QString title(); 
    QString author();
    QString publisher();
    QString description();
    QString published();
    QStringList subjects();
    QStringList spine();
    QList<ManifestItem> manifest();
    QList<GuideItem> guide();

    bool hasNCX() const;

    QList<NCXItem> ncx();

    QByteArray manifestItem(const QString &id) const;
    QString guideItem(const QString &title);
    QString ncxItem(const QString &id);

    QByteArray file(const QString &link);

    friend class TestEpub;

private:
    bool parseContainer(const QString &container);
    bool readOPF(OPFDataType type, QList<OPFHandler::OPFData> *result);
    QString opfMetadata(OPFDataType type);
    QString m_filename;
    bool m_valid;
    ZipHandler m_zip;

    bool m_hasNCX;
    QString m_rootfile;
    QString m_rootpath;
};

// typedef ManifestItemList QList<ManifestItem>;
// typedef GuideItemList QList<GuideItem>;

#endif
