#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QStandardItemModel>

#include "book.h"
#include "roleitemmodel.h"

namespace MeBook{


    //Catalog is the base class for all online catalogs.
    //To add a catalog one needs subclass Catalog, provide appropriate URLs and implement the pure virtual function parseBooks which is used to parse the search results.
    class Catalog : public QObject{
        Q_OBJECT
    public:
        Catalog(QObject *parent = 0);
        Q_INVOKABLE void searchBooks(const QString&);
        void download(Book*, bool = false);
        void getCoverImage(const QString&, Book*);

        RoleItemModel *searchResultsModel() const;

    private:
        virtual void parseBooks(const QString&) = 0;


    protected:
        QString searchUrl;
        QString downloadUrl;
        QHash<QString, Book*> currentBooks;
        QHash<QNetworkReply*, Book*> downloading;
        QHash<QNetworkReply*, Book*> downloadingCovers;

        QNetworkAccessManager *networkManager;
        QNetworkAccessManager *coverDownloader;
        QNetworkReply *searchReply;

        Book *openAfterDownload;

        RoleItemModel *m_searchResultsModel;

        enum Roles{
            Title = Qt::UserRole + 1,
            Author,
            Subjects,
            CoverURL,
            EpubURL,
            Description
        };

    public slots:
        void searchReady();
        void saveBook(QNetworkReply*);
        void coverDownloaded(QNetworkReply*); 
        void error(QNetworkReply::NetworkError);

    signals:
        void addBook(Book*);
        void bookSaved(QString);
        void bookSaved(Book*);
        void foundBooksCount(int);
        void openBook(Book*);

        //Messages are emitted in case of errors etc.
        void message(const QString);
    };

}
#endif
