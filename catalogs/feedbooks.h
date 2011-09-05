#ifndef FEEDBOOKS_H
#define FEEDBOOKS_H

#include "catalog.h"

#include <QXmlDefaultHandler>

namespace MeBook{

    struct DownloadBookInfo{
        QString title;
        QString author;
        QString subjects;
        QString description;
        QString bookUrl;
        QString coverUrl;
    };

    class FeedBooksXmlHandler : public QXmlDefaultHandler{
    public:
        bool startDocument();
        bool startElement(const QString &namespaceURI, const QString &localName, const QString &name, const QXmlAttributes &atts);
        bool characters(const QString &text);
        bool endElement(const QString &namespaceURI, const QString &localName, const QString &name);

        QList<DownloadBookInfo> getParsedBooks();
        int getNumberOfBooks();

    private:
        DownloadBookInfo currentBook;
        QString currentValue;
        QString parentTag;
        QList<DownloadBookInfo> parsedBooks;

        bool parsingAuthor;
        bool parsingEntry;

        bool gettingNumPages;
        int numberOfBooks;

    };

    class FeedBooks : public Catalog{
        Q_OBJECT
    public:
        FeedBooks(QObject *parent = 0);

    private:
        void parseBooks(const QString&);
    };

}
#endif
