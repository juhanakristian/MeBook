#include "catalog.h"

#include <QFile>
#include <QtAlgorithms>

using namespace MeBook;

Catalog::Catalog(QObject *parent) : QObject(parent), openAfterDownload(0){
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveBook(QNetworkReply*)));

    coverDownloader = new QNetworkAccessManager(this);
    connect(coverDownloader, SIGNAL(finished(QNetworkReply*)), this, SLOT(coverDownloaded(QNetworkReply*)));

    QHash<int, QByteArray> roleNames;
    roleNames.insert(Title, "title");
    roleNames.insert(Author, "author");
    roleNames.insert(Subjects, "subjects");
    roleNames.insert(CoverURL, "cover_url");
    roleNames.insert(EpubURL, "epub_url");
    roleNames.insert(Description, "description");
    m_searchResultsModel = new RoleItemModel(roleNames, this);
}

void Catalog::searchBooks(const QString &searchPhrase)
{
    QString search = searchPhrase;
    QUrl url;
    url.setUrl(search);
    QString fullSearchURL = searchUrl + url.toString();
    QNetworkRequest request;
    request.setUrl(fullSearchURL);

//    qDeleteAll(currentBooks);
    //Clear currenBooks(QHash) and delete all Books that aren't in the library
    if(currentBooks.count() > 0){
        QHash<QString, Book*>::iterator iter = currentBooks.begin();
        while(iter != currentBooks.end()){
            if(!(*iter)->isInLibrary() && !downloading.key(*iter))
                delete (*iter);

            iter = currentBooks.erase(iter);
        }
    }
//    currentBooks.clear();

    searchReply = networkManager->get(request);
    connect(searchReply, SIGNAL(finished()), this, SLOT(searchReady()));
}

void Catalog::download(Book *book, bool open)
{
    QString url = currentBooks.key(book);
    if(!url.isNull()){
        QNetworkRequest request;
        request.setUrl(url);
        QNetworkReply *reply = 0;
        reply = networkManager->get(request);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
        downloading.insert(reply, book);

        if(open)
            openAfterDownload = book;
    }

}

void Catalog::getCoverImage(const QString &coverUrl, Book *book)
{
    if(!coverUrl.isNull()){
        QNetworkRequest request;
        request.setUrl(coverUrl);
        QNetworkReply *reply = 0;
        reply = coverDownloader->get(request);
        downloadingCovers.insert(reply, book);
    }
}
        
RoleItemModel* Catalog::searchResultsModel() const
{
    return m_searchResultsModel;
}

void Catalog::searchReady()
{
    if(searchReply){
        if(searchReply->error() == QNetworkReply::NoError){
            if(searchReply->open(QIODevice::ReadOnly | QIODevice::Text)){
                QByteArray ba = searchReply->readAll();
                QString result(ba);
                parseBooks(result);
            }
        }else{
            qDebug() << searchReply->errorString();
            emit message(tr("Search failed") + " " + searchReply->errorString());
        }

        searchReply->deleteLater();
    }
}

void Catalog::saveBook(QNetworkReply *reply)
{
    if(reply != searchReply){
        //TODO: make sure book is not null
        Book *book = downloading.value(reply);
        if(book){
            if(reply->error() == QNetworkReply::NoError){
                reply->open(QIODevice::ReadOnly);
                QByteArray ba = reply->readAll();
                QFile file("/home/user/MyDocs/books/" + book->getTitle() + ".epub");
                file.open(QIODevice::WriteOnly);
                file.write(ba);

                book->setFilename(file.fileName());


//                emit bookSaved(file.fileName());
                emit bookSaved(book);
                emit message(tr("Finished downloading") + "\"" + book->getTitle() + "\"");
                
                if(openAfterDownload == book){
                    openAfterDownload = 0;
                    emit openBook(book);
                }
                    
            } else{
                qDebug() << reply->errorString();
                emit message(tr("Failed to download book") + " " + reply->errorString());
            }

            downloading.remove(reply);
            reply->deleteLater();
        }
    }
}

void Catalog::coverDownloaded(QNetworkReply *reply)
{ 
    Book *book = downloadingCovers.value(reply);
    if(book){
        if(reply->error() == QNetworkReply::NoError){
            reply->open(QIODevice::ReadOnly);
            QByteArray ba = reply->readAll();
            QPixmap pm;
            pm.loadFromData(ba, "JPEG");
            book->addCoverImage(pm);
        } else{
            qDebug() << reply->errorString();
//            emit message(tr("Downloading cov
        }

        downloadingCovers.remove(reply);
        reply->deleteLater();
    }
}

void Catalog::error(QNetworkReply::NetworkError e)
{
    emit message("Failed to download book, " + QString::number(e));
}
