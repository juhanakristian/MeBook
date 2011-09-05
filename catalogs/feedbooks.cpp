#include "feedbooks.h"

#include <QXmlInputSource>
#include <QXmlSimpleReader>

#include <QStandardItem>

using namespace MeBook;

bool FeedBooksXmlHandler::startDocument()
{
    parsingEntry = false;
    parsingAuthor = false;
    gettingNumPages = false;
    numberOfBooks = 0;
    return true;
}

bool FeedBooksXmlHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &name, const QXmlAttributes &atts)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(name);

    if(localName == "entry"){
        parsingEntry = true;
        currentBook.subjects = QString();
    } else if(localName == "author" && parsingEntry){
        parsingAuthor = true;
    } else if(localName == "category" && parsingEntry){
        if(atts.count() > 0){
                currentBook.subjects += atts.value("label") + ", ";
        }
    } else if(parsingEntry && localName == "link" && atts.value("type") == "image/jpeg"){
        QString href = atts.value("href");
        if(!href.contains("thumbnail")){
            currentBook.coverUrl = href;
        }
    }

    return true;
}

bool FeedBooksXmlHandler::characters(const QString &text)
{
    if(!text.isEmpty())
        currentValue = text;

    return true;
}

bool FeedBooksXmlHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &name)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(name);

    if(parsingEntry){
        if(localName == "title")
            currentBook.title = currentValue;
        if(localName == "id")
            currentBook.bookUrl = currentValue + ".epub";
        if(localName == "name" && parsingAuthor)
            currentBook.author = currentValue;
        if(localName == "summary")
            currentBook.description = currentValue;
        if(localName == "entry"){
            parsedBooks.push_back(currentBook);                    
            parsingEntry = false;
        }
        if(localName == "author")
            parsingAuthor = false;
        // if(localName == "extent"){
        //     QString temp = currentValue;
        //     temp = temp.remove(" words");
        //     temp = temp.remove(",");
        //     currentBook.book->setLengthInWords(temp.toUInt());
        // }
        // if(localName == "issued")
        //     currentBook.book->setPublishDate(currentValue);
    } else{
        if(localName == "totalResults"){
            numberOfBooks = currentValue.toInt();
        }
    }

    return true;
}

QList<DownloadBookInfo> FeedBooksXmlHandler::getParsedBooks()
{
    return parsedBooks;
}

int FeedBooksXmlHandler::getNumberOfBooks()
{
    return numberOfBooks;
}

FeedBooks::FeedBooks(QObject *parent) : Catalog(parent)
{
    searchUrl = QString("http://www.feedbooks.com/books/search.atom?query=");
    downloadUrl = QString("http://www.feedbooks.com/book/");
}

void FeedBooks::parseBooks(const QString &result)
{
    QXmlInputSource *source = new QXmlInputSource();
    source->setData(result);

    QXmlSimpleReader reader;
    FeedBooksXmlHandler handler;
    reader.setContentHandler(&handler);
    bool ok = reader.parse(source);

    if(!ok)
        return;

    int number = handler.getNumberOfBooks();
    emit foundBooksCount(number);
    if(number > 0){
        QList<DownloadBookInfo> books = handler.getParsedBooks();

        for(QList<DownloadBookInfo>::iterator iter = books.begin(); iter != books.end(); ++iter)
        {
            DownloadBookInfo dbi = *iter;
            // emit addBook(dbi.book);
            // getCoverImage(dbi.coverUrl, dbi.book);
            // currentBooks.insert(dbi.bookUrl + ".epub", dbi.book);

            QStandardItem *item = new QStandardItem();
            item->setData(dbi.title, Title);
            item->setData(dbi.author, Author);
            item->setData(dbi.subjects, Subjects);
            item->setData(dbi.coverUrl, CoverURL);
            item->setData(dbi.bookUrl, EpubURL);
            item->setData(dbi.description, Description);

            m_searchResultsModel->appendRow(item);

        }
    }

    
}
