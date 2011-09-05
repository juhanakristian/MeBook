#include "library.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QStringList>
#include <QDir>

#include <QSqlError>
#include <QBuffer>
#include <QByteArray>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QRegExp>

#ifndef QT_NO_DEBUG
#include <QDebug>
const QString color = "\033[35;1m";
const QString colorend = "\033[0m";
const QString func = __PRETTY_FUNCTION__;
#endif

using namespace MeBook;

//TODO: Move progress, bookmarks and annotations related code to Book-class

Library Library::instance;

Library &Library::getInstance()
{
    return instance;
}

Library::Library() : QObject(),
    libraryDirectory("/home/user/MyDocs/books")
{

    //Create library directory if it doesn't exits before connecting to database
    QDir dir(libraryDirectory);
    if(!dir.exists())
        dir.mkdir(dir.absolutePath());

    connectdb("/home/user/MyDocs/books/bookdb.db");

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(bookDownloaded(QNetworkReply*)));
}

Library::~Library()
{
    // for(QList<Book*>::iterator iter = books.begin(); iter != books.end(); ++iter)
    // {
    //     saveBookProgress(*iter);
    //     saveBooktodb(*iter);
    // }
    delete m_booksModel;
    database.close();
}

void Library::loadLibrary()
{
    // scanDirectory(libraryDirectory);
    // loadBooksfromdb();
    // checkForChanges();
    // TODO: Fix support for adding new books from directory
    m_booksModel->sortByColumn(2);
    qDebug() << "Num:" << m_booksModel->rowCount();
}

void Library::scanDirectory(const QString &directory)
{
    QDir dir(directory);
    if(dir.exists()){
        QStringList sbooks = dir.entryList(QStringList("*.epub"), QDir::Files);
        for(QStringList::iterator iter = sbooks.begin(); iter != sbooks.end(); ++iter)
        {
            (*iter) = dir.absolutePath() + "/" + (*iter);
        }
        filenames.append(sbooks);
        QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoSymLinks);

        QStringList::const_iterator iter;
        for(iter = dirs.begin(); iter != dirs.end(); ++iter){
            if((*iter) != "." && (*iter) != "..")
                scanDirectory(dir.absolutePath() + "/" + *iter);
        }
    }
}

void Library::connectdb(const QString &databasefile)
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(databasefile);

    if(!database.open()){
        qDebug() << "Cannot connect to database! " << database.lastError();
        return;
    }

    if(!database.tables().contains("recent")){
        QSqlQuery query("CREATE TABLE recent(INTEGER PRIMARY KEY, filename)", database);
    }
    if(!database.tables().contains("lastspot")){
        QSqlQuery query("CREATE TABLE lastspot(filename, section, percentage)", database);
    }
    if(!database.tables().contains("bookmarks")){
        QSqlQuery query("CREATE TABLE bookmarks(filename, name, color, section, percentage)", database);
    }
    if(!database.tables().contains("annotations")){
        QSqlQuery query("CREATE TABLE annotations(filename, section, percentage, paragraph, text, color, annotation)", database);
    }
    if(!database.tables().contains("books")){
        QSqlQuery query("CREATE TABLE books(id INTEGER PRIMARY KEY, filename, title, author, subject, cover)", database);
    }

    m_booksModel = new RoleSqlTableModel(this);
    m_booksModel->setTable("books");
    m_booksModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_booksModel->generateRoleNames();


    // m_booksModel->select();
    // m_booksModel->sort(1, Qt::DescendingOrder);
    // m_booksModel->sortByColumn(4);
    // m_booksModel->select();


}

void Library::saveBookProgress(Book *book)
{
    
    BookProgress progress = book->getProgress();
    if(!progress.isValid())
        return;
    QSqlQuery selectQuery("SELECT * FROM lastspot WHERE filename=:filename", database);
    selectQuery.bindValue(":filename", book->getFilename());
    selectQuery.exec();
    if(!selectQuery.next()){
        QSqlQuery insertQuery("INSERT INTO lastspot VALUES(:filename, :section, :percentage)");
        insertQuery.bindValue(":filename", book->getFilename());
        insertQuery.bindValue(":section", progress.getSection());
        insertQuery.bindValue(":percentage", progress.getPercentage());

        if(!insertQuery.exec())
            qDebug() << "Database error: " << insertQuery.lastError();
    }else{
        QSqlQuery updateQuery("UPDATE lastspot SET section=:section, percentage=:percentage WHERE filename=:filename", database);
        updateQuery.bindValue(":section", progress.getSection());
        updateQuery.bindValue(":percentage", progress.getPercentage());
        updateQuery.bindValue(":filename", book->getFilename());

        if(!updateQuery.exec())
            qDebug() << "Database error: " << updateQuery.lastError();
    }
}

void Library::loadBookProgress(Book *book)
{
    QSqlQuery query("SELECT * FROM lastspot WHERE filename=:filename", database);
    query.bindValue(":filename", book->getFilename());
    if(query.exec()){
        if(query.next()){

            BookProgress progress;
            progress.setProgress(query.value(1).toInt(), query.value(2).toFloat());
            book->setProgress(progress);
        }
    }
    else{
        qDebug() << "Database error: " << query.lastError();
    }
}


void Library::saveBookmark(const Bookmark &bookmark, Book *book)
{ 
    QSqlQuery insertQuery("INSERT INTO bookmarks VALUES(:filename, :name, :color, :section, :percentage)");
    insertQuery.bindValue(":filename", book->getFilename());
    insertQuery.bindValue(":name", bookmark.getName());
    insertQuery.bindValue(":color", bookmark.getColor().name());
    insertQuery.bindValue(":section", bookmark.getSection());
    insertQuery.bindValue(":percentage", bookmark.getMultiplier());

    if(!insertQuery.exec())
        qDebug() << "Database error: " << insertQuery.lastError();
}

void Library::loadBookmarks(Book *book)
{
    QSqlQuery query("SELECT * FROM bookmarks WHERE filename=:filename", database);
    query.bindValue(":filename", book->getFilename());
    if(query.exec()){
        while(query.next()){
            Bookmark bookmark(query.value(1).toString(), QColor(query.value(2).toString()), query.value(3).toInt(), query.value(4).toFloat());

            book->addBookmark(bookmark);
        }
    } else{
        qDebug() << "Database error: " << query.lastError();
    }
}


void Library::saveAnnotation(const Annotation &annotation, Book *book)
{

    QSqlQuery insertQuery("INSERT INTO annotations VALUES(:filename, :section, :percentage, :paragraph, :text, :color, :annotation)");
    insertQuery.bindValue(":filename", book->getFilename());
    insertQuery.bindValue(":section", annotation.getSection());
    insertQuery.bindValue(":percentage", annotation.getPercentage());
    insertQuery.bindValue(":paragraph", annotation.getParagraph());
    insertQuery.bindValue(":text", annotation.getText());
    insertQuery.bindValue(":color", annotation.getColor().name());
    insertQuery.bindValue(":annotation", annotation.getAnnotation());

    if(!insertQuery.exec())
        qDebug() << "Database error: " << insertQuery.lastError();
}

void Library::bookDownloaded(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        QListIterator<BookDownload> iter(m_downloadQueue);
        BookDownload download;
        bool found = false;
        while(iter.hasNext()){
            BookDownload d = iter.next();
            if(d.reply == reply){
                download = d;
                found = true;
                break;
            }
        }
        if(!found)
            return;

        m_downloadQueue.removeOne(download);

        reply->open(QIODevice::ReadOnly);
        QByteArray ba = reply->readAll();

        QString filename = "/home/juhana/Projects/mebook/books/" + download.filename;

        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(ba);

        addToLibrary(filename);
    }

    reply->deleteLater();
}

void Library::loadAnnotations(Book *book)
{
    QSqlQuery query("SELECT * FROM annotations WHERE filename=:filename", database);
    query.bindValue(":filename", book->getFilename());
    if(query.exec()){
        while(query.next()){
            Annotation annotation(query.value(1).toInt(), query.value(2).toFloat(), query.value(3).toString(), query.value(4).toString(), query.value(6).toString(), QColor(query.value(5).toString()));

            book->addAnnotation(annotation);
        }
    }
    else{
        qDebug() << "Database error: " << query.lastError();
    }
}

bool Library::checkForChanges()
{
    QStringList filesindb;
    for(QList<Book*>::iterator iter = books.begin(); iter != books.end(); ++iter)
    {
        Book *book = *iter;
        filesindb.push_back(book->getFilename());
    }

    QStringList newfiles;
    for(QStringList::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
    {
        if(!filesindb.contains((*iter))){
            newfiles.push_back(*iter);
        }
    }

    if(newfiles.isEmpty())
        return false;

    for(QStringList::const_iterator iter = newfiles.begin(); iter != newfiles.end(); ++iter)
    { 
        Book *book = new Book(*iter, true);         
        book->loadMetaData();
        addToLibrary(book);
    }

    return true;

}

void Library::loadBooksfromdb()
{

    QSqlQuery query("SELECT * FROM books");

    if(!query.exec())
        return;

    while(query.next()){
        QString filename = query.value(1).toString();
        QString title = query.value(2).toString();
        QString author = query.value(3).toString();
        QString subject = query.value(4).toString();
        QByteArray ba = query.value(5).toByteArray();
        QPixmap cover;
        cover.loadFromData(ba, "PNG");

        Book *book = new Book(filename, true);         
        book->setTitle(title);
        book->setAuthor(author);
        book->setSubject(subject);
        if(!cover.isNull())
            book->addCoverImage(cover);

        loadBookProgress(book);
        // loadBookmarks(book);
        loadAnnotations(book);

        if(QFile::exists(filename)){
            addToLibrary(book, false);
        }
    }
}

void Library::saveBooktodb(Book *book)
{
    
    QSqlQuery selectQuery("SELECT * FROM books WHERE filename=:filename", database);
    selectQuery.bindValue(":filename", book->getFilename());
    selectQuery.exec();
    if(!selectQuery.next()){
        QSqlQuery insertQuery("INSERT INTO books VALUES(NULL, :filename, :title, :author, :subject, :cover)");
        insertQuery.bindValue(":filename", book->getFilename());
        insertQuery.bindValue(":title", book->getTitle());
        insertQuery.bindValue(":author", book->getAuthor());
        insertQuery.bindValue(":subject", book->getSubject());

        QPixmap pixmap = book->getCoverImage();
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        if(!pixmap.isNull())
            pixmap.scaledToWidth(100);
        pixmap.save(&buffer, "PNG");
        insertQuery.bindValue(":cover", ba);

        if(!insertQuery.exec())
            qDebug() << "Database error: " << insertQuery.lastError();
    }else{
        QByteArray bat = selectQuery.value(5).toByteArray();
        QPixmap covert;
        covert.loadFromData(bat, "PNG");

        //Updating only the cover for now
        if(covert.isNull()){
            int id = selectQuery.value(0).toInt();
            QSqlQuery updateQuery("UPDATE books SET cover=:cover WHERE id=:id", database);
            QPixmap pixmap = book->getCoverImage();
            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "PNG");
            updateQuery.bindValue(":cover", ba);
            updateQuery.bindValue(":id", id);

            if(!updateQuery.exec())
                qDebug() << "Database error: " << updateQuery.lastError();
        }
    }
}


Book *Library::bookWithId(const QString &id)
{
    QSqlQuery selectQuery("SELECT * FROM books WHERE id=:id");
    selectQuery.bindValue(":id", id);
    selectQuery.exec();
    if(!selectQuery.next())
        return 0;


    QString filename = selectQuery.value(1).toString();
    QString title = selectQuery.value(2).toString();
    QString author = selectQuery.value(3).toString();
    QString subject = selectQuery.value(4).toString();
    QByteArray ba = selectQuery.value(5).toByteArray();

    Book *book = new Book(filename);         
    book->setTitle(title);
    book->setAuthor(author);
    book->setSubject(subject);

    return book;
}

void Library::addToLibrary(Book *book, bool writeToDB)
{
    //Check if book is already in library.
    for(QList<Book*>::iterator iter = books.begin(); iter != books.end(); ++iter)
    {
        Book *tbook = *iter;
        if(tbook->getFilename() == book->getFilename())
            return;
    }
    books.push_back(book);
    book->changeLibraryStatus(true);
    emit bookAdded(book);
    if(writeToDB)
        saveBooktodb(book);
    connect(book, SIGNAL(bookmarkAdded(const Bookmark&, Book*)), this, SLOT(saveBookmark(const Bookmark&, Book*)));
    connect(book, SIGNAL(annotationAdded(const Annotation&, Book*)), this, SLOT(saveAnnotation(const Annotation&, Book*)));
    // m_booksModel->sort(4, Qt::AscendingOrder);
    // m_booksModel->select();
}

void Library::addToLibrary(const QString& path)
{
    QUrl url(path);
    if(url.scheme().isEmpty()){
        if(!QFile::exists(path))
            return;
        Book *book = new Book(path, true);
        book->loadMetaData();
        addToLibrary(book);
        return;
    }

    //path is an URL so we download the book and add it to library
    qDebug() << "Downloading" << path;
    QNetworkRequest request;
    request.setUrl(url);

    QRegExp re("\\w+.epub");
    qDebug() << "Indexin:" << re.indexIn(path);
    if(re.indexIn(path) < 0)
        return;

    BookDownload download;
    download.filename = re.cap();
    download.url = path;
    download.reply = m_manager->get(request);
    m_downloadQueue.append(download); 
    qDebug() << "Now downloading";
}

void Library::removeFromLibrary(Book *book)
{
    //TODO: Remove annotations, bookmarks etc..
    QSqlQuery query("DELETE FROM books WHERE filename=:filename");
    query.bindValue("filename", book->getFilename());
    if(!query.exec())
        qDebug() << "Error removing book" << query.lastError();
    books.removeOne(book);
    emit bookRemoved(book);
    if(QFile::exists(book->getFilename()))
        QFile::remove(book->getFilename());
    delete book;

}

void Library::deleteAnnotation(Book *book, Annotation annotation)
{
    QSqlQuery query("DELETE * FROM annotations WHERE filename=:filename AND annotation=:annotation");
    query.bindValue("filename", book->getFilename());
    query.bindValue("annotation", annotation.getAnnotation());
    if(!query.exec())
        qDebug() << "Error deleting annotation" << query.lastError();
    book->removeAnnotation(annotation);

}

void Library::deleteBookmark(Book *book, Bookmark bookmark)
{
    qDebug() << "deleteBookmark(Book*, Bookmark&)";
    QSqlQuery query("DELETE * FROM bookmarks WHERE filename=:filename AND name=:name");
    query.bindValue("filename", book->getFilename());
    query.bindValue("name", bookmark.getName());
    if(!query.exec())
        qDebug() << "Error deleting bookmark" << query.lastError();

    book->removeBookmark(bookmark);
}

QList<Book*> Library::getBooks(Settings::Sorting sorting)
{ 
    return books;
}
