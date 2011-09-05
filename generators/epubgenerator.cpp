#include <QTextCursor>
#include <QTextFrame>
#include "shared.h"
#include "epub.h"
#include "epubgenerator.h"
#include <QDebug>

using namespace MeBook;

EpubResourceReply::EpubResourceReply(QObject *parent, Epub *epub, const QNetworkRequest &req) : QNetworkReply(parent)
{
    qDebug() << __PRETTY_FUNCTION__;
    setRequest(req);
    setUrl(req.url());
    setOperation(QNetworkAccessManager::GetOperation);
    open(ReadOnly | Unbuffered);
    m_epub = epub;    
    requestUrl = req.url();
    QString name = requestUrl.toString().remove(dummyURL + "/"); 
    qDebug() << name;
    // contentSize = epub_get_data(mEpub, name.toUtf8(), &buffer);
    qDebug() << "Getting file:" << name;
    QByteArray ba = m_epub->file(name);
    qDebug() << "Size:" << ba.size();
    contentSize = ba.size();
    buffer = ba.data();
    offset = 0;
    setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/css; charset=UTF-8"));

    QMetaObject::invokeMethod(this, "metaDataChanged", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "downloadProgress", Qt::QueuedConnection, Q_ARG(qint64, contentSize), Q_ARG(qint64, contentSize));
    QMetaObject::invokeMethod(this, "readyRead",Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "finished",Qt::QueuedConnection);
}

qint64 EpubResourceReply::readData(char *data, qint64 maxSize)
{
    qDebug() << __PRETTY_FUNCTION__;
    if(offset < contentSize)
    {
        qint64 number = qMin(maxSize, contentSize - offset);
        memcpy(data, buffer + offset, number);
        offset += number;
        return number;
    }
    return -1;
}

qint64 EpubResourceReply::bytesAvailable() const
{
    qDebug() << __PRETTY_FUNCTION__;
    return contentSize - offset;
}

void EpubResourceReply::abort()
{ 
    qDebug() << __PRETTY_FUNCTION__;
}

EpubGenerator::EpubGenerator(const QString &filename, QObject *parent) : 
    DocumentGenerator(filename, parent),
    m_epub(0)
{
    qDebug() << __PRETTY_FUNCTION__;
    m_epub = new Epub(filename, this);
}

EpubGenerator::~EpubGenerator()
{
    qDebug() << __PRETTY_FUNCTION__;
}

// QString EpubGenerator::getMetadata(enum epub_metadata type)
// {
//     int size;
//     unsigned char **data = 0;
// 
//     data = epub_get_metadata(mEpub, type, &size);
// 
//     if(data){
//         QString metadata = QString::fromUtf8((char*)data[0]);
//         for(int i = 1; i < size; i++){
//             metadata += ", ";
//             metadata += QString::fromUtf8((char*)data[i]);
//         }
// 
//         for(int i = 0; i < size; i++)
//             free(data[i]);
//         free(data);
//         return metadata;
//     }
// 
//     return QString();
// }


QNetworkReply* EpubGenerator::createRequest(Operation op, const QNetworkRequest &req, QIODevice* outgoingData)
{ 
    qDebug() << __PRETTY_FUNCTION__;
    if(req.url().scheme() != "ebook")
        return QNetworkAccessManager::createRequest(op, req, outgoingData);
    return new EpubResourceReply(this, m_epub, req);
}

void EpubGenerator::readMetaData()
{
    qDebug() << __PRETTY_FUNCTION__;

    if(!m_epub || !m_epub->isValid()){
        qDebug() << "Error while opening EPub file.";
        //TODO: emit error?
        return;
    }
    //Get document metadata
    emit setTitle(m_epub->title());
    emit setAuthor(m_epub->author());
    emit setSubject(m_epub->subjects().join(", "));
    emit setPublisher(m_epub->publisher());
    emit setDescription(m_epub->description());
    emit setPublishDate(m_epub->published());
    // emit setCategory();
    // emit setCopyright(getMetadata(EPUB_RIGHTS));

    QList<Epub::ManifestItem> manifest = m_epub->manifest();
    QListIterator<Epub::ManifestItem> iter(manifest);
    while(iter.hasNext()){
        Epub::ManifestItem item = iter.next();
        if(!item.id.contains("cover") || !item.href.contains("png"))
            continue;

        QByteArray ba = m_epub->file(item.href);
        QPixmap cover;
        cover.loadFromData(ba);
        if(!cover.isNull())
            emit addCoverImage(cover);
    }

}

void EpubGenerator::readContents()
{
    qDebug() << __PRETTY_FUNCTION__;
    if(!m_epub || !m_epub->isValid()){
        qDebug() << "Error while opening EPub file.";
        return;
    }


    QHash<QString, QString> toc;


    if(m_epub->hasNCX()){
        QList<Epub::NCXItem> ncx = m_epub->ncx();
        QListIterator<Epub::NCXItem> iter(ncx);
        while(iter.hasNext()){
            Epub::NCXItem item = iter.next();
            // toc.insert(item.href, item.title);
            QString *content = new QString(m_epub->ncxItem(item.id));
            emit addSection(content, item.href);
            emit addTOCItem(item.title, item.href);
        }
    } 
    else {
        QList<Epub::GuideItem> guide = m_epub->guide();
        QListIterator<Epub::GuideItem> iter(guide);
        while(iter.hasNext()){
            Epub::GuideItem item = iter.next();
            // toc.insert(item.href, item.title);
            QString *content = new QString(m_epub->guideItem(item.title));
            emit addSection(content, item.href);
            emit addTOCItem(item.title, item.href);
        }
    }


}

char *EpubGenerator::readFile(const QString &filename, int &size){
    qDebug() << __PRETTY_FUNCTION__;
    // if(!mEpub)
    //     return 0;
    // char *data;
    // size = -1;
    // size = epub_get_data(mEpub, filename.toUtf8(), &data);
    
    return 0;
}
